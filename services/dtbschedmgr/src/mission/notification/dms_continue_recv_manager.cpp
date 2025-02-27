/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mission/notification/dms_continue_recv_manager.h"

#include <sys/prctl.h>

#include "datetime_ex.h"

#include "datashare_manager.h"
#include "dfx/distributed_radar.h"
#include "dfx/distributed_ue.h"
#include "distributed_sched_utils.h"
#include "distributed_sched_adapter.h"
#include "dtbschedmgr_device_info_storage.h"
#include "dtbschedmgr_log.h"
#include "mission/dsched_sync_e2e.h"
#include "mission/wifi_state_adapter.h"
#include "multi_user_manager.h"
#include "parcel_helper.h"
#include "softbus_adapter/softbus_adapter.h"
#include "switch_status_dependency.h"
#include "os_account_manager.h"
#include "util/dsched_continue_info_util.h"

namespace OHOS {
namespace DistributedSchedule {
namespace {
constexpr int32_t ON_CALLBACK = 0;
constexpr int32_t ACTIVE = 0;
constexpr int32_t INACTIVE = 1;
constexpr int32_t INDEX_2 = 2;
constexpr int32_t INDEX_3 = 3;
constexpr int32_t DBMS_RETRY_MAX_TIME = 5;
constexpr int32_t DBMS_RETRY_DELAY = 2000;
constexpr int32_t REGIST_MAX_SIZE = 1000;
const std::string TAG = "DMSContinueRecvMgr";
const std::string DBMS_RETRY_TASK = "retry_on_boradcast_task";
const std::u16string DESCRIPTOR = u"ohos.aafwk.RemoteOnListener";
const std::string QUICK_START_CONFIGURATION = "_ContinueQuickStart";
}

DMSContinueRecvMgr::~DMSContinueRecvMgr()
{
    HILOGI("~DMSContinueRecvMgr. accountId: %{public}d.", accountId_);
    UnInit();
}

void DMSContinueRecvMgr::Init(int32_t accountId)
{
    HILOGI("Init start. accountId: %{public}d.", accountId);
    accountId_ = accountId;
    if (eventHandler_ != nullptr) {
        HILOGI("Already inited, end.");
        return;
    }
    {
        missionDiedListener_ = new DistributedMissionDiedListener();
        eventThread_ = std::thread(&DMSContinueRecvMgr::StartEvent, this);
        std::unique_lock<std::mutex> lock(eventMutex_);
        eventCon_.wait(lock, [this] {
            return eventHandler_ != nullptr;
        });
    }
    HILOGI("Init end");
}

void DMSContinueRecvMgr::UnInit()
{
    HILOGI("UnInit start. accountId: %{public}d.", accountId_);
    if (eventHandler_ != nullptr && eventHandler_->GetEventRunner() != nullptr) {
        eventHandler_->GetEventRunner()->Stop();
        if (eventThread_.joinable()) {
            eventThread_.join();
        }
        eventHandler_ = nullptr;
    } else {
        HILOGE("eventHandler_ is nullptr");
    }
    HILOGI("UnInit end");
}

void DMSContinueRecvMgr::NotifyDataRecv(std::string& senderNetworkId,
    uint8_t* payload, uint32_t dataLen)
{
    HILOGI("NotifyDataRecv start, senderNetworkId: %{public}s, dataLen: %{public}u. accountId: %{public}d.",
        GetAnonymStr(senderNetworkId).c_str(), dataLen, accountId_);
    if (!DmsKvSyncE2E::GetInstance()->CheckCtrlRule()) {
        HILOGE("Forbid sending and receiving");
        return;
    }
    if (!DataShareManager::GetInstance().IsCurrentContinueSwitchOn()) {
        HILOGE("ContinueSwitch status is off");
        return;
    }
    if (!WifiStateAdapter::GetInstance().IsWifiActive()) {
        HILOGE("wifi is not activated");
        return;
    }
    if (dataLen < DMS_SEND_LEN) {
        HILOGE("dataLen error, dataLen: %{public}u", dataLen);
        return;
    }
    uint8_t type = (payload[0] & DMS_0XF0) >> CONTINUE_SHIFT_04;
    uint8_t len = payload[0] & DMS_0X0F;
    if (len < DMS_DATA_LEN || (type > DMS_0X0F)) {
        HILOGE("len or type error, len: %{public}u, type: %{public}u", len, type);
        return;
    }
    uint16_t bundleNameId = (payload[1] << CONTINUE_SHIFT_08) | payload[INDEX_2];
    uint8_t continueTypeId = payload[INDEX_3];
    HILOGI("bundleNameId: %{public}u, continueTypeId: %{public}u", bundleNameId, continueTypeId);
    int32_t state = ACTIVE;
    if (type == DMS_UNFOCUSED_TYPE) {
        state = INACTIVE;
    }
    PostOnBroadcastBusiness(senderNetworkId, bundleNameId, continueTypeId, state);
    HILOGI("NotifyDataRecv end");
}

int32_t DMSContinueRecvMgr::RegisterOnListener(const std::string& type, const sptr<IRemoteObject>& obj)
{
    HILOGI("RegisterOnListener start, type: %{public}s. accountId: %{public}d.", type.c_str(), accountId_);
    if (obj == nullptr) {
        HILOGE("obj is null, type: %{public}s", type.c_str());
        return INVALID_PARAMETERS_ERR;
    }
    onType_ = type;
    std::lock_guard<std::mutex> registerOnListenerMapLock(eventMutex_);
    auto iterItem = registerOnListener_.find(type);
    if (iterItem == registerOnListener_.end() && registerOnListener_.size() < REGIST_MAX_SIZE) {
        HILOGD("The itemItem does not exist in the registerOnListener_, adding, type: %{public}s", type.c_str());
        std::vector<sptr<IRemoteObject>> objs;
        obj->AddDeathRecipient(missionDiedListener_);
        objs.emplace_back(obj);
        registerOnListener_[type] = objs;
        HILOGI("RegisterOnListener end");
        return ERR_OK;
    }
    for (auto iter : iterItem->second) {
        if (iter == obj) {
            HILOGI("already have obj");
            return NO_MISSION_INFO_FOR_MISSION_ID;
        }
    }
    obj->AddDeathRecipient(missionDiedListener_);
    iterItem->second.emplace_back(obj);
    HILOGI("RegisterOnListener end");
    return ERR_OK;
}

int32_t DMSContinueRecvMgr::RegisterOffListener(const std::string& type,
    const sptr<IRemoteObject>& obj)
{
    HILOGI("RegisterOffListener start, type: %{public}s. accountId: %{public}d.", type.c_str(), accountId_);
    if (obj == nullptr) {
        HILOGE("obj is null, type: %{public}s", type.c_str());
        return INVALID_PARAMETERS_ERR;
    }
    for (auto iterItem = registerOnListener_.begin(); iterItem != registerOnListener_.end();) {
        for (auto iter = iterItem->second.begin(); iter != iterItem->second.end();) {
            if (*iter == obj) {
                std::lock_guard<std::mutex> registerOnListenerMapLock(eventMutex_);
                iter = iterItem->second.erase(iter);
                obj->RemoveDeathRecipient(missionDiedListener_);
                break;
            } else {
                iter++;
            }
        }
        if (iterItem->second.empty()) {
            std::lock_guard<std::mutex> registerOnListenerMapLock(eventMutex_);
            iterItem = registerOnListener_.erase(iterItem);
        } else {
            iterItem++;
        }
    }
    HILOGI("RegisterOffListener end");
    return ERR_OK;
}

void DMSContinueRecvMgr::StartEvent()
{
    HILOGI("StartEvent start");
    prctl(PR_SET_NAME, CONTINUE_RECV_MANAGER.c_str());
    auto runner = AppExecFwk::EventRunner::Create(false);
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        eventHandler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    }
    eventCon_.notify_one();
    if (runner != nullptr) {
        runner->Run();
    } else {
        HILOGE("runner is null");
    }
    HILOGI("StartEvent end");
}

int32_t DMSContinueRecvMgr::VerifyBroadcastSource(const std::string& senderNetworkId, const std::string& srcBundleName,
    const std::string& sinkBundleName, const std::string& continueType, const int32_t state)
{
    HILOGI("accountId: %{public}d.", accountId_);
    std::lock_guard<std::mutex> currentIconLock(iconMutex_);
    if (state == ACTIVE) {
        iconInfo_.senderNetworkId = senderNetworkId;
        iconInfo_.bundleName = sinkBundleName;
        iconInfo_.sourceBundleName = srcBundleName;
        iconInfo_.continueType = continueType;
    } else {
        if (senderNetworkId != iconInfo_.senderNetworkId) {
            HILOGW("Sender not match, task abort. senderNetworkId: %{public}s, saved NetworkId: %{public}s.",
                GetAnonymStr(senderNetworkId).c_str(), GetAnonymStr(iconInfo_.senderNetworkId).c_str());
            return INVALID_PARAMETERS_ERR;
        }

        if (sinkBundleName != iconInfo_.bundleName) {
            HILOGW("BundleName not match, task abort. bundleName: %{public}s, saved bundleName: %{public}s",
                sinkBundleName.c_str(), iconInfo_.bundleName.c_str());
            return INVALID_PARAMETERS_ERR;
        }

        iconInfo_.senderNetworkId = "";
        iconInfo_.bundleName = "";
        iconInfo_.continueType = "";
    }
    return ERR_OK;
}

void DMSContinueRecvMgr::PostOnBroadcastBusiness(const std::string& senderNetworkId,
    uint16_t bundleNameId, uint8_t continueTypeId, const int32_t state, const int32_t delay, const int32_t retry)
{
    HILOGI("accountId: %{public}d.", accountId_);
    auto feedfunc = [this, senderNetworkId, bundleNameId, continueTypeId, state, retry]() mutable {
        DealOnBroadcastBusiness(senderNetworkId, bundleNameId, continueTypeId, state, retry);
    };
    if (eventHandler_ != nullptr) {
        eventHandler_->RemoveTask(DBMS_RETRY_TASK);
        eventHandler_->PostTask(feedfunc, DBMS_RETRY_TASK, delay);
    } else {
        HILOGE("eventHandler_ is nullptr");
    }
}

int32_t DMSContinueRecvMgr::RetryPostBroadcast(const std::string& senderNetworkId,
    uint16_t bundleNameId, uint8_t continueTypeId, const int32_t state, const int32_t retry)
{
    HILOGI("Retry post broadcast, current retry times %{public}d. accountId: %{public}d.", retry, accountId_);
    if (retry == DBMS_RETRY_MAX_TIME) {
        HILOGE("meet max retry time!");
        return INVALID_PARAMETERS_ERR;
    }
    PostOnBroadcastBusiness(senderNetworkId, bundleNameId, continueTypeId, state, DBMS_RETRY_DELAY, retry + 1);
    return ERR_OK;
}

int32_t DMSContinueRecvMgr::DealOnBroadcastBusiness(const std::string& senderNetworkId,
    uint16_t bundleNameId, uint8_t continueTypeId, const int32_t state, const int32_t retry)
{
    HILOGI("start, senderNetworkId: %{public}s, bundleNameId: %{public}u, state: %{public}d. accountId: %{public}d.",
        GetAnonymStr(senderNetworkId).c_str(), bundleNameId, state, accountId_);

    DSchedContinueInfo continueInfo;
    if (!DSchedContinueInfoUtil::GetInstance().CompleteContinueInfo(senderNetworkId, bundleNameId, continueTypeId,
                                                                    continueInfo, retry)) {
        HILOGE("The app is not installed on the local device.");
        NotifyIconDisappear(bundleNameId, senderNetworkId, state);
        return INVALID_PARAMETERS_ERR;
    }
    HILOGI("got finalBundleName: %{public}s", continueInfo.sinkBundleName_.c_str());
    AppExecFwk::BundleInfo localBundleInfo;
    int32_t localBundleInfoQueryResult = BundleManagerInternal::GetLocalBundleInfo(continueInfo.sinkBundleName_,
                                                                                   localBundleInfo);
    if (localBundleInfoQueryResult != ERR_OK ||
        localBundleInfo.applicationInfo.bundleType != AppExecFwk::BundleType::APP) {
        HILOGE("The bundleType must be app, but it is %{public}d", localBundleInfo.applicationInfo.bundleType);
        NotifyIconDisappear(bundleNameId, senderNetworkId, state);
        return INVALID_PARAMETERS_ERR;
    }
    if (state == ACTIVE
        && !IsBundleContinuable(localBundleInfo, continueInfo.sourceAbilityName_, continueInfo.sourceModuleName_,
                                continueInfo.continueType_)) {
        HILOGE("Bundle %{public}s is not continuable", continueInfo.sinkBundleName_.c_str());
        NotifyIconDisappear(bundleNameId, senderNetworkId, state);
        return BUNDLE_NOT_CONTINUABLE;
    }

    int32_t ret = VerifyBroadcastSource(senderNetworkId, continueInfo.sourceBundleName_, continueInfo.sinkBundleName_,
                                        continueInfo.continueType_, state);
    if (ret != ERR_OK) {
        return ret;
    }
    ret = NotifyDockDisplay(bundleNameId, currentIconInfo(senderNetworkId, continueInfo.sourceBundleName_,
                                                          continueInfo.sinkBundleName_, continueInfo.continueType_),
                            state);
    if (ret != ERR_OK) {
        return ret;
    }
    HILOGI("DealOnBroadcastBusiness end");
    return ERR_OK;
}

void DMSContinueRecvMgr::NotifyIconDisappear(uint16_t bundleNameId, const std::string &senderNetworkId,
    const int32_t state)
{
    if (state == ACTIVE && senderNetworkId == iconInfo_.senderNetworkId) {
        NotifyDockDisplay(bundleNameId, iconInfo_, INACTIVE);
    }
}

int32_t DMSContinueRecvMgr::NotifyDockDisplay(uint16_t bundleNameId, const currentIconInfo& continueInfo,
    const int32_t state)
{
    std::lock_guard<std::mutex> registerOnListenerMapLock(eventMutex_);
    auto iterItem = registerOnListener_.find(onType_);
    if (iterItem == registerOnListener_.end()) {
        HILOGE("get iterItem failed from registerOnListener_, bundleNameId: %{public}u", bundleNameId);
        return INVALID_PARAMETERS_ERR;
    }
    std::vector<sptr<IRemoteObject>> objs = iterItem->second;
    for (auto iter : objs) {
        NotifyRecvBroadcast(iter, continueInfo, state);
    }
    return ERR_OK;
}

bool DMSContinueRecvMgr::IsBundleContinuable(const AppExecFwk::BundleInfo& bundleInfo,
    const std::string &srcAbilityName, const std::string &srcModuleName, const std::string &srcContinueType)
{
    HILOGI("accountId: %{public}d.", accountId_);
    std::string formatSrcContinueType = ContinueTypeFormat(srcContinueType);
    for (auto &abilityInfo: bundleInfo.abilityInfos) {
        if (!abilityInfo.continuable) {
            continue;
        }
        for (const auto &continueTypeItem: abilityInfo.continueType) {
            HILOGI("IsBundleContinuable check: srcAbilityName:%{public}s; srcContinueType:%{public}s;"
                   " sinkAbilityName:%{public}s; sinkContinueType:%{public}s;",
                   srcAbilityName.c_str(), srcContinueType.c_str(), abilityInfo.name.c_str(),
                   continueTypeItem.c_str());
            if (continueTypeItem == srcContinueType || continueTypeItem == formatSrcContinueType) {
                return true;
            }
            if (abilityInfo.moduleName == srcModuleName && abilityInfo.name == srcAbilityName) {
                return true;
            }
        }
    }
    return false;
}

std::string DMSContinueRecvMgr::ContinueTypeFormat(const std::string &continueType)
{
    HILOGI("accountId: %{public}d.", accountId_);
    std::string suffix = QUICK_START_CONFIGURATION;
    if (suffix.length() <= continueType.length() &&
        continueType.rfind(suffix) == (continueType.length() - suffix.length())) {
        return continueType.substr(0, continueType.rfind(QUICK_START_CONFIGURATION));
    } else {
        return continueType + QUICK_START_CONFIGURATION;
    }
}

void DMSContinueRecvMgr::NotifyRecvBroadcast(const sptr<IRemoteObject>& obj,
    const currentIconInfo& continueInfo, const int32_t state)
{
    HILOGI("accountId: %{public}d.", accountId_);
    std::string networkId = continueInfo.senderNetworkId;
    std::string srcBundleName = continueInfo.sourceBundleName;
    std::string sinkBundleName = continueInfo.bundleName;
    std::string continueType = continueInfo.continueType.empty() ? "" : continueInfo.continueType;

    HILOGI("NotifyRecvBroadcast start");
    if (obj == nullptr) {
        HILOGE("obj is null");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(DESCRIPTOR)) {
        HILOGE("NotifyRecvBroadcast write interface token failed");
        return;
    }
    PARCEL_WRITE_HELPER_NORET(data, Int32, state);
    PARCEL_WRITE_HELPER_NORET(data, String, networkId);
    PARCEL_WRITE_HELPER_NORET(data, String, sinkBundleName);
    PARCEL_WRITE_HELPER_NORET(data, String, continueType);
    PARCEL_WRITE_HELPER_NORET(data, String, srcBundleName);
    HILOGI("[PerformanceTest] NotifyRecvBroadcast called, IPC begin = %{public}" PRId64, GetTickCount());
    int32_t error = obj->SendRequest(ON_CALLBACK, data, reply, option);
    if (state != INACTIVE) {
        std::string bName = sinkBundleName;
        std::string cType = continueType;
        std::string abilityName = BundleManagerInternal::GetInstance().GetAbilityName(networkId,
            bName, cType);
        DmsUE::GetInstance().NotifyDockShowIcon(sinkBundleName, abilityName, networkId, error);
    }
    if (error != ERR_NONE) {
        HILOGE("NotifyRecvBroadcast fail, error: %{public}d", error);
        return;
    }
    HILOGI("NotifyRecvBroadcast end");
}

void DMSContinueRecvMgr::NotifyDied(const sptr<IRemoteObject>& obj)
{
    HILOGI("NotifyDied start. accountId: %{public}d.", accountId_);
    if (obj == nullptr) {
        HILOGE("obj is null");
        return;
    }
    for (auto iterItem = registerOnListener_.begin(); iterItem != registerOnListener_.end();) {
        for (auto iter = iterItem->second.begin(); iter != iterItem->second.end();) {
            if (*iter == obj) {
                obj->RemoveDeathRecipient(missionDiedListener_);
                iter = iterItem->second.erase(iter);
            } else {
                iter++;
            }
        }
        if (iterItem->second.empty()) {
            std::lock_guard<std::mutex> registerOnListenerMapLock(eventMutex_);
            iterItem = registerOnListener_.erase(iterItem);
        } else {
            iterItem++;
        }
    }
    HILOGI("NotifyDied end");
}

#ifdef SUPPORT_COMMON_EVENT_SERVICE
void DMSContinueRecvMgr::OnDeviceScreenOff()
{
    HILOGI("OnDeviceScreenOff called. accountId: %{public}d.", accountId_);
    auto func = [this]() {
        std::string senderNetworkId;
        std::string bundleName;
        std::string continueType;
        {
            std::lock_guard<std::mutex> currentIconLock(iconMutex_);
            if (iconInfo_.isEmpty()) {
                HILOGW("Saved iconInfo has already been cleared, task abort.");
                return;
            }
            senderNetworkId = iconInfo_.senderNetworkId;
            bundleName = iconInfo_.bundleName;
            continueType = iconInfo_.continueType;
            iconInfo_.senderNetworkId = "";
            iconInfo_.bundleName = "";
            iconInfo_.continueType = "";
        }
        HILOGI("Saved iconInfo cleared, networkId: %{public}s, bundleName: %{public}s.",
            GetAnonymStr(senderNetworkId).c_str(), bundleName.c_str());
        {
            std::lock_guard<std::mutex> registerOnListenerMapLock(eventMutex_);
            auto iterItem = registerOnListener_.find(onType_);
            if (iterItem == registerOnListener_.end()) {
                HILOGI("Get iterItem failed from registerOnListener_, nobody registed");
                return;
            }
            std::vector<sptr<IRemoteObject>> objs = iterItem->second;
            for (auto iter : objs) {
                NotifyRecvBroadcast(iter,
                    currentIconInfo(senderNetworkId, iconInfo_.sourceBundleName, bundleName, continueType),
                    INACTIVE);
            }
        }
    };
    if (eventHandler_ == nullptr) {
        HILOGE("eventHandler_ is nullptr");
        return;
    }
    eventHandler_->PostTask(func);
}
#endif

void DMSContinueRecvMgr::FindToNotifyRecvBroadcast(const std::string& senderNetworkId, const std::string& bundleName,
    const std::string& continueType)
{
    HILOGI("accountId: %{public}d.", accountId_);
    std::lock_guard<std::mutex> registerOnListenerMapLock(eventMutex_);
    auto iterItem = registerOnListener_.find(onType_);
    if (iterItem == registerOnListener_.end()) {
        HILOGI("Get iterItem failed from registerOnListener_, nobody registed");
        return;
    }
    std::vector<sptr<IRemoteObject>> objs = iterItem->second;
    for (auto iter : objs) {
        NotifyRecvBroadcast(iter,
            currentIconInfo(senderNetworkId, iconInfo_.sourceBundleName, bundleName, continueType),
            INACTIVE);
    }
}

void DMSContinueRecvMgr::OnContinueSwitchOff()
{
    HILOGI("accountId: %{public}d.", accountId_);
    auto func = [this]() {
        std::string senderNetworkId;
        std::string bundleName;
        std::string continueType;
        {
            std::lock_guard<std::mutex> currentIconLock(iconMutex_);
            if (iconInfo_.isEmpty()) {
                HILOGW("Saved iconInfo has already been cleared, task abort.");
                return;
            }
            senderNetworkId = iconInfo_.senderNetworkId;
            bundleName = iconInfo_.bundleName;
            continueType = iconInfo_.continueType;
            iconInfo_.senderNetworkId = "";
            iconInfo_.bundleName = "";
            iconInfo_.continueType = "";
        }
        HILOGI("Saved iconInfo cleared, networkId: %{public}s, bundleName: %{public}s.",
            GetAnonymStr(senderNetworkId).c_str(), bundleName.c_str());
        FindToNotifyRecvBroadcast(senderNetworkId, bundleName, continueType);
    };
    if (eventHandler_ == nullptr) {
        HILOGE("eventHandler_ is nullptr");
        return;
    }
    eventHandler_->PostTask(func);
}

void DMSContinueRecvMgr::OnUserSwitch()
{
    HILOGI("OnUserSwitch start. accountId: %{public}d.", accountId_);
    std::string senderNetworkId;
    std::string bundleName;
    std::string continueType;
    {
        std::lock_guard<std::mutex> currentIconLock(iconMutex_);
        if (iconInfo_.isEmpty()) {
            HILOGW("Saved iconInfo has already been cleared, task abort.");
            return;
        }
        senderNetworkId = iconInfo_.senderNetworkId;
        bundleName = iconInfo_.bundleName;
        continueType = iconInfo_.continueType;
        iconInfo_.senderNetworkId = "";
        iconInfo_.bundleName = "";
        iconInfo_.continueType = "";
    }
    HILOGI("Saved iconInfo cleared, networkId: %{public}s, bundleName: %{public}s.",
        GetAnonymStr(senderNetworkId).c_str(), bundleName.c_str());
    FindToNotifyRecvBroadcast(senderNetworkId, bundleName, continueType);
    HILOGI("OnUserSwitch end.");
}

void DMSContinueRecvMgr::NotifyDeviceOffline(const std::string& networkId)
{
    HILOGI("accountId: %{public}d.", accountId_);
    if (networkId.empty()) {
        HILOGE("NotifyDeviceOffline networkId empty");
        return;
    }
    HILOGI("NotifyDeviceOffline begin. networkId: %{public}s.", GetAnonymStr(networkId).c_str());
    std::string localNetworkId;
    if (!DtbschedmgrDeviceInfoStorage::GetInstance().GetLocalDeviceId(localNetworkId)) {
        HILOGE("Get local networkId failed");
        return;
    }
    std::string senderNetworkId;
    std::string bundleName;
    std::string continueType;
    {
        std::lock_guard<std::mutex> currentIconLock(iconMutex_);
        if (networkId != iconInfo_.senderNetworkId && networkId != localNetworkId) {
            HILOGI("NotifyDeviceOffline irrelevant device offline, ignore");
            return;
        }
        senderNetworkId = iconInfo_.senderNetworkId;
        bundleName = iconInfo_.bundleName;
        continueType = iconInfo_.continueType;
        iconInfo_.senderNetworkId = "";
        iconInfo_.bundleName = "";
        iconInfo_.continueType = "";
    }
    HILOGI("Saved iconInfo cleared, networkId: %{public}s, bundleName: %{public}s.",
        GetAnonymStr(senderNetworkId).c_str(), bundleName.c_str());
    FindToNotifyRecvBroadcast(senderNetworkId, bundleName, continueType);
    HILOGI("NotifyDeviceOffline end");
}

void DMSContinueRecvMgr::NotifyPackageRemoved(const std::string& sinkBundleName)
{
    HILOGI("accountId: %{public}d.", accountId_);
    if (sinkBundleName.empty()) {
        HILOGE("NotifyPackageRemoved sinkBundleName empty");
        return;
    }
    if (iconInfo_.bundleName != sinkBundleName) {
        HILOGI("NotifyPackageRemoved current sinkBundleName: %{public}s; removed package: %{public}s.",
            iconInfo_.bundleName.c_str(), sinkBundleName.c_str());
        return;
    }
    HILOGI("NotifyPackageRemoved begin. sinkBundleName: %{public}s.", sinkBundleName.c_str());
    std::string senderNetworkId;
    std::string bundleName;
    std::string continueType;
    {
        std::lock_guard<std::mutex> currentIconLock(iconMutex_);
        senderNetworkId = iconInfo_.senderNetworkId;
        bundleName = iconInfo_.bundleName;
        continueType = iconInfo_.continueType;
        iconInfo_.senderNetworkId = "";
        iconInfo_.bundleName = "";
        iconInfo_.continueType = "";
    }
    HILOGI("Saved iconInfo cleared, sinkBundleName: %{public}s.",  bundleName.c_str());
    FindToNotifyRecvBroadcast(senderNetworkId, bundleName, continueType);
    HILOGI("NotifyPackageRemoved end");
}

std::string DMSContinueRecvMgr::GetContinueType(const std::string& bundleName)
{
    HILOGI("accountId: %{public}d.", accountId_);
    std::lock_guard<std::mutex> currentIconLock(iconMutex_);
    if (iconInfo_.isEmpty()) {
        HILOGW("get continueType failed, Saved iconInfo has already been cleared.");
        return "";
    }
    if (iconInfo_.bundleName != bundleName) {
        HILOGW("BundleName not match, task abort. bundleName: %{public}s, saved bundleName: %{public}s",
            bundleName.c_str(), iconInfo_.bundleName.c_str());
        return "";
    }

    return iconInfo_.continueType;
}
} // namespace DistributedSchedule
} // namespace OHOS

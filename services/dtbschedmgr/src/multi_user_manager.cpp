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
#include "multi_user_manager.h"

#include "adapter/mmi_adapter.h"
#include "datashare_manager.h"
#include "distributed_sched_utils.h"
#include "dtbschedmgr_log.h"
#include "softbus_adapter/softbus_adapter.h"
#include "os_account_manager.h"

namespace OHOS {
namespace DistributedSchedule {
namespace {
    constexpr static int32_t INVALID_ID = 0;
    const std::string TAG = "MultiUserManager";
}

IMPLEMENT_SINGLE_INSTANCE(MultiUserManager);

void MultiUserManager::Init()
{
    HILOGI("Init start.");
    currentUserId_ = GetForegroundUser();
    MMIAdapter::GetInstance().Init();
    SoftbusAdapter::GetInstance().Init();
    CreateNewSendMgr();
    CreateNewRecvMgr();
    std::shared_ptr<SoftbusAdapterListener> missionBroadcastListener =
        std::make_shared<DistributedMissionBroadcastListener>();
    int32_t ret = SoftbusAdapter::GetInstance().RegisterSoftbusEventListener(missionBroadcastListener);
    if (ret != ERR_OK) {
        HILOGE("get RegisterSoftbusEventListener failed, ret: %{public}d", ret);
    }
    HILOGI("Init end.");
}

void MultiUserManager::UnInit()
{
    HILOGI("UnInit start");
    MMIAdapter::GetInstance().UnInit();
    SoftbusAdapter::GetInstance().UnInit();
    HILOGI("UnInit end");
}

void MultiUserManager::OnUserSwitched(int32_t userId)
{
    HILOGI("UserSwitched start");
    MultiUserManager::GetInstance().GetCurrentSendMgr()->SendScreenOffEvent(DMS_UNFOCUSED_TYPE);
    MultiUserManager::GetInstance().GetCurrentSendMgr()->UserSwitchedRemoveMMIListener();
    currentUserId_ = userId;

    AccountSA::OsAccountType type = GetOsAccountType(currentUserId_);
    HILOGI("OnUserSwitched called, currentUserId = %{public}d, type = %{public}d", currentUserId_, type);
    if (type == AccountSA::OsAccountType::PRIVATE) {
        HILOGI("GetOsAccountType : OsAccountType is PRIVATE, type : %{public}d", type);
        DataShareManager::GetInstance().UpdateSwitchStatus(SwitchStatusDependency::GetInstance()
            .CONTINUE_SWITCH_STATUS_KEY, SwitchStatusDependency::GetInstance().CONTINUE_SWITCH_OFF);
    }

    DataShareManager::GetInstance().SetCurrentContinueSwitch(SwitchStatusDependency::GetInstance()
        .IsContinueSwitchOn());
    MultiUserManager::GetInstance().GetCurrentSendMgr()->Init();
    MultiUserManager::GetInstance().GetCurrentRecvMgr()->Init();

    if (!DataShareManager::GetInstance().IsCurrentContinueSwitchOn()) {
        MultiUserManager::GetInstance().GetCurrentRecvMgr()->OnContinueSwitchOff();
        HILOGI("ICurrentContinueSwitch is off, %{public}d", DataShareManager::GetInstance()
            .IsCurrentContinueSwitchOn());
    };
    {
        std::lock_guard<std::mutex> lock(listenerMutex_);
        if (!listenerCache_.empty() && listenerCache_.find(currentUserId_) != listenerCache_.end()) {
            HILOGI("Cache invoke register listener. userId: %{public}d.", currentUserId_);
            auto it = listenerCache_.find(currentUserId_);
            std::map<std::string, sptr<IRemoteObject>> value = it->second;
            if (!value.empty()) {
                std::string type = value.begin()->first;
                sptr<IRemoteObject> obj = value.begin()->second;
                MultiUserManager::GetInstance().GetCurrentRecvMgr()->RegisterOnListener(type, obj);
            }
            listenerCache_.erase(it);
            HILOGI("Cache remove. userId: %{public}d.", currentUserId_);
        }
    }
    HILOGI("UserSwitched end");
}

void MultiUserManager::OnUserRemoved(int32_t userId)
{
    HILOGI("UserRemoved start");
    {
        std::lock_guard<std::mutex> lock(sendMutex_);
        if (!sendMgrMap_.empty()) {
            auto it = sendMgrMap_.find(userId);
            if (it != sendMgrMap_.end()) {
                it->second->UnInit();
                sendMgrMap_.erase(it);
            }
        }
    }
    {
        std::lock_guard<std::mutex> lock(recvMutex_);
        if (!recvMgrMap_.empty()) {
            auto it = recvMgrMap_.find(userId);
            if (it != recvMgrMap_.end()) {
                it->second->UnInit();
                recvMgrMap_.erase(it);
            }
        }
    }
    HILOGI("UserRemoved end");
}

AccountSA::OsAccountType MultiUserManager::GetOsAccountType(int32_t &accountId)
{
    AccountSA::OsAccountType type;
    ErrCode err = AccountSA::OsAccountManager::GetOsAccountType(accountId, type);
    if (err != ERR_OK) {
        HILOGE("GetOsAccountType passing param invalid or return error!, err : %{public}d", err);
    }
    return type;
}

int32_t MultiUserManager::CreateNewSendMgr()
{
    HILOGI("CreateNewSendMgr begin. userId: %{public}d.", currentUserId_);
    {
        std::lock_guard<std::mutex> lock(sendMutex_);
        sendMgrMap_.emplace(currentUserId_, std::make_shared<DMSContinueSendMgr>());
    }
    HILOGI("CreateNewSendMgr end.");
    return ERR_OK;
}

int32_t MultiUserManager::CreateNewRecvMgr()
{
    HILOGI("CreateNewRecvMgr begin. userId: %{public}d.", currentUserId_);
    {
        std::lock_guard<std::mutex> lock(recvMutex_);
        recvMgrMap_.emplace(currentUserId_, std::make_shared<DMSContinueRecvMgr>());
    }
    HILOGI("CreateNewRecvMgr end.");
    return ERR_OK;
}

std::share_ptr<DMSContinueSendMgr> MultiUserManager::GetCurrentSendMgr()
{
    HILOGI("GetCurrentSendMgr. userId: %{public}d.", currentUserId_);
    std::lock_guard<std::mutex> lock(sendMutex_);
    if (sendMgrMap_.empty() || sendMgrMap_.find(currentUserId_) == sendMgrMap_.end()) {
        HILOGI("sendMgr need to create.");
        CreateNewSendMgr();
    }
    auto cur = sendMgrMap_.find(currentUserId_);
    return cur->second;
}

std::share_ptr<DMSContinueRecvMgr> MultiUserManager::GetCurrentRecvMgr()
{
    HILOGI("GetCurrentRecvMgr. userId: %{public}d.", currentUserId_);
    std::lock_guard<std::mutex> lock(recvMutex_);
    if (recvMgrMap_.empty() || recvMgrMap_.find(currentUserId_) == recvMgrMap_.end()) {
        HILOGI("recvMgr need to create.");
        CreateNewRecvMgr();
    }
    auto cur = recvMgrMap_.find(currentUserId_);
    return cur->second;
}

std::share_ptr<DMSContinueSendMgr> MultiUserManager::GetSendMgrByCallingUid(int32_t callingUid)
{
    int32_t userId = -1;
    OHOS::AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(callingUid, userId);
    HILOGI("GetRecvMgrByCallingUid. userId: %{public}d , callingUid: %{public}d.", userId, callingUid);
    std::lock_guard<std::mutex> lock(sendMutex_);
    if (sendMgrMap_.empty() || sendMgrMap_.find(userId) == sendMgrMap_.end()) {
        HILOGI("sendMgr need to create.");
        CreateNewSendMgr();
    }
    auto cur = sendMgrMap_.find(userId);
    return cur->second;
}

std::share_ptr<DMSContinueRecvMgr> MultiUserManager::GetRecvMgrByCallingUid(int32_t callingUid)
{
    int32_t userId = -1;
    OHOS::AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(callingUid, userId);
    HILOGI("GetRecvMgrByCallingUid. userId: %{public}d , callingUid: %{public}d.", userId, callingUid);
    std::lock_guard<std::mutex> lock(recvMutex_);
    if (recvMgrMap_.empty() || recvMgrMap_.find(userId) == recvMgrMap_.end()) {
        HILOGI("recvMgr need to create.");
        CreateNewRecvMgr();
    }
    auto cur = recvMgrMap_.find(userId);
    return cur->second;
}

int32_t MultiUserManager::OnRegisterOnListener(const std::string& type,
    const sptr<IRemoteObject>& obj, int32_t callingUid)
{
    int32_t userId = -1;
    OHOS::AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(callingUid, userId);
    HILOGI("OnRegisterOnListener. userId: %{public}d , callingUid: %{public}d.", userId, callingUid);
    {
        std::lock_guard<std::mutex> lock(listenerMutex_);
        if (!IsUserForeground(userId) || userId != currentUserId_) {
            HILOGW("The current user is not foreground. userId: %{public}d , currentUserId_: %{public}d.",
                userId, currentUserId_);
            std::map<std::string, sptr<IRemoteObject>> param;
            param.emplace(type, obj);
            listenerCache_.emplace(userId, param);
            return DMS_NOT_FOREGROUND_USER;
        }
    }
    return MultiUserManager::GetInstance().GetCurrentRecvMgr()->RegisterOnListener(type, obj);
}

int32_t MultiUserManager::OnRegisterOffListener(const std::string& type,
    const sptr<IRemoteObject>& obj, int32_t callingUid)
{
    int32_t userId = -1;
    OHOS::AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(callingUid, userId);
    HILOGI("OnRegisterOffListener. userId: %{public}d , callingUid: %{public}d.", userId, callingUid);
    {
        std::lock_guard<std::mutex> lock(listenerMutex_);
        if (!IsUserForeground(userId) || userId != currentUserId_) {
            HILOGW("The current user is not foreground. userId: %{public}d , currentUserId_: %{public}d.",
                userId, currentUserId_);
            return DMS_NOT_FOREGROUND_USER;
        }
    }
    return MultiUserManager::GetInstance().GetCurrentRecvMgr()->RegisterOffListener(type, obj);
}

int32_t MultiUserManager::GetForegroundUser()
{
    int32_t accountId = INVALID_ID;
    ErrCode err = AccountSA::OsAccountManager::GetForegroundOsAccountLocalId(accountId);
    if (err != ERR_OK || accountId == INVALID_ID) {
        HILOGE("GetForegroundOsAccountLocalId passing param invalid or return error!, err : %{public}d", err);
        return INVALID_PARAMETERS_ERR;
    }
    HILOGD("GetForegroundOsAccountLocalId accountId is: %{public}d", accountId);
    return accountId;
}

bool MultiUserManager::IsUserForeground(int32_t userId)
{
    bool isForeground = false;
    std::vector<AccountSA::ForegroundOsAccount> accounts;
    ErrCode err = OHOS::AccountManager::GetForegroundOsAccounts(accounts);
    if (err != ERR_OK) {
        HILOGE("GetForegroundOsAccounts passing param inval id or return error!, err : %{public}d", err);
    }
    if (!accounts.empty() && accounts[0].localId == userId) {
        isForeground = true;
    }
    HILOGD("Current account. accounts[0].localId: %{public}d, userId: %{public}d.", accounts[0].localId, userId);
    return isForeground;
}

bool MultiUserManager::IsCallerForeground(int32_t callingUid)
{
    int32_t userId = -1;
    OHOS::AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(callingUid, userId);
    return IsUserForeground(userId);
}
}  // namespace DistributedSchedule
}  // namespace OHOS

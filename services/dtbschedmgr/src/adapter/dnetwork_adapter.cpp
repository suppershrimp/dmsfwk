/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "adapter/dnetwork_adapter.h"

#include <mutex>
#include <thread>
#include <chrono>
#include <unistd.h>
#include "datetime_ex.h"
#include "dtbschedmgr_log.h"

namespace OHOS {
namespace DistributedSchedule {
using namespace std::chrono_literals;

namespace {
constexpr int32_t DEVICE_ID_SIZE = 65;
constexpr int32_t RETRY_REGISTER_CALLBACK_TIMES = 1;
const std::string PKG_NAME = "DBinderBus";

constexpr int32_t NON_ANONYMIZED_LENGTH = 6;
const std::string EMPTY_DEVICE_ID = "";
}

IMPLEMENT_SINGLE_INSTANCE(DnetworkAdapter);

std::shared_ptr<AppExecFwk::EventHandler> DnetworkAdapter::dnetworkHandler_;
std::mutex DnetworkAdapter::listenerSetMutex_;
std::set<std::shared_ptr<DeviceListener>> DnetworkAdapter::listenerSet_;

void DnetworkAdapter::Init()
{
    nodeStateCb_.events = EVENT_NODE_STATE_MASK;
    nodeStateCb_.onNodeOnline = OnNodeOnline;
    nodeStateCb_.onNodeOffline = OnNodeOffline;
    nodeStateCb_.onNodeBasicInfoChanged = OnNodeBasicInfoChanged;

    auto runner = AppExecFwk::EventRunner::Create("dmsDnetworkAdapter");
    dnetworkHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
}

void DnetworkAdapter::OnNodeOnline(NodeBasicInfo* info)
{
    HILOGI("OnNodeOnline netwokId: %{public}s", info->networkId/*AnonymizeDviceId(info->networkId).c_str()*/);
    char uuidOrUdid[DEVICE_ID_SIZE] = {0};
    std::string pkgName = PKG_NAME + "_" + std::to_string(getpid());
    int32_t ret = GetNodeKeyInfo(pkgName.c_str(), info->networkId, NodeDeivceInfoKey::NODE_KEY_UDID,
        reinterpret_cast<uint8_t*>(uuidOrUdid), DEVICE_ID_SIZE);
    HILOGI("OnNodeOnline uuidOrUdid:%{public}s", uuidOrUdid);
    if (info == nullptr) {
        HILOGE("OnNodeOnline invalid parameter");
        return;
    }

    auto onlineNotifyTask = [info = *info]() {
        std::lock_guard<std::mutex> lck(listenerSetMutex_);
        for (auto& listener : listenerSet_) {
            listener->OnDeviceOnline(&info);
        }
    };
    if(!dnetworkHandler_->PostTask(onlineNotifyTask)) {
        HILOGE("OnNodeOnline post task failed");
        return;
    }
}

void DnetworkAdapter::OnNodeOffline(NodeBasicInfo* info)
{
    HILOGI("OnNodeOffline netwokId: %{public}s", AnonymizeDeviceId(info->networkId).c_str());
    if (info == nullptr) {
        HILOGE("OnNodeOffline invalid parameter");
        return;
    }

    auto offlineNotifyTask = [info = *info]() {
        std::lock_guard<std::mutex> lck(listenerSetMutex_);
        for (auto& listener : listenerSet_) {
            listener->OnDeviceOffline(&info);
        }
    };
    if(!dnetworkHandler_->PostTask(offlineNotifyTask)) {
        HILOGE("OnNodeOffline post task failed");
        return;
    }
}

void DnetworkAdapter::OnNodeBasicInfoChanged(NodeBasicInfoType type, NodeBasicInfo* info)
{
}

bool DnetworkAdapter::AddDeviceChangeListener(const std::shared_ptr<DeviceListener>& listener)
{
    HILOGD("AddDeviceChangeListener called");
    if (dnetworkHandler_ == nullptr) {
        HILOGE("DnetworkAdapter handler is null");
        return false;
    }

    {
        std::lock_guard<std::mutex> lck(listenerSetMutex_);
        auto it = listenerSet_.find(listener);
        if (it == listenerSet_.end()) {
            listenerSet_.insert(listener);
        }
        if (listenerSet_.size() > 1) {
            return true;
        }
    }

    auto registerTask = [this]() {
        HILOGD("AddDeviceChangeListener register task...");
        int32_t retryTimes = 0;
        int32_t ret = ERR_OK;
        while (retryTimes++ < RETRY_REGISTER_CALLBACK_TIMES) {
            std::string pkgName = PKG_NAME + "_" + std::to_string(getpid());
            ret = RegNodeDeviceStateCb(pkgName.c_str(), &nodeStateCb_);
            if (ret != ERR_OK) {
                HILOGD("AddDeviceChangeListener ret = %{public}d, retrying...", ret);
                std::this_thread::sleep_for(1s);
                continue;
            }
        }
        GetDeviceList();
        HILOGI("AddDeviceChangeListener %{public}s", (ret == ERR_OK) ? "success" : "timeout");
    };

    if(!dnetworkHandler_->PostTask(registerTask)) {
        HILOGE("AddDeviceChangeListener post task failed");
        return false;
    }
    return true;
}

void DnetworkAdapter::GetDeviceList()
{
    NodeBasicInfo* info = nullptr;
    int32_t numInfos = 0;
    std::string pkgName = PKG_NAME + "_" + std::to_string(getpid());
    int32_t ret = GetAllNodeDeviceInfo(pkgName.c_str(), &info, &numInfos);
    if (ret != ERR_OK) {
        HILOGE("GetAllNodeDeviceInfo error %{public}d", ret);
    }
    HILOGD("GetAllNodeDeviceInfo numInfos = %{public}d", numInfos);
    
    for (int32_t i = 0; i < numInfos; i++) {
        std::string uuid = GetUuidByNodeId(std::string(info[i].networkId));
    }
    
    if (info != nullptr) {
        FreeNodeInfo(info);
    }
    return;
}

void DnetworkAdapter::RemoveDeviceChangeListener(const std::shared_ptr<DeviceListener>& listener)
{
    HILOGD("RemoveDeviceChangeListener called");
    {
        std::lock_guard<std::mutex> autoLock(listenerSetMutex_);
        listenerSet_.erase(listener);
        if (listenerSet_.size() > 0) {
            return;
        }
    }

    int32_t ret = UnregNodeDeviceStateCb(&nodeStateCb_);
    if (ret != ERR_OK) {
        HILOGE("RemoveDeviceChangeListener remove failed");
    }
    HILOGE("RemoveDeviceChangeListener remove ok");
}

std::string DnetworkAdapter::GetLocalBasicInfo()
{
    NodeBasicInfo info;
    std::string pkgName = PKG_NAME + "_" + std::to_string(getpid());
    int32_t ret = GetLocalNodeDeviceInfo(pkgName.c_str(), &info);
    if (ret != ERR_OK) {
        HILOGE("DnetworkAdapter::GetLocalBasicInfo error %{public}d", ret);
        return std::string();
    }
    return std::string(info.networkId);
}

std::string DnetworkAdapter::GetUdidByNodeId(const std::string& nodeId)
{
    return GetUuidOrUdidByNodeId(nodeId, NodeDeivceInfoKey::NODE_KEY_UDID);
}

std::string DnetworkAdapter::GetUuidByNodeId(const std::string& nodeId)
{
    return GetUuidOrUdidByNodeId(nodeId, NodeDeivceInfoKey::NODE_KEY_UUID);
}

std::string DnetworkAdapter::GetUuidOrUdidByNodeId(const std::string& nodeId, NodeDeivceInfoKey keyType)
{
    if (nodeId.empty()) {
        return std::string();
    }

    char uuidOrUdid[DEVICE_ID_SIZE] = {0};
    std::string pkgName = PKG_NAME + "_" + std::to_string(getpid());
    int32_t ret = GetNodeKeyInfo(pkgName.c_str(), nodeId.c_str(), keyType, reinterpret_cast<uint8_t*>(uuidOrUdid),
        DEVICE_ID_SIZE);
    return (ret == ERR_OK) ? std::string(uuidOrUdid) : std::string();
}

std::string DnetworkAdapter::AnonymizeDeviceId(const std::string& deviceId)
{
    if (deviceId.length() < NON_ANONYMIZED_LENGTH) {
        return EMPTY_DEVICE_ID;
    }
    std::string anonDeviceId = deviceId.substr(0, NON_ANONYMIZED_LENGTH);
    anonDeviceId.append("******");
    return anonDeviceId;
}
} // namespace DistributedSchedule
} // namespace OHOS
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

#include "dtbschedmgr_device_info_storage.h"

#include <chrono>
#include <thread>

#include "dtbschedmgr_log.h"
#include "distributed_sched_adapter.h"
#include "ipc_object_proxy.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "distributed_device_node_listener.h"

namespace OHOS {
namespace DistributedSchedule {
using namespace std::chrono_literals;

namespace {
constexpr int32_t CONNECT_SOFTBUS_RETRY_TIMES = 60;
constexpr int32_t RETRY_TIMES = 30;
}

IMPLEMENT_SINGLE_INSTANCE(DtbschedmgrDeviceInfoStorage);

bool DtbschedmgrDeviceInfoStorage::Init()
{
    if (initHandler_ == nullptr) {
        auto deviceInfoStorageRunner = AppExecFwk::EventRunner::Create("DmsDeviceInfoStorageManager");
        initHandler_ = std::make_shared<AppExecFwk::EventHandler>(deviceInfoStorageRunner);
    }
    
    dnetworkDeathRecipient_ = sptr<IRemoteObject::DeathRecipient>(new DnetServiceDeathRecipient());
    auto func = [this]() {
        HILOGI("DtbschedmgrDeviceInfoStorage begin connect softbus");
        for (int32_t retryTimes = 0; retryTimes <= CONNECT_SOFTBUS_RETRY_TIMES; retryTimes++) {
            if (ConnectSoftbus()) {
                return;
            }
            HILOGI("DtbschedmgrDeviceInfoStorage retry connect softbus %{public}d times", retryTimes);
            std::this_thread::sleep_for(1s);
        }
    };
    
    if (!initHandler_->PostTask(func)) {
        HILOGE("DtbschedmgrDeviceInfoStorage postTask failed");
        return false;
    }
    return true;
}

void DtbschedmgrDeviceInfoStorage::Stop()
{

}

bool DtbschedmgrDeviceInfoStorage::ConnectSoftbus()
{
    bool isReady = WaitForDnetworkReady();
    if (!isReady) {
        HILOGE("DtbschedmgrDeviceInfoStorage::ConnectSoftbus wait Dnetwork failed");
        return false;
    }
    if (!InitNodeIdManager(DnetworkAdapter::GetInstance())) {
        HILOGE("DtbschedmgrDeviceInfoStorage::InitNodeIdManager failed");
        return false;
    }
    HILOGI("DtbschedmgrDeviceInfoStorage::ConnectSoftbus success");
    return true;
}

bool DtbschedmgrDeviceInfoStorage::WaitForDnetworkReady()
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        HILOGE("WaitForDnetworkReady failed to get samgr!");
        return false;
    }
    int32_t retryTimeout = RETRY_TIMES;
    do {
        auto dnetwork = samgr->CheckSystemAbility(SOFTBUS_SERVER_SA_ID);
        if (dnetwork != nullptr) {
            IPCObjectProxy* proxy = reinterpret_cast<IPCObjectProxy*>(dnetwork.GetRefPtr());
            // make sure the proxy is not dead
            if (proxy != nullptr && !proxy->IsObjectDead()) {

                dnetwork->AddDeathRecipient(dnetworkDeathRecipient_);
                return true;
            }
        }
        HILOGI("DtbschedmgrDeviceInfoStorage::Waiting for dnentwork service...");
        std::this_thread::sleep_for(1s);
        if (--retryTimeout <= 0) {
            HILOGI("DtbschedmgrDeviceInfoStorage::Waiting for dnentwork service timeout(30)s");
            return false;
        }
    } while (true);
    return false;
}

bool DtbschedmgrDeviceInfoStorage::InitNodeIdManager(DnetworkAdapter& dnetworkAdapter)
{
    if (nodeIdMgrHandler_ == nullptr) {
        auto runner = AppExecFwk::EventRunner::Create("DmsNodeIdManager");
        nodeIdMgrHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    }

    if (deviceNodeListener_ != nullptr) {
        HILOGE("DtbschedmgrDeviceInfoStorage::RemoveDeviceChangeListener first");
        dnetworkAdapter.RemoveDeviceChangeListener(deviceNodeListener_);
    }

    deviceNodeListener_ = std::make_shared<DistributedDeviceNodeListener>();
    if (!dnetworkAdapter.AddDeviceChangeListener(deviceNodeListener_)) {
        HILOGE("DtbschedmgrDeviceInfoStorage::AddDeviceChangeListener failed");
        deviceNodeListener_ = nullptr;
        return false;
    }
    return true;
}

void DtbschedmgrDeviceInfoStorage::DeviceOnlineNotify(const std::shared_ptr<DmsDeviceInfo> devInfo)
{
    if (devInfo == nullptr) {
        return;
    }
    if (nodeIdMgrHandler_ == nullptr) {
        return;
    }
    auto nodeOnline = [this, devInfo]() {
        HILOGI("DtbschedmgrDeviceInfoStorage::DeviceOnlineNotify called");
    };
    if (!nodeIdMgrHandler_->PostTask(nodeOnline)) {
        HILOGE("DtbschedmgrDeviceInfoStorage::DeviceOnlineNotify post task failed");
    }
}

void DtbschedmgrDeviceInfoStorage::DeviceOfflineNotify(const std::string& deviceId)
{
    if (deviceId.empty()) {
        return;
    }
    if (nodeIdMgrHandler_ == nullptr) {
        return;
    }
    auto nodeOffline = [this, deviceId]() {
        HILOGI("DtbschedmgrDeviceInfoStorage::DeviceOfflineNotify called");
        DistributedSchedAdapter::GetInstance().DeviceOffline(deviceId);
    };
    if (!nodeIdMgrHandler_->PostTask(nodeOffline)) {
        HILOGE("DtbschedmgrDeviceInfoStorage::DeviceOfflineNotify post task failed");
    }
}

void DtbschedmgrDeviceInfoStorage::OnDeviceInfoChanged(const std::string& deviceId, DeviceInfoType type)
{
    HILOGI("DtbschedmgrDeviceInfoStorage::OnDeviceInfoChanged called");
}

bool DtbschedmgrDeviceInfoStorage::GetLocalDeviceId(std::string& deviceId)
{
    deviceId = DnetworkAdapter::GetInstance().GetLocalBasicInfo();
    return true;
}

void DnetServiceDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    HILOGE("DnetServiceDeathRecipient::OnRemoteDied dnetwork service died");
    DtbschedmgrDeviceInfoStorage::GetInstance().Init();
}
}
}
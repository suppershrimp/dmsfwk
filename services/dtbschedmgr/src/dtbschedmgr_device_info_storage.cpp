/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "device_manager.h"
#include "distributed_device_node_listener.h"
#include "distributed_sched_service.h"
#include "dtbschedmgr_log.h"
#include "ipc_object_proxy.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

using namespace std;
namespace OHOS {
namespace DistributedSchedule {
using namespace std::chrono_literals;
using namespace DistributedHardware;

namespace {
constexpr int32_t RETRY_TIMES = 30;
constexpr int32_t CONNECT_SOFTBUS_RETRY_TIMES = 60;
const std::string TAG = "DtbschedmgrDeviceInfoStorage";
const std::string PKG_NAME = "DBinderBus_Dms_" + std::to_string(getprocpid());
}

IMPLEMENT_SINGLE_INSTANCE(DtbschedmgrDeviceInfoStorage);

bool DtbschedmgrDeviceInfoStorage::Init()
{
    if (initHandler_ == nullptr) {
        auto deviceInfoStorageRunner = AppExecFwk::EventRunner::Create("DmsDeviceInfoStorageManager");
        initHandler_ = std::make_shared<AppExecFwk::EventHandler>(deviceInfoStorageRunner);
    }

    auto func = [this]() {
        HILOGI("begin connect softbus");
        for (int32_t retryTimes = 0; retryTimes <= CONNECT_SOFTBUS_RETRY_TIMES; retryTimes++) {
            if (ConnectSoftbus()) {
                return;
            }
            HILOGE("retry connect softbus %{public}d times", retryTimes);
            std::this_thread::sleep_for(1s);
        }
        HILOGE("connect softbus 60times * 30s, error!!");
    };
    if (!initHandler_->PostTask(func)) {
        HILOGE("Init handler postTask failed");
        return false;
    }
    return true;
}

bool DtbschedmgrDeviceInfoStorage::ConnectSoftbus()
{
    ClearAllDevices();
    bool isReady = WaitForDnetworkReady();
    if (!isReady) {
        HILOGE("ConnectSoftbus wait Dnetwork failed!");
        return false;
    }
    std::shared_ptr<DnetworkAdapter> dnetworkAdapter = DnetworkAdapter::GetInstance();
    if (dnetworkAdapter == nullptr) {
        HILOGE("DnetworkAdapter::GetInstance() null");
        return false;
    }
    if (!InitNetworkIdManager(dnetworkAdapter)) {
        HILOGE("InitNetworkIdManager failed");
        return false;
    }
    HILOGI("ConnectSoftbus success");
    return true;
}

bool DtbschedmgrDeviceInfoStorage::InitNetworkIdManager(std::shared_ptr<DnetworkAdapter> dnetworkAdapter)
{
    if (networkIdMgrHandler_ == nullptr) {
        auto runner = AppExecFwk::EventRunner::Create("DmsNetworkIdManager");
        networkIdMgrHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    }

    deviceNodeListener_ = std::make_shared<DistributedDeviceNodeListener>();
    if (!dnetworkAdapter->AddDeviceChangeListener(deviceNodeListener_)) {
        deviceNodeListener_ = nullptr;
        HILOGE("AddDeviceChangeListener failed!");
        return false;
    }
    return true;
}

void DtbschedmgrDeviceInfoStorage::Stop()
{
    ClearAllDevices();
    if (deviceNodeListener_ != nullptr) {
        DnetworkAdapter::GetInstance()->RemoveDeviceChangeListener(deviceNodeListener_);
        deviceNodeListener_ = nullptr;
    }
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
        auto dnetwork = samgr->CheckSystemAbility(DISTRIBUTED_HARDWARE_DEVICEMANAGER_SA_ID);
        if (dnetwork != nullptr) {
            IPCObjectProxy* proxy = reinterpret_cast<IPCObjectProxy*>(dnetwork.GetRefPtr());
            // make sure the proxy is not dead
            if (proxy != nullptr && !proxy->IsObjectDead()) {
                return true;
            }
        }
        HILOGI("Waiting for dnentwork service...");
        std::this_thread::sleep_for(1s);
        if (--retryTimeout <= 0) {
            HILOGI("Waiting for dnentwork service timeout(30)s");
            return false;
        }
    } while (true);
    return false;
}

void DtbschedmgrDeviceInfoStorage::RegisterUuidNetworkIdMap(const std::string& networkId)
{
    std::string uuid = DnetworkAdapter::GetInstance()->GetUuidByNetworkId(networkId);
    if (uuid.empty()) {
        HILOGE("GetUuidByNetworkId return an empty uuid!");
        return;
    }
    {
        std::lock_guard<std::mutex> autoLock(uuidNetworkIdLock_);
        uuidNetworkIdMap_[uuid] = networkId;
    }
}

void DtbschedmgrDeviceInfoStorage::UnregisterUuidNetworkIdMap(const std::string& networkId)
{
    std::string uuid = DnetworkAdapter::GetInstance()->GetUuidByNetworkId(networkId);
    if (uuid.empty()) {
        HILOGE("GetUuidByNetworkId return an empty uuid");
        return;
    }
    {
        std::lock_guard<std::mutex> autoLock(uuidNetworkIdLock_);
        uuidNetworkIdMap_.erase(uuid);
    }
}

void DtbschedmgrDeviceInfoStorage::GetDeviceIdSet(std::set<std::string>& deviceIdSet)
{
    deviceIdSet.clear();
    lock_guard<mutex> autoLock(deviceLock_);
    for (const auto& device : remoteDevices_) {
        deviceIdSet.emplace(device.first);
    }
}

bool DtbschedmgrDeviceInfoStorage::UpdateDeviceInfoStorage()
{
    std::vector<DistributedHardware::DmDeviceInfo> dmDeviceInfoList;
    int32_t errCode = DeviceManager::GetInstance().GetTrustedDeviceList(PKG_NAME, "", dmDeviceInfoList);
    if (errCode != ERR_OK) {
        HILOGE("Get device manager trusted device list fail, errCode %{public}d", errCode);
        return false;
    }
    for (const auto& dmDeviceInfo : dmDeviceInfoList) {
        auto deviceInfo = std::make_shared<DmsDeviceInfo>(
            dmDeviceInfo.deviceName, dmDeviceInfo.deviceTypeId, dmDeviceInfo.networkId);
        std::string networkId = deviceInfo->GetNetworkId();
        RegisterUuidNetworkIdMap(networkId);
        {
            HILOGI("Add remote device networkId %{public}s", DnetworkAdapter::AnonymizeNetworkId(networkId).c_str());
            lock_guard<mutex> autoLock(deviceLock_);
            remoteDevices_[networkId] = deviceInfo;
        }
    }
    HILOGI("Update remote devices info storage success.");
    return true;
}

bool DtbschedmgrDeviceInfoStorage::GetLocalDeviceId(std::string& networkId)
{
    return GetLocalDeviceFromDnet(networkId);
}

bool DtbschedmgrDeviceInfoStorage::GetLocalUdid(std::string& udid)
{
    return GetLocalDeviceUdid(udid);
}

bool DtbschedmgrDeviceInfoStorage::GetLocalDeviceFromDnet(std::string& networkId)
{
    auto dnetworkAdapter = DnetworkAdapter::GetInstance();
    if (dnetworkAdapter == nullptr) {
        HILOGE("GetLocalDeviceFromDnet dnetworkAdapter null");
        return false;
    }
    DmDeviceInfo dmDeviceInfo;
    if (!dnetworkAdapter->GetLocalBasicInfo(dmDeviceInfo)) {
        HILOGE("GetLocalBasicInfo error");
        return false;
    }
    networkId = dmDeviceInfo.networkId;
    HILOGI("get local networkId from DnetworkAdapter, networkId = %{public}s",
        DnetworkAdapter::AnonymizeNetworkId(networkId).c_str());
    return true;
}

bool DtbschedmgrDeviceInfoStorage::GetLocalDeviceUdid(std::string& udid)
{
    auto dnetworkAdapter = DnetworkAdapter::GetInstance();
    if (dnetworkAdapter == nullptr) {
        HILOGE("GetLocalDeviceFromDnet dnetworkAdapter null");
        return false;
    }
    DmDeviceInfo dmDeviceInfo;
    if (!dnetworkAdapter->GetLocalBasicInfo(dmDeviceInfo)) {
        HILOGE("GetLocalBasicInfo error");
        return false;
    }
    udid = GetUdidByNetworkId(dmDeviceInfo.networkId);
    HILOGD("GetLocalDeviceUdid = %{public}s", DnetworkAdapter::AnonymizeNetworkId(udid).c_str());
    return true;
}

void DtbschedmgrDeviceInfoStorage::ClearAllDevices()
{
    lock_guard<mutex> autoLock(deviceLock_);
    remoteDevices_.clear();
}

std::shared_ptr<DmsDeviceInfo> DtbschedmgrDeviceInfoStorage::FindDeviceInfoInStorage(const std::string& networkId)
{
    lock_guard<mutex> autoLock(deviceLock_);
    auto iter = remoteDevices_.find(networkId);
    if (iter == remoteDevices_.end()) {
        HILOGE("Get remote device info from storage fail, networkId %{public}s.",
            DnetworkAdapter::AnonymizeNetworkId(networkId).c_str());
        return nullptr;
    }
    HILOGI("Get remote device info from storage success, networkId %{public}s.",
        DnetworkAdapter::AnonymizeNetworkId(networkId).c_str());
    return iter->second;
}

std::shared_ptr<DmsDeviceInfo> DtbschedmgrDeviceInfoStorage::GetDeviceInfoById(const std::string& networkId)
{
    HILOGI("Get device info by networkId %{public}s start.", DnetworkAdapter::AnonymizeNetworkId(networkId).c_str());
    auto devInfo = FindDeviceInfoInStorage(networkId);
    if (devInfo != nullptr) {
        return devInfo;
    }

    HILOGI("NetworkId %{public}s not in storage, update devices info from device manager.",
        DnetworkAdapter::AnonymizeNetworkId(networkId).c_str());
    if (!UpdateDeviceInfoStorage()) {
        HILOGE("Update device info storage from device manager trusted device list fail.");
        return nullptr;
    }

    devInfo = FindDeviceInfoInStorage(networkId);
    return devInfo;
}

std::string DtbschedmgrDeviceInfoStorage::GetUuidByNetworkId(const std::string& networkId)
{
    if (networkId.empty()) {
        HILOGW("GetUuidByNetworkId networkId empty!");
        return "";
    }
    {
        std::lock_guard<std::mutex> autoLock(uuidNetworkIdLock_);
        auto iter = uuidNetworkIdMap_.begin();
        while (iter != uuidNetworkIdMap_.end()) {
            if (iter->second == networkId) {
                return iter->first;
            } else {
                ++iter;
            }
        }
    }
    std::string uuid = DnetworkAdapter::GetInstance()->GetUuidByNetworkId(networkId);
    return uuid;
}

std::string DtbschedmgrDeviceInfoStorage::GetUdidByNetworkId(const std::string& networkId)
{
    if (networkId.empty()) {
        HILOGW("GetUdidByNetworkId networkId empty!");
        return "";
    }
    std::string udid = DnetworkAdapter::GetInstance()->GetUdidByNetworkId(networkId);
    return udid;
}

std::string DtbschedmgrDeviceInfoStorage::GetNetworkIdByUuid(const std::string& uuid)
{
    if (uuid.empty()) {
        HILOGW("GetNetworkIdByUuid uuid empty!");
        return "";
    }
    {
        std::lock_guard<std::mutex> autoLock(uuidNetworkIdLock_);
        auto iter = uuidNetworkIdMap_.find(uuid);
        if (iter != uuidNetworkIdMap_.end()) {
            return iter->second;
        }
        return "";
    }
}
void DtbschedmgrDeviceInfoStorage::DeviceOnlineNotify(const std::shared_ptr<DmsDeviceInfo> devInfo)
{
    if (devInfo == nullptr) {
        HILOGE("DeviceOnlineNotify devInfo null");
        return;
    }

    if (networkIdMgrHandler_ == nullptr) {
        HILOGE("networkIdMgrHandler null");
        return;
    }
    auto nodeOnline = [this, devInfo]() {
        std::string networkId = devInfo->GetNetworkId();
        RegisterUuidNetworkIdMap(networkId);
        std::string uuid = GetUuidByNetworkId(networkId);
        HILOGI("networkId = %{public}s, uuid = %{public}s, deviceName = %{public}s",
            DnetworkAdapter::AnonymizeNetworkId(networkId).c_str(),
            DnetworkAdapter::AnonymizeNetworkId(uuid).c_str(), devInfo->GetDeviceName().c_str());
        {
            lock_guard<mutex> autoLock(deviceLock_);
            remoteDevices_[networkId] = devInfo;
        }
        DistributedSchedService::GetInstance().DeviceOnlineNotify(networkId);
    };
    if (!networkIdMgrHandler_->PostTask(nodeOnline)) {
        HILOGE("DeviceOnlineNotify handler postTask failed");
    }
}

void DtbschedmgrDeviceInfoStorage::DeviceOfflineNotify(const std::string& networkId)
{
    if (networkId.empty()) {
        HILOGE("DeviceOfflineNotify networkId empty");
        return;
    }
    HILOGD("DeviceOfflineNotify networkId = %{public}s",
        DnetworkAdapter::AnonymizeNetworkId(networkId).c_str());
    if (networkIdMgrHandler_ == nullptr) {
        HILOGE("DeviceOfflineNotify networkIdMgrHandler null");
        return;
    }
    auto nodeOffline = [this, networkId]() {
        std::string uuid = GetUuidByNetworkId(networkId);
        HILOGI("DeviceOfflineNotify process networkId = %{public}s, uuid = %{public}s",
            DnetworkAdapter::AnonymizeNetworkId(networkId).c_str(), DnetworkAdapter::AnonymizeNetworkId(uuid).c_str());
        DistributedSchedService::GetInstance().DeviceOfflineNotify(networkId);
        UnregisterUuidNetworkIdMap(networkId);
        lock_guard<mutex> autoLock(deviceLock_);
        remoteDevices_.erase(networkId);
    };
    if (!networkIdMgrHandler_->PostTask(nodeOffline)) {
        HILOGE("DeviceOfflineNotify handler postTask failed");
    }
}

void DtbschedmgrDeviceInfoStorage::OnDeviceInfoChanged(const std::string& deviceId)
{
    HILOGI("OnDeviceInfoChanged called");
}

void DnetServiceDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    HILOGI("OnRemoteDied dnetwork service died");
    DtbschedmgrDeviceInfoStorage::GetInstance().Init();
}

std::string DtbschedmgrDeviceInfoStorage::GetDeviceName(std::string netWorkId)
{
    for (auto device = remoteDevices_.begin(); device != remoteDevices_.end(); ++device) {
        if (device->second->GetNetworkId() == netWorkId) {
            HILOGI("deviceName = %{public}s", device->second->GetDeviceName().c_str());
            return device->second->GetDeviceName();
        }
    }
    return "";
}

std::vector<std::string> DtbschedmgrDeviceInfoStorage::GetNetworkIdList()
{
    std::vector<std::string> devices;
    for (auto device = remoteDevices_.begin(); device != remoteDevices_.end(); ++device) {
        HILOGI("NetworkId: %{public}s", DnetworkAdapter::AnonymizeNetworkId(device->second->GetNetworkId()).c_str());
        devices.push_back(device->second->GetNetworkId());
    }
    return devices;
}
}
}

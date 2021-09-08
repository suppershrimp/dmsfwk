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

#ifndef OHOS_DISTRIBUTED_DTBSCHEDMGR_DEVICE_INFO_INTERFACE_H
#define OHOS_DISTRIBUTED_DTBSCHEDMGR_DEVICE_INFO_INTERFACE_H

#include "deviceManager/dms_device_info.h"
#include "adapter/dnetwork_adapter.h"
#include "single_instance.h"
#include "iremote_object.h"
#include "event_handler.h"
#include "distributed_device_node_listener.h"

namespace OHOS {
namespace DistributedSchedule {

class DnetServiceDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    DnetServiceDeathRecipient() = default;
    ~DnetServiceDeathRecipient() override = default;
    void OnRemoteDied(const wptr<IRemoteObject>& remote) override;
};

class DtbschedmgrDeviceInfoStorage {
    DECLARE_SINGLE_INSTANCE(DtbschedmgrDeviceInfoStorage);

public:
    bool Init();
    void Stop();

    /**
     * GetLocalDeviceId get local device id
     * 
     * @param deviceId
     */
    bool GetLocalDeviceId(std::string& deviceId);

    /**
     * DeviceOnlineNotify callback for new node adding to network
     * 
     * @param devInfo
     */
    void DeviceOnlineNotify(const std::shared_ptr<DmsDeviceInfo> devInfo);

    /**
     * DeviceOfflineNotify callback for new node leaving to network
     * 
     * @param deviceId
     */
    void DeviceOfflineNotify(const std::string& deviceId);

    /**
     * OnDeviceInfoChanged callback for device info has changed
     * 
     * @param deviceId
     * @param type
     */
    void OnDeviceInfoChanged(const std::string& deviceId, DeviceInfoType type);

private:
    bool ConnectSoftbus();
    bool WaitForDnetworkReady();
    bool InitNodeIdManager(DnetworkAdapter& dnetworkAdapter);
private:
    std::shared_ptr<AppExecFwk::EventHandler> initHandler_;
    std::shared_ptr<AppExecFwk::EventHandler> nodeIdMgrHandler_;
    std::shared_ptr<DistributedDeviceNodeListener> deviceNodeListener_;
    sptr<IRemoteObject::DeathRecipient> dnetworkDeathRecipient_;
};
} // namespace DistributedSchedule
} // namespace OHOS
#endif // OHOS_DISTRIBUTED_DTBSCHEDMGR_DEVICE_INFO_INTERFACE_H
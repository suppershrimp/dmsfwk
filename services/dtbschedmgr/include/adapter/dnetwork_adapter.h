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

#ifndef OHOS_DISTRIBUTED_DNETWORK_ADAPTER_H
#define OHOS_DISTRIBUTED_DNETWORK_ADAPTER_H

#include <string>
#include <set>

#include "single_instance.h"
#include "softbus_bus_center.h"
#include "event_handler.h"

namespace OHOS {
namespace DistributedSchedule {
enum DeviceInfoType {
    UNKNOWN_INFO = 0,
    BASIC_INFO = 1,
    NETWORK_INFO = 2,
    TRUST_INFO = 3,
};

class DeviceListener {
public:
    DeviceListener() = default;
    virtual ~DeviceListener() = default;

    virtual void OnDeviceOnline(const NodeBasicInfo* nodeBasicInfo) = 0;
    virtual void OnDeviceOffline(const NodeBasicInfo* nodeBasicInfo) = 0;
    virtual void OnDeviceInfoChanged(const std::string& nodeId, DeviceInfoType type) = 0;
};

class DnetworkAdapter {
    DECLARE_SINGLE_INSTANCE_BASE(DnetworkAdapter);

public:
    DnetworkAdapter() = default;
    ~DnetworkAdapter() = default;

    bool AddDeviceChangeListener(const std::shared_ptr<DeviceListener>& listener);
    void RemoveDeviceChangeListener(const std::shared_ptr<DeviceListener>& listener);

    void GetDeviceList();
    std::string GetUdidByNodeId(const std::string& nodeId);
    std::string GetUuidByNodeId(const std::string& nodeId);
    std::string GetLocalBasicInfo();
    void Init();

    static std::string AnonymizeDeviceId(const std::string& deviceId);

private:
    std::string GetUuidOrUdidByNodeId(const std::string& nodeId, NodeDeivceInfoKey keyType);

    static void OnNodeOnline(NodeBasicInfo* info);
    static void OnNodeOffline(NodeBasicInfo* info);
    static void OnNodeBasicInfoChanged(NodeBasicInfoType type, NodeBasicInfo* info);

private:
    INodeStateCb nodeStateCb_;
    static std::shared_ptr<AppExecFwk::EventHandler> dnetworkHandler_;
    static std::mutex listenerSetMutex_;
    static std::set<std::shared_ptr<DeviceListener>> listenerSet_;
};
} // namespace DistributedSchedule
} // namespace OHOS
#endif // OHOS_DISTRIBUTED_DNETWORK_ADAPTER_H
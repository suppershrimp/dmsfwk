/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_DISTRIBUTED_SCHED_DEVICE_SELECTION_NOTIFIER_PROXY_H
#define OHOS_DISTRIBUTED_SCHED_DEVICE_SELECTION_NOTIFIER_PROXY_H

#include "idevice_selection_notifier.h"

#include "iremote_object.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace DistributedSchedule {

class DeviceSelectionNotifierProxy : public IRemoteProxy<AAFwk::IDeviceSelectionNotifier> {
public:
    explicit DeviceSelectionNotifierProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IDeviceSelectionNotifier>(impl) {}
    virtual ~DeviceSelectionNotifierProxy() = default;

    void OnDeviceConnect(const std::vector<AAFwk::ContinuationResult>& continuationResults) override;
    void OnDeviceDisconnect(const std::vector<std::string>& deviceIds) override;
private:
    static inline BrokerDelegator<DeviceSelectionNotifierProxy> delegator_;
};
}  // namespace DistributedSchedule
}  // namespace OHOS
#endif  // OHOS_DISTRIBUTED_SCHED_DEVICE_SELECTION_NOTIFIER_PROXY_H

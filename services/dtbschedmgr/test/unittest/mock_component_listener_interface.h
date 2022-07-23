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

#ifndef OHOS_DMS_MOCK_COMPONENT_LISTENER_INTERFACE_H
#define OHOS_DMS_MOCK_COMPONENT_LISTENER_INTERFACE_H

#include "iremote_broker.h"

namespace OHOS {
namespace DistributedSchedule {
/**
 * @class IMockComponentListener
 * IAbilityConnection is used to notify caller that component change.
 */
class IMockComponentListener : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rms.DistributedComponent");

    enum {
        // ipc id for OnComponentChange
        ON_COMPONENT_CHANGE = 1,
    };
};
}  // namespace DistributedSchedule
}  // namespace OHOS
#endif  // OHOS_DMS_MOCK_COMPONENT_LISTENER_INTERFACE_H

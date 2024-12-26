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

#ifndef OHOS_DISTRIBUTED_ABILITY_CONNECTION_MANAGER_INTERFACE_H
#define OHOS_DISTRIBUTED_ABILITY_CONNECTION_MANAGER_INTERFACE_H

#include "iremote_broker.h"

namespace OHOS {
namespace DistributedCollab {
class IAbilityConnectionManager : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.DistributedCollab.IAbilityConnectionManager");
    virtual ~IAbilityConnectionManager() {};

    virtual int32_t NotifyCollabResult(int32_t sessionId, int32_t result, const std::string& peerSocketName) = 0;
    virtual int32_t NotifyDisconnect(int32_t sessionId) = 0;

    enum class Message {
        // ipc id for DSchedEventCallback
        NOTIFY_COLLAB_RESULT = 0,
        NOTIFY_DIS_CONNECT = 1,
    };
};
} // namespace DistributedCollab
} // namespace OHOS
#endif // OHOS_DISTRIBUTED_ABILITY_CONNECTION_MANAGER_INTERFACE_H
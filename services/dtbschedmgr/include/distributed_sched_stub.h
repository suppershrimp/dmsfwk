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

#ifndef HARMONYOS_DISTRIBUTED_SCHED_STUB_H
#define HARMONYOS_DISTRIBUTED_SCHED_STUB_H

#include "distributed_sched_interface.h"
#include "iremote_stub.h"

namespace OHOS {
namespace DistributedSchedule {
class DistributedSchedStub : public IRemoteStub<IDistributedSched> {
public:
    DistributedSchedStub();
    ~DistributedSchedStub();
    int32_t OnRemoteRequest(uint32_t code,
        MessageParcel& data, MessageParcel& reply, MessageOption &option) override;
private:
    bool EnforceInterfaceToken(MessageParcel& data);
    int32_t StartRemoteAbilityInner(MessageParcel& data, MessageParcel& reply);
    int32_t StartAbilityFromRemoteInner(MessageParcel& data, MessageParcel& reply);
    using DistributedSchedFunc = int32_t(DistributedSchedStub::*)(MessageParcel& data, MessageParcel& reply);
    std::map<uint32_t, DistributedSchedFunc> memberFuncMap_;
    std::map<uint32_t, DistributedSchedFunc> localMemberFuncMap_;
};
} // namespace DistributedSchedule
} // namespace OHOS

#endif // OHOS_DISTRIBUTED_SCHEDULE_STUB_H


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

#include "mock_component_listener_stub.h"

#include "ipc_types.h"
#include "message_parcel.h"

namespace OHOS {
namespace DistributedSchedule {
MockComponentListenerStub::MockComponentListenerStub()
{}

MockComponentListenerStub::~MockComponentListenerStub()
{}

int MockComponentListenerStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string descriptor = MockComponentListenerStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        return ERR_INVALID_STATE;
    }
    switch (code) {
        case IMockComponentListener::ON_COMPONENT_CHANGE: {
            return NO_ERROR;
        }
        default: {
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
}
}  // namespace DistributedSchedule
}  // namespace OHOS

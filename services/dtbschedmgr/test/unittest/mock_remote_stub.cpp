/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "mock_remote_stub.h"

namespace OHOS {
namespace DistributedSchedule {
namespace {
constexpr int32_t CODE_NOTIFY_MISSION = 0;
constexpr int32_t NOTIFY_SNAP_SHOT = 1;
constexpr int32_t NOTIFY_NET_DISCONNECT = 2;
const std::string MOCK_DEVICEID_TO_RETURN_FALSE = "deviceId to return false";
}

int32_t MockRemoteStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    if (code == CODE_NOTIFY_MISSION || code == NOTIFY_SNAP_SHOT || code == NOTIFY_NET_DISCONNECT) {
        data.ReadInterfaceToken();
        std::string deviceId = data.ReadString();
        if (deviceId == MOCK_DEVICEID_TO_RETURN_FALSE) {
            return -1;
        }
    }
    return 0;
}
} // namespace DistributedSchedule
} // namespace OHOS
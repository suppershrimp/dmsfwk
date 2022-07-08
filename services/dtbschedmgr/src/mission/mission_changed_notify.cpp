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

#include "mission/mission_changed_notify.h"

#include "datetime_ex.h"
#include "parcel_helper.h"
#include "string_ex.h"

namespace OHOS {
namespace DistributedSchedule {
namespace {
constexpr int32_t CODE_NOTIFY_MISSION = 0;
constexpr int32_t NOTIFY_SNAP_SHOT = 1;
constexpr int32_t NOTIFY_NET_DISCONNECT = 2;

const std::u16string DESCRIPTOR = u"ohos.aafwk.RemoteMissionListener";
const std::string TAG = "MissionChangedNotify";
}
void MissionChangedNotify::NotifyMissionsChanged(const sptr<IRemoteObject>& remoteObject,
    const std::u16string& deviceId)
{
    if (remoteObject == nullptr) {
        HILOGE("NotifyMissionsChanged remote service is null!");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(DESCRIPTOR)) {
        HILOGE("NotifyMissionsChanged write interface token failed!");
        return;
    }
    PARCEL_WRITE_HELPER_NORET(data, String, Str16ToStr8(deviceId));
    HILOGI("[PerformanceTest] NotifyMissionsChanged called, IPC begin = %{public}" PRId64, GetTickCount());
    int32_t error = remoteObject->SendRequest(CODE_NOTIFY_MISSION, data, reply, option);
    if (error != ERR_NONE) {
        HILOGE("NotifyMissionsChanged fail, error: %{public}d", error);
        return;
    }
    HILOGI("NotifyMissionsChanged finished!");
}

void MissionChangedNotify::NotifySnapshot(const sptr<IRemoteObject>& remoteObject, const std::u16string& deviceId,
    int32_t missionId)
{
    if (remoteObject == nullptr) {
        HILOGE("NotifySnapshot remote service is null!");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(DESCRIPTOR)) {
        HILOGE("NotifySnapshot write interface token failed!");
        return;
    }
    PARCEL_WRITE_HELPER_NORET(data, String, Str16ToStr8(deviceId));
    PARCEL_WRITE_HELPER_NORET(data, Int32, missionId);
    HILOGI("[PerformanceTest] NotifySnapshot called, IPC begin = %{public}" PRId64, GetTickCount());
    int32_t error = remoteObject->SendRequest(NOTIFY_SNAP_SHOT, data, reply, option);
    if (error != ERR_NONE) {
        HILOGE("NotifySnapshot fail, error: %{public}d", error);
        return;
    }
    HILOGI("NotifySnapshot finished!");
}

void MissionChangedNotify::NotifyNetDisconnect(const sptr<IRemoteObject>& remoteObject, const std::u16string& deviceId,
    int32_t state)
{
    if (remoteObject == nullptr) {
        HILOGE("NotifyNetDisconnect remote service is null!");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(DESCRIPTOR)) {
        HILOGE("NotifyNetDisconnect write interface token failed!");
        return;
    }
    PARCEL_WRITE_HELPER_NORET(data, String, Str16ToStr8(deviceId));
    PARCEL_WRITE_HELPER_NORET(data, Int32, state);
    HILOGI("[PerformanceTest] NotifyNetDisconnect called, IPC begin = %{public}" PRId64, GetTickCount());
    int32_t error = remoteObject->SendRequest(NOTIFY_NET_DISCONNECT, data, reply, option);
    if (error != ERR_NONE) {
        HILOGE("NotifyNetDisconnect fail, error: %{public}d", error);
        return;
    }
    HILOGI("NotifyNetDisconnect finished!");
}
} // namespace DistributedSchedule
} // namespace OHOS

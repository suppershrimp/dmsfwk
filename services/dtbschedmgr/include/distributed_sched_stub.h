/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_DISTRIBUTED_SCHED_STUB_H
#define OHOS_DISTRIBUTED_SCHED_STUB_H

#include "distributed_sched_interface.h"
#include "iremote_stub.h"
#ifdef SUPPORT_DISTRIBUTED_MISSION_MANAGER
#include "mission/snapshot.h"
#endif

namespace OHOS {
namespace DistributedSchedule {
class DistributedSchedStub : public IRemoteStub<IDistributedSched> {
public:
    DistributedSchedStub();
    ~DistributedSchedStub();
    int32_t OnRemoteRequest(uint32_t code, MessageParcel& data,
        MessageParcel& reply, MessageOption& option) override;

private:
    int32_t StartRemoteAbilityInner(MessageParcel& data, MessageParcel& reply);
    int32_t StartAbilityFromRemoteInner(MessageParcel& data, MessageParcel& reply);
    int32_t SendResultFromRemoteInner(MessageParcel& data, MessageParcel& reply);
    int32_t ContinueMissionInner(MessageParcel& data, MessageParcel& reply);
    int32_t ContinueMissionOfBundleNameInner(MessageParcel& data, MessageParcel& reply);
    int32_t StartContinuationInner(MessageParcel& data, MessageParcel& reply);
    int32_t NotifyCompleteContinuationInner(MessageParcel& data, MessageParcel& reply);
    int32_t NotifyContinuationResultFromRemoteInner(MessageParcel& data, MessageParcel& reply);
    int32_t NotifyDSchedEventResultFromRemoteInner(MessageParcel& data, MessageParcel& reply);
    int32_t ConnectRemoteAbilityInner(MessageParcel& data, MessageParcel& reply);
    int32_t DisconnectRemoteAbilityInner(MessageParcel& data, MessageParcel& reply);
    int32_t ConnectAbilityFromRemoteInner(MessageParcel& data, MessageParcel& reply);
    int32_t ReadDataForConnect(MessageParcel& data, CallerInfo& callerInfo, AccountInfo& accountInfo);
    int32_t DisconnectAbilityFromRemoteInner(MessageParcel& data, MessageParcel& reply);
    int32_t NotifyProcessDiedFromRemoteInner(MessageParcel& data, MessageParcel& reply);
#ifdef SUPPORT_DISTRIBUTED_MISSION_MANAGER
    int32_t GetMissionInfosInner(MessageParcel& data, MessageParcel& reply);
    int32_t GetRemoteMissionSnapshotInfoInner(MessageParcel& data, MessageParcel& reply);
    int32_t RegisterMissionListenerInner(MessageParcel& data, MessageParcel& reply);
    int32_t RegisterDSchedEventListenerInner(MessageParcel& data, MessageParcel& reply);
    int32_t UnRegisterDSchedEventListenerInner(MessageParcel& data, MessageParcel& reply);
    int32_t GetContinueInfoInner(MessageParcel& data, MessageParcel& reply);
    int32_t RegisterOnListenerInner(MessageParcel& data, MessageParcel& reply);
    int32_t RegisterOffListenerInner(MessageParcel& data, MessageParcel& reply);
    int32_t UnRegisterMissionListenerInner(MessageParcel& data, MessageParcel& reply);
    int32_t StartSyncRemoteMissionsInner(MessageParcel& data, MessageParcel& reply);
    int32_t StartSyncMissionsFromRemoteInner(MessageParcel& data, MessageParcel& reply);
    int32_t StopSyncRemoteMissionsInner(MessageParcel& data, MessageParcel& reply);
    int32_t StopSyncMissionsFromRemoteInner(MessageParcel& data, MessageParcel& reply);
    int32_t NotifyMissionsChangedFromRemoteInner(MessageParcel& data, MessageParcel& reply);
    int32_t SetMissionContinueStateInner(MessageParcel& data, MessageParcel& reply);
#endif
    int32_t StartRemoteAbilityByCallInner(MessageParcel& data, MessageParcel& reply);
    int32_t ReleaseRemoteAbilityInner(MessageParcel& data, MessageParcel& reply);
    int32_t StartAbilityByCallFromRemoteInner(MessageParcel& data, MessageParcel& reply);
    int32_t ReleaseAbilityFromRemoteInner(MessageParcel& data, MessageParcel& reply);
    int32_t StartRemoteFreeInstallInner(MessageParcel& data, MessageParcel& reply);
    int32_t CreateJsonObject(std::string& extraInfo, CallerInfo& callerInfo);
    int32_t StartFreeInstallFromRemoteInner(MessageParcel& data, MessageParcel& reply);
    int32_t ReadDataForFreeInstall(MessageParcel& data,
        CallerInfo &callerInfo, AccountInfo &accountInfo, int64_t &taskId);
    int32_t NotifyCompleteFreeInstallFromRemoteInner(MessageParcel& data, MessageParcel& reply);
#ifdef SUPPORT_DISTRIBUTED_FORM_SHARE
    int32_t StartRemoteShareFormInner(MessageParcel& data, MessageParcel& reply);
    int32_t StartShareFormFromRemoteInner(MessageParcel& data, MessageParcel& reply);
#endif
    int32_t NotifyStateChangedFromRemoteInner(MessageParcel& data, MessageParcel& reply);
    int32_t GetDistributedComponentListInner(MessageParcel& data, MessageParcel& reply);
    void ReportEvent(const OHOS::AAFwk::Want& want, const std::string& eventName, int32_t result, int32_t callerUid);
    bool CheckDmsRequestPermission();
    bool CheckCallingUid();
    bool EnforceInterfaceToken(MessageParcel& data);
    bool CallerInfoUnmarshalling(CallerInfo& callerInfo, MessageParcel& data);
    bool IsUsingQos(const std::string& remoteDeviceId);
    void SaveExtraInfo(const nlohmann::json& extraInfoJson, CallerInfo& callerInfo);
    void InitExtendedLocalFuncsInner();
    void InitLocalFuncsInner();
    void InitLocalMissionManagerInner();
    void InitRemoteFuncsInner();
    std::shared_ptr<AAFwk::Want> ReadDistributedWant(MessageParcel& data);

    int32_t StopRemoteExtensionAbilityInner(MessageParcel& data, MessageParcel& reply);
    int32_t StopExtensionAbilityFromRemoteInner(MessageParcel& data, MessageParcel& reply);

    using DistributedSchedFunc = int32_t(DistributedSchedStub::*)(MessageParcel& data, MessageParcel& reply);
    std::map<uint32_t, DistributedSchedFunc> remoteFuncsMap_;
    std::map<uint32_t, DistributedSchedFunc> localFuncsMap_;
};
} // namespace DistributedSchedule
} // namespace OHOS
#endif // OHOS_DISTRIBUTED_SCHEDULE_STUB_H

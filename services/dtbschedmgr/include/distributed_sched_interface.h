/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_DISTRIBUTED_SCHED_INTERFACE_H
#define OHOS_DISTRIBUTED_SCHED_INTERFACE_H

#include <vector>
#include "ability_info.h"
#include "ability_manager_interface.h"
#include "caller_info.h"
#include "distributedsched_ipc_interface_code.h"
#ifdef SUPPORT_DISTRIBUTED_FORM_SHARE
#include "form_share_info.h"
#endif
#include "iremote_broker.h"
#ifdef SUPPORT_DISTRIBUTED_MISSION_MANAGER
#include "mission_info.h"
#include "mission/distributed_mission_info.h"
#include "mission_snapshot.h"
#endif
#include "want.h"

namespace OHOS {
namespace DistributedSchedule {
class IDistributedSched : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.DistributedSchedule.IDistributedSched");
    enum {
        SAME_ACCOUNT_TYPE = 0,
        DIFF_ACCOUNT_TYPE,
    };
    struct AccountInfo {
        int32_t accountType = DIFF_ACCOUNT_TYPE;
        std::vector<std::string> groupIdList;
    };
    enum {
        CALLER = 0,
        CALLEE,
    };
    enum {
        CONNECT = 0,
        CALL,
    };

    struct FreeInstallInfo {
        OHOS::AAFwk::Want want;
        int32_t requestCode = OHOS::AAFwk::DEFAULT_INVAL_VALUE;
        CallerInfo callerInfo = {};
        AccountInfo accountInfo = {};
    };

    virtual int32_t StartRemoteAbility(const OHOS::AAFwk::Want& want, int32_t callerUid, int32_t requestCode,
        uint32_t accessToken) = 0;
    virtual int32_t StartAbilityFromRemote(const OHOS::AAFwk::Want& want,
        const OHOS::AppExecFwk::AbilityInfo& abilityInfo, int32_t requestCode, const CallerInfo& callerInfo,
        const AccountInfo& accountInfo) = 0;
    virtual int32_t SendResultFromRemote(OHOS::AAFwk::Want& want, int32_t requestCode,
        const CallerInfo& callerInfo, const AccountInfo& accountInfo, int32_t resultCode) = 0;
    virtual int32_t ContinueMission(const std::string& srcDeviceId, const std::string& dstDeviceId,
        int32_t missionId, const sptr<IRemoteObject>& callback, const OHOS::AAFwk::WantParams& wantParams) = 0;
    virtual int32_t ContinueMission(const std::string& srcDeviceId, const std::string& dstDeviceId,
        const std::string& bundleName, const sptr<IRemoteObject>& callback, const OHOS::AAFwk::WantParams& wantParams)
    {
        return 0;
    }
    virtual int32_t StartContinuation(const OHOS::AAFwk::Want& want, int32_t missionId, int32_t callerUid,
        int32_t status, uint32_t accessToken) = 0;
    virtual void NotifyCompleteContinuation(const std::u16string& devId, int32_t sessionId, bool isSuccess) = 0;
    virtual int32_t NotifyContinuationResultFromRemote(int32_t sessionId, bool isSuccess,
        const std::string dstInfo) = 0;
    virtual int32_t NotifyDSchedEventResultFromRemote(const std::string type, int32_t dSchedEventResult)
    {
        return 0;
    }
    virtual int32_t ConnectRemoteAbility(const OHOS::AAFwk::Want& want, const sptr<IRemoteObject>& connect,
        int32_t callerUid, int32_t callerPid, uint32_t accessToken) = 0;
    virtual int32_t DisconnectRemoteAbility(const sptr<IRemoteObject>& connect, int32_t callerUid,
        uint32_t accessToken) = 0;
    virtual int32_t ConnectAbilityFromRemote(const OHOS::AAFwk::Want& want, const AppExecFwk::AbilityInfo& abilityInfo,
        const sptr<IRemoteObject>& connect, const CallerInfo& callerInfo, const AccountInfo& accountInfo) = 0;
    virtual int32_t DisconnectAbilityFromRemote(const sptr<IRemoteObject>& connect,
        int32_t uid, const std::string& sourceDeviceId) = 0;
    virtual int32_t NotifyProcessDiedFromRemote(const CallerInfo& callerInfo) = 0;
#ifdef SUPPORT_DISTRIBUTED_MISSION_MANAGER
    virtual int32_t StartSyncRemoteMissions(const std::string& devId, bool fixConflict, int64_t tag) = 0;
    virtual int32_t StartSyncMissionsFromRemote(const CallerInfo& callerInfo,
        std::vector<DstbMissionInfo>& missionInfos) = 0;
    virtual int32_t StopSyncRemoteMissions(const std::string& devId) = 0;
    virtual int32_t StopSyncMissionsFromRemote(const CallerInfo& callerInfo) = 0;
    virtual int32_t RegisterMissionListener(const std::u16string& devId, const sptr<IRemoteObject>& obj) = 0;
    virtual int32_t RegisterDSchedEventListener(const std::string& type, const sptr<IRemoteObject>& obj)
    {
        return 0;
    }
    virtual int32_t UnRegisterDSchedEventListener(const std::string& type, const sptr<IRemoteObject>& obj)
    {
        return 0;
    }
    virtual int32_t GetContinueInfo(std::string& dstNetworkId, std::string& srcNetworkId)
    {
        return 0;
    }
    virtual int32_t RegisterOnListener(const std::string& type, const sptr<IRemoteObject>& obj)
    {
        return 0;
    }
    virtual int32_t RegisterOffListener(const std::string& type, const sptr<IRemoteObject>& obj)
    {
        return 0;
    }
    virtual int32_t UnRegisterMissionListener(const std::u16string& devId, const sptr<IRemoteObject>& obj) = 0;
    virtual int32_t GetMissionInfos(const std::string& deviceId, int32_t numMissions,
        std::vector<AAFwk::MissionInfo>& missionInfos) = 0;
    virtual int32_t GetRemoteMissionSnapshotInfo(const std::string& networkId, int32_t missionId,
        std::unique_ptr<AAFwk::MissionSnapshot>& missionSnapshot) = 0;
    virtual int32_t NotifyMissionsChangedFromRemote(const std::vector<DstbMissionInfo>& missionInfos,
         const CallerInfo& callerInfo) = 0;
    virtual int32_t SetMissionContinueState(int32_t missionId, const AAFwk::ContinueState &state)
    {
        return 0;
    }
#endif
    virtual int32_t StartRemoteAbilityByCall(const OHOS::AAFwk::Want& want, const sptr<IRemoteObject>& connect,
        int32_t callerUid, int32_t callerPid, uint32_t accessToken) = 0;
    virtual int32_t ReleaseRemoteAbility(const sptr<IRemoteObject>& connect,
        const AppExecFwk::ElementName &element) = 0;
    virtual int32_t StartAbilityByCallFromRemote(const OHOS::AAFwk::Want& want, const sptr<IRemoteObject>& connect,
        const CallerInfo& callerInfo, const AccountInfo& accountInfo) = 0;
    virtual int32_t ReleaseAbilityFromRemote(const sptr<IRemoteObject>& connect, const AppExecFwk::ElementName &element,
        const CallerInfo& callerInfo) = 0;
    virtual int32_t StartRemoteFreeInstall(const OHOS::AAFwk::Want& want, int32_t callerUid, int32_t requestCode,
        uint32_t accessToken, const sptr<IRemoteObject>& callback)
    {
        return 0;
    }
    virtual int32_t StartFreeInstallFromRemote(const FreeInstallInfo& info, int64_t taskId)
    {
        return 0;
    }
    virtual int32_t NotifyCompleteFreeInstallFromRemote(int64_t taskId, int32_t resultCode)
    {
        return 0;
    }
#ifdef SUPPORT_DISTRIBUTED_FORM_SHARE
    virtual int32_t StartRemoteShareForm(const std::string& remoteDeviceId,
        const OHOS::AppExecFwk::FormShareInfo& formShareInfo) = 0;
    virtual int32_t StartShareFormFromRemote(
        const std::string& remoteDeviceId, const OHOS::AppExecFwk::FormShareInfo& formShareInfo) = 0;
#endif
    virtual int32_t NotifyStateChangedFromRemote(int32_t abilityState, int32_t connectToken,
        const AppExecFwk::ElementName& element) = 0;
    virtual int32_t GetDistributedComponentList(std::vector<std::string>& distributedComponents) = 0;
    virtual int32_t StopRemoteExtensionAbility(
        const OHOS::AAFwk::Want& want, int32_t callerUid, uint32_t accessToken, int32_t extensionType) = 0;
    virtual int32_t StopExtensionAbilityFromRemote(const OHOS::AAFwk::Want& want,
        const CallerInfo& callerInfo, const AccountInfo& accountInfo, int32_t extensionType) = 0;
};
} // namespace DistributedSchedule
} // namespace OHOS
#endif // OHOS_DISTRIBUTED_SCHED_INTERFACE_H

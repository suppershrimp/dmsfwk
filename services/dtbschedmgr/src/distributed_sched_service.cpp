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

#include "distributed_sched_service.h"

#include <cinttypes>
#include <unistd.h>

#include "ability_manager_client.h"
#include "distributed_sched_ability_shell.h"
#include "dtbschedmgr_device_info_storage.h"
#include "dtbschedmgr_log.h"
#include "element_name.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "string_ex.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace DistributedSchedule {
using namespace AAFwk;
using namespace AppExecFwk;
namespace {
    const std::string PARAM_CALLING_UID = "dms.extra.param.key.callingUid";
    const std::string PARAM_CALLING_PID = "dms.extra.param.key.callingPid";
    const std::string PARAM_CALLING_NETWORK_ID = "dms.extra.param.key.srcNetworkId";
}

IMPLEMENT_SINGLE_INSTANCE(DistributedSchedService);

const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(&DistributedSchedService::GetInstance());

DistributedSchedService::DistributedSchedService() : SystemAbility(DISTRIBUTED_SCHED_SA_ID, true)
{
}

void DistributedSchedService::OnStart()
{
    if (!Init()) {
        HILOGE("failed to init DistributedSchedService");
        return;
    }
    FuncContinuationCallback continuationCallback = [this] (const sptr<IRemoteObject>& abilityToken) {
        HILOGW("continuationCallback timeout.");
        NotifyContinuationCallbackRet(abilityToken, CONTINUE_ABILITY_TIMEOUT_ERR);
    };
    dschedContinuation_ = std::make_shared<DSchedContinuation>();
    dschedContinuation_->Init(continuationCallback);
    HILOGI("DistributedSchedService::OnStart start service success.");
}

bool DistributedSchedService::Init()
{
    HILOGD("DistributedSchedService::Init ready to init.");
    if (!registerToService_) {
        bool ret = Publish(this);
        if (!ret) {
            HILOGE("DistributedSchedService::Init Publish failed!");
            return false;
        }
        registerToService_ = true;
    }
    HILOGD("DistributedSchedService::Init init success.");
    return true;
}

void DistributedSchedService::OnStop()
{
    HILOGD("DistributedSchedService::OnStop ready to stop service.");
    registerToService_ = false;
}

int32_t DistributedSchedService::StartRemoteAbility(const OHOS::AAFwk::Want& userWant,
    const OHOS::AppExecFwk::AbilityInfo& abilityInfo, int32_t requestCode)
{
    std::string deviceId = userWant.GetElement().GetDeviceID();
    sptr<IDistributedSched> remoteDms = GetRemoteDms(deviceId);
    if (remoteDms == nullptr) {
        HILOGE("StartRemoteAbility DMS get remoteDms failed");
        return INVALID_PARAMETERS_ERR;
    }
    CallerInfo callerInfo;
    AccountInfo accountInfo;
    HILOGI("[PerformanceTest] DistributedSchedService StartRemoteAbility transact begin");
    int32_t result = remoteDms->StartAbilityFromRemote(userWant, abilityInfo, requestCode, callerInfo, accountInfo);
    HILOGI("[PerformanceTest] DistributedSchedService StartRemoteAbility transact end");
    return result;
}

int32_t DistributedSchedService::StartAbilityFromRemote(const OHOS::AAFwk::Want& userWant,
    const OHOS::AppExecFwk::AbilityInfo& abilityInfo, int32_t requestCode,
    const CallerInfo& callerInfo, const AccountInfo& accountInfo)
{
    ErrCode err = AAFwk::AbilityManagerClient::GetInstance()->Connect();
    if (err != ERR_OK) {
        HILOGE("StartAbilityFromRemote connect ability server failed %{public}d", err);
        return err;
    }
    err = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(userWant, requestCode);
    if (err != ERR_OK) {
        HILOGE("StartAbilityFromRemote is failed %{public}d", err);
    }
    return err;
}

int32_t DistributedSchedService::StartContinuation(const OHOS::AAFwk::Want& userWant,
    const OHOS::AppExecFwk::AbilityInfo& abilityInfo, const sptr<IRemoteObject>& abilityToken)
{
    //校验 remote deviceId
    HILOGD("[PerformanceTest] DistributedSchedService StartContinuation begin");
    if (abilityToken == nullptr) {
        HILOGE("StartContinuation abilityToken is null!");
        return INVALID_REMOTE_PARAMETERS_ERR;
    }
    auto flags = userWant.GetFlags();
    if ((flags & AAFwk::Want::FLAG_ABILITY_CONTINUATION) == 0) {
        HILOGE("StartContinuation userWant continuation flags invalid!");
        return INVALID_REMOTE_PARAMETERS_ERR;
    }

    std::string devId;
    if (!GetLocalDeviceId(devId)) {
        HILOGE("StartContinuation get local deviceId failed!");
        return INVALID_REMOTE_PARAMETERS_ERR;
    }

    if (dschedContinuation_ == nullptr) {
        HILOGE("StartContinuation continuation object null!");
        return INVALID_PARAMETERS_ERR;
    }

    int32_t sessionId = dschedContinuation_->GenerateSessionId();
    AAFwk::Want newWant = userWant;
    newWant.SetParam("sessionId", sessionId);
    newWant.SetParam("deviceId", devId);
    int32_t result = ERR_OK;
    result = StartRemoteAbility(newWant, abilityInfo, 0);
    if (result != ERR_OK) {
        HILOGE("DistributedSchedService:continue ability failed, errorCode = %{public}d", result);
        return result;
    }

    bool ret = dschedContinuation_->PushAbilityToken(sessionId, abilityToken);
    if (!ret) {
        HILOGW("StartContinuation PushAbilityToken failed!");
        return INVALID_REMOTE_PARAMETERS_ERR;
    }
    HILOGD("[PerformanceTest] DistributedSchedService StartContinuation end");
    return result;
}

void DistributedSchedService::NotifyCompleteContinuation(const std::u16string& devId, int32_t sessionId, bool isSuccess)
{
    if (!isSuccess) {
        HILOGE("NotifyCompleteContinuation failed!");
    }
    if (sessionId <= 0) {
        HILOGE("NotifyCompleteContinuation sessionId invalid!");
        return;
    }
    std::string deviceId = Str16ToStr8(devId);
    sptr<IDistributedSched> remoteDms = GetRemoteDms(deviceId);
    if (remoteDms == nullptr) {
        HILOGE("NotifyCompleteContinuation get remote dms null!");
        return;
    }
    remoteDms->NotifyContinuationResultFromRemote(sessionId, isSuccess);
}

int32_t DistributedSchedService::NotifyContinuationResultFromRemote(int32_t sessionId, bool isSuccess)
{
    if (sessionId <= 0) {
        HILOGE("NotifyContinuationResultFromRemote sessionId:%{public}d invalid!", sessionId);
        return INVALID_REMOTE_PARAMETERS_ERR;
    }
    if (dschedContinuation_ == nullptr) {
        HILOGE("NotifyContinuationResultFromRemote continuation object null!");
        return INVALID_REMOTE_PARAMETERS_ERR;
    }

    auto abilityToken = dschedContinuation_->PopAbilityToken(sessionId);
    if (abilityToken == nullptr) {
        HILOGE("DSchedContinuationCallback NotifyContinuationResultFromRemote abilityToken null!");
        return INVALID_REMOTE_PARAMETERS_ERR;
    }
    NotifyContinuationCallbackRet(abilityToken, isSuccess ? 0 : NOTIFYCOMPLETECONTINUATION_FAILED);
    return ERR_OK;
}

void DistributedSchedService::NotifyContinuationCallbackRet(const sptr<IRemoteObject>& abilityToken,
    int32_t isSuccess)
{
    if (isSuccess != ERR_OK) {
        HILOGE("NotifyContinuationCallbackRet failed!");
    }
    HILOGD("NotifyContinuationCallbackRet ContinuationRet result:%{public}d", isSuccess);
    if (abilityToken == nullptr) {
        HILOGE("NotifyContinuationCallbackRet abilityToken null!");
        return;
    }

    int32_t result = DistributedSchedAbilityShell::GetInstance().ScheduleCompleteContinuation(
        abilityToken, isSuccess);
    HILOGD("NotifyContinuationCallbackRet ScheduleCompleteContinuation result:%{public}d", result);
}

int32_t DistributedSchedService::RegisterAbilityToken(const sptr<IRemoteObject>& abilityToken,
    const sptr<IRemoteObject>& continuationCallback)
{
    return DistributedSchedAbilityShell::GetInstance().RegisterAbilityToken(abilityToken, continuationCallback);
}

int32_t DistributedSchedService::UnregisterAbilityToken(const sptr<IRemoteObject>& abilityToken,
    const sptr<IRemoteObject>& continuationCallback)
{
    return DistributedSchedAbilityShell::GetInstance().UnregisterAbilityToken(abilityToken, continuationCallback);
}

bool DistributedSchedService::GetLocalDeviceId(std::string& localDeviceId)
{
    if (!DtbschedmgrDeviceInfoStorage::GetInstance().GetLocalDeviceId(localDeviceId)) {
        HILOGE("GetLocalDeviceId fail");
        return false;
    }
    return true;
}

sptr<IDistributedSched> DistributedSchedService::GetRemoteDms(const std::string& remoteDeviceId)
{
    if (remoteDeviceId.empty()) {
        HILOGE("GetRemoteDms remoteDeviceId is empty");
        return nullptr;
    }
    HILOGD("GetRemoteDms connect deviceid is %s", remoteDeviceId.c_str());
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        HILOGE("GetRemoteDms failed to connect to systemAbilityMgr!");
        return nullptr;
    }
    HILOGD("[PerformanceTest] DistributedSchedService GetRemoteDms begin");
    auto object = samgr->CheckSystemAbility(DISTRIBUTED_SCHED_SA_ID, remoteDeviceId);
    HILOGD("[PerformanceTest] DistributedSchedService GetRemoteDms end");
    if (object == nullptr) {
        HILOGE("GetRemoteDms failed to get remote DistributedSched %{private}s", remoteDeviceId.c_str());
        return nullptr;
    }
    return iface_cast<IDistributedSched>(object);
}
} // namespace DistributedSchedule
} // namespace OHOS
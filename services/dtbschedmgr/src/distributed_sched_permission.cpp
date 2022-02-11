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

#include "distributed_sched_permission.h"

#include "accesstoken_kit.h"
#include "caller_info.h"
#include "distributed_sched_adapter.h"
#include "dtbschedmgr_log.h"

namespace OHOS {
namespace DistributedSchedule {
using namespace OHOS::Security;
namespace {
const std::string TAG = "DistributedSchedPermission";
}
IMPLEMENT_SINGLE_INSTANCE(DistributedSchedPermission);

int32_t DistributedSchedPermission::CheckDPermission(const AAFwk::Want& want, const CallerInfo& callerInfo,
    const AccountInfo& accountInfo, const AppExecFwk::AbilityInfo& abilityInfo, const std::string& localDeviceId)
{
    if (localDeviceId.empty()) {
        return INVALID_PARAMETERS_ERR;
    }
    AppExecFwk::AbilityInfo targetAbility;
    bool result = getTargetAbility(want, abilityInfo, localDeviceId, targetAbility, callerInfo);
    if (!result) {
        HILOGE("CheckDPermission can not find the target ability");
        return INVALID_PARAMETERS_ERR;
    }
    HILOGD("target ability info bundleName:%{public}s abilityName:%{public}s uri:%{public}s visible:%{public}d",
        targetAbility.bundleName.c_str(), targetAbility.name.c_str(), targetAbility.uri.c_str(),
        targetAbility.visible);
    HILOGD("callerType:%{public}d accountType:%{public}d callerUid:%{public}d", callerInfo.callerType,
        accountInfo.accountType, callerInfo.uid);
    // 2.check component access permission, when the ability is not visible.
    if (!CheckComponentAccessPermission(targetAbility, callerInfo, accountInfo, want)) {
        HILOGE("CheckComponentAccessPermission denied or failed! the caller component do not have permission");
        return DMS_COMPONENT_ACCESS_PERMISSION_DENIED;
    }
    // 3.check application custom permissions
    if (CheckCustomPermission(targetAbility, callerInfo)) {
        HILOGE("CheckCustomPermission denied or failed! the caller component do not have permission");
        return DMS_COMPONENT_ACCESS_PERMISSION_DENIED;
    }
    HILOGI("CheckDPermission success!!");
    return ERR_OK;
}

bool DistributedSchedPermission::getTargetAbility(const AAFwk::Want& want,
    const AppExecFwk::AbilityInfo& abilityInfo, const std::string& localDeviceId,
    AppExecFwk::AbilityInfo& targetAbility, const CallerInfo& callerInfo) const
{
    HILOGD("getTargetAbility");
    bool result = DistributedSchedAdapter::GetInstance().QueryAbilityInfo(want, targetAbility);
    if (!result) {
        HILOGE("getTargetAbility QueryAbilityInfo fail");
        return false;
    }
    return true;
}

bool DistributedSchedPermission::CheckComponentAccessPermission(const AppExecFwk::AbilityInfo& targetAbility,
    const CallerInfo& callerInfo, const AccountInfo& accountInfo, const AAFwk::Want& want) const
{
    // reject directly when in no account networking environment and target ability is not visible,
    if (!targetAbility.visible) {
        HILOGE("CheckComponentAccessPermission target ability is not visible, permission denied!");
        return false;
    }
    HILOGD("CheckComponentAccessPermission success");
    return true;
}

bool DistributedSchedPermission::CheckCustomPermission(const AppExecFwk::AbilityInfo& targetAbility,
    const CallerInfo& callerInfo) const
{
    const auto& permissions = targetAbility.permissions;
    if (permissions.empty()) {
        HILOGD("no need any permission, so granted!");
        return true;
    }
    if (callerInfo.tokenCaller == 0) {
        HILOGD("kernel is not support or field is not parsed, so granted!");
        return true;
    }
    uint32_t dTokenCaller = AccessToken::AccessTokenKit::AllocLocalTokenID(
        callerInfo.sourceDeviceId, callerInfo.tokenCaller);
    if (dTokenCaller == 0) {
        HILOGE("AccessTokenID is invalid!");
        return false;
    }
    for (const auto& permission : permissions) {
        if (permission.empty()) {
            continue;
        }
        int32_t result = AccessToken::AccessTokenKit::VerifyAccessToken(dTokenCaller, permission);
        if (result == AccessToken::PermissionState::PERMISSION_GRANTED) {
            HILOGD("AccessTokenID:%{public}d, permission:%{public}s granted!", dTokenCaller, permission.c_str());
            return true;
        }
        HILOGI("AccessTokenID:%{public}d, permission:%{public}s check failed!", dTokenCaller, permission.c_str());
    }
    return false;
}
}
}
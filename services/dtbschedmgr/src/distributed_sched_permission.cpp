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
#include "adapter/dnetwork_adapter.h"
#include "bundle/bundle_manager_internal.h"
#include "caller_info.h"
#include "datetime_ex.h"
#include "device_auth_defines.h"
#include "distributed_sched_adapter.h"
#include "dtbschedmgr_log.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace DistributedSchedule {
using namespace OHOS::Security;
using namespace AAFwk;
namespace {
const std::string TAG = "DistributedSchedPermission";
const std::string FOUNDATION_PROCESS_NAME = "foundation";
const std::string DMS_API_VERSION = "dmsApiVersion";
const std::string DMS_IS_CALLER_BACKGROUND = "dmsIsCallerBackGround";
const std::string DMS_MISSION_ID = "dmsMissionId";
const std::string DMS_VERSION_ID = "dmsVersion";
const std::string PERMISSION_START_ABILIIES_FROM_BACKGROUND = "ohos.permission.START_ABILIIES_FROM_BACKGROUND";
const std::string PERMISSION_START_ABILITIES_FROM_BACKGROUND = "ohos.permission.START_ABILITIES_FROM_BACKGROUND";
const std::string PERMISSION_START_INVISIBLE_ABILITY = "ohos.permission.START_INVISIBLE_ABILITY";
constexpr int32_t DEFAULT_DMS_API_VERSION = 9;
const int DEFAULT_DMS_MISSION_ID = -1;
const int FA_MODULE_ALLOW_MIN_API_VERSION = 8;
}
IMPLEMENT_SINGLE_INSTANCE(DistributedSchedPermission);
void from_json(const nlohmann::json& jsonObject, GroupInfo& groupInfo)
{
    if (jsonObject.find(FIELD_GROUP_NAME) != jsonObject.end()) {
        jsonObject.at(FIELD_GROUP_NAME).get_to(groupInfo.groupName);
    }
    if (jsonObject.find(FIELD_GROUP_ID) != jsonObject.end()) {
        jsonObject.at(FIELD_GROUP_ID).get_to(groupInfo.groupId);
    }
    if (jsonObject.find(FIELD_GROUP_OWNER) != jsonObject.end()) {
        jsonObject.at(FIELD_GROUP_OWNER).get_to(groupInfo.groupOwner);
    }
    if (jsonObject.find(FIELD_GROUP_TYPE) != jsonObject.end()) {
        jsonObject.at(FIELD_GROUP_TYPE).get_to(groupInfo.groupType);
    }
    if (jsonObject.find(FIELD_GROUP_VISIBILITY) != jsonObject.end()) {
        jsonObject.at(FIELD_GROUP_VISIBILITY).get_to(groupInfo.groupVisibility);
    }
}

int32_t DistributedSchedPermission::CheckSendResultPermission(const AAFwk::Want& want, const CallerInfo& callerInfo,
    const AccountInfo& accountInfo, AppExecFwk::AbilityInfo& targetAbility)
{
    // 1.check account access permission in no account networking environment.
    if (!CheckAccountAccessPermission(callerInfo, accountInfo, targetAbility.bundleName)) {
        HILOGE("CheckAccountAccessPermission denied or failed!");
        return DMS_ACCOUNT_ACCESS_PERMISSION_DENIED;
    }
    // 2.check component access permission, when the ability is not visible.
    if (!CheckComponentAccessPermission(targetAbility, callerInfo, accountInfo, want)) {
        HILOGE("CheckComponentAccessPermission denied or failed! the callee component do not have permission");
        return DMS_COMPONENT_ACCESS_PERMISSION_DENIED;
    }
    // 3.check application custom permissions
    if (!CheckCustomPermission(targetAbility, callerInfo)) {
        HILOGE("CheckCustomPermission denied or failed! the caller component do not have permission");
        return DMS_COMPONENT_ACCESS_PERMISSION_DENIED;
    }
    HILOGI("CheckSendResultPermission success!!");
    return ERR_OK;
}

int32_t DistributedSchedPermission::CheckStartPermission(const AAFwk::Want& want, const CallerInfo& callerInfo,
    const AccountInfo& accountInfo, AppExecFwk::AbilityInfo& targetAbility)
{
    // 1.check account access permission in no account networking environment.
    if (!CheckAccountAccessPermission(callerInfo, accountInfo, targetAbility.bundleName)) {
        HILOGE("CheckAccountAccessPermission denied or failed!");
        return DMS_ACCOUNT_ACCESS_PERMISSION_DENIED;
    }
    // 2.check start control permissions.
    if (!CheckStartControlPermission(targetAbility, callerInfo, want)) {
        HILOGE("CheckStartControlPermission denied or failed! the callee component do not have permission");
        return DMS_START_CONTROL_PERMISSION_DENIED;
    }
    // 3.check application custom permissions
    if (!CheckCustomPermission(targetAbility, callerInfo)) {
        HILOGE("CheckCustomPermission denied or failed! the caller component do not have permission");
        return DMS_COMPONENT_ACCESS_PERMISSION_DENIED;
    }
    HILOGI("CheckDistributedPermission success!!");
    return ERR_OK;
}

int32_t DistributedSchedPermission::GetAccountInfo(const std::string& remoteNetworkId,
    const CallerInfo& callerInfo, AccountInfo& accountInfo)
{
    if (remoteNetworkId.empty()) {
        HILOGE("remoteNetworkId is empty");
        return ERR_NULL_OBJECT;
    }
    std::string udid = DnetworkAdapter::GetInstance()->GetUdidByNetworkId(remoteNetworkId);
    if (udid.empty()) {
        HILOGE("udid is empty");
        return ERR_NULL_OBJECT;
    }
    if (!GetRelatedGroups(udid, callerInfo.bundleNames, accountInfo)) {
        HILOGE("GetRelatedGroups failed");
        return INVALID_PARAMETERS_ERR;
    }
    return ERR_OK;
}

bool DistributedSchedPermission::GetRelatedGroups(const std::string& udid,
    const std::vector<std::string>& bundleNames, AccountInfo& accountInfo)
{
    for (const auto& bundleName : bundleNames) {
        std::string returnGroups;
        if (!DistributedSchedAdapter::GetInstance().GetRelatedGroups(udid, bundleName, returnGroups)) {
            continue;
        }
        std::vector<GroupInfo> groupInfos;
        if (!ParseGroupInfos(returnGroups, groupInfos)) {
            continue;
        }
        for (const auto& groupInfo : groupInfos) {
            // check group type is whether (same count or point to point) or not
            if (groupInfo.groupType != GroupType::IDENTICAL_ACCOUNT_GROUP
                && groupInfo.groupType != GroupType::PEER_TO_PEER_GROUP) {
                continue;
            }
            accountInfo.groupIdList.push_back(groupInfo.groupId);
            if (groupInfo.groupType == GroupType::IDENTICAL_ACCOUNT_GROUP
                && accountInfo.accountType != IDistributedSched::SAME_ACCOUNT_TYPE) {
                accountInfo.accountType = IDistributedSched::SAME_ACCOUNT_TYPE;
            }
        }
    }
    if (accountInfo.groupIdList.empty()) {
        HILOGE("groupIdList is empty");
        return false;
    }
    return true;
}

bool DistributedSchedPermission::ParseGroupInfos(const std::string& returnGroupStr, std::vector<GroupInfo>& groupInfos)
{
    nlohmann::json groupInfoJson = nlohmann::json::parse(returnGroupStr, nullptr, false);
    if (groupInfoJson.is_discarded()) {
        HILOGE("returnGroupStr parse failed");
        return false;
    }
    HILOGD("groupInfoJson:%{public}s", groupInfoJson.dump().c_str());
    groupInfos = groupInfoJson.get<std::vector<GroupInfo>>();
    if (groupInfos.empty()) {
        HILOGE("groupInfos is empty");
        return false;
    }
    return true;
}

bool DistributedSchedPermission::GetTargetAbility(const AAFwk::Want& want,
    AppExecFwk::AbilityInfo& targetAbility, bool needQueryExtension) const
{
    if (BundleManagerInternal::QueryAbilityInfo(want, targetAbility)) {
        if (want.GetIntParam(DMS_MISSION_ID, DEFAULT_DMS_MISSION_ID) != DEFAULT_DMS_MISSION_ID &&
            (targetAbility.type == AppExecFwk::AbilityType::SERVICE ||
            targetAbility.type == AppExecFwk::AbilityType::EXTENSION)) {
            HILOGE("StartAbilityForResult can not start service and extension ability");
            return false;
        }
        return true;
    }
    if (needQueryExtension) {
        HILOGI("QueryAbilityInfo failed, try to QueryExtensionAbilityInfo");
        // try to find extension
        AppExecFwk::ExtensionAbilityInfo extensionAbility;
        if (BundleManagerInternal::QueryExtensionAbilityInfo(want, extensionAbility)) {
            // extensionAbilityInfo translates to abilityInfo
            BundleManagerInternal::InitAbilityInfoFromExtension(extensionAbility, targetAbility);
            return true;
        }
    }
    HILOGE("QueryAbilityInfo failed, want bundle name=%{public}s, ability name=%{public}s.",
        want.GetElement().GetBundleName().c_str(), want.GetElement().GetAbilityName().c_str());
    return false;
}

int32_t DistributedSchedPermission::CheckGetCallerPermission(const AAFwk::Want& want, const CallerInfo& callerInfo,
    const AccountInfo& accountInfo, AppExecFwk::AbilityInfo& targetAbility)
{
    // 1.check account access permission in no account networking environment.
    if (!CheckAccountAccessPermission(callerInfo, accountInfo, targetAbility.bundleName)) {
        HILOGE("CheckAccountAccessPermission denied or failed!");
        return DMS_ACCOUNT_ACCESS_PERMISSION_DENIED;
    }
    // 2. check call with same appid
    if (!BundleManagerInternal::IsSameAppId(callerInfo.callerAppId, targetAbility.bundleName)) {
        HILOGE("the appId is different, check permission denied!");
        return CALL_PERMISSION_DENIED;
    }
    // 3. check background permission
    if (!CheckBackgroundPermission(targetAbility, callerInfo, want, false)) {
        HILOGE("Check background permission failed!");
        return DMS_BACKGROUND_PERMISSION_DENIED;
    }
    // 4.check application custom permissions
    if (!CheckCustomPermission(targetAbility, callerInfo)) {
        HILOGE("CheckCustomPermission denied or failed! the caller component do not have permission");
        return DMS_COMPONENT_ACCESS_PERMISSION_DENIED;
    }
    HILOGI("CheckGetCallerPermission success!!");
    return ERR_OK;
}

bool DistributedSchedPermission::IsFoundationCall() const
{
    uint32_t accessToken = IPCSkeleton::GetCallingTokenID();
    AccessToken::NativeTokenInfo nativeTokenInfo;
    int32_t result = AccessToken::AccessTokenKit::GetNativeTokenInfo(accessToken, nativeTokenInfo);
    if (result == ERR_OK && nativeTokenInfo.processName == FOUNDATION_PROCESS_NAME) {
        return true;
    }
    HILOGE("not foundation called, processName:%{private}s", nativeTokenInfo.processName.c_str());
    return false;
}

int32_t DistributedSchedPermission::CheckPermission(uint32_t accessToken, const std::string& permissionName) const
{
    HILOGI("called.");
    // if called from xts, granted directly, no need to check permissions.
    if (IsNativeCall(accessToken)) {
        return ERR_OK;
    }
    if (VerifyPermission(accessToken, permissionName)) {
        return ERR_OK;
    }
    return DMS_PERMISSION_DENIED;
}

bool DistributedSchedPermission::IsNativeCall(uint32_t accessToken) const
{
    auto tokenType = AccessToken::AccessTokenKit::GetTokenTypeFlag(accessToken);
    if (tokenType == AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        return true;
    }
    return false;
}

bool DistributedSchedPermission::VerifyPermission(uint32_t accessToken, const std::string& permissionName) const
{
    int32_t result = AccessToken::AccessTokenKit::VerifyAccessToken(accessToken, permissionName);
    if (result == AccessToken::PermissionState::PERMISSION_DENIED) {
        HILOGE("permission denied, permissionName:%{public}s", permissionName.c_str());
        return false;
    }
    HILOGD("permission matched.");
    return true;
}

bool DistributedSchedPermission::CheckAccountAccessPermission(const CallerInfo& callerInfo,
    const AccountInfo& accountInfo, const std::string& targetBundleName)
{
    if (accountInfo.accountType == IDistributedSched::SAME_ACCOUNT_TYPE) {
        HILOGD("no need to check");
        return true;
    }
    if (targetBundleName.empty() || accountInfo.groupIdList.empty()) {
        HILOGE("targetBundleName or groupIdList is empty");
        return false;
    }

    for (const auto& groupId : accountInfo.groupIdList) {
        HILOGD("groupId:%{public}s targetBundleName:%{public}s", groupId.c_str(), targetBundleName.c_str());
        if (DistributedSchedAdapter::GetInstance().CheckAccessToGroup(groupId, targetBundleName)) {
            return true;
        }
    }
    HILOGE("check account permission failed");
    return false;
}

bool DistributedSchedPermission::CheckComponentAccessPermission(const AppExecFwk::AbilityInfo& targetAbility,
    const CallerInfo& callerInfo, const AccountInfo& accountInfo, const AAFwk::Want& want) const
{
    // reject directly when in no account networking environment and target ability is not visible,
    if (!targetAbility.visible) {
        HILOGE("target ability is not visible, permission denied!");
        return false;
    }
    HILOGD("check component permission success");
    return true;
}

bool DistributedSchedPermission::CheckStartControlPermission(const AppExecFwk::AbilityInfo& targetAbility,
    const CallerInfo& callerInfo, const AAFwk::Want& want) const
{
    // 1. check if continuation with same appid
    if ((want.GetFlags() & AAFwk::Want::FLAG_ABILITY_CONTINUATION) != 0) {
        if (BundleManagerInternal::IsSameAppId(callerInfo.callerAppId, targetAbility.bundleName)) {
            HILOGD("the appId is the same, check permission success!");
            return true;
        }
        HILOGE("the appId is different in the migration scenario, permission denied!");
        return false;
    }
    // 2. check background permission
    if (!CheckBackgroundPermission(targetAbility, callerInfo, want, true)) {
        HILOGE("Check background permission failed!");
        return false;
    }
    // 3. check start or connect ability with same appid
    if (BundleManagerInternal::IsSameAppId(callerInfo.callerAppId, targetAbility.bundleName)) {
        HILOGD("the appId is the same, check permission success!");
        return true;
    }
    // 4. check if target ability is not visible and without PERMISSION_START_INVISIBLE_ABILITY
    if (!CheckTargetAbilityVisible(targetAbility, callerInfo)) {
        HILOGE("target ability is not visible and has no PERMISSION_START_INVISIBLE_ABILITY, permission denied!");
        return false;
    }
    // 5. check if service of fa mode can associatedWakeUp
    if (!targetAbility.isStageBasedModel && targetAbility.type == AppExecFwk::AbilityType::SERVICE &&
        !targetAbility.applicationInfo.associatedWakeUp) {
        HILOGE("target ability is service ability(FA) and associatedWakeUp is false, permission denied!");
        return false;
    }
    HILOGD("CheckStartControlPermission success");
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
    if (callerInfo.accessToken == 0) {
        HILOGW("kernel is not support or field is not parsed, so denied!");
        return false;
    }
    int64_t begin = GetTickCount();
    uint32_t dAccessToken = AccessToken::AccessTokenKit::AllocLocalTokenID(callerInfo.sourceDeviceId,
        callerInfo.accessToken);
    HILOGI("[PerformanceTest] AllocLocalTokenID spend %{public}" PRId64 " ms", GetTickCount() - begin);
    if (dAccessToken == 0) {
        HILOGE("dAccessTokenID is invalid!");
        return false;
    }
    for (const auto& permission : permissions) {
        if (permission.empty()) {
            continue;
        }
        int32_t result = AccessToken::AccessTokenKit::VerifyAccessToken(dAccessToken, permission);
        if (result == AccessToken::PermissionState::PERMISSION_DENIED) {
            HILOGD("dAccessTokenID:%{public}u, permission:%{public}s denied!", dAccessToken, permission.c_str());
            return false;
        }
        HILOGD("dAccessTokenID:%{public}u, permission:%{public}s matched!", dAccessToken, permission.c_str());
    }
    return true;
}

bool DistributedSchedPermission::CheckBackgroundPermission(const AppExecFwk::AbilityInfo& targetAbility,
    const CallerInfo& callerInfo, const AAFwk::Want& want, bool needCheckApiVersion) const
{
    if (callerInfo.extraInfoJson.empty() || 
        callerInfo.extraInfoJson.find(DMS_VERSION_ID) == callerInfo.extraInfoJson.end()) {
        HILOGD("the version is low");
        return true;
    }
    AAFwk::Want* remoteWant = const_cast<Want*>(&want);
    bool isCallerBackGround = remoteWant->GetBoolParam(DMS_IS_CALLER_BACKGROUND, true);
    remoteWant->RemoveParam(DMS_IS_CALLER_BACKGROUND);
    if (!isCallerBackGround) {
        HILOGD("the app is foreground");
        return true;
    }
    int apiVersion = remoteWant->GetIntParam(DMS_API_VERSION, DEFAULT_DMS_API_VERSION);
    remoteWant->RemoveParam(DMS_API_VERSION);
    // service in fa mode(API 8) do not need check
    if (needCheckApiVersion && CheckMinApiVersion(targetAbility, apiVersion)) {
        HILOGD("the app is service ability of fa mode and is under api 8");
        return true;
    }
    uint32_t dAccessToken = AccessToken::AccessTokenKit::AllocLocalTokenID(callerInfo.sourceDeviceId,
        callerInfo.accessToken);
    if (dAccessToken == 0) {
        HILOGE("dAccessTokenID is invalid!");
        return false;
    }
    // check if background's ability has PERMISSION_START_ABILITIES_FROM_BACKGROUND
    if (CheckPermission(dAccessToken, PERMISSION_START_ABILITIES_FROM_BACKGROUND) == ERR_OK ||
        CheckPermission(dAccessToken, PERMISSION_START_ABILIIES_FROM_BACKGROUND) == ERR_OK) {
        HILOGD("the app has PERMISSION_START_ABILITIES_FROM_BACKGROUND");
        return true;
    }
    HILOGE("CheckBackgroundPermission failed!");
    return false;
}

bool DistributedSchedPermission::CheckMinApiVersion(const AppExecFwk::AbilityInfo& targetAbility,
    int32_t apiVersion) const
{
    if (!targetAbility.isStageBasedModel && targetAbility.type == AppExecFwk::AbilityType::SERVICE &&
        apiVersion <= FA_MODULE_ALLOW_MIN_API_VERSION) {
        HILOGD("CheckMinApiVersion pass");
        return true;
    }
    return false;
}

bool DistributedSchedPermission::CheckTargetAbilityVisible(const AppExecFwk::AbilityInfo& targetAbility,
    const CallerInfo& callerInfo) const
{
    if (targetAbility.visible) {
        HILOGD("Target ability is visible.");
        return true;
    }
    uint32_t dAccessToken = AccessToken::AccessTokenKit::AllocLocalTokenID(callerInfo.sourceDeviceId,
        callerInfo.accessToken);
    if (dAccessToken == 0) {
        HILOGE("dAccessTokenID is invalid!");
        return false;
    }
    if (CheckPermission(dAccessToken, PERMISSION_START_INVISIBLE_ABILITY) != ERR_OK) {
        HILOGE("CheckTargetAbilityVisible failed.");
        return false;
    }
    HILOGD("CheckTargetAbilityVisible passed.");
    return true;
}
}
}
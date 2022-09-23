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

#include "dms_version_manager.h"

#include "distributed_device_profile_client.h"
#include "dtbschedmgr_log.h"
#include "string_ex.h"

namespace OHOS {
namespace DistributedSchedule {
namespace {
const std::string TAG = "DmsVersionManager";
const std::string DMS_SERVICE_ID = "appInfo";
const std::string DMS_SERVICE_TYPE = "appInfo";
const std::string PACKAGE_NAMES = "packageNames";
const std::string VERSIONS = "versions";
const std::string DMS_NAME = "dmsfwk";
const int32_t DMS_VERSION_LENGTH = 3;
const int32_t DMS_MAJOR_VERSION_INDEX = 0;
const int32_t DMS_MINOR_VERSION_INDEX = 1;
const int32_t DMS_FEATURE_VERSION_INDEX = 2;
const int32_t KEY_NOT_FOUND = 27459591;
}

IMPLEMENT_SINGLE_INSTANCE(DmsVersionManager);

bool DmsVersionManager::IsRemoteDmsVersionLower(const std::string& remoteDeviceId,
    const DmsVersion& thresholdDmsVersion)
{
    DmsVersion dmsVersion;
    int32_t result = GetRemoteDmsVersion(remoteDeviceId, dmsVersion);
    if (result == DMS_VERSION_EMPTY) {
        return true;
    }
    if (result != ERR_OK) {
        return false;
    }
    return CompareDmsVersion(dmsVersion, thresholdDmsVersion);
}

int32_t DmsVersionManager::GetRemoteDmsVersion(const std::string& deviceId, DmsVersion& dmsVersion)
{
    std::string packageNamesData;
    std::string versionsData;
    int32_t result = GetAppInfoFromDP(deviceId, packageNamesData, versionsData);
    if (result != ERR_OK) {
        return result;
    }

    std::string dmsVersionData;
    result = GetDmsVersionDataFromAppInfo(packageNamesData, versionsData, dmsVersionData);
    if (result != ERR_OK) {
        return result;
    }
    if (!ParseDmsVersion(dmsVersionData, dmsVersion)) {
        HILOGE("parse dms version failed");
        return DMS_VERSION_PARSE_EXCEPTION;
    }
    return ERR_OK;
}

int32_t DmsVersionManager::GetAppInfoFromDP(const std::string& deviceId, std::string& packageNamesData,
    std::string& versionsData)
{
    DeviceProfile::ServiceCharacteristicProfile profile;
    int32_t result = IN_PROCESS_CALL(
        DeviceProfile::DistributedDeviceProfileClient::GetInstance().GetDeviceProfile(deviceId,
            DMS_SERVICE_ID, profile)
    );
    if (result == KEY_NOT_FOUND) {
        return DMS_VERSION_EMPTY;
    }
    if (result != ERR_OK) {
        HILOGE("get device profile failed");
        return DEVICE_PROFILE_SERVICE_EXCEPTION;
    }
    std::string jsonData = profile.GetCharacteristicProfileJson();
    if (jsonData.empty()) {
        return DMS_VERSION_EMPTY;
    }
    nlohmann::json dpJson = nlohmann::json::parse(jsonData.c_str(), nullptr, false);
    if (dpJson.find(PACKAGE_NAMES) != dpJson.end() && dpJson.at(PACKAGE_NAMES).is_string()) {
        dpJson.at(PACKAGE_NAMES).get_to(packageNamesData);
    }
    if (dpJson.find(VERSIONS) != dpJson.end() && dpJson.at(VERSIONS).is_string()) {
        dpJson.at(VERSIONS).get_to(versionsData);
    }
    return ERR_OK;
}

int32_t DmsVersionManager::GetDmsVersionDataFromAppInfo(const std::string& packageNamesData,
    const std::string& versionsData, std::string& dmsVersionData)
{
    dmsVersionData = "";
    if (packageNamesData.empty() && versionsData.empty()) {
        return DMS_VERSION_EMPTY;
    }
    std::vector<std::string> packageNameList;
    std::vector<std::string> versionsList;
    SplitStr(packageNamesData, ",", packageNameList);
    SplitStr(versionsData, ",", versionsList);
    if (packageNameList.size() != versionsList.size()) {
        return DMS_VERSION_PARSE_EXCEPTION;
    }
    for (int i = 0; i < packageNameList.size(); i++) {
        if (packageNameList[i] == DMS_NAME) {
            dmsVersionData = versionsList[i];
            return ERR_OK;
        }
    }
    return DMS_VERSION_EMPTY;
}

bool DmsVersionManager::ParseDmsVersion(const std::string& dmsVersionData, DmsVersion& dmsVersion)
{
    std::vector<std::string> versionNumList;
    SplitStr(dmsVersionData, ".", versionNumList);
    if (versionNumList.size() != DMS_VERSION_LENGTH) {
        return false;
    }
    int32_t majorVersionNum = -1;
    if (!OHOS::StrToInt(versionNumList[DMS_MAJOR_VERSION_INDEX], majorVersionNum) || majorVersionNum < 0) {
        return false;
    }
    dmsVersion.majorVersionNum = majorVersionNum;

    int32_t minorVersionNum = -1;
    if (!OHOS::StrToInt(versionNumList[DMS_MINOR_VERSION_INDEX], minorVersionNum) || minorVersionNum < 0) {
        return false;
    }
    dmsVersion.minorVersionNum = minorVersionNum;

    int32_t featureVersionNum = -1;
    if (!OHOS::StrToInt(versionNumList[DMS_FEATURE_VERSION_INDEX], featureVersionNum) || featureVersionNum < 0) {
        return false;
    }
    dmsVersion.featureVersionNum = featureVersionNum;
    return true;
}

bool DmsVersionManager::CompareDmsVersion(const DmsVersion& dmsVersion, const DmsVersion& thresholdDmsVersion)
{
    if (dmsVersion.majorVersionNum < thresholdDmsVersion.majorVersionNum) {
        return true;
    }
    if (dmsVersion.majorVersionNum == thresholdDmsVersion.majorVersionNum &&
        dmsVersion.minorVersionNum < thresholdDmsVersion.minorVersionNum) {
        return true;
    }
    if (dmsVersion.majorVersionNum == thresholdDmsVersion.majorVersionNum &&
        dmsVersion.minorVersionNum == thresholdDmsVersion.minorVersionNum &&
        dmsVersion.featureVersionNum < thresholdDmsVersion.featureVersionNum) {
        return true;
    }
    return false;
}
} // namespace DistributedSchedule
} // namespace OHOS

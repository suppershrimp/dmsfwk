/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include <utility>

#include "util/dsched_continue_info_util.h"
#include "dtbschedmgr_log.h"
#include "mission/dsched_sync_e2e.h"
#include "bundle/bundle_manager_internal.h"

namespace OHOS {
namespace DistributedSchedule {
namespace {
const std::string TAG = "DSchedContinueInfoUtil";
constexpr int32_t DBMS_RETRY_MAX_TIME = 5;
const std::string QUICK_START_CONFIGURATION = "_ContinueQuickStart";
}
IMPLEMENT_SINGLE_INSTANCE(DSchedContinueInfoUtil);
bool DSchedContinueInfoUtil::CompleteContinueInfo(std::string srcNetWorkId, uint16_t srcBundleNameId,
    uint8_t srcContinueTypeId, DSchedContinueInfo &continueInfo, int32_t retryTimes)
{
    HILOGI("start to complete continue info.");
    if (retryTimes >= DBMS_RETRY_MAX_TIME) {
        HILOGE("meet max retry time!");
        return false;
    }
    DmsBundleInfo distributedBundleInfo;
    if (!DmsBmStorage::GetInstance()->GetDistributedBundleInfo(
        srcNetWorkId, srcBundleNameId, distributedBundleInfo)) {
        HILOGW("get distributedBundleInfo failed, retryTimes = %{public}d", retryTimes);
        DmsKvSyncE2E::GetInstance()->PushAndPullData(srcNetWorkId);
        return CompleteContinueInfo(srcNetWorkId, srcBundleNameId, srcContinueTypeId, continueInfo,
                                    retryTimes + 1);
    }
    HILOGD("src distributedBundleInfo got. bundleName: %{public}s", distributedBundleInfo.bundleName.c_str());
    std::vector<std::string> srcContinueBundleNameSort;
    if (!CompleteSrcContinueInfo(continueInfo, distributedBundleInfo, srcContinueTypeId,
                                 srcContinueBundleNameSort)) {
        HILOGE("complete src continue info failed!");
        return false;
    }

    std::vector<std::string> bundleNameList;
    bool continueBundleGot = BundleManagerInternal::GetContinueBundle4Src(continueInfo.sourceBundleName_,
                                                                          bundleNameList);
    if (!continueBundleGot || bundleNameList.empty()) {
        HILOGW("Continue Bundle for Src got failed or list is empty. check same bundle name on the sink.");
        AppExecFwk::BundleInfo localBundleInfo;
        if (BundleManagerInternal::GetLocalBundleInfo(continueInfo.sourceBundleName_, localBundleInfo) ==
            ERR_OK) {
            HILOGI("same bundle is on the sink as src bundle name: %{public}s", distributedBundleInfo.bundleName.c_str());
            bundleNameList.push_back(distributedBundleInfo.bundleName);
        } else {
            HILOGE("can not get local bundle info or continue bundle for bundle name: %{public}s",
                distributedBundleInfo.bundleName.c_str());
            return false;
        }
    }

    return CompleteSinkContinueInfo(continueInfo, distributedBundleInfo, srcContinueBundleNameSort,
                                    bundleNameList);
}

bool DSchedContinueInfoUtil::CompleteContinueInfo(std::string srcNetWorkId, std::string srcBundleName,
    std::string srcContinueType, DSchedContinueInfo &continueInfo, int32_t retryTimes)
{
    HILOGI("start to complete continue info.");
    if (retryTimes >= DBMS_RETRY_MAX_TIME) {
        HILOGE("meet max retry time!");
        return false;
    }
    DmsBundleInfo distributedBundleInfo;
    if (!DmsBmStorage::GetInstance()->GetDistributedBundleInfo(
        srcNetWorkId, srcBundleName, distributedBundleInfo)) {
        HILOGW("get distributedBundleInfo failed, retryTimes = %{public}d", retryTimes);
        DmsKvSyncE2E::GetInstance()->PushAndPullData(srcNetWorkId);
        return CompleteContinueInfo(srcNetWorkId, srcBundleName, std::move(srcContinueType), continueInfo, retryTimes + 1);
    }

    HILOGD("src distributedBundleInfo got. bundleName: %{public}s", distributedBundleInfo.bundleName.c_str());
    std::vector<std::string> srcContinueBundleNameSort;
    if (!CompleteSrcContinueInfo(continueInfo, distributedBundleInfo, srcContinueType,
                                 srcContinueBundleNameSort)) {
        return false;
    }

    std::vector<std::string> bundleNameList;
    bool continueBundleGot = BundleManagerInternal::GetContinueBundle4Src(continueInfo.sourceBundleName_,
                                                                          bundleNameList);
    if (!continueBundleGot || bundleNameList.empty()) {
        HILOGW("Continue Bundle for Src got failed or list is empty. check same bundle name on the sink.");
        AppExecFwk::BundleInfo localBundleInfo;
        if (BundleManagerInternal::GetLocalBundleInfo(continueInfo.sourceBundleName_, localBundleInfo) ==
            ERR_OK) {
            HILOGI("same bundle is on the sink as src bundle name: %{public}s",
                distributedBundleInfo.bundleName.c_str());
            bundleNameList.push_back(distributedBundleInfo.bundleName);
        } else {
            HILOGE("can not get local bundle info or continue bundle for bundle name: %{public}s",
                distributedBundleInfo.bundleName.c_str());
            return false;
        }
    }

    return CompleteSinkContinueInfo(continueInfo, distributedBundleInfo, srcContinueBundleNameSort,
                                    bundleNameList);
}

bool DSchedContinueInfoUtil::IsSameContinueType(std::string continueType1, std::string continueType2)
{
    std::string continueType1Format;
    std::string suffix = QUICK_START_CONFIGURATION;
    if (suffix.length() <= continueType1.length() &&
        continueType1.rfind(suffix) == (continueType1.length() - suffix.length())) {
        continueType1Format = continueType1.substr(0, continueType1.rfind(QUICK_START_CONFIGURATION));
    } else {
        continueType1Format = continueType1 + QUICK_START_CONFIGURATION;
    }
    bool isSameContinueType = continueType1 == continueType2 || continueType1Format == continueType2;
    HILOGI("compare is same continue type: they are %{public}s for types: %{public}s; %{public}s",
           isSameContinueType ? "the same" : "different", continueType1.c_str(), continueType2.c_str());
    return isSameContinueType;
}

bool DSchedContinueInfoUtil::CompleteSrcContinueInfo(DSchedContinueInfo &continueInfo,
    DmsBundleInfo &distributedBundleInfo, uint8_t srcContinueTypeId,
    std::vector<std::string> &srcContinueBundleNameSort)
{
    HILOGI("start complete src bundle info");
    uint32_t pos = 0;
    bool abilityGot = false;
    for (auto dmsAbilityInfo: distributedBundleInfo.dmsAbilityInfos) {
        for (auto continueTypeElement: dmsAbilityInfo.continueType) {
            HILOGD("compare ability: %{public}s; continue type: %{public}s",
                dmsAbilityInfo.abilityName.c_str(), continueTypeElement.c_str());
            if (pos == srcContinueTypeId) {
                continueInfo.continueType_ = continueTypeElement;
                continueInfo.sourceBundleName_ = distributedBundleInfo.bundleName;
                continueInfo.sourceModuleName_ = dmsAbilityInfo.moduleName;
                continueInfo.sourceAbilityName_ = dmsAbilityInfo.abilityName;
                srcContinueBundleNameSort = dmsAbilityInfo.continueBundleName;
                abilityGot = true;
                break;
            }
            ++pos;
        }
        if (abilityGot) {
            break;
        }
    }
    if (!abilityGot) {
        return false;
    }
    HILOGI("complete src bundle info success,bundleName: %{public}s; continue type: %{public}s;"
           " module: %{public}s;  ability: %{public}s; ",
           continueInfo.sourceBundleName_.c_str(), continueInfo.continueType_.c_str(),
           continueInfo.sourceModuleName_.c_str(), continueInfo.sourceAbilityName_.c_str());
    srcContinueBundleNameSort.push_back(continueInfo.sourceBundleName_);
    return true;
}

bool DSchedContinueInfoUtil::CompleteSrcContinueInfo(DSchedContinueInfo &continueInfo,
    DmsBundleInfo &distributedBundleInfo, std::string srcContinueType,
    std::vector<std::string> &srcContinueBundleNameSort)
{
    HILOGI("start complete src bundle info");
    bool abilityGot = false;
    for (auto dmsAbilityInfo: distributedBundleInfo.dmsAbilityInfos) {
        for (auto continueTypeElement: dmsAbilityInfo.continueType) {
            HILOGD("compare ability: %{public}s; continue type: %{public}s",
                   dmsAbilityInfo.abilityName.c_str(), continueTypeElement.c_str());
            if (srcContinueType == continueTypeElement) {
                continueInfo.continueType_ = continueTypeElement;
                continueInfo.sourceBundleName_ = distributedBundleInfo.bundleName;
                continueInfo.sourceModuleName_ = dmsAbilityInfo.moduleName;
                continueInfo.sourceAbilityName_ = dmsAbilityInfo.abilityName;
                srcContinueBundleNameSort = dmsAbilityInfo.continueBundleName;
                abilityGot = true;
                break;
            }
        }
        if (abilityGot) {
            break;
        }
    }
    if (!abilityGot) {
        return false;
    }
    HILOGI("complete src bundle info success,bundleName: %{public}s; continue type: %{public}s;"
           " module: %{public}s;  ability: %{public}s; ",
           continueInfo.sourceBundleName_.c_str(), continueInfo.continueType_.c_str(),
           continueInfo.sourceModuleName_.c_str(), continueInfo.sourceAbilityName_.c_str());
    srcContinueBundleNameSort.push_back(continueInfo.sourceBundleName_);
    return true;
}

bool DSchedContinueInfoUtil::CompleteSinkContinueInfo(DSchedContinueInfo &continueInfo,
    DmsBundleInfo &distributedBundleInfo, std::vector<std::string> &srcContinueBundleNameSort,
    std::vector<std::string> &bundleNameList)
{
    HILOGI("start complete sink bundle info");
    AppExecFwk::BundleInfo localBundleInfo;
    if(!CompareDstBundleName(continueInfo, distributedBundleInfo, srcContinueBundleNameSort,
        bundleNameList, localBundleInfo)){
        HILOGE("No dst bundle name matches for src bundle name: %{public}s", distributedBundleInfo.bundleName.c_str());
        return false;
    }

    std::vector<AbilityInfo> localAbilityInfos = localBundleInfo.abilityInfos;
    bool diffModuleGot = false;
    for (const auto &abilityInfoElement: localAbilityInfos) {
        std::vector<std::string> continueTypes = abilityInfoElement.continueType;
        for (std::string &continueTypeElement: continueTypes) {
            HILOGD("compare ability, continue type: %{public}s; ability name: %{public}s",
                continueTypeElement.c_str(), abilityInfoElement.name.c_str());
            if (!IsSameContinueType(continueTypeElement, continueInfo.continueType_)) {
                HILOGD("continue type is not same as src");
                continue;
            }
            if (continueInfo.sourceModuleName_ == abilityInfoElement.moduleName) {
                HILOGD("got sink ability info that module is same as src."
                       "ability name: %{public}s; module name: %{public}s",
                    abilityInfoElement.name.c_str(), abilityInfoElement.moduleName.c_str());
                continueInfo.sinkModuleName_ = abilityInfoElement.moduleName;
                continueInfo.sinkAbilityName_ = abilityInfoElement.name;
                return true;
            } else if (!diffModuleGot) {
                HILOGD("got sink ability info that module is different as src. "
                    "ability name: %{public}s; module name: %{public}s",
                    abilityInfoElement.name.c_str(), abilityInfoElement.moduleName.c_str());
                diffModuleGot = true;
                continueInfo.sinkModuleName_ = abilityInfoElement.moduleName;
                continueInfo.sinkAbilityName_ = abilityInfoElement.name;
                continue;
            }
        }
    }
    HILOGI("complete sink bundle info success, bundleName: %{public}s; continue type: %{public}s;"
           " module: %{public}s;  ability: %{public}s; ",
           continueInfo.sinkBundleName_.c_str(), continueInfo.continueType_.c_str(),
           continueInfo.sinkModuleName_.c_str(), continueInfo.sinkAbilityName_.c_str());
    return diffModuleGot;
}

bool DSchedContinueInfoUtil::CompareDstBundleName(DSchedContinueInfo &continueInfo,
    const DmsBundleInfo &distributedBundleInfo, std::vector<std::string> &srcContinueBundleNameSort,
    std::vector<std::string> &bundleNameList, AppExecFwk::BundleInfo &localBundleInfo) {
    for (const auto &bundleNameInSrcConfig: srcContinueBundleNameSort) {
        HILOGD("compare bundle name in src config: %{public}s", bundleNameInSrcConfig.c_str());
        if (std::find(bundleNameList.begin(), bundleNameList.end(), bundleNameInSrcConfig) ==
            bundleNameList.end()) {
            HILOGD("bundle: %{public}s not exist in sink list.", bundleNameInSrcConfig.c_str());
            continue;
        }
        if (BundleManagerInternal::GetLocalBundleInfo(bundleNameInSrcConfig, localBundleInfo) != ERR_OK) {
            HILOGD("bundle: %{public}s not installed on the sink.", bundleNameInSrcConfig.c_str());
            continue;
        }
        if (bundleNameInSrcConfig == distributedBundleInfo.bundleName) {
            HILOGI("bundle: %{public}s is same as src bundle Name.", bundleNameInSrcConfig.c_str());
            continueInfo.sinkBundleName_ = bundleNameInSrcConfig;
            break;
        }
        AppExecFwk::AppProvisionInfo appProvisionInfo;
        if (BundleManagerInternal::GetAppProvisionInfo4CurrentUser(bundleNameInSrcConfig, appProvisionInfo)
            && appProvisionInfo.developerId == distributedBundleInfo.developerId) {
            HILOGI("got sink bundle name: %{public}s ", bundleNameInSrcConfig.c_str());
            continueInfo.sinkBundleName_ = bundleNameInSrcConfig;
            break;
        }
    }
    return !continueInfo.sinkBundleName_.empty();
}
}  // namespace DistributedSchedule
}  // namespace OHOS

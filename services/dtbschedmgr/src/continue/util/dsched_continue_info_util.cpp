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

#include "util/dsched_continue_info_util.h"
#include "dtbschedmgr_log.h"
#include "mission/dsched_sync_e2e.h"
#include "bundle/bundle_manager_internal.h"

namespace OHOS {
namespace DistributedSchedule {
namespace {
constexpr int32_t DBMS_RETRY_MAX_TIME = 5;
const std::string QUICK_START_CONFIGURATION = "_ContinueQuickStart";
}
bool DSchedContinueInfoUtil::CompleteContinueInfo(std::string srcNetWorkId, uint16_t srcBundleNameId,
    uint8_t srcContinueTypeId, DSchedContinueInfo &continueInfo, int32_t retryTimes)
{
    if (retryTimes >= DBMS_RETRY_MAX_TIME) {
        HILOGE("meet max retry time!");
        return false;
    }
    DmsBundleInfo distributedBundleInfo;
    if (!DmsBmStorage::GetInstance()->GetDistributedBundleInfo(
            srcNetWorkId, srcBundleNameId, distributedBundleInfo)) {
        HILOGW("get distributedBundleInfo failed, retryTimes = %{public}d", retryTimes);
        DmsKvSyncE2E::GetInstance()->PushAndPullData(srcNetWorkId);
        return CompleteContinueInfo(srcNetWorkId, srcBundleNameId, srcContinueTypeId, continueInfo, retryTimes + 1);
    }

    std::vector<std::string> srcContinueBundleNameSort;
    if (!CompleteSrcContinueInfo(continueInfo, distributedBundleInfo, srcContinueTypeId, srcContinueBundleNameSort)) {
        return false;
    }

    std::vector<std::string> bundleNameList;
    AppExecFwk::BundleInfo localBundleInfo;
    bool continueBundleGot = BundleManagerInternal::GetContinueBundle4Src(continueInfo.sourceBundleName_,
                                                                          bundleNameList);
    if (!continueBundleGot || bundleNameList.empty()) {
        if (BundleManagerInternal::GetLocalBundleInfo(continueInfo.sourceBundleName_, localBundleInfo) == ERR_OK) {
            bundleNameList.push_back(distributedBundleInfo.bundleName);
        } else {
            HILOGE("can not get local bundle info or continue bundle for bundle name: %{public}s",
                distributedBundleInfo.bundleName.c_str());
            return false;
        }
    }

    return CompleteSinkContinueInfo(continueInfo, distributedBundleInfo, srcContinueBundleNameSort, bundleNameList);
}
bool DSchedContinueInfoUtil::CompleteContinueInfo(std::string srcNetWorkId, std::string srcBundleName,
    std::string srcContinueType, DSchedContinueInfo &continueInfo, int32_t retryTimes)
{
    if (retryTimes >= DBMS_RETRY_MAX_TIME) {
        HILOGE("meet max retry time!");
        return false;
    }
    DmsBundleInfo distributedBundleInfo;
    if (!DmsBmStorage::GetInstance()->GetDistributedBundleInfo(
        srcNetWorkId, srcBundleName, distributedBundleInfo)) {
        HILOGW("get distributedBundleInfo failed, retryTimes = %{public}d", retryTimes);
        DmsKvSyncE2E::GetInstance()->PushAndPullData(srcNetWorkId);
        return CompleteContinueInfo(srcNetWorkId, srcBundleName, srcContinueType, continueInfo, retryTimes + 1);
    }

    std::vector<std::string> srcContinueBundleNameSort;
    if (!CompleteSrcContinueInfo(continueInfo, distributedBundleInfo, srcContinueType, srcContinueBundleNameSort)) {
        return false;
    }

    std::vector<std::string> bundleNameList;
    AppExecFwk::BundleInfo localBundleInfo;
    bool continueBundleGot = BundleManagerInternal::GetContinueBundle4Src(continueInfo.sourceBundleName_,
        bundleNameList);
    if (!continueBundleGot || bundleNameList.empty()) {
        if (BundleManagerInternal::GetLocalBundleInfo(continueInfo.sourceBundleName_, localBundleInfo) == ERR_OK) {
            bundleNameList.push_back(distributedBundleInfo.bundleName);
        } else {
            HILOGE("can not get local bundle info or continue bundle for bundle name: %{public}s",
                distributedBundleInfo.bundleName.c_str());
            return false;
        }
    }

    return CompleteSinkContinueInfo(continueInfo, distributedBundleInfo, srcContinueBundleNameSort, bundleNameList);
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
    HILOGD("compare is same continue type: they are %{public}s for types: %{public}s; %{public}s",
        isSameContinueType ? "the same" : "different", continueType1.c_str(), continueType2.c_str());
    return isSameContinueType;
}

bool DSchedContinueInfoUtil::CompleteSrcContinueInfo(DSchedContinueInfo &continueInfo,
    DmsBundleInfo &distributedBundleInfo, uint8_t srcContinueTypeId,
    std::vector<std::string> &srcContinueBundleNameSort)
{
    uint32_t pos = 0;
    bool abilityGot = false;
    for (auto dmsAbilityInfo: distributedBundleInfo.dmsAbilityInfos) {
        for (auto continueTypeElement: dmsAbilityInfo.continueType) {
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
    srcContinueBundleNameSort.push_back(continueInfo.sourceBundleName_);
}

bool DSchedContinueInfoUtil::CompleteSrcContinueInfo(DSchedContinueInfo &continueInfo,
    DmsBundleInfo &distributedBundleInfo, std::string srcContinueType,
    std::vector<std::string> &srcContinueBundleNameSort)
{
    bool abilityGot = false;
    for (auto dmsAbilityInfo: distributedBundleInfo.dmsAbilityInfos) {
        for (auto continueTypeElement: dmsAbilityInfo.continueType) {
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
    srcContinueBundleNameSort.push_back(continueInfo.sourceBundleName_);
}

bool DSchedContinueInfoUtil::CompleteSinkContinueInfo(DSchedContinueInfo &continueInfo,
    DmsBundleInfo &distributedBundleInfo,std::vector<std::string> &srcContinueBundleNameSort,
    std::vector<std::string> &bundleNameList)
{
    AppExecFwk::BundleInfo localBundleInfo;
    for (const auto &bundleNameInSrcConfig: srcContinueBundleNameSort) {
        if (std::find(bundleNameList.begin(), bundleNameList.end(), bundleNameInSrcConfig) == bundleNameList.end()) {
            continue;
        }
        if (BundleManagerInternal::GetLocalBundleInfo(bundleNameInSrcConfig, localBundleInfo) != ERR_OK) {
            continue;
        }
        if (bundleNameInSrcConfig == distributedBundleInfo.bundleName) {
            continueInfo.sinkBundleName_ = bundleNameInSrcConfig;
            break;
        }
        AppExecFwk::AppProvisionInfo appProvisionInfo;
        if (BundleManagerInternal::GetAppProvisionInfo4CurrentUser(bundleNameInSrcConfig, appProvisionInfo)
            && appProvisionInfo.developerId == distributedBundleInfo.developerId) {
            continueInfo.sinkBundleName_ = bundleNameInSrcConfig;
            break;
        }
    }

    std::vector<AbilityInfo> localAbilityInfos = localBundleInfo.abiliityInfos;
    bool diffModuleGot = false;
    for (const auto &abilityInfoElement: localAbilityInfos) {
        std::vector<std::string> continueTypes = abilityInfoElement.continueType;
        for (std::string &continueTypeElement: continueTypes) {
            if (!IsSameContinueType(continueTypeElement, continueInfo.continueType_)) {
                continue;
            }
            if (continueInfo.sourceModuleName_ == abilityInfoElement.moduleName) {
                continueInfo.sinkModuleName_ = abilityInfoElement.moduleName;
                continueInfo.sinkAbilityName_ = abilityInfoElement.name;
                return true;
            } else if (!diffModuleGot) {
                diffModuleGot = true;
                continueInfo.sinkModuleName_ = abilityInfoElement.moduleName;
                continueInfo.sinkAbilityName_ = abilityInfoElement.name;
                break;
            }
        }
    }
    return diffModuleGot;
}
}  // namespace DistributedSchedule
}  // namespace OHOS

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

#ifndef ABILITY_DMSFWK_1_DSCHED_CONTINUE_INFO_UTIL_H
#define ABILITY_DMSFWK_1_DSCHED_CONTINUE_INFO_UTIL_H

#include "dsched_continue.h"
#include "single_instance.h"

namespace OHOS {
namespace DistributedSchedule {
class DSchedContinueInfoUtil{
    DECLARE_SINGLE_INSTANCE(DSchedContinueInfoUtil);
public:
    bool CompleteContinueInfo(std::string srcNetWorkId, uint16_t srcBundleNameId,
        uint8_t srcContinueTypeId, DSchedContinueInfo &continueInfo, int32_t retryTimes = 0);
    bool CompleteContinueInfo(std::string srcNetWorkId, std::string srcBundleName,
        std::string srcContinueType, DSchedContinueInfo &continueInfo, int32_t retryTimes = 0);
    bool IsSameContinueType(std::string continueType1, std::string continueType2);

private:
    bool CompleteSrcContinueInfo(DSchedContinueInfo &continueInfo,
        DmsBundleInfo &distributedBundleInfo, uint8_t srcContinueTypeId,
        std::vector<std::string> &srcContinueBundleNameSort);
    bool CompleteSrcContinueInfo(DSchedContinueInfo &continueInfo,
        DmsBundleInfo &distributedBundleInfo, std::string srcContinueType,
        std::vector<std::string> &srcContinueBundleNameSort);
    bool CompleteSinkContinueInfo(DSchedContinueInfo &continueInfo,
        DmsBundleInfo &distributedBundleInfo,std::vector<std::string> &srcContinueBundleNameSort,
        std::vector<std::string> &bundleNameList);
};

}  // namespace DistributedSchedule
}  // namespace OHOS
#endif //ABILITY_DMSFWK_1_DSCHED_CONTINUE_INFO_UTIL_H

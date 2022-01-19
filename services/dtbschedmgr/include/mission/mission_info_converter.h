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

#ifndef DISTRIBUTEDSCHED_MISSION_INFO_CONVERTER_H
#define DISTRIBUTEDSCHED_MISSION_INFO_CONVERTER_H

#include <string>
#include <vector>

#include "mission_info.h"
#include "mission/distributed_mission_info.h"

namespace OHOS {
namespace DistributedSchedule {
class MissionInfoConverter {
public:
    static int32_t ConvertToDstbMissionInfos(std::vector<AAFwk::MissionInfo>& missionInfos,
        std::vector<DstbMissionInfo>& dstbMissionInfos);
    static int32_t ConvertToMissionInfos(std::vector<DstbMissionInfo>& dstbMissionInfos,
        std::vector<AAFwk::MissionInfo>& missionInfos);
    static bool ReadMissionInfosFromParcel(Parcel& parcel, std::vector<AAFwk::MissionInfo>& missionInfos);
    static bool WriteMissionInfosToParcel(Parcel& parcel, const std::vector<AAFwk::MissionInfo>& missionInfos);
};
}
}
#endif // DISTRIBUTEDSCHED_MISSION_INFO_CONVERTER_H

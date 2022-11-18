/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "mission/mission_info_converter.h"

#include "adapter/adapter_constant.h"
#include "mission/mission_constant.h"
#include "dtbschedmgr_log.h"
#include "parcel_helper.h"

using namespace std;
namespace OHOS {
namespace DistributedSchedule {
using namespace Constants::Adapter;
namespace {
const std::string TAG = "MissionInfoConverter";
}
int32_t MissionInfoConverter::ConvertToDstbMissionInfos(std::vector<AAFwk::MissionInfo>& missionInfos,
    std::vector<DstbMissionInfo>& dstbMissionInfos)
{
    if (missionInfos.empty()) {
        return ERR_OK;
    }
    for (auto iter = missionInfos.begin(); iter != missionInfos.end(); iter++) {
        DstbMissionInfo dstbMissionInfo;
        dstbMissionInfo.id  = iter->id;
        dstbMissionInfo.runingState = iter->runningState;
        dstbMissionInfo.lockedState = iter->lockedState;
        dstbMissionInfo.label = iter->label;
        dstbMissionInfo.iconPath = iter->iconPath;
        shared_ptr<AAFwk::Want> spWant = make_shared<AAFwk::Want>(iter->want);
        dstbMissionInfo.baseWant = spWant;
        dstbMissionInfo.continuable = iter->continuable ? 1 : 0;
        dstbMissionInfos.push_back(dstbMissionInfo);
    }
    return ERR_OK;
}

int32_t MissionInfoConverter::ConvertToMissionInfos(std::vector<DstbMissionInfo>& dstbMissionInfos,
    std::vector<AAFwk::MissionInfo>& missionInfos)
{
    if (dstbMissionInfos.empty()) {
        return ERR_OK;
    }
    for (auto iter = dstbMissionInfos.begin(); iter != dstbMissionInfos.end(); iter++) {
        AAFwk::MissionInfo missionInfo;
        missionInfo.id = iter->id;
        missionInfo.runningState = iter->runingState;
        missionInfo.lockedState = iter->lockedState;
        missionInfo.label = iter->label;
        missionInfo.iconPath = iter->iconPath;
        missionInfo.want = *(iter->baseWant);
        missionInfo.continuable = iter->continuable == 1;
        missionInfos.push_back(missionInfo);
    }
    return ERR_OK;
}

bool MissionInfoConverter::ReadMissionInfosFromParcel(Parcel& parcel,
    std::vector<AAFwk::MissionInfo>& missionInfos)
{
    int32_t empty = parcel.ReadInt32();
    if (empty == VALUE_OBJECT) {
        int32_t len = parcel.ReadInt32();
        HILOGD("ReadMissionInfosFromParcel::readLength is:%{public}d", len);
        if (len < 0) {
            return false;
        }
        size_t size = static_cast<size_t>(len);
        if ((size > parcel.GetReadableBytes()) || (size > Constants::Mission::GET_MAX_MISSIONS)) {
            HILOGE("Failed to read MissionInfo vector, size = %{public}zu", size);
            return false;
        }
        missionInfos.clear();
        for (size_t i = 0; i < size; i++) {
            AAFwk::MissionInfo *ptr = parcel.ReadParcelable<AAFwk::MissionInfo>();
            if (ptr == nullptr) {
                HILOGW("read MissionInfo failed");
                return false;
            }
            missionInfos.emplace_back(*ptr);
            delete ptr;
        }
    }

    HILOGI("ReadMissionInfosFromParcel end. info size is:%{public}zu", missionInfos.size());
    return true;
}

bool MissionInfoConverter::WriteMissionInfosToParcel(Parcel& parcel,
    const std::vector<AAFwk::MissionInfo>& missionInfos)
{
    size_t size = missionInfos.size();
    if (size == 0) {
        PARCEL_WRITE_HELPER_RET(parcel, Int32, VALUE_NULL, false);
        return true;
    }

    PARCEL_WRITE_HELPER_RET(parcel, Int32, VALUE_OBJECT, false);
    PARCEL_WRITE_HELPER_RET(parcel, Int32, size, false);
    for (auto& info : missionInfos) {
        PARCEL_WRITE_HELPER_RET(parcel, Parcelable, &info, false);
    }
    return true;
}
}
}

/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_DMS_CONTINUE_CONDITION_MANAGER_H
#define OHOS_DMS_CONTINUE_CONDITION_MANAGER_H

#include <atomic>
#include <map>
#include <mutex>
#include <string>

#include "ability_manager_client.h"
#include "single_instance.h"

namespace OHOS {
namespace DistributedSchedule {

struct MissionStatus {
    int32_t missionId;
    std::string bundleName;
    std::string abilityName;
    bool isContinuable;
    unsigned int launchFlag;

    bool isFocused;
    AAFwk::ContinueState continueState;

    std::string ToString() const
    {
        return "missionId: " + std::to_string(this->missionId) + " " +
            "bundleName: " + this->bundleName + " " +
            "abilityName: " + this->abilityName + " " +
            "isContinuable: "+ std::to_string(this->isContinuable) + " " +
            "launchFlag: " + std::to_string(this->launchFlag) + " " +
            "isFocused: " + std::to_string(this->isFocused) + " " +
            "continueState: " + std::to_string(this->continueState);
    }
};

typedef enum {
    MISSION_EVENT_INVALID = -1,
    MISSION_EVENT_FOCUSED = 0,
    MISSION_EVENT_UNFOCUSED = 1,
    MISSION_EVENT_DISTORYED = 2,
    MISSION_EVENT_ACTIVE = 4,
    MISSION_EVENT_INACTIVE = 5,
    MISSION_EVENT_TIMEOUT = 6,
    MISSION_EVENT_MMI = 7,
    MISSION_EVENT_MAX,
} MissionEventType;

typedef enum {
    SYS_EVENT_INVALID = -1,
    SYS_EVENT_CONTINUE_SWITCH = 0,
    SYS_EVENT_WIFI = 1,
    SYS_EVENT_SCREEN = 2,
    SYS_EVENT_MAX,
} SysEventType;

class DmsContinueConditionMgr {
DECLARE_SINGLE_INSTANCE_BASE(DmsContinueConditionMgr);

public:
    DmsContinueConditionMgr() = default;
    virtual ~DmsContinueConditionMgr() = default;

    static std::shared_ptr<DmsContinueConditionMgr> GetInstance();
    void Init();
    void UnInit();

    int32_t UpdateSystemStatus(SysEventType type, bool value);
    int32_t UpdateMissionStatus(int32_t accountId, int32_t missionId, MissionEventType type);

    void OnUserSwitched(int32_t accountId);
    void OnUserRemoved(int32_t accountId);

    bool CheckSystemSendCondition();
    bool CheckMissionSendCondition(const MissionStatus& status, MissionEventType type);
    int32_t GetCurrentFocusedMission(int32_t accountId);
    int32_t GetMissionStatus(int32_t accountId, int32_t missionId, MissionStatus& status);
    int32_t GetMissionIdByBundleName(int32_t accountId, const std::string& bundleName, int32_t& missionId);
    std::string TypeEnumToString(MissionEventType type);

private:
    void InitConditionFuncs();
    void InitSystemCondition();
    int32_t SetIsContinueSwitchOn(bool isSwitchOn);
    int32_t SetIsWifiActive(bool isWifiActive);
    int32_t OnScreenOffChanged(bool isScreenOff);

    void InitMissionCondition();
    void InitMissionStatusLocked(int32_t accountId);
    int32_t OnMissionFocusedLocked(int32_t accountId, int32_t missionId);
    int32_t OnMissionUnfocusedLocked(int32_t accountId, int32_t missionId);
    int32_t OnMissionDestoryLocked(int32_t accountId, int32_t missionId);
    int32_t OnMissionActiveLocked(int32_t accountId, int32_t missionId);
    int32_t OnMissionInactiveLocked(int32_t accountId, int32_t missionId);
    int32_t OnUserSwitchedLocked(int32_t accountId, int32_t missionId);
    int32_t OnUserDestoryLocked(int32_t accountId, int32_t missionId);

    bool CheckSendFocusedCondition(const MissionStatus& status);
    bool CheckSendUnfocusedCondition(const MissionStatus& status);
    bool CheckSendActiveCondition(const MissionStatus& status);
    bool CheckSendInactiveCondition(const MissionStatus& status);

    int32_t GetMissionInfo(int32_t missionId, AAFwk::MissionInfo& info);
    int32_t GetMissionInfos(std::vector<AAFwk::MissionInfo>& missions);

    void ConvertToMissionStatus(const AAFwk::MissionInfo& missionInfo, MissionStatus& status);
    bool IsMissionStatusExistLocked(int32_t accountId, int32_t missionId);

    std::atomic<bool> isCfgDevice_ = false;
    std::atomic<bool> isSwitchOn_ = false;
    std::atomic<bool> isWifiActive_ = false;
    std::atomic<bool> isScreenOff_ = false;

    using DSchedSysEventFunc = int32_t (DmsContinueConditionMgr::*)(bool value);
    std::map<SysEventType, DSchedSysEventFunc> sysFuncMap_;
    using DSchedMissionEventFunc = int32_t (DmsContinueConditionMgr::*)(int32_t accountId, int32_t missionId);
    std::map<MissionEventType, DSchedMissionEventFunc> missionFuncMap_;
    using DSchedSendConditionFunc = bool (DmsContinueConditionMgr::*)(const MissionStatus& status);
    std::map<MissionEventType, DSchedSendConditionFunc> conditionFuncMap_;

    std::mutex missionMutex_;
    std::map<int32_t, std::map<int32_t, MissionStatus>> missionMap_;
};
} // namespace DistributedSchedule
} // namespace OHOS
#endif // OHOS_DMS_CONTINUE_CONDITION_MANAGER_H

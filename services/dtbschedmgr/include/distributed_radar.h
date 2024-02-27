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

#ifndef OHOS_DISTRIBUTED_RADAR_H
#define OHOS_DISTRIBUTED_RADAR_H

#include <cstdint>
#include <chrono>
#include <string>
#include <vector>

#include "single_instance.h"

namespace OHOS {
namespace DistributedSchedule {
const std::string ORGPKGNAME = "ohos.distributedschedule";
const std::string DISTRIBUTED_SCHEDULE = "DISTRIBUTED_SCHEDULE";
const std::string AMS = "ohos.abilitymanagerservice";
const std::string NORMAL = "normal";
const std::string MULTIMODE = "multimode";
const std::string FOCUSED = "focused";
const std::string UNFOCUESD = "unfocused";
const std::string BMS = "bundlename";//BMS模块名
const std::string DBMS = "distribuedbundlename";//DBMS模块名
const std::string SOFTBUS = "";//软总线模块名
constexpr int32_t SUBSYS_DISTRIBUTEDHARDWARE_DM = 14;
constexpr int32_t INVALID_UDID_LENGTH = 10;
constexpr int32_t SUBSTR_UDID_LENGTH = 5;
enum class BizScene : int32_t {
    DMS_INIT = 1,
    NORMAL_MULTIMODE_FOCUSED  = 2,
    CHANGE_STATE_FOCUSED = 3,
    NORMAL_UNFOCUSED = 4,
    MULTIMODE_UNFOCUSED = 5,
    CHANGE_STATE_UNFOCUSED = 6,
    RECV_FOCUSED = 7,
    RECV_UNFOCUSED = 8,
    CLICK_ICON = 9;
    SAVE_DATA = 10;

    ChangeStateUnfocused ClickIcon  SaveData  = 2,
};

enum class StageRes : int32_t {
    STAGE_IDLE = 0,
    STAGE_SUCC = 1,
    STAGE_FAIL = 2,
    STAGE_CANCEL = 3,
    STAGE_UNKNOW = 4,
};

enum class BizState : int32_t {
    BIZ_STATE_START = 1,
    BIZ_STATE_END = 2,
};

enum class DmsInit : int32_t {
    REGISTER_SOFTBUS_CALLBACK = 1,
    REGISTER_AMS_FOCUSED = 2,
};

enum class NormalOrMultimodeFocused : int32_t {
    DMS_FOCUSED = 1,
    GET_BMS_ACCESSTOKENID = 2,
    SOFTBUS_SENDEVENT = 3,
};

enum class ChangeStateFocused : int32_t {
    SET_STATE = 1,
    GET_BMS_ACCESSTOKENID = 2,
    SOFTBUS_SENDEVENT = 3,
};

enum class NormalUnfocused : int32_t {
    DMS_UNFOCUSED = 1,
    GET_BMS_ACCESSTOKENID = 2,
    SOFTBUS_SENDEVENT = 3,
};

enum class MultimodeUnfocused : int32_t {
    RECORD_TIME = 1,
    GET_ACCESSTOKENID = 2,
    SOFTBUS_SENDEVENT = 3,
};

enum class ChangeStateUnfocused : int32_t {
    SET_STATE = 1,
    GET_BMS_ACCESSTOKENID = 2,
    SOFTBUS_SENDEVENT = 3,
};

enum class RecvFocused : int32_t {
    RECV_FOCUSED = 1,
    GET_DBMS_BUNDLENAME = 2,
    NOTIFY_DOCK_FOCUSED = 3,
};

enum class RecvUnfocused : int32_t {
    RECV_UNFOCUSED = 1,
    GET_DBMS_BUNDLENAME = 2,
    NOTIFY_DOCK_UNFOCUSED = 3,
};

enum class ClickIcon : int32_t {
    DMS_CONTINUE = 1,
    DMS_STARTABILITY = 2,
    DMS_RECV_OVER = 3,
};

enum class SaveData : int32_t {
    DMS_CONTINUE = 1,
    APP_SAVE_DATA = 2,
    DMS_REMOTE_WANT = 3,
};

enum class Module : int32_t {
    DMS_CONTINUE = 0,
}; 

class DmsRadar {
    DECLARE_SINGLE_INSTANCE(DmsRadar);
public:
    bool RegisterSoftbusCallbackRes(string func, ErrCode errCode) override;
    bool RegisterFocusedRes(string func, ErrCode errCode) override;
    void DmsFocused(struct RadarInfo &info) override;
    bool NormalFocusedGetAccessTokenIdRes(std::string peerUdid) override;
    bool NormalFocusedSendEventRes(struct RadarInfo &info) override;
    void SetFocusedState(struct RadarInfo &info) override;
    bool ChangeStateFocusedGetAccessTokenIdRes(struct RadarInfo &info) override;
    bool ChangeStateFocusedSendEventRes(struct RadarInfo &info) override;
    void DmsUnfocused(struct RadarInfo &info) override;
    bool NormalUnfocusedGetAccessTokenIdRes(struct RadarInfo &info) override;
    bool NormalUnfocusedSendEventRes(std::string funcName, int32_t stageRes) override;
    bool RecordTime(struct RadarInfo &info) override;
    bool MultimodeUnfocusedGetAccessTokenIdRes(struct RadarInfo &info) override;
    bool MultimodeUnfocusedSendEventRes(std::string funcName, int32_t stageRes) override;
    void SetUnfocusedState(struct RadarInfo &info) override;
    bool ChangeStateUnfocusedGetAccessTokenIdRes(struct RadarInfo &info) override;
    bool ChangeStateUnfocusedSendEventRes(struct RadarInfo &info) override;
    bool RecvFocused(struct RadarInfo &info) override;
    bool FocusedGetBundleName(struct RadarInfo &info) override;
    bool NotifyDockFocused(struct RadarInfo &info) override;
    bool RecvUnfocused(std::string funcName, int32_t stageRes) override;
    bool UnfocusedGetBundleName(struct RadarInfo &info) override;
    bool NotifyDockUnfocused(struct RadarInfo &info) override;
    bool ClickIconDmsContinue(struct RadarInfo &info) override;
    bool ClickIconDmsStartAbility(struct RadarInfo &info) override;
    bool ClickIconDmsRecvOver(struct RadarInfo &info) override;
    bool SaveDataDmsContinue(struct RadarInfo &info) override;
    bool SaveDataDmsRemoteWant(struct RadarInfo &info) override;
private:
    std::string GetAnonyUdid(std::string udid);
    std::string GetLocalUdid();
};
} // namespace DistributedSchedule
} // namespace OHOS
#endif // OHOS_DISTRIBUTED_RADAR_H
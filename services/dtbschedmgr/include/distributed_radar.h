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
const std::string UNFOCUSED = "unfocused";
const std::string BMS = "bundlename";//BMS模块名
const std::string DBMS = "distribuedbundlename";//DBMS模块名
const std::string SOFTBUS = "";//软总线模块名
constexpr int32_t SUBSYS_DISTRIBUTEDHARDWARE_DM = 14;
constexpr int32_t INVALID_UDID_LENGTH = 10;
constexpr int32_t SUBSTR_UDID_LENGTH = 5;
enum class BizScene : int32_t {
    DMS_INIT = 1,
    NORMAL_MULTIMODE_FOCUSED  = 3,
    CHANGE_STATE_FOCUSED = 4,
    NORMAL_UNFOCUSED = 5,
    MULTIMODE_UNFOCUSED = 6,
    CHANGE_STATE_UNFOCUSED = 7,
    RECV_FOCUSED = 8,
    RECV_UNFOCUSED = 9,
    CLICK_ICON = 10,
    SAVE_DATA = 11,
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
    DMS_CONTINUE = 4,
    DMS_STARTABILITY = 5,
    DMS_RECV_OVER = 8,
};

enum class SaveData : int32_t {
    DMS_CONTINUE = 1,
    DMS_REMOTE_WANT = 5,
};

enum class Module : int32_t {
    DMS_CONTINUE = 0,
}; 

class DmsRadar {
    DECLARE_SINGLE_INSTANCE(DmsRadar);
public:
    bool RegisterSoftbusCallbackRes(std::string func, int32_t errCode);
    bool RegisterFocusedRes(std::string func, int32_t errCode);
    bool DmsFocused(std::string func, std::string focusMode);
    bool NormalFocusedGetAccessTokenIdRes(std::string func, int32_t errCode);
    bool NormalFocusedSendEventRes(std::string func, int32_t errCode) ;
    bool SetFocusedState(std::string func);
    bool ChangeStateFocusedGetAccessTokenIdRes(std::string func, int32_t errCode);
    bool ChangeStateFocusedSendEventRes(std::string func, int32_t errCode);
    bool DmsUnfocused(std::string func);
    bool NormalUnfocusedGetAccessTokenIdRes(std::string func, int32_t errCode);
    bool NormalUnfocusedSendEventRes(std::string func,int32_t errCode);
    bool RecordTime(std::string func);
    bool MultimodeUnfocusedGetAccessTokenIdRes(std::string func);
    bool MultimodeUnfocusedSendEventRes(std::string func, int32_t errCode);
    bool SetUnfocusedState(std::string func);
    bool ChangeStateUnfocusedGetAccessTokenIdRes(std::string func, int32_t errCode);
    bool ChangeStateUnfocusedSendEventRes(std::string func, int32_t errCode);
    bool RecvFocused(std::string func);
    bool FocusedGetBundleName(std::string func, int32_t errCode);
    bool NotifyDockFocused(std::string func, int32_t errCode);
    bool RecvUnfocused(std::string func);
    bool UnfocusedGetBundleName(std::string func, int32_t errCode);
    bool NotifyDockUnfocused(std::string func, int32_t errCode);
    bool ClickIconDmsContinue(std::string func, int32_t errCode);
    bool ClickIconDmsStartAbility(std::string func, int32_t errCode);
    bool ClickIconDmsRecvOver(std::string func, int32_t errCode);
    bool SaveDataDmsContinue(std::string func, int32_t errCode);
    bool SaveDataDmsRemoteWant(std::string func, int32_t errCode);
};
} // namespace DistributedSchedule
} // namespace OHOS
#endif // OHOS_DISTRIBUTED_RADAR_H
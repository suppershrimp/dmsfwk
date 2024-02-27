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

#include "distributed_radar.h"

#include "hisysevent.h"

namespace OHOS {
namespace DistributedSchedule {
const std::string TAG = "DmsRadar";
IMPLEMENT_SINGLE_INSTANCE(DmsRadar);
bool DmsRadar::RegisterSoftbusCallbackRes(std::string func, int32_t errCode)
{
    int32_t res = ERR_OK;
    int32_t stageRes = (errCode == ERR_OK) ? static_cast<int32_t>(StageRes::STAGE_SUCC) : static_cast<int32_t>(StageRes::STAGE_FAIL);
    if (stageRes == static_cast<int32_t>(StageRes::STAGE_SUCC)) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::DMS_INIT,
            "BIZ_STAGE", DmsInit::REGISTER_SOFTBUS_CALLBACK,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
            "BIZ_STATE", BizState::BIZ_STATE_START);
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::DMS_INIT,
            "BIZ_STAGE", DmsInit::REGISTER_SOFTBUS_CALLBACK,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_FAIL),
            "BIZ_STATE", BizState::BIZ_STATE_START,
            "ERROR_CODE", errCode);
    }
    if (res != ERR_OK) {
        HILOGE("RegisterSoftbusCallbackRes error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::RegisterFocusedRes(std::string func, int32_t errCode)
{
    int32_t res = ERR_OK;
    int32_t stageRes = (errCode == ERR_OK) ? static_cast<int32_t>(StageRes::STAGE_SUCC) : static_cast<int32_t>(StageRes::STAGE_FAIL);
    if (stageRes == static_cast<int32_t>(StageRes::STAGE_SUCC)) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::DMS_INIT,
            "BIZ_STAGE", DmsInit::REGISTER_AMS_FOCUSED,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "TO_CALL_PKG", AMS);
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::DMS_INIT,
            "BIZ_STAGE", DmsInit::REGISTER_AMS_FOCUSED,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_FAIL),
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "TO_CALL_PKG", AMS,
            "ERROR_CODE", errCode);
    }
    if (res != ERR_OK) {
        HILOGE("RegisterFocusedRes error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::DmsFocused(std::string func, std::string focusMode)
{
    int32_t res = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
        DISTRIBUTED_SCHEDULE,
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ORG_PKG", ORGPKGNAME,
        "FUNC", func,
        "BIZ_SCENE", BizScene::NORMAL_MULTIMODE_FOCUSED,
        "BIZ_STAGE", NormalOrMultimodeFocused::DMS_FOCUSED,
        "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
        "BIZ_STATE", BizState::BIZ_STATE_START,
        "FOCUS_MODE", focusMode);
    if (res != ERR_OK) {
        HILOGE("DmsFocused error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::NormalFocusedGetAccessTokenIdRes(std::string func, int32_t errCode)
{
    int32_t res = ERR_OK;
    int32_t stageRes = (errCode == ERR_OK) ? static_cast<int32_t>(StageRes::STAGE_SUCC) : static_cast<int32_t>(StageRes::STAGE_FAIL);
    if (stageRes == static_cast<int32_t>(StageRes::STAGE_SUCC)) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::NORMAL_MULTIMODE_FOCUSED,
            "BIZ_STAGE", NormalOrMultimodeFocused::GET_BMS_ACCESSTOKENID,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
            "TO_CALL_PKG", BMS);
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::NORMAL_MULTIMODE_FOCUSED,
            "BIZ_STAGE", NormalOrMultimodeFocused::GET_BMS_ACCESSTOKENID,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_FAIL),
            "TO_CALL_PKG", BMS,
            "ERROR_CODE", errCode);
    }
    if (res != ERR_OK) {
        HILOGE("NormalFocusedGetAccessTokenIdRes error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::NormalFocusedSendEventRes(std::string func, int32_t errCode)
{
    int32_t res = ERR_OK;
    int32_t stageRes = (errCode == ERR_OK) ? static_cast<int32_t>(StageRes::STAGE_SUCC) : static_cast<int32_t>(StageRes::STAGE_FAIL);
    if (stageRes == static_cast<int32_t>(StageRes::STAGE_SUCC)) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::NORMAL_MULTIMODE_FOCUSED,
            "BIZ_STAGE", NormalOrMultimodeFocused::SOFTBUS_SENDEVENT,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "TO_CALL_PKG", SOFTBUS);
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::NORMAL_MULTIMODE_FOCUSED,
            "BIZ_STAGE", NormalOrMultimodeFocused::SOFTBUS_SENDEVENT,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_FAIL),
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "TO_CALL_PKG", SOFTBUS,
            "ERROR_CODE", errCode);
    }
    if (res != ERR_OK) {
        HILOGE("NormalFocusedSendEventRes error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::SetFocusedState(std::string func)
{
    int32_t res = ERR_OK;
    res = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
        DISTRIBUTED_SCHEDULE,
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ORG_PKG", ORGPKGNAME,
        "FUNC", func,
        "BIZ_SCENE", BizScene::CHANGE_STATE_FOCUSED,
        "BIZ_STAGE", ChangeStateFocused::SET_STATE,
        "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
        "BIZ_STATE", BizState::BIZ_STATE_START);
    if (res != ERR_OK) {
        HILOGE("SetFocusedState error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::ChangeStateFocusedGetAccessTokenIdRes(std::string func, int32_t errCode)
{
    int32_t res = ERR_OK;
    int32_t stageRes = (errCode == ERR_OK) ? static_cast<int32_t>(StageRes::STAGE_SUCC) : static_cast<int32_t>(StageRes::STAGE_FAIL);
    if (stageRes == static_cast<int32_t>(StageRes::STAGE_SUCC)) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CHANGE_STATE_FOCUSED,
            "BIZ_STAGE", ChangeStateFocused::GET_BMS_ACCESSTOKENID,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
            "TO_CALL_PKG", BMS);
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CHANGE_STATE_FOCUSED,
            "BIZ_STAGE", ChangeStateFocused::GET_BMS_ACCESSTOKENID,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_FAIL),
            "TO_CALL_PKG", BMS,
            "ERROR_CODE", errCode);
    }
    if (res != ERR_OK) {
        HILOGE("ChangeStateFocusedGetAccessTokenIdRes error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::ChangeStateFocusedSendEventRes(std::string func, int32_t errCode)
{
    int32_t res = ERR_OK;
    int32_t stageRes = (errCode == ERR_OK) ? static_cast<int32_t>(StageRes::STAGE_SUCC) : static_cast<int32_t>(StageRes::STAGE_FAIL);
    if (stageRes == static_cast<int32_t>(StageRes::STAGE_SUCC)) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CHANGE_STATE_FOCUSED,
            "BIZ_STAGE", ChangeStateFocused::SOFTBUS_SENDEVENT,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "TO_CALL_PKG", SOFTBUS);
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CHANGE_STATE_FOCUSED,
            "BIZ_STAGE", ChangeStateFocused::SOFTBUS_SENDEVENT,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_FAIL),
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "TO_CALL_PKG", SOFTBUS,
            "ERROR_CODE", errCode);
    }
    if (res != ERR_OK) {
        HILOGE("ChangeStateFocusedSendEventRes error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::DmsUnfocused(std::string func)
{
    int32_t res = ERR_OK;
    res = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
        DISTRIBUTED_SCHEDULE,
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ORG_PKG", ORGPKGNAME,
        "FUNC", func,
        "BIZ_SCENE", BizScene::NORMAL_UNFOCUSED,
        "BIZ_STAGE", NormalUnfocused::DMS_UNFOCUSED,
        "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
        "BIZ_STATE", BizState::BIZ_STATE_START);
    if (res != ERR_OK) {
        HILOGE("DmsUnfocused error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::NormalUnfocusedGetAccessTokenIdRes(std::string func, int32_t errCode)
{
    int32_t res = ERR_OK;
    int32_t stageRes = (errCode == ERR_OK) ? static_cast<int32_t>(StageRes::STAGE_SUCC) : static_cast<int32_t>(StageRes::STAGE_FAIL);
    if (stageRes == static_cast<int32_t>(StageRes::STAGE_SUCC)) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::NORMAL_UNFOCUSED,
            "BIZ_STAGE", NormalUnfocused::GET_BMS_ACCESSTOKENID,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
            "TO_CALL_PKG", BMS);
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::NORMAL_UNFOCUSED,
            "BIZ_STAGE", NormalUnfocused::GET_BMS_ACCESSTOKENID,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_FAIL),
            "TO_CALL_PKG", BMS,
            "ERROR_CODE", errCode);
    }
    if (res != ERR_OK) {
        HILOGE("NormalUnfocusedGetAccessTokenIdRes error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::NormalUnfocusedSendEventRes(std::string func,int32_t errCode)
{
    int32_t res = ERR_OK;
    int32_t stageRes = (errCode == ERR_OK) ? static_cast<int32_t>(StageRes::STAGE_SUCC) : static_cast<int32_t>(StageRes::STAGE_FAIL);
    if (stageRes == static_cast<int32_t>(StageRes::STAGE_SUCC)) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::NORMAL_UNFOCUSED,
            "BIZ_STAGE", NormalUnfocused::SOFTBUS_SENDEVENT,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "TO_CALL_PKG", SOFTBUS);
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::NORMAL_UNFOCUSED,
            "BIZ_STAGE", NormalUnfocused::SOFTBUS_SENDEVENT,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_FAIL),
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "TO_CALL_PKG", SOFTBUS,
            "ERROR_CODE", errCode);
    }
    if (res != ERR_OK) {
        HILOGE("NormalUnfocusedSendEventRes error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::RecordTime(std::string func)
{
    int32_t res = ERR_OK;
    res = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
        DISTRIBUTED_SCHEDULE,
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ORG_PKG", ORGPKGNAME,
        "FUNC", func,
        "BIZ_SCENE", BizScene::MULTIMODE_UNFOCUSED,
        "BIZ_STAGE", MultimodeUnfocused::RECORD_TIME,
        "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
        "BIZ_STATE", BizState::BIZ_STATE_START);
    if (res != ERR_OK) {
        HILOGE("RecordTime error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::MultimodeUnfocusedGetAccessTokenIdRes(std::string func)
{
    int32_t res = ERR_OK;
    res = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
        DISTRIBUTED_SCHEDULE,
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ORG_PKG", ORGPKGNAME,
        "FUNC", func,
        "BIZ_SCENE", BizScene::MULTIMODE_UNFOCUSED,
        "BIZ_STAGE", MultimodeUnfocused::GET_ACCESSTOKENID,
        "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC));
    if (res != ERR_OK) {
        HILOGE("RecordTime error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::MultimodeUnfocusedSendEventRes(std::string func, int32_t errCode)
{
    int32_t res = ERR_OK;
    int32_t stageRes = (errCode == ERR_OK) ? static_cast<int32_t>(StageRes::STAGE_SUCC) : static_cast<int32_t>(StageRes::STAGE_FAIL);
    if (stageRes == static_cast<int32_t>(StageRes::STAGE_SUCC)) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::MULTIMODE_UNFOCUSED,
            "BIZ_STAGE", MultimodeUnfocused::SOFTBUS_SENDEVENT,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "TO_CALL_PKG", SOFTBUS);
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::MULTIMODE_UNFOCUSED,
            "BIZ_STAGE", MultimodeUnfocused::SOFTBUS_SENDEVENT,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_FAIL),
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "TO_CALL_PKG", SOFTBUS,
            "ERROR_CODE", errCode);
    }
    if (res != ERR_OK) {
        HILOGE("MultimodeUnfocusedSendEventRes error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::SetUnfocusedState(std::string func)
{
    int32_t res = ERR_OK;
    res = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
        DISTRIBUTED_SCHEDULE,
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ORG_PKG", ORGPKGNAME,
        "FUNC", func,
        "BIZ_SCENE", BizScene::CHANGE_STATE_UNFOCUSED,
        "BIZ_STAGE", ChangeStateUnfocused::SET_STATE,
        "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
        "BIZ_STATE", BizState::BIZ_STATE_START);
    if (res != ERR_OK) {
        HILOGE("SetFocusedState error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::ChangeStateUnfocusedGetAccessTokenIdRes(std::string func, int32_t errCode)
{
    int32_t res = ERR_OK;
    int32_t stageRes = (errCode == ERR_OK) ? static_cast<int32_t>(StageRes::STAGE_SUCC) : static_cast<int32_t>(StageRes::STAGE_FAIL);
    if (stageRes == static_cast<int32_t>(StageRes::STAGE_SUCC)) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CHANGE_STATE_UNFOCUSED,
            "BIZ_STAGE", ChangeStateUnfocused::GET_BMS_ACCESSTOKENID,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
            "TO_CALL_PKG", BMS);
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CHANGE_STATE_UNFOCUSED,
            "BIZ_STAGE", ChangeStateUnfocused::GET_BMS_ACCESSTOKENID,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_FAIL),
            "TO_CALL_PKG", BMS,
            "ERROR_CODE", errCode);
    }
    if (res != ERR_OK) {
        HILOGE("ChangeStateUnfocusedGetAccessTokenIdRes error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::ChangeStateUnfocusedSendEventRes(std::string func, int32_t errCode)
{
    int32_t res = ERR_OK;
    int32_t stageRes = (errCode == ERR_OK) ? static_cast<int32_t>(StageRes::STAGE_SUCC) : static_cast<int32_t>(StageRes::STAGE_FAIL);
    if (stageRes == static_cast<int32_t>(StageRes::STAGE_SUCC)) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CHANGE_STATE_UNFOCUSED,
            "BIZ_STAGE", ChangeStateUnfocused::SOFTBUS_SENDEVENT,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "TO_CALL_PKG", SOFTBUS);
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CHANGE_STATE_UNFOCUSED,
            "BIZ_STAGE", ChangeStateUnfocused::SOFTBUS_SENDEVENT,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_FAIL),
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "TO_CALL_PKG", SOFTBUS,
            "ERROR_CODE", errCode);
    }
    if (res != ERR_OK) {
        HILOGE("ChangeStateUnfocusedSendEventRes error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::RecvFocused(std::string func)
{
    int32_t res = ERR_OK;
    res = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
        DISTRIBUTED_SCHEDULE,
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ORG_PKG", ORGPKGNAME,
        "FUNC", func,
        "BIZ_SCENE", BizScene::RECV_FOCUSED,
        "BIZ_STAGE", RecvFocused::RECV_FOCUSED,
        "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
        "BIZ_STATE", BizState::BIZ_STATE_START,
        "BROADCAST_INFO", FOCUSED,
        "HOST_PKG", SOFTBUS);
    if (res != ERR_OK) {
        HILOGE("RecvFocused error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::FocusedGetBundleName(std::string func, int32_t errCode)
{
    int32_t res = ERR_OK;
    int32_t stageRes = (errCode == ERR_OK) ? static_cast<int32_t>(StageRes::STAGE_SUCC) : static_cast<int32_t>(StageRes::STAGE_FAIL);
    if (stageRes == static_cast<int32_t>(StageRes::STAGE_SUCC)) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::RECV_FOCUSED,
            "BIZ_STAGE", RecvFocused::GET_DBMS_BUNDLENAME,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
            "TO_CALL_PKG", DBMS);
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::RECV_FOCUSED,
            "BIZ_STAGE", RecvFocused::GET_DBMS_BUNDLENAME,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_FAIL),
            "TO_CALL_PKG", DBMS,
            "ERROR_CODE", errCode);
    }
    if (res != ERR_OK) {
        HILOGE("FocusedGetBundleName error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::NotifyDockFocused(std::string func, int32_t errCode)
{
    int32_t res = ERR_OK;
    int32_t stageRes = (errCode == ERR_OK) ? static_cast<int32_t>(StageRes::STAGE_SUCC) : static_cast<int32_t>(StageRes::STAGE_FAIL);
    if (stageRes == static_cast<int32_t>(StageRes::STAGE_SUCC)) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::RECV_FOCUSED,
            "BIZ_STAGE", RecvFocused::NOTIFY_DOCK_FOCUSED,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC));
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::RECV_FOCUSED,
            "BIZ_STAGE", RecvFocused::NOTIFY_DOCK_FOCUSED,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_FAIL),
            "ERROR_CODE", errCode);
    }
    if (res != ERR_OK) {
        HILOGE("NotifyDockFocused error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::RecvUnfocused(std::string func)
{
    int32_t res = ERR_OK;
    res = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
        DISTRIBUTED_SCHEDULE,
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ORG_PKG", ORGPKGNAME,
        "FUNC", func,
        "BIZ_SCENE", BizScene::RECV_UNFOCUSED,
        "BIZ_STAGE", RecvUnfocused::RECV_UNFOCUSED,
        "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
        "BIZ_STATE", BizState::BIZ_STATE_START,
        "BROADCAST_INFO", UNFOCUSED,
        "TO_CALL_PKG", SOFTBUS);
    if (res != ERR_OK) {
        HILOGE("RecvUnfocused error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::UnfocusedGetBundleName(std::string func, int32_t errCode)
{
    int32_t res = ERR_OK;
    int32_t stageRes = (errCode == ERR_OK) ? static_cast<int32_t>(StageRes::STAGE_SUCC) : static_cast<int32_t>(StageRes::STAGE_FAIL);
    if (stageRes == static_cast<int32_t>(StageRes::STAGE_SUCC)) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::RECV_UNFOCUSED,
            "BIZ_STAGE", RecvUnfocused::GET_DBMS_BUNDLENAME,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
            "TO_CALL_PKG", DBMS);
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::RECV_UNFOCUSED,
            "BIZ_STAGE", RecvUnfocused::GET_DBMS_BUNDLENAME,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_FAIL),
            "TO_CALL_PKG", DBMS,
            "ERROR_CODE", errCode);
    }
    if (res != ERR_OK) {
        HILOGE("UnfocusedGetBundleName error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::NotifyDockUnfocused(std::string func, int32_t errCode)
{
    int32_t res = ERR_OK;
    int32_t stageRes = (errCode == ERR_OK) ? static_cast<int32_t>(StageRes::STAGE_SUCC) : static_cast<int32_t>(StageRes::STAGE_FAIL);
    if (stageRes == static_cast<int32_t>(StageRes::STAGE_SUCC)) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::RECV_UNFOCUSED,
            "BIZ_STAGE", RecvUnfocused::NOTIFY_DOCK_UNFOCUSED,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC));
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::RECV_UNFOCUSED,
            "BIZ_STAGE", RecvUnfocused::NOTIFY_DOCK_UNFOCUSED,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_FAIL),
            "ERROR_CODE", errCode);
    }
    if (res != ERR_OK) {
        HILOGE("NotifyDockUnfocused error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::ClickIconDmsContinue(std::string func, int32_t errCode)
{   
    int32_t res = ERR_OK;
    int32_t stageRes = (errCode == ERR_OK) ? static_cast<int32_t>(StageRes::STAGE_SUCC) : static_cast<int32_t>(StageRes::STAGE_FAIL);
    if (stageRes == static_cast<int32_t>(StageRes::STAGE_SUCC)) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CLICK_ICON,
            "BIZ_STAGE", ClickIcon::DMS_CONTINUE,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC));
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CLICK_ICON,
            "BIZ_STAGE", ClickIcon::DMS_CONTINUE,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_FAIL),
            "ERROR_CODE", errCode);
    }
    if (res != ERR_OK) {
        HILOGE("ClickIconDmsContinue error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::ClickIconDmsStartAbility(std::string func, int32_t errCode)
{
    int32_t res = ERR_OK;
    int32_t stageRes = (errCode == ERR_OK) ? static_cast<int32_t>(StageRes::STAGE_SUCC) : static_cast<int32_t>(StageRes::STAGE_FAIL);
    if (stageRes == static_cast<int32_t>(StageRes::STAGE_SUCC)) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CLICK_ICON,
            "BIZ_STAGE", ClickIcon::DMS_STARTABILITY,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
            "TO_CALL_PKG", AMS);
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CLICK_ICON,
            "BIZ_STAGE", ClickIcon::DMS_STARTABILITY,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_FAIL),
            "TO_CALL_PKG", AMS,
            "ERROR_CODE", errCode);
    }
    if (res != ERR_OK) {
        HILOGE("ClickIconDmsStartAbility error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::ClickIconDmsRecvOver(std::string func, int32_t errCode)
{
    int32_t res = ERR_OK;
    int32_t stageRes = (errCode == ERR_OK) ? static_cast<int32_t>(StageRes::STAGE_SUCC) : static_cast<int32_t>(StageRes::STAGE_FAIL);
    if (stageRes == static_cast<int32_t>(StageRes::STAGE_SUCC)) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CLICK_ICON,
            "BIZ_STAGE", ClickIcon::DMS_RECV_OVER,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
            "BIZ_STATE", BizState::BIZ_STATE_END);
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CLICK_ICON,
            "BIZ_STAGE", ClickIcon::DMS_RECV_OVER,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_FAIL),
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "ERROR_CODE", errCode);
    }
    if (res != ERR_OK) {
        HILOGE("ClickIconDmsRecvOver error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::SaveDataDmsContinue(std::string func, int32_t errCode)
{
    int32_t res = ERR_OK;
    int32_t stageRes = (errCode == ERR_OK) ? static_cast<int32_t>(StageRes::STAGE_SUCC) : static_cast<int32_t>(StageRes::STAGE_FAIL);
    if (stageRes == static_cast<int32_t>(StageRes::STAGE_SUCC)) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::SAVE_DATA,
            "BIZ_STAGE", SaveData::DMS_CONTINUE,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
            "BIZ_STATE", BizState::BIZ_STATE_START,
            "TO_CALL_PKG", AMS);
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::SAVE_DATA,
            "BIZ_STAGE", SaveData::DMS_CONTINUE,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_FAIL),
            "BIZ_STATE", BizState::BIZ_STATE_START,
            "TO_CALL_PKG", AMS,
            "ERROR_CODE", errCode);
    }
    if (res != ERR_OK) {
        HILOGE("SaveDataDmsContinue error, res:%{public}d", res);
        return false;
    }
    return true;
}

bool DmsRadar::SaveDataDmsRemoteWant(std::string func, int32_t errCode)
{
    int32_t res = ERR_OK;
    int32_t stageRes = (errCode == ERR_OK) ? static_cast<int32_t>(StageRes::STAGE_SUCC) : static_cast<int32_t>(StageRes::STAGE_FAIL);
    if (stageRes == static_cast<int32_t>(StageRes::STAGE_SUCC)) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::SAVE_DATA,
            "BIZ_STAGE", SaveData::DMS_REMOTE_WANT,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_SUCC),
            "BIZ_STATE", BizState::BIZ_STATE_END);
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::SAVE_DATA,
            "BIZ_STAGE", SaveData::DMS_REMOTE_WANT,
            "STAGE_RES", static_cast<int32_t>(StageRes::STAGE_FAIL),
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "ERROR_CODE", errCode);
    }
    if (res != ERR_OK) {
        HILOGE("SaveDataDmsRemoteWant error, res:%{public}d", res);
        return false;
    }
    return true;
}
} // namespace DistributedSchedule
} // namespace OHOS
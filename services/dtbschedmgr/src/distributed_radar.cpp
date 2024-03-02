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
IMPLEMENT_SINGLE_INSTANCE(DmsRadar);
bool DmsRadar::RegisterSoftbusCallbackRes(string func, inte32_t errCode)
{
    int32_t res = DMS_OK;
    int32_t stageRes = (errCode == ERR_OK) ? StageRes::STAGE_SUCC : StageRes::STAGE_FAIL;
    if (stageRes == StageRes::STAGE_SUCC) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::DMS_INIT,
            "BIZ_STAGE", DmsInit::REGISTER_SOFTBUS_CALLBACK,
            "STAGE_RES", StageRes::STAGE_SUCC,
            "BIZ_STATE", BizState::BIZ_STATE_START,
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::DMS_INIT,
            "BIZ_STAGE", DmsInit::REGISTER_SOFTBUS_CALLBACK,
            "STAGE_RES", StageRes::STAGE_FAIL,
            "BIZ_STATE", BizState::BIZ_STATE_START,
            "ERROR_CODE", errCode);
    }
    if (res != DMS_OK) {
        LOGE("RegisterSoftbusCallbackRes error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::RegisterFocusedRes(string func, inte32_t errCode)
{
    int32_t res = DMS_OK;
    int32_t stageRes = (errCode == ERR_OK) ? StageRes::STAGE_SUCC : StageRes::STAGE_FAIL;
    if (stageRes == StageRes::STAGE_SUCC) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::DMS_INIT,
            "BIZ_STAGE", DmsInit::REGISTER_AMS_FOCUSED,
            "STAGE_RES", StageRes::STAGE_SUCC,
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
            "STAGE_RES", StageRes::STAGE_FAIL,
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "TO_CALL_PKG", AMS,
            "ERROR_CODE", errCode);
    }
    if (res != DMS_OK) {
        LOGE("RegisterFocusedRes error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::DmsFocused(string func, string focusMode)
{
    int32_t res = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
        DISTRIBUTED_SCHEDULE,
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ORG_PKG", ORGPKGNAME,
        "FUNC", func,
        "BIZ_SCENE", BizScene::NORMAL_MULTIMODE_FOCUSED,
        "BIZ_STAGE", NormalOrMultimodeFocused::DMS_FOCUSED,
        "STAGE_RES", StageRes::STAGE_SUCC,
        "BIZ_STATE", BizState::BIZ_STATE_START,
        "FOCUS_MODE", focusMode);
    if (res != DMS_OK) {
        LOGE("DmsFocused error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::NormalFocusedGetAccessTokenIdRes(string func, inte32_t errCode)
{
    int32_t res = DMS_OK;
    int32_t stageRes = (errCode == ERR_OK) ? StageRes::STAGE_SUCC : StageRes::STAGE_FAIL;
    if (stageRes == StageRes::STAGE_SUCC) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::NORMAL_MULTIMODE_FOCUSED,
            "BIZ_STAGE", NormalOrMultimodeFocused::::GET_BMS_ACCESSTOKENID,
            "STAGE_RES", StageRes::STAGE_SUCC,
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
            "STAGE_RES", StageRes::STAGE_FAIL,
            "TO_CALL_PKG", BMS,
            "ERROR_CODE", errCode);
    }
    if (res != DMS_OK) {
        LOGE("NormalFocusedGetAccessTokenIdRes error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::NormalFocusedSendEventRes(string func, inte32_t errCode)
{
    int32_t res = DMS_OK;
    int32_t stageRes = (errCode == ERR_OK) ? StageRes::STAGE_SUCC : StageRes::STAGE_FAIL;
    if (stageRes == StageRes::STAGE_SUCC) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::NORMAL_MULTIMODE_FOCUSED,
            "BIZ_STAGE", NormalOrMultimodeFocused::SOFTBUS_SENDEVENT,
            "STAGE_RES", StageRes::STAGE_SUCC,
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
            "STAGE_RES", StageRes::STAGE_FAIL,
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "TO_CALL_PKG", SOFTBUS,
            "ERROR_CODE", errCode);
    }
    if (res != DMS_OK) {
        LOGE("NormalFocusedSendEventRes error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::SetFocusedState(string func)
{
    int32_t res = DMS_OK;
    res = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
        DISTRIBUTED_SCHEDULE,
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ORG_PKG", ORGPKGNAME,
        "FUNC", func,
        "BIZ_SCENE", BizScene::CHANGE_STATE_FOCUSED,
        "BIZ_STAGE", ChangeStateFocused::SET_STATE,
        "STAGE_RES", StageRes::STAGE_SUCC,
        "BIZ_STATE", BizState::BIZ_STATE_START);
    if (res != DMS_OK) {
        LOGE("SetFocusedState error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::ChangeStateFocusedGetAccessTokenIdRes(string func, inte32_t errCode)
{
    int32_t res = DMS_OK;
    int32_t stageRes = (errCode == ERR_OK) ? StageRes::STAGE_SUCC : StageRes::STAGE_FAIL;
    if (stageRes == StageRes::STAGE_SUCC) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CHANGE_STATE_FOCUSED,
            "BIZ_STAGE", ChangeStateFocused::GET_BMS_ACCESSTOKENID,
            "STAGE_RES", StageRes::STAGE_SUCC,
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
            "STAGE_RES", StageRes::STAGE_FAIL,
            "TO_CALL_PKG", BMS,
            "ERROR_CODE", errCode);
    }
    if (res != DMS_OK) {
        LOGE("ChangeStateFocusedGetAccessTokenIdRes error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::ChangeStateFocusedSendEventRes(string func, inte32_t errCode)
{
    int32_t res = DMS_OK;
    int32_t stageRes = (errCode == ERR_OK) ? StageRes::STAGE_SUCC : StageRes::STAGE_FAIL;
    if (stageRes == StageRes::STAGE_SUCC) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CHANGE_STATE_FOCUSED,
            "BIZ_STAGE", ChangeStateFocused::SOFTBUS_SENDEVENT,
            "STAGE_RES", StageRes::STAGE_SUCC,
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
            "STAGE_RES", StageRes::STAGE_FAIL,
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "TO_CALL_PKG", SOFTBUS,
            "ERROR_CODE", errCode);
    }
    if (res != DMS_OK) {
        LOGE("ChangeStateFocusedSendEventRes error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::DmsUnfocused(string func)
{
    int32_t res = DMS_OK;
    res = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
        DISTRIBUTED_SCHEDULE,
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ORG_PKG", ORGPKGNAME,
        "FUNC", func,
        "BIZ_SCENE", BizScene::NORMAL_UNFOCUSED,
        "BIZ_STAGE", NormalUnfocused::DMS_UNFOCUSED,
        "STAGE_RES", StageRes::STAGE_SUCC,
        "BIZ_STATE", BizState::BIZ_STATE_START);
    if (res != DMS_OK) {
        LOGE("DmsUnfocused error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::NormalUnfocusedGetAccessTokenIdRes(string func, inte32_t errCode)
{
    int32_t res = DMS_OK;
    int32_t stageRes = (errCode == ERR_OK) ? StageRes::STAGE_SUCC : StageRes::STAGE_FAIL;
    if (stageRes == StageRes::STAGE_SUCC) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::NORMAL_UNFOCUSED,
            "BIZ_STAGE", NormalUnfocused::GET_BMS_ACCESSTOKENID,
            "STAGE_RES", StageRes::STAGE_SUCC,
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
            "STAGE_RES", StageRes::STAGE_FAIL,
            "TO_CALL_PKG", BMS,
            "ERROR_CODE", errCode);
    }
    if (res != DMS_OK) {
        LOGE("NormalUnfocusedGetAccessTokenIdRes error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::NormalUnfocusedSendEventRes(string func,inte32_t errCode)
{
    int32_t res = DMS_OK;
    int32_t stageRes = (errCode == ERR_OK) ? StageRes::STAGE_SUCC : StageRes::STAGE_FAIL;
    if (stageRes == StageRes::STAGE_SUCC) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::NORMAL_UNFOCUSED,
            "BIZ_STAGE", NormalUnfocused::SOFTBUS_SENDEVENT,
            "STAGE_RES", StageRes::STAGE_SUCC,
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
            "STAGE_RES", StageRes::STAGE_FAIL,
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "TO_CALL_PKG", SOFTBUS,
            "ERROR_CODE", errCode);
    }
    if (res != DMS_OK) {
        LOGE("NormalUnfocusedSendEventRes error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::RecordTime(string func, int32_t bizState, string toCallPkg)
{
    int32_t res = DMS_OK;
    res = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
        DISTRIBUTED_SCHEDULE,
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ORG_PKG", ORGPKGNAME,
        "FUNC", func,
        "BIZ_SCENE", BizScene::MULTIMODE_UNFOCUSED,
        "BIZ_STAGE", MultimodeUnfocused::RECORD_TIME,
        "STAGE_RES", StageRes::STAGE_SUCC,
        "BIZ_STATE", BizState::BIZ_STATE_START);
    if (res != DMS_OK) {
        LOGE("RecordTime error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::MultimodeUnfocusedGetAccessTokenIdRes(string func, int32_t bizState, string toCallPkg, inte32_t errCode)
{
    int32_t res = DMS_OK;
    res = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
        DISTRIBUTED_SCHEDULE,
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ORG_PKG", ORGPKGNAME,
        "FUNC", func,
        "BIZ_SCENE", BizScene::MULTIMODE_UNFOCUSED,
        "BIZ_STAGE", MultimodeUnfocused::GET_ACCESSTOKENID,
        "STAGE_RES", StageRes::STAGE_SUCC);
    if (res != DMS_OK) {
        LOGE("RecordTime error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::MultimodeUnfocusedSendEventRes(string func, int32_t bizState, string toCallPkg, inte32_t errCode)
{
    int32_t res = DMS_OK;
    int32_t stageRes = (errCode == ERR_OK) ? StageRes::STAGE_SUCC : StageRes::STAGE_FAIL;
    if (stageRes == StageRes::STAGE_SUCC) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::MULTIMODE_UNFOCUSED,
            "BIZ_STAGE", MultimodeUnfocused::SOFTBUS_SENDEVENT,
            "STAGE_RES", StageRes::STAGE_SUCC,
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
            "STAGE_RES", StageRes::STAGE_FAIL,
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "TO_CALL_PKG", SOFTBUS,
            "ERROR_CODE", errCode);
    }
    if (res != DMS_OK) {
        LOGE("MultimodeUnfocusedSendEventRes error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::SetUnfocusedState(string func)
{
    int32_t res = DMS_OK;
    res = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
        DISTRIBUTED_SCHEDULE,
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ORG_PKG", ORGPKGNAME,
        "FUNC", func,
        "BIZ_SCENE", BizScene::CHANGE_STATE_UNFOCUSED,
        "BIZ_STAGE", ChangeStateUnfocused::SET_STATE,
        "STAGE_RES", StageRes::STAGE_SUCC,
        "BIZ_STATE", BizState::BIZ_STATE_START);
    if (res != DMS_OK) {
        LOGE("SetFocusedState error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::ChangeStateUnfocusedGetAccessTokenIdRes(string func, inte32_t errCode)
{
    int32_t res = DMS_OK;
    int32_t stageRes = (errCode == ERR_OK) ? StageRes::STAGE_SUCC : StageRes::STAGE_FAIL;
    if (stageRes == StageRes::STAGE_SUCC) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CHANGE_STATE_UNFOCUSED,
            "BIZ_STAGE", ChangeStateUnfocused::GET_BMS_ACCESSTOKENID,
            "STAGE_RES", StageRes::STAGE_SUCC,
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
            "STAGE_RES", StageRes::STAGE_FAIL,
            "TO_CALL_PKG", BMS,
            "ERROR_CODE", errCode);
    }
    if (res != DMS_OK) {
        LOGE("ChangeStateUnfocusedGetAccessTokenIdRes error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::ChangeStateUnfocusedSendEventRes(string func, inte32_t errCode)
{
    int32_t res = DMS_OK;
    int32_t stageRes = (errCode == ERR_OK) ? StageRes::STAGE_SUCC : StageRes::STAGE_FAIL;
    if (stageRes == StageRes::STAGE_SUCC) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CHANGE_STATE_UNFOCUSED,
            "BIZ_STAGE", ChangeStateUnfocused::SOFTBUS_SENDEVENT,
            "STAGE_RES", StageRes::STAGE_SUCC,
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
            "STAGE_RES", StageRes::STAGE_FAIL,
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "TO_CALL_PKG", SOFTBUS,
            "ERROR_CODE", errCode);
    }
    if (res != DMS_OK) {
        LOGE("ChangeStateUnfocusedSendEventRes error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::RecvFocused(string func)
{
    int32_t res = DMS_OK;
    res = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
        DISTRIBUTED_SCHEDULE,
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ORG_PKG", ORGPKGNAME,
        "FUNC", func,
        "BIZ_SCENE", BizScene::RECV_FOCUSED,
        "BIZ_STAGE", RecvFocused::RECV_FOCUSED,
        "STAGE_RES", StageRes::STAGE_SUCC,
        "BIZ_STATE", BizState::BIZ_STATE_START,
        "BROADCAST_INFO", FOCUSED,
        "HOST_PKG", SOFTBUS);
    if (res != DMS_OK) {
        LOGE("RecvFocused error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::FocusedGetBundleName(string func, inte32_t errCode)
{
    int32_t res = DMS_OK;
    int32_t stageRes = (errCode == ERR_OK) ? StageRes::STAGE_SUCC : StageRes::STAGE_FAIL;
    if (stageRes == StageRes::STAGE_SUCC) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::RECV_FOCUSED,
            "BIZ_STAGE", RecvFocused::GET_DBMS_BUNDLENAME,
            "STAGE_RES", StageRes::STAGE_SUCC,
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
            "STAGE_RES", StageRes::STAGE_FAIL,
            "TO_CALL_PKG", DBMS,
            "ERROR_CODE", errCode);
    }
    if (res != DMS_OK) {
        LOGE("FocusedGetBundleName error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::NotifyDockFocused(string func, inte32_t errCode)
{
    int32_t res = DMS_OK;
    int32_t stageRes = (errCode == ERR_OK) ? StageRes::STAGE_SUCC : StageRes::STAGE_FAIL;
    if (stageRes == StageRes::STAGE_SUCC) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::RECV_FOCUSED,
            "BIZ_STAGE", RecvFocused::NOTIFY_DOCK_FOCUSED,
            "STAGE_RES", StageRes::STAGE_SUCC);
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::RECV_FOCUSED,
            "BIZ_STAGE", RecvFocused::NOTIFY_DOCK_FOCUSED,
            "STAGE_RES", StageRes::STAGE_FAIL,
            "ERROR_CODE", errCode);
    }
    if (res != DMS_OK) {
        LOGE("NotifyDockFocused error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::RecvUnfocused(string func)
{
    int32_t res = DMS_OK;
    res = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
        DISTRIBUTED_SCHEDULE,
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ORG_PKG", ORGPKGNAME,
        "FUNC", func,
        "BIZ_SCENE", BizScene::RECV_UNFOCUSED,
        "BIZ_STAGE", RecvUnfocused::RECV_UNFOCUSED,
        "STAGE_RES", StageRes::STAGE_SUCC,
        "BIZ_STATE", BizState::BIZ_STATE_START,
        "BROADCAST_INFO", UNFOCUSED,
        "TO_CALL_PKG", SOFTBUS);
    if (res != DMS_OK) {
        LOGE("RecvUnfocused error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::UnfocusedGetBundleName(string func, inte32_t errCode)
{
    int32_t res = DMS_OK;
    int32_t stageRes = (errCode == ERR_OK) ? StageRes::STAGE_SUCC : StageRes::STAGE_FAIL;
    if (stageRes == StageRes::STAGE_SUCC) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::RECV_UNFOCUSED,
            "BIZ_STAGE", RecvUnfocused::GET_DBMS_BUNDLENAME,
            "STAGE_RES", StageRes::STAGE_SUCC,
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
            "STAGE_RES", StageRes::STAGE_FAIL,
            "TO_CALL_PKG", DBMS,
            "ERROR_CODE", errCode);
    }
    if (res != DMS_OK) {
        LOGE("UnfocusedGetBundleName error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::NotifyDockUnfocused(string func, inte32_t errCode)
{
    int32_t res = DMS_OK;
    int32_t stageRes = (errCode == ERR_OK) ? StageRes::STAGE_SUCC : StageRes::STAGE_FAIL;
    if (stageRes == StageRes::STAGE_SUCC) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::RECV_UNFOCUSED,
            "BIZ_STAGE", RecvUnfocused::NOTIFY_DOCK_FOCUSED,
            "STAGE_RES", StageRes::STAGE_SUCC);
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::RECV_UNFOCUSED,
            "BIZ_STAGE", RecvUnfocused::NOTIFY_DOCK_FOCUSED,
            "STAGE_RES", StageRes::STAGE_FAIL,
            "ERROR_CODE", errCode);
    }
    if (res != DMS_OK) {
        LOGE("NotifyDockUnfocused error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::ClickIconDmsContinue(string func, int32_t errCode)
{   
    int32_t res = DMS_OK;
    int32_t stageRes = (errCode == ERR_OK) ? StageRes::STAGE_SUCC : StageRes::STAGE_FAIL;
    if (stageRes == StageRes::STAGE_SUCC) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CLICK_ICON,
            "BIZ_STAGE", ClickIcon::DMS_CONTINUE,
            "STAGE_RES", StageRes::STAGE_SUCC);
    } else {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CLICK_ICON,
            "BIZ_STAGE", ClickIcon::DMS_CONTINUE,
            "STAGE_RES", StageRes::STAGE_FAIL,
            "ERROR_CODE", errCode);
    }
    if (res != DMS_OK) {
        LOGE("ClickIconDmsContinue error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::ClickIconDmsStartAbility(string func, inte32_t errCode)
{
    int32_t res = DMS_OK;
    int32_t stageRes = (errCode == ERR_OK) ? StageRes::STAGE_SUCC : StageRes::STAGE_FAIL;
    if (stageRes == StageRes::STAGE_SUCC) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CLICK_ICON,
            "BIZ_STAGE", ClickIcon::DMS_STARTABILITY,
            "STAGE_RES", StageRes::STAGE_SUCC,
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
            "STAGE_RES", StageRes::STAGE_FAIL,
            "TO_CALL_PKG", AMS,
            "ERROR_CODE", errCode);
    }
    if (res != DMS_OK) {
        LOGE("ClickIconDmsStartAbility error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::ClickIconDmsRecvOver(string func, inte32_t errCode)
{
    int32_t res = DMS_OK;
    int32_t stageRes = (errCode == ERR_OK) ? StageRes::STAGE_SUCC : StageRes::STAGE_FAIL;
    if (stageRes == StageRes::STAGE_SUCC) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::CLICK_ICON,
            "BIZ_STAGE", ClickIcon::DMS_RECV_OVER,
            "STAGE_RES", StageRes::STAGE_SUCC,
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
            "STAGE_RES", StageRes::STAGE_FAIL,
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "ERROR_CODE", errCode);
    }
    if (res != DMS_OK) {
        LOGE("ClickIconDmsRecvOver error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::SaveDataDmsContinue(string func, inte32_t errCode)
{
    int32_t res = DMS_OK;
    int32_t stageRes = (errCode == ERR_OK) ? StageRes::STAGE_SUCC : StageRes::STAGE_FAIL;
    if (stageRes == StageRes::STAGE_SUCC) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::SAVE_DATA,
            "BIZ_STAGE", SaveData::DMS_CONTINUE,
            "STAGE_RES", StageRes::STAGE_SUCC,
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
            "STAGE_RES", StageRes::STAGE_FAIL,
            "BIZ_STATE", BizState::BIZ_STATE_START,
            "TO_CALL_PKG", AMS,
            "ERROR_CODE", errCode);
    }
    if (res != DMS_OK) {
        LOGE("SaveDataDmsContinue error, res:%d", res);
        return false;
    }
    return true;
}

bool DmsRadar::SaveDataDmsRemoteWant(string func, inte32_t errCode)
{
    int32_t res = DMS_OK;
    int32_t stageRes = (errCode == ERR_OK) ? StageRes::STAGE_SUCC : StageRes::STAGE_FAIL;
    if (stageRes == StageRes::STAGE_SUCC) {
        res = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
            DISTRIBUTED_SCHEDULE,
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "ORG_PKG", ORGPKGNAME,
            "FUNC", func,
            "BIZ_SCENE", BizScene::SAVE_DATA,
            "BIZ_STAGE", SaveData::DMS_REMOTE_WANT,
            "STAGE_RES", StageRes::STAGE_SUCC,
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
            "STAGE_RES", StageRes::STAGE_FAIL,
            "BIZ_STATE", BizState::BIZ_STATE_END,
            "ERROR_CODE", errCode);
    }
    if (res != DMS_OK) {
        LOGE("SaveDataDmsRemoteWant error, res:%d", res);
        return false;
    }
    return true;
}

std::string DmRadarHelper::GetAnonyUdid(std::string udid)
{
    if (udid.empty() || udid.length() < INVALID_UDID_LENGTH) {
        return "unknown";
    }
    return udid.substr(0, SUBSTR_UDID_LENGTH) + "**" + udid.substr(udid.length() - SUBSTR_UDID_LENGTH);
}

std::string DmRadarHelper::GetLocalUdid()
{
    char localDeviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(localDeviceId, DEVICE_UUID_LENGTH);
    return std::string(localDeviceId);
}

IDmRadarHelper *CreateDmRadarInstance()
{
    return &DmRadarHelper::GetInstance();
}
} // namespace DistributedSchedule
} // namespace OHOS
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

#include "mission/notification/dms_continue_send_manager.h"

#include <sys/prctl.h>

#include "adapter/mmi_adapter.h"
#include "bundle/bundle_manager_internal.h"
#include "datetime_ex.h"
#include "distributed_sched_utils.h"
#include "dsched_data_buffer.h"
#include "dtbschedmgr_log.h"
#include "mission/dms_continue_condition_manager.h"
#include "mission/notification/dms_continue_send_strategy.h"
#include "softbus_adapter/softbus_adapter.h"

namespace OHOS {
namespace DistributedSchedule {
namespace {
const std::string TAG = "DMSContinueSendMgr";

constexpr int32_t INDEX_0 = 0;
constexpr int32_t INDEX_1 = 1;
constexpr int32_t INDEX_2 = 2;
constexpr int32_t INDEX_3 = 3;
constexpr int32_t FOCUS_TIMEOUT_DELAY_TIME = 60000;
constexpr int32_t SCREEN_OFF_DELAY_TIME = 10000;
constexpr int64_t SCREEN_OFF_EVENT_INTERVAL = 500; // determines whether normal unfocused or lockoff
const std::string TIMEOUT_UNFOCUSED_TASK = "timeout_unfocused_task";
const std::string SCREEN_OFF_UNFOCUSED_TASK = "screen_off_unfocused_task";
}

void DMSContinueSendMgr::Init(int32_t currentUserId)
{
    HILOGI("Init start");
    if (eventHandler_ != nullptr) {
        HILOGI("Already inited, end.");
        return;
    }
    {
        screenOffHandler_ = std::make_shared<ScreenOffHandler>(shared_from_this());
        screenOffHandler_->ResetScreenOffInfo();

        eventThread_ = std::thread(&DMSContinueSendMgr::StartEvent, this);
        std::unique_lock<std::mutex> lock(eventMutex_);
        eventCon_.wait(lock, [this] {
            return eventHandler_ != nullptr;
        });
    }

    accountId_ = currentUserId;
    HILOGI("Init end");
}

void DMSContinueSendMgr::StartEvent()
{
    HILOGI("StartEvent start");
    prctl(PR_SET_NAME, TAG.c_str());
    auto runner = AppExecFwk::EventRunner::Create(false);
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        eventHandler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    }
    eventCon_.notify_one();
    CHECK_POINTER_RETURN(runner, "runner");
    runner->Run();
    HILOGI("StartEvent end");
}

void DMSContinueSendMgr::UnInit()
{
    HILOGI("UnInit start");
    CHECK_POINTER_RETURN(eventHandler_, "eventHandler_");
    if (eventHandler_->GetEventRunner() != nullptr) {
        eventHandler_->GetEventRunner()->Stop();
        eventThread_.join();
        eventHandler_ = nullptr;
    }
    HILOGI("UnInit end");
}

void DMSContinueSendMgr::OnMissionStatusChanged(int32_t missionId, MissionEventType type)
{
    MissionStatus status;
    int32_t ret = DmsContinueConditionMgr::GetInstance().GetMissionStatus(accountId_, missionId, status);
    if (ret != ERR_OK) {
        HILOGE("GetMissionStatus failed, ret: %{public}d, missionId %{public}d, type: %{public}s",
            ret, missionId, DmsContinueConditionMgr::GetInstance().TypeEnumToString(type).c_str());
        return;
    }

    auto feedfunc = [this, status, type]() {
        SendContinueBroadcast(status, type);
    };
    CHECK_POINTER_RETURN(eventHandler_, "eventHandler_");
    eventHandler_->RemoveTask(TIMEOUT_UNFOCUSED_TASK + std::to_string(missionId));
    eventHandler_->PostTask(feedfunc);
}

void DMSContinueSendMgr::OnMMIEvent()
{
    auto feedfunc = [this]() {
        int32_t missionId = DmsContinueConditionMgr::GetInstance().GetCurrentFocusedMission(accountId_);

        CHECK_POINTER_RETURN(eventHandler_, "eventHandler_");
        eventHandler_->RemoveTask(TIMEOUT_UNFOCUSED_TASK + std::to_string(missionId));

        SendContinueBroadcast(missionId, MISSION_EVENT_MMI);
    };
    CHECK_POINTER_RETURN(eventHandler_, "eventHandler_");
    eventHandler_->PostTask(feedfunc);
}

int32_t DMSContinueSendMgr::OnDeviceOnline()
{
    HILOGD("OnDeviceOnline called");
    int32_t missionId = GetCurrentMissionId();
    if (missionId <= 0) {
        return INVALID_MISSION_ID;
    }
    OnMissionStatusChanged(missionId, MISSION_EVENT_FOCUSED);
    return ERR_OK;
}

void DMSContinueSendMgr::OnDeviceScreenOff()
{
    HILOGI("OnDeviceScreenOff called");
    auto feedfunc = [this]() {
        CHECK_POINTER_RETURN(screenOffHandler_, "screenOffHandler_");
        screenOffHandler_->OnDeviceScreenOff();
    };
    CHECK_POINTER_RETURN(eventHandler_, "eventHandler_");
    eventHandler_->PostTask(feedfunc);
}

void DMSContinueSendMgr::OnDeviceScreenOn()
{
    HILOGI("OnDeviceScreenOn called");
    auto feedfunc = [this]() {
        CHECK_POINTER_RETURN(screenOffHandler_, "screenOffHandler_");
        screenOffHandler_->OnDeviceScreenOn();
    };
    CHECK_POINTER_RETURN(eventHandler_, "eventHandler_");
    eventHandler_->PostTask(feedfunc);
}

void DMSContinueSendMgr::OnUserSwitched()
{
    HILOGI("OnUserSwitched called");
    auto feedfunc = [this]() {
        RemoveMMIListener();
        SendScreenOffEvent(DMS_UNFOCUSED_TYPE);
    };
    CHECK_POINTER_RETURN(eventHandler_, "eventHandler_");
    eventHandler_->PostTask(feedfunc);
    CHECK_POINTER_RETURN(screenOffHandler_, "screenOffHandler_");
    std::string timeoutTaskName = TIMEOUT_UNFOCUSED_TASK + std::to_string(screenOffHandler_->GetMissionId());
    eventHandler_->RemoveTask(timeoutTaskName);
}

void DMSContinueSendMgr::SendContinueBroadcast(int32_t missionId, MissionEventType type)
{
    MissionStatus status;
    int32_t ret = DmsContinueConditionMgr::GetInstance().GetMissionStatus(accountId_, missionId, status);
    if (ret != ERR_OK) {
        HILOGE("GetMissionStatus failed, ret: %{public}d, missionId %{public}d, type: %{public}s",
            ret, missionId, DmsContinueConditionMgr::GetInstance().TypeEnumToString(type).c_str());
        return;
    }
    SendContinueBroadcast(status, type);
    return;
}

void DMSContinueSendMgr::SendContinueBroadcast(const MissionStatus& status, MissionEventType type)
{
    auto typeStr = DmsContinueConditionMgr::GetInstance().TypeEnumToString(type);
    HILOGI("start, missionId: %{public}d, type: %{public}s", status.missionId, typeStr.c_str());

    if (!CheckBroadcastCondition(status, type)) {
        HILOGE("CheckBroadcastCondition %{public}s failed! status: {%{public}s}",
            typeStr.c_str(), status.ToString().c_str());
        return;
    }

    uint8_t sendType = 0;
    int32_t ret = ExecuteSendStrategy(type, status, sendType);
    if (ret != ERR_OK) {
        HILOGE("ExecuteSendStrategy %{public}s failed, ret: %{public}d. status: {%{public}s}",
            typeStr.c_str(), ret, status.ToString().c_str());
        return;
    }

    uint16_t bundleNameId = 0;
    uint8_t continueTypeId = 0;
    ret = QueryBroadcastInfo(status, bundleNameId, continueTypeId);
    if (ret != ERR_OK) {
        HILOGE("QueryBroadcastInfo %{public}s failed! status: {%{public}s}",
            typeStr.c_str(), status.ToString().c_str());
        return;
    }

    SendSoftbusEvent(bundleNameId, continueTypeId, sendType);
    HILOGI("end");
    return;
}

bool DMSContinueSendMgr::CheckBroadcastCondition(const MissionStatus& status, MissionEventType type)
{
    return DmsContinueConditionMgr::GetInstance().CheckSystemSendCondition() &&
        DmsContinueConditionMgr::GetInstance().CheckMissionSendCondition(status, type);
}

int32_t DMSContinueSendMgr::ExecuteSendStrategy(MissionEventType type, const MissionStatus& status, uint8_t &sendType)
{
    HILOGI("start, missionId: %{public}d, type: %{public}s", status.missionId,
        DmsContinueConditionMgr::GetInstance().TypeEnumToString(type).c_str());
    ContinueSendContext context;
    auto sendMgr = std::make_unique<SendStrategyFocused>(shared_from_this());
    switch (type) {
        case MISSION_EVENT_FOCUSED:
            context.SetStrategy(sendMgr);
            break;
        case MISSION_EVENT_UNFOCUSED:
            context.SetStrategy(sendMgr);
            break;
        case MISSION_EVENT_DISTORYED:
            context.SetStrategy(sendMgr);
            break;
        case MISSION_EVENT_ACTIVE:
            context.SetStrategy(sendMgr);
            break;
        case MISSION_EVENT_INACTIVE:
            context.SetStrategy(sendMgr);
            break;
        case MISSION_EVENT_TIMEOUT:
            context.SetStrategy(sendMgr);
            break;
        case MISSION_EVENT_MMI:
            context.SetStrategy(sendMgr);
            break;
        default:
            HILOGE("Invalid type %{public}d!", type);
            break;
    }
    int32_t ret = context.ExecuteSendStrategy(status, sendType);
    HILOGI("end, ExecuteSendStrategy ret: %{public}d", ret);
    return ret;
}

int32_t DMSContinueSendMgr::QueryBroadcastInfo(
    const MissionStatus& status, uint16_t& bundleNameId, uint8_t& continueTypeId)
{
    if (status.bundleName.empty() || status.abilityName.empty()) {
        HILOGE("bundleName or ability name is null!");
        return INVALID_PARAMETERS_ERR;
    }
    HILOGI("start, bundleName: %{public}s, abilityName: %{public}s", status.bundleName.c_str(),
        status.abilityName.c_str());

    int32_t ret = BundleManagerInternal::GetBundleNameId(status.bundleName, bundleNameId);
    if (ret != ERR_OK) {
        HILOGE("GetBundleNameId failed, bundleName: %{public}s, ret: %{public}d", status.bundleName.c_str(), ret);
        return ret;
    }

    ret = BundleManagerInternal::GetContinueTypeId(status.bundleName, status.abilityName, continueTypeId);
    if (ret != ERR_OK) {
        HILOGE("GetContinueTypeId failed, abilityName: %{public}s, ret: %{public}d", status.abilityName.c_str(), ret);
        return ret;
    }
    HILOGI("success, bundleNameId: %{public}d, continueTypeId: %{public}d", bundleNameId, continueTypeId);
    return ERR_OK;
}

void DMSContinueSendMgr::SendSoftbusEvent(uint16_t& bundleNameId, uint8_t& continueTypeId, uint8_t type)
{
    HILOGI("bundleNameId: %{public}u, continueTypeId: %{public}u, sendType %{public}u",
        bundleNameId, continueTypeId, type);

    std::shared_ptr<DSchedDataBuffer> buffer = std::make_shared<DSchedDataBuffer>(DMS_SEND_LEN);
    buffer->Data()[INDEX_0] = (type << CONTINUE_SHIFT_04) | DMS_DATA_LEN;
    buffer->Data()[INDEX_1] = (bundleNameId >> CONTINUE_SHIFT_08) & DMS_0XFF;
    buffer->Data()[INDEX_2] = bundleNameId & DMS_0XFF;
    buffer->Data()[INDEX_3] = continueTypeId & DMS_0XFF;

    SoftbusAdapter::GetInstance().SendSoftbusEvent(buffer);
}

void DMSContinueSendMgr::SendContinueBroadcastAfterDelay(int32_t missionId)
{
    CHECK_POINTER_RETURN(eventHandler_, "eventHandler_");
    HILOGI("send continue broadcast for missionId: %{public}d after %{public}d delay",
        missionId, FOCUS_TIMEOUT_DELAY_TIME);
    auto func = [this, missionId]() {
        SendContinueBroadcast(missionId, MISSION_EVENT_TIMEOUT);
    };
    std::string timeoutTaskName = TIMEOUT_UNFOCUSED_TASK + std::to_string(missionId);
    eventHandler_->RemoveTask(timeoutTaskName);
    eventHandler_->PostTask(func, timeoutTaskName, FOCUS_TIMEOUT_DELAY_TIME);
}

void DMSContinueSendMgr::AddMMIListener()
{
    if (mmiMonitorId_ >= 0) {
        HILOGD("MMI listener already exist, monitor id: %{public}d", mmiMonitorId_);
        return;
    }
    mmiMonitorId_ = MMIAdapter::GetInstance().AddMMIListener();
    if (mmiMonitorId_ < 0) {
        HILOGW("Add MMI listener failed, ret: %{public}d", mmiMonitorId_);
        return;
    }
    HILOGI("MMI listener has been added, monitor id: %{public}d", mmiMonitorId_);
}

void DMSContinueSendMgr::RemoveMMIListener()
{
    if (mmiMonitorId_ < 0) {
        HILOGI("No MMI listener to be removed, monitor id: %{public}d", mmiMonitorId_);
        return;
    }
    MMIAdapter::GetInstance().RemoveMMIListener(mmiMonitorId_);
    HILOGI("MMI listener has been removed, monitor id: %{public}d", mmiMonitorId_);

    mmiMonitorId_ = INVALID_MISSION_ID;
    return;
}

void DMSContinueSendMgr::PostScreenOffEventAfterDelay(int32_t missionId, uint8_t type, int32_t timeout)
{
    CHECK_POINTER_RETURN(eventHandler_, "eventHandler_");
    auto func = [this, type]() {
        SendScreenOffEvent(type);
    };
    std::string timeoutTaskName = TIMEOUT_UNFOCUSED_TASK + std::to_string(missionId);
    eventHandler_->RemoveTask(timeoutTaskName);
    if (timeout == 0) {
        eventHandler_->PostTask(func);
        return;
    }
    eventHandler_->PostTask(func, timeoutTaskName, timeout);
}

int32_t DMSContinueSendMgr::SendScreenOffEvent(uint8_t type)
{
    HILOGI("start, type: %{public}u", type);
    CHECK_POINTER_RETURN_VALUE(screenOffHandler_, ERR_NULL_OBJECT, "screenOffHandler_");

    uint16_t bundleNameId = 0;
    uint8_t continueTypeId = 0;
    MissionStatus status = screenOffHandler_->GetMissionStatus();
    int32_t ret = QueryBroadcastInfo(status, bundleNameId, continueTypeId);
    if (ret != ERR_OK) {
        HILOGE("QueryBroadcastInfo failed, ret: %{public}d, status: %{public}s", ret, status.ToString().c_str());
        return ret;
    }

    if (!DmsContinueConditionMgr::GetInstance().CheckSystemSendCondition()) {
        HILOGE("check system send condition failed!");
        return DMS_PERMISSION_DENIED;
    }

    SendSoftbusEvent(bundleNameId, continueTypeId, type);
    HILOGI("end");
    return ERR_OK;
}

DMSContinueSendMgr::ScreenOffHandler::ScreenOffHandler(const std::shared_ptr<DMSContinueSendMgr>& dmsContinueSendMgr)
    : dmsContinueSendMgr_(dmsContinueSendMgr)
{
}

int32_t DMSContinueSendMgr::ScreenOffHandler::GetMissionId()
{
    return unfoInfo_.missionId;
}

MissionStatus DMSContinueSendMgr::ScreenOffHandler::GetMissionStatus()
{
    return unfoInfo_.status;
}

bool DMSContinueSendMgr::ScreenOffHandler::IsDeviceScreenOn()
{
    return isScreenOn_;
}

void DMSContinueSendMgr::ScreenOffHandler::OnDeviceScreenOff()
{
    HILOGI("ScreenOffHandler::OnDeviceScreenOff called");
    isScreenOn_ = false;
    std::shared_ptr<DMSContinueSendMgr> sendMgr = dmsContinueSendMgr_.lock();
    if (sendMgr == nullptr) {
        HILOGW("sendMgr is nullptr.");
        return;
    }
    // first NEQ was changed to EQ before, figure out why
    if (unfoInfo_.missionId != INVALID_MISSION_ID && (GetTickCount()- unfoInfo_.unfoTime) < SCREEN_OFF_EVENT_INTERVAL) {
        // handle unfocus before screen off
        sendMgr->PostScreenOffEventAfterDelay(unfoInfo_.missionId, DMS_FOCUSED_TYPE, 0);
    }
    sendMgr->PostScreenOffEventAfterDelay(
        unfoInfo_.missionId, DMS_UNFOCUSED_TYPE, SCREEN_OFF_DELAY_TIME);
}

void DMSContinueSendMgr::ScreenOffHandler::OnDeviceScreenOn()
{
    HILOGI("ScreenOffHandler::OnDeviceScreenOn called");
    isScreenOn_ = true;
}

void DMSContinueSendMgr::ScreenOffHandler::ResetScreenOffInfo()
{
    HILOGI("clear last unfocused info");
    unfoInfo_.missionId = INVALID_MISSION_ID;
    unfoInfo_.unfoTime = 0;
    unfoInfo_.status = {};
}

void DMSContinueSendMgr::ScreenOffHandler::SetScreenOffInfo(LastUnfoInfo info)
{
    HILOGI("set last unfocused info, missionId: %{public}d, bundleName: %{public}s, abilityName: %{public}s",
        info.missionId, info.status.bundleName.c_str(), info.status.abilityName.c_str());
    unfoInfo_.missionId = info.missionId;
    unfoInfo_.unfoTime = GetTickCount();
    unfoInfo_.status = info.status;
}
} // namespace DistributedSchedule
} // namespace OHOS

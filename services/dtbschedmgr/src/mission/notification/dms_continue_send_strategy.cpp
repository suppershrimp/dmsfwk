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

#include "mission/notification/dms_continue_send_strategy.h"

#include "dtbschedmgr_log.h"

namespace OHOS {
namespace DistributedSchedule {
namespace {
const std::string TAG = "DmsContinueSendStrategy";
}

void ContinueSendContext::SetStrategy(std::unique_ptr<ContinueSendStrategy>&& strategy)
{
    strategy_ = std::move(strategy);
}

int32_t ContinueSendContext::ExecuteSendStrategy(const MissionStatus& status, uint8_t &sendType) const
{
    CHECK_POINTER_RETURN_VALUE(strategy_, ERR_NULL_OBJECT, "strategy_");
    return strategy_->ExecuteSendStrategy(status, sendType);
}

int32_t SendStrategyFocused::ExecuteSendStrategy(const MissionStatus& status, uint8_t &sendType) const
{
    std::shared_ptr<DMSContinueSendMgr> sendMgr = dmsContinueSendMgr_.lock();
    sendMgr->AddMMIListener();
    sendMgr->SendContinueBroadcastAfterDelay(status.missionId);

    CHECK_POINTER_RETURN_VALUE(sendMgr->screenOffHandler_, ERR_OK, "screenOffHandler_");
    sendMgr->screenOffHandler_->ResetScreenOffInfo();
    sendType = BROADCAST_TYPE_APPEAR;
    return ERR_OK;
}

int32_t SendStrategyUnfocused::ExecuteSendStrategy(const MissionStatus& status, uint8_t &sendType) const
{
    std::shared_ptr<DMSContinueSendMgr> sendMgr = dmsContinueSendMgr_.lock();
    sendMgr->RemoveMMIListener();
    sendType = BROADCAST_TYPE_DISAPPEAR;

    ScreenOffHandler::LastUnfoInfo info = { status.missionId, 0, status };
    CHECK_POINTER_RETURN_VALUE(sendMgr->screenOffHandler_, ERR_OK, "screenOffHandler_");
    sendMgr->screenOffHandler_->SetScreenOffInfo(info);
    if (!sendMgr->screenOffHandler_->IsDeviceScreenOn()) {
        HILOGW("already screenOff, task abort");
        return DMS_PERMISSION_DENIED;
    }
    return ERR_OK;
}

int32_t SendStrategyDestoryed::ExecuteSendStrategy(const MissionStatus& status, uint8_t &sendType) const
{
    std::shared_ptr<DMSContinueSendMgr> sendMgr = dmsContinueSendMgr_.lock();
    sendMgr->RemoveMMIListener();
    sendType = BROADCAST_TYPE_DISAPPEAR;
    return ERR_OK;
}

int32_t SendStrategyActive::ExecuteSendStrategy(const MissionStatus& status, uint8_t &sendType) const
{
    std::shared_ptr<DMSContinueSendMgr> sendMgr = dmsContinueSendMgr_.lock();
    sendMgr->AddMMIListener();
    sendMgr->SendContinueBroadcastAfterDelay(status.missionId);
    sendType = BROADCAST_TYPE_APPEAR;
    return ERR_OK;
}

int32_t SendStrategyInactive::ExecuteSendStrategy(const MissionStatus& status, uint8_t &sendType) const
{
    std::shared_ptr<DMSContinueSendMgr> sendMgr = dmsContinueSendMgr_.lock();
    sendMgr->RemoveMMIListener();
    sendType = BROADCAST_TYPE_DISAPPEAR;
    return ERR_OK;
}

int32_t SendStrategyTimeout::ExecuteSendStrategy(const MissionStatus& status, uint8_t &sendType) const
{
    sendType = BROADCAST_TYPE_DISAPPEAR;
    return ERR_OK;
}

int32_t SendStrategyMMI::ExecuteSendStrategy(const MissionStatus& status, uint8_t &sendType) const
{
    sendType = BROADCAST_TYPE_APPEAR;
    return ERR_OK;
}
}  // namespace DistributedSchedule
}  // namespace OHOS

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

#include "dsched_continue_event_handler.h"

#include "dsched_continue.h"
#include "dtbschedmgr_log.h"

namespace OHOS {
namespace DistributedSchedule {
namespace {
const std::string TAG = "DSchedContinueEventHandler";
}

DSchedContinueEventHandler::DSchedContinueEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner,
    const std::shared_ptr<DSchedContinue>& dschedContinue) : AppExecFwk::EventHandler(runner),
    dschedContinue_(dschedContinue)
{
}

DSchedContinueEventHandler::~DSchedContinueEventHandler()
{
}

void DSchedContinueEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        HILOGE("continue eventhandler event is nullptr");
        return;
    }

    std::shared_ptr<DSchedContinue> dContinue = dschedContinue_.lock();
    if (dContinue == nullptr) {
        HILOGE("continue eventhandler dcontinue is nullptr");
        return;
    }
    dContinue->ProcessEvent(event);
}
}  // namespace DistributedSchedule
}  // namespace OHOS

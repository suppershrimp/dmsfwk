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

#ifndef OHOS_DSCHED_CONTINUE_STATE_MACHINE_H
#define OHOS_DSCHED_CONTINUE_STATE_MACHINE_H

#include "dsched_continue_state.h"

namespace OHOS {
namespace DistributedSchedule {
class DSchedContinue;
class DSchedContinueStateMachine : public std::enable_shared_from_this<DSchedContinueStateMachine> {
public:
    explicit DSchedContinueStateMachine(std::shared_ptr<DSchedContinue> dContinue);
    ~DSchedContinueStateMachine();
    int32_t Execute(const AppExecFwk::InnerEvent::Pointer &event);
    void UpdateState(DSchedContinueStateType stateType);
    DSchedContinueStateType GetStateType();

private:
    std::shared_ptr<DSchedContinueState> CreateState(DSchedContinueStateType stateType);
    std::shared_ptr<DSchedContinueState> currentState_;
    std::weak_ptr<DSchedContinue> dContinue_;
};
}  // namespace DistributedSchedule
}  // namespace OHOS
#endif  // OHOS_DSCHED_CONTINUE_STATE_MACHINE_H

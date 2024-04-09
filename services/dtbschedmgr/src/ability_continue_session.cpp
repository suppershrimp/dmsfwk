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

#include "ability_continue_session.h"

#include <chrono>

#include "dtbschedmgr_log.h"
#include "scene_board_judgement.h"
#include "session_manager_lite.h"

namespace OHOS {
namespace DistributedSchedule {
namespace {
const std::string TAG = "AbilityContinueSession";
}

IMPLEMENT_SINGLE_INSTANCE(AbilityContinueSession);

using OHOS::Rosen::SessionManagerLite;
using OHOS::Rosen::WSError;

void AbilityContinueSession::UpdateContinueSessionId(const std::string& bundleName, const std::string& abilityName);
{
    HILOGD("Update continueSessionId, bundleName: %{public}s, abilityName: %{public}s", bundleName.c_str(), abilityName.c_str());
    auto now = std::chrono::system_clock::now();
    int64_t timeStamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    std::size_t hash = std::hash<std::string>()(bundleName + abilityName + std::to_string(timeStamp));
    continueSessionId_ = std::to_string(hash);
}

std::string AbilityContinueSession::GetContinueSessionId() const
{
    return continueSessionId_;
}

int32_t AbilityContinueSession::GetPersistentId(int32_t& persistentId)
{
    if(Rosen::SceneBoardJudgement::IsSceneBoardEnabked()) {
        auto sceneSessionManager = SessionManagerLite::GetInstance().GetSceneSessionManagerLiteProxy();
        if(!sceneSessionManager) {
            HILOGE("proxy is nullptr.")
            return ERR_INVALID_VALUE;
        }
        AAFwk::MissionInfo missionInfo;
        auto err = sceneSessionManager->SetSessionInfoByContinueSessionId(continueSessionId_, missionInfo);
        if(err == WSError::WS_OK) {
            persistentId = missionInfo.id;
            return ERR_OK;
        } else {
            HILOGE("Get sessionInfo failed, continueSessionId: %{public}s, errorCode: %{public}d", continueSessionId_.c_str(), err);
            retrun static_cast<int32_t>(err);
        }
    }
    return ERR_INVALID_VALUE;
}
} // namespace DistributedSchedule
} // namespace OHOS
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
#include <thread>

#include "dtbschedmgr_log.h"
#include "scene_board_judgement.h"
#include "session_manager_lite.h"

namespace OHOS {
namespace DistributedSchedule {
namespace {
const std::string TAG = "AbilityContinueSession";
constexpr int32_t RETRY_TIMES = 2;
constexpr int32_t SLEEP_TIME = 50;
}

IMPLEMENT_SINGLE_INSTANCE(AbilityContinueSession);

using OHOS::Rosen::SessionManagerLite;
using OHOS::Rosen::WSError;

void AbilityContinueSession::UpdateContinueSessionId(const std::string& bundleName, const std::string& abilityName);
{
    HILOGD("Update continueSessionId, bundleName: %{public}s, abilityName: %{public}s",
        bundleName.c_str(), abilityName.c_str());
    auto now = std::chrono::system_clock::now();
    int64_t timeStamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    std::size_t hash = std::hash<std::string>()(bundleName + abilityName + std::to_string(timeStamp));
    continueSessionId_ = std::to_string(hash);
}

std::string AbilityContinueSession::GetContinueSessionId() const
{
    return continueSessionId_;
}

void AbilityContinueSession::ClearContinueSessionId()
{
    continueSessionId_.clear();
}

int32_t AbilityContinueSession::GetPersistentId(int32_t& persistentId)
{
    HILOGI("%{public}s called", __func__);
    if (continueSessionId_.empty()) {
        HILOGE("continueSessionId is empty.");
        return ERR_INVALID_VALUE;
    }
    if (!Rosen::SceneBoardJudgement::IsSceneBoardEnabked()) {
        HILOGE("sceneBoard not available.");
        return ERR_INVALID_VALUE;
    }

    auto sceneSessionManager = SessionManagerLite::GetInstance().GetSceneSessionManagerLiteProxy();
    if (!sceneSessionManager) {
        HILOGE("proxy is nullptr.")
        return ERR_INVALID_VALUE;
    }
    
    int32_t retryTimeout = RETRY_TIMES;
    AAFwk::MissionInfo missionInfo;
    do {
        auto err = sceneSessionManager->SetSessionInfoByContinueSessionId(continueSessionId_, missionInfo);
        if (err == WSError::WS_OK) {
            continueSessionId_.clear();
            persistentId = missionInfo.id;
            return ERR_OK;
        } 
        HILOGI("Waiting gor get session info...");
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
        if (--retryTimeout < 0) {
            HILOGE("Get sessionInfo failed, continueSessionId: %{public}s, errorCode: %{public}d", continueSessionId_.c_str(), err);
            return ERR_INVALID_VALUE;
        }
    } while (true);
}
} // namespace DistributedSchedule
} // namespace OHOS
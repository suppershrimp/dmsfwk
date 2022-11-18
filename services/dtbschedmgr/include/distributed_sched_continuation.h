/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_DISTRIBUTED_SCHED_CONTINUATION_H
#define OHOS_DISTRIBUTED_SCHED_CONTINUATION_H

#include <cstdint>
#include <map>
#include <mutex>

#include "event_handler.h"
#include "iremote_object.h"
#include "refbase.h"

namespace OHOS {
namespace DistributedSchedule {
using FuncContinuationCallback = std::function<void(int32_t missionId)>;

class DSchedContinuation : public std::enable_shared_from_this<DSchedContinuation> {
public:
    void Init(const FuncContinuationCallback& contCallback);
    bool PushAbilityToken(int32_t sessionId, const sptr<IRemoteObject>& abilityToken);
    sptr<IRemoteObject> PopAbilityToken(int32_t sessionId);
    int32_t GenerateSessionId();
    bool IsInContinuationProgress(int32_t missionId);
    void SetTimeOut(int32_t missionId, int32_t timeout);
    void RemoveTimeOut(int32_t missionId);
    bool PushCallback(int32_t missionId, const sptr<IRemoteObject>& callback,
        std::string deviceId, bool isFreeInstall);
    sptr<IRemoteObject> PopCallback(int32_t missionId);
    int32_t NotifyMissionCenterResult(int32_t missionId, int32_t resultCode);
    bool IsFreeInstall(int32_t missionId);
    std::string GetTargetDevice(int32_t missionId);

private:
    class ContinuationHandler : public AppExecFwk::EventHandler {
    public:
        ContinuationHandler(const std::shared_ptr<AppExecFwk::EventRunner>& runner,
            const std::shared_ptr<DSchedContinuation>& continuationObj,
            const FuncContinuationCallback& contCallback)
            : AppExecFwk::EventHandler(runner), continuationObj_(continuationObj), contCallback_(contCallback) {}
        ~ContinuationHandler() = default;

        void ProcessEvent(const OHOS::AppExecFwk::InnerEvent::Pointer& event) override;
    private:
        std::weak_ptr<DSchedContinuation> continuationObj_;
        FuncContinuationCallback contCallback_;
    };

    std::shared_ptr<ContinuationHandler> continuationHandler_;
    std::mutex continuationLock_;
    int32_t currSessionId_ = 1;
    std::map<int32_t, sptr<IRemoteObject>> continuationMap_;
    std::map<int32_t, sptr<IRemoteObject>> callbackMap_;
    std::map<int32_t, bool> freeInstall_;
    std::map<int32_t, std::string> continuationDevices_;
};
} // namespace DistributedSchedule
} // namespace OHOS

#endif // OHOS_DISTRIBUTED_SCHED_CONTINUATION_H
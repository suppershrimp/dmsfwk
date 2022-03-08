/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "dms_callback_session.h"

#include "dtbschedmgr_log.h"
#include "distributed_sched_service.h"
#include "parcel_helper.h"

namespace OHOS {
namespace DistributedSchedule {
using namespace OHOS::AppExecFwk;

namespace {
constexpr int64_t Callback_DELAY_TIME = 20000;
const std::string TAG = "DmsCallbackSession";
}

void DmsCallbackSession::Init(const DmsCallbackSessionInitCallbackFunc& callback)
{
    auto runner = EventRunner::Create("dsched_callback");
    dmsCallbackHandler_ = std::make_shared<DmsCallbackHandler>(runner, shared_from_this(), callback);
}

int32_t DmsCallbackSession::GenerateSessionId()
{
    std::lock_guard<std::mutex> autoLock(sessionMutex_);
    int32_t currValue = currSessionId_;
    if (++currSessionId_ <= 0) {
        currSessionId_ = 1;
    }
    return currValue;
}

bool DmsCallbackSession::PushCallback(int32_t sessionId, const sptr<IRemoteObject>& callback,
    const std::string& deviceId, bool isContinue)
{
    if (sessionId <= 0) {
        HILOGE("PushCallback sessionId invalid!");
        return false;
    }

    if (callback == nullptr) {
        HILOGE("PushCallback callback null!");
        return false;
    }

    if (dmsCallbackHandler_ == nullptr) {
        HILOGE("PushCallback not initialized!");
        return false;
    }

    bool ret = dmsCallbackHandler_->SendEvent(sessionId, 0, Callback_DELAY_TIME);
    if (!ret) {
        HILOGE("PushCallback SendEvent failed!");
        return false;
    }

    std::lock_guard<std::mutex> autoLock(callbackMapMutex_);
    auto iterSession = callbackMap_.find(sessionId);
    if (iterSession != callbackMap_.end()) {
        HILOGE("PushCallback sessionId:%{public}d exist!", sessionId);
        return false;
    }

    CallbackSessionItem item {
        .callback = callback,
        .sessionId = sessionId,
        .deviceId = deviceId,
        .isContinue = isContinue
    };
    (void)callbackMap_.emplace(sessionId, item);
    return true;
}

sptr<IRemoteObject> DmsCallbackSession::PopCallback(int32_t sessionId)
{
    std::lock_guard<std::mutex> autoLock(callbackMapMutex_);
    auto iter = callbackMap_.find(sessionId);
    if (iter == callbackMap_.end()) {
        HILOGW("PopCallback not found sessionId:%{public}d", sessionId);
        return nullptr;
    }

    CallbackSessionItem item = iter->second;
    (void)callbackMap_.erase(iter);
    if (dmsCallbackHandler_ != nullptr) {
        dmsCallbackHandler_->RemoveEvent(sessionId);
    }

    return item.callback;
}

int32_t DmsCallbackSession::GetContinuaionSessionId(int32_t freeInstallSessionId)
{
    if (freeInstallSessionId <= 0) {
        return INVALID_PARAMETERS_ERR;
    }

    std::lock_guard<std::mutex> autoLock(callbackMapMutex_);
    auto iter = continuationSessionMap_.find(freeInstallSessionId);
    if (iter != continuationSessionMap_.end()) {
        return INVALID_PARAMETERS_ERR;
    }
    return iter->second;
}

void DmsCallbackSession::SetContinuationSessionMap(int32_t freeInstallSessionId, int32_t continueSessionId)
{
    if (freeInstallSessionId <= 0 || continueSessionId <= 0) {
        HILOGD("param invalid");
        return;
    }
    std::lock_guard<std::mutex> autoLock(callbackMapMutex_);
    auto itreator = continuationSessionMap_.find(freeInstallSessionId);
    if (itreator != continuationSessionMap_.end()) {
        HILOGD("freeInstallSessionId = %{public}d", freeInstallSessionId);
        return;
    }
    continuationSessionMap_[freeInstallSessionId] = continueSessionId;
}

bool DmsCallbackSession::GetContinueFlag(int32_t sessionId)
{
    if (sessionId <= 0) {
        HILOGD("GetContinueFlag param sessionId invalid");
        return false;
    }
    std::lock_guard<std::mutex> autoLock(callbackMapMutex_);
    auto iterSession = callbackMap_.find(sessionId);
    if (iterSession == callbackMap_.end()) {
        HILOGE("GetContinueFlag not found sessionId : %{public}d", sessionId);
        return false;
    }
    CallbackSessionItem item = iterSession->second;
    return item.isContinue;
}

void DmsCallbackSession::NotifyDeviceOffline(const std::string& deviceId)
{
    HILOGI("%{public}s begin.", __func__);
    std::lock_guard<std::mutex> autoLock(callbackMapMutex_);
    for (const auto& [key, value] : callbackMap_) {
        if (value.deviceId == deviceId) {
            DistributedSchedService::GetInstance().NotifyFreeInstall(value.sessionId,
                DEVICE_OFFLINE_ERR);
        }
    }
}
}  // namespace DistributedSchedule
}  // namespace OHOS
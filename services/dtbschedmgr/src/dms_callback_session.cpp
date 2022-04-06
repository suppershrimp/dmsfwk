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

int64_t DmsCallbackSession::GenerateSessionId()
{
    std::lock_guard<std::mutex> autoLock(sessionMutex_);
    int64_t currValue = currSessionId_.load(std::memory_order_relaxed);
    currSessionId_++;
    return currValue;
}

int32_t DmsCallbackSession::PushCallback(int64_t sessionId, const sptr<IRemoteObject>& callback,
    const std::string& deviceId, bool isContinue, const OHOS::AAFwk::Want& want)
{
    if (sessionId < 0) {
        HILOGE("PushCallback sessionId invalid!");
        return INVALID_PARAMETERS_ERR;
    }

    if (callback == nullptr) {
        HILOGE("PushCallback callback null!");
        return INVALID_PARAMETERS_ERR;
    }

    if (dmsCallbackHandler_ == nullptr) {
        HILOGE("PushCallback not initialized!");
        return INVALID_PARAMETERS_ERR;
    }

    bool ret = dmsCallbackHandler_->SendEvent(sessionId, 0, Callback_DELAY_TIME);
    if (!ret) {
        HILOGE("PushCallback SendEvent failed!");
        return INVALID_PARAMETERS_ERR;
    }

    std::lock_guard<std::mutex> autoLock(callbackMapMutex_);
    auto iterSession = callbackMap_.find(sessionId);
    if (iterSession != callbackMap_.end()) {
        HILOGE("PushCallback sessionId:%{public}" PRId64 " exist!", sessionId);
        return INVALID_PARAMETERS_ERR;
    }

    CallbackSessionItem item {
        .callback = callback,
        .sessionId = sessionId,
        .deviceId = deviceId,
        .isContinue = isContinue,
        .want = want,
    };
    (void)callbackMap_.emplace(sessionId, item);
    return ERR_OK;
}

int32_t DmsCallbackSession::PopCallback(int64_t sessionId, CallbackSessionItem& item)
{
    std::lock_guard<std::mutex> autoLock(callbackMapMutex_);
    auto iter = callbackMap_.find(sessionId);
    if (iter == callbackMap_.end()) {
        HILOGW("PopCallback not found sessionId:%{public}" PRId64 "!", sessionId);
        return INVALID_PARAMETERS_ERR;
    }

    item = iter->second;
    (void)callbackMap_.erase(iter);
    if (dmsCallbackHandler_ != nullptr) {
        dmsCallbackHandler_->RemoveEvent(sessionId);
    }

    return ERR_OK;
}

int64_t DmsCallbackSession::GetContinuaionSessionId(int64_t freeInstallSessionId)
{
    if (freeInstallSessionId <= 0) {
        return INVALID_PARAMETERS_ERR;
    }

    std::lock_guard<std::mutex> autoLock(callbackMapMutex_);
    auto iter = continuationSessionMap_.find(freeInstallSessionId);
    if (iter == continuationSessionMap_.end()) {
        HILOGW("GetContinuaionSessionId not found freeInstallSessionId:%{public}" PRId64 "!", freeInstallSessionId);
        return INVALID_PARAMETERS_ERR;
    }
    HILOGI("GetContinuaionSessionId end, freeInstallSessionId :%{public}" PRId64 ".", freeInstallSessionId);
    return iter->second;
}

void DmsCallbackSession::SetContinuationSessionMap(int64_t freeInstallSessionId, int64_t continueSessionId)
{
    HILOGI("SetContinuationSessionMap freeInstallSessionId = %{public}" PRId64 ", sessionId = %{public}" PRId64 ".",
        freeInstallSessionId, continueSessionId);
    if (freeInstallSessionId <= 0 || continueSessionId <= 0) {
        HILOGD("param invalid");
        return;
    }
    std::lock_guard<std::mutex> autoLock(callbackMapMutex_);
    auto itreator = continuationSessionMap_.find(freeInstallSessionId);
    if (itreator != continuationSessionMap_.end()) {
        HILOGD("freeInstallSessionId = %{public}" PRId64 ".", freeInstallSessionId);
        return;
    }
    continuationSessionMap_[freeInstallSessionId] = continueSessionId;
}

bool DmsCallbackSession::GetContinueFlag(int64_t sessionId)
{
    if (sessionId <= 0) {
        HILOGD("GetContinueFlag param sessionId invalid");
        return false;
    }
    std::lock_guard<std::mutex> autoLock(callbackMapMutex_);
    auto iterSession = callbackMap_.find(sessionId);
    if (iterSession == callbackMap_.end()) {
        HILOGE("GetContinueFlag not found sessionId : %{public}" PRId64 "!", sessionId);
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
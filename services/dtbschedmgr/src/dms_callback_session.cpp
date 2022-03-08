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

void DmsCallbackSession::SetTimeOut(int32_t sessionId)
{
    if (dmsCallbackHandler_ == nullptr) {
        HILOGE("continuationHandler not initialized!");
        return;
    }
    dmsCallbackHandler_->SendEvent(sessionId, 0, Callback_DELAY_TIME);
}

void DmsCallbackSession::RemoveTimeOut(int32_t sessionId)
{
    if (dmsCallbackHandler_ == nullptr) {
        HILOGE("continuationHandler not initialized!");
        return;
    }
    dmsCallbackHandler_->RemoveEvent(sessionId);
}

bool DmsCallbackSession::PushCallback(int32_t sessionId, const sptr<IRemoteObject>& callback)
{
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

    (void)callbackMap_.emplace(sessionId, callback);
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

    sptr<IRemoteObject> callback = iter->second;
    (void)callbackMap_.erase(iter);
    if (dmsCallbackHandler_ != nullptr) {
        dmsCallbackHandler_->RemoveEvent(sessionId);
    }

    return callback;
}
}  // namespace DistributedSchedule
}  // namespace OHOS
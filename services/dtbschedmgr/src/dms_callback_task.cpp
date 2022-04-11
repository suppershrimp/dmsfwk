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

#include "dms_callback_task.h"

#include "dtbschedmgr_log.h"
#include "distributed_sched_service.h"
#include "parcel_helper.h"

namespace OHOS {
namespace DistributedSchedule {
using namespace OHOS::AppExecFwk;

namespace {
constexpr int64_t CALLBACK_DELAY_TIME = 30000;
const std::string TAG = "DmsCallbackTask";
}

void DmsCallbackTask::Init(const DmsCallbackTaskInitCallbackFunc &callback)
{
    auto runner = EventRunner::Create("dsched_callback");
    dmsCallbackHandler_ = std::make_shared<DmsCallbackHandler>(runner, shared_from_this(), callback);
}

int64_t DmsCallbackTask::GenerateTaskId()
{
    std::lock_guard<std::mutex> autoLock(taskMutex_);
    int64_t currValue = currTaskId_.load(std::memory_order_relaxed);
    currTaskId_++;
    return currValue;
}

int32_t DmsCallbackTask::PushCallback(int64_t taskId, const sptr<IRemoteObject> &callback,
    const std::string &deviceId, bool isContinue, const OHOS::AAFwk::Want &want)
{
    if (taskId < 0) {
        HILOGE("PushCallback taskId invalid!");
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

    bool ret = dmsCallbackHandler_->SendEvent(taskId, 0, CALLBACK_DELAY_TIME);
    if (!ret) {
        HILOGE("PushCallback SendEvent failed!");
        return INVALID_PARAMETERS_ERR;
    }

    std::lock_guard<std::mutex> autoLock(callbackMapMutex_);
    auto iterTask = callbackMap_.find(taskId);
    if (iterTask != callbackMap_.end()) {
        HILOGE("PushCallback taskId:%{public}" PRId64 " exist!", taskId);
        return INVALID_PARAMETERS_ERR;
    }

    CallbackTaskItem item {
        .callback = callback,
        .taskId = taskId,
        .isContinue = isContinue,
        .deviceId = deviceId,
        .want = want,
    };
    (void)callbackMap_.emplace(taskId, item);
    return ERR_OK;
}

CallbackTaskItem DmsCallbackTask::PopCallback(int64_t taskId)
{
    std::lock_guard<std::mutex> autoLock(callbackMapMutex_);
    auto iter = callbackMap_.find(taskId);
    CallbackTaskItem item = {};
    if (iter == callbackMap_.end()) {
        HILOGW("PopCallback not found taskId:%{public}" PRId64 "!", taskId);
        return item;
    }
    item = iter->second;
    (void)callbackMap_.erase(iter);
    if (dmsCallbackHandler_ != nullptr) {
        dmsCallbackHandler_->RemoveEvent(taskId);
    }
    return item;
}

int64_t DmsCallbackTask::GetContinuaionMissionId(int64_t taskId)
{
    if (taskId <= 0) {
        return INVALID_PARAMETERS_ERR;
    }

    std::lock_guard<std::mutex> autoLock(callbackMapMutex_);
    auto iter = continuationMissionMap_.find(taskId);
    if (iter == continuationMissionMap_.end()) {
        HILOGW("GetContinuaionMissionId not found taskId:%{public}" PRId64 "!", taskId);
        return INVALID_PARAMETERS_ERR;
    }
    HILOGI("GetContinuaionMissionId end, taskId :%{public}" PRId64 ".", taskId);
    return iter->second;
}

void DmsCallbackTask::SetContinuationMissionMap(int64_t taskId, int64_t missionId)
{
    HILOGI("SetContinuationMissionMap taskId = %{public}" PRId64 ", taskId = %{public}" PRId64 ".", taskId, missionId);
    if (taskId <= 0 || missionId <= 0) {
        HILOGD("param invalid");
        return;
    }
    std::lock_guard<std::mutex> autoLock(callbackMapMutex_);
    auto itreator = continuationMissionMap_.find(taskId);
    if (itreator != continuationMissionMap_.end()) {
        HILOGD("taskId = %{public}" PRId64 ".", taskId);
        return;
    }
    continuationMissionMap_[taskId] = missionId;
}

bool DmsCallbackTask::GetContinueFlag(int64_t taskId)
{
    if (taskId <= 0) {
        HILOGD("GetContinueFlag param taskId invalid");
        return false;
    }
    std::lock_guard<std::mutex> autoLock(callbackMapMutex_);
    auto iterTask = callbackMap_.find(taskId);
    if (iterTask == callbackMap_.end()) {
        HILOGE("GetContinueFlag not found taskId : %{public}" PRId64 "!", taskId);
        return false;
    }
    CallbackTaskItem item = iterTask->second;
    return item.isContinue;
}

void DmsCallbackTask::NotifyDeviceOffline(const std::string &deviceId)
{
    HILOGI("%{public}s begin.", __func__);
    std::lock_guard<std::mutex> autoLock(callbackMapMutex_);
    for (auto it = callbackMap_.begin(); it != callbackMap_.end();) {
        if (it->second.deviceId == deviceId) {
            (void)callbackMap_.erase(it++);
            if (dmsCallbackHandler_ != nullptr) {
                dmsCallbackHandler_->RemoveEvent(it->second.taskId);
            }
            DistributedSchedService::GetInstance().NotifyFreeInstallResult(it->second, DEVICE_OFFLINE_ERR);
        } else {
            it++;
        }
    }
}
}  // namespace DistributedSchedule
}  // namespace OHOS

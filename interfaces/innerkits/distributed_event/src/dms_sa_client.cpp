/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include "dms_sa_client.h"

namespace OHOS {
namespace DistributedSchedule {
DmsSaClient &DmsSaClient::GetInstance()
{
    HILOGD("called.");
    static DmsSaClient instance;
    return instance;
}

bool DmsSaClient::SubscribeDmsSA()
{
    HILOGD("called.");
    sptr<DmsSystemAbilityStatusChange> callback(new DmsSystemAbilityStatusChange());
    int32_t ret = saMgrProxy_->SubscribeSystemAbility(DISTRIBUTED_SCHED_SA_ID, callback);
    if (ret != ERR_OK) {
        HILOGE("Failed to subscribe system ability DISTRIBUTED_SCHED_SA_ID ret:%{public}d", ret);
        return false;
    }
    return true;
}

int32_t DmsSaClient::AddDSchedEventListener(const std::string& type, const sptr<IDSchedEventListener>& listener)
{
    HILOGI("%{public}s called, the type is %{public}s", __func__, type.c_str());
    saMgrProxy_ = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saMgrProxy_ == nullptr) {
        HILOGE("fail to get saMgrProxy.");
        return AAFwk::INNER_ERR;
    }
    if (saMgrProxy_->CheckSystemAbility(DISTRIBUTED_SCHED_SA_ID)) {
        DistributedClient distributedClient;
        distributedClient.RegisterDSchedEventListener(type, listener);
    }
    std::lock_guard<std::mutex> lock(eventMutex_);
    if (!hasSubscribeDmsSA_) {
        if (SubscribeDmsSA()) {
            hasSubscribeDmsSA_ = true;
            listeners_[type] = listener;
        } else {
            return AAFwk::INNER_ERR;
        }
    }
    return NO_ERROR;
}

int32_t DmsSaClient::DelDSchedEventListener(const std::string& type, const sptr<IDSchedEventListener>& listener)
{
    HILOGI("%{public}s called, the type is %{public}s", __func__, type.c_str());
    if (saMgrProxy_->CheckSystemAbility(DISTRIBUTED_SCHED_SA_ID)) {
        DistributedClient distributedClient;
        distributedClient.UnRegisterDSchedEventListener(type, listener);
    }
    std::lock_guard<std::mutex> lock(eventMutex_);
    listeners_.erase(type);
    return NO_ERROR;
}

int32_t DmsSaClient::GetContinueInfo(ContinueInfo &continueInfo)
{
    HILOGI("%{public}s called", __func__);
    saMgrProxy_ = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saMgrProxy_ == nullptr) {
        HILOGE("fail to get saMgrProxy.");
        return AAFwk::INNER_ERR;
    }
    if (saMgrProxy_->CheckSystemAbility(DISTRIBUTED_SCHED_SA_ID)) {
        DistributedClient distributedClient;
        distributedClient.GetContinueInfo(continueInfo);
    }
    return NO_ERROR;
}

void DmsSaClient::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    HILOGI("%{public}s called, the systemAbilityId is %{public}d", __func__, systemAbilityId);
    std::lock_guard<std::mutex> lock(eventMutex_);
    if (systemAbilityId == DISTRIBUTED_SCHED_SA_ID) {
        HILOGI("%{public}s listeners size: %{public}zu .", __func__, listeners_.size());
        for (auto& listener : listeners_) {
            DistributedClient distributedClient;
            distributedClient.RegisterDSchedEventListener(listener.first, listener.second);
        }
    } else {
        HILOGE("SystemAbilityId must be DISTRIBUTED_SCHED_SA_ID,but it is %{public}d", systemAbilityId);
    }
}

void DmsSaClient::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
}

DmsSystemAbilityStatusChange::DmsSystemAbilityStatusChange()
{
    HILOGI("DmsSystemAbilityStatusChange create");
}

DmsSystemAbilityStatusChange::~DmsSystemAbilityStatusChange()
{
    HILOGI("DmsSystemAbilityStatusChange delete");
}

void DmsSystemAbilityStatusChange::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    HILOGI("OnAddSystemAbility called, the systemAbilityId is %d", systemAbilityId);
    if (systemAbilityId != DISTRIBUTED_SCHED_SA_ID) {
        HILOGE("SystemAbilityId must be DISTRIBUTED_SCHED_SA_ID,but it is %d", systemAbilityId);
        return;
    }

    DmsSaClient::GetInstance().OnAddSystemAbility(systemAbilityId, deviceId);
    HILOGI("OnAddSystemAbility called end, the systemAbilityId is %d", systemAbilityId);
}

void DmsSystemAbilityStatusChange::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    HILOGI("OnRemoveSystemAbility called, the systemAbilityId is %d", systemAbilityId);
    if (systemAbilityId != DISTRIBUTED_SCHED_SA_ID) {
        HILOGE("SystemAbilityId must be DISTRIBUTED_SCHED_SA_ID,but it is %d", systemAbilityId);
        return;
    }

    // notify listener
}
}  // namespace DistributedSchedule
}  // namespace OHOS
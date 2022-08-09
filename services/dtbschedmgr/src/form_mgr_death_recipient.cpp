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

#include "form_mgr_death_recipient.h"

#include "distributed_sched_service.h"
#include "dtbschedmgr_log.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace DistributedSchedule {
namespace {
const std::string TAG = "FormMgrDeathRecipient";
}
void FormMgrDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    HILOGI("OnRemoteDied called");
    DistributedSchedService::GetInstance().ProcessFormMgrDied(remote);
}
} // namespace DistributedSchedule
} // namespace OHOS
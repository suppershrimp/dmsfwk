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

#ifndef OHOS_DMS_FREE_INSTALL_CALLBACK_H
#define OHOS_DMS_FREE_INSTALL_CALLBACK_H

#include <iremote_object.h>
#include <iremote_stub.h>

#include "ability_info.h"
#include "caller_info.h"
#include "dms_free_install_callback_stub.h"
#include "want.h"

namespace OHOS {
namespace DistributedSchedule {
/**
 * @class DmsFreeInstallCallback
 * DmsFreeInstallCallback.
 */
class DmsFreeInstallCallback : public DmsFreeInstallCallbackStub {
public:
    DmsFreeInstallCallback(int32_t sessionId, const CallerInfo& callerInfo,
        const AAFwk::Want& want);
    virtual ~DmsFreeInstallCallback() = default;

    /**
     * OnInstallFinished, FreeInstall is complete.
     *
     * @param resultCode, ERR_OK on success, others on failure.
     */
    void OnInstallFinished(int resultCode) override;

private:
    int32_t sessionId_;
    CallerInfo callerInfo_;
    AAFwk::Want want_;
};
}  // namespace DistributedSchedule
}  // namespace OHOS
#endif  // OHOS_DMS_FREE_INSTALL_CALLBACK_H

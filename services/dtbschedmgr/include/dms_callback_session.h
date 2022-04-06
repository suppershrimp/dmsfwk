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

#ifndef OHOS_DMS_CALLBACK_SESSION_H
#define OHOS_DMS_CALLBACK_SESSION_H

#include <atomic>
#include <cstdint>
#include <map>
#include <mutex>

#include "event_handler.h"
#include "iremote_object.h"
#include "refbase.h"
#include "want.h"

namespace OHOS {
namespace DistributedSchedule {
using DmsCallbackSessionInitCallbackFunc = std::function<void(int64_t sessionId)>;

struct CallbackSessionItem {
    sptr<IRemoteObject> callback = nullptr;
    int64_t sessionId = -1;
    bool isContinue = false;
    std::string deviceId = "";
    OHOS::AAFwk::Want want;
};

class DmsCallbackSession : public std::enable_shared_from_this<DmsCallbackSession> {
public:
    void Init(const DmsCallbackSessionInitCallbackFunc& callback);
    int64_t GenerateSessionId();
    int32_t PushCallback(int64_t sessionId, const sptr<IRemoteObject>& callback,
        const std::string& deviceId, bool isContinue, const OHOS::AAFwk::Want& want);
    int32_t PopCallback(int64_t sessionId, CallbackSessionItem& item);
    void SetContinuationSessionMap(int64_t freeInstallSessionId, int64_t continueSessionId);
    bool GetContinueFlag(int64_t sessionId);
    int64_t GetContinuaionSessionId(int64_t sessionId);
    void NotifyDeviceOffline(const std::string& deviceId);

private:
    class DmsCallbackHandler : public AppExecFwk::EventHandler {
    public:
        DmsCallbackHandler(const std::shared_ptr<AppExecFwk::EventRunner>& runner,
            const std::shared_ptr<DmsCallbackSession>& callbackSession,
            const DmsCallbackSessionInitCallbackFunc& callback)
            : AppExecFwk::EventHandler(runner), callbackSession_(callbackSession), callback_(callback) {}
        ~DmsCallbackHandler() = default;

    private:
        std::weak_ptr<DmsCallbackSession> callbackSession_;
        DmsCallbackSessionInitCallbackFunc callback_;
    };

    std::shared_ptr<DmsCallbackHandler> dmsCallbackHandler_;
    std::mutex callbackMapMutex_;
    std::mutex sessionMutex_;
    std::atomic<int64_t> currSessionId_ {1};
    std::map<int64_t, CallbackSessionItem> callbackMap_;
    std::map<int64_t, int64_t> continuationSessionMap_;
};
}  // namespace DistributedSchedule
}  // namespace OHOS
#endif  // OHOS_DMS_CALLBACK_SESSION_H
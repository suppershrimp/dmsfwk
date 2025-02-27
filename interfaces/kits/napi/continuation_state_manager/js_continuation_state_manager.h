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
#ifndef ABILITY_DMSFWK_JS_CONTINUATION_STATE_MANAGER_H
#define ABILITY_DMSFWK_JS_CONTINUATION_STATE_MANAGER_H

#include "js_continuation_state_manager_stub.h"
#include "base/continuationmgr_log.h"
#include "ability.h"

#include "native_engine/native_engine.h"
#include "napi_base_context.h"

namespace OHOS {
namespace DistributedSchedule {
class JsContinuationStateManager {
public:
    static napi_value ContinueStateCallbackOn(napi_env env, napi_callback_info info);
    static napi_value ContinueStateCallbackOff(napi_env env, napi_callback_info info);
    static napi_value MakeContinueStateCodeEnumObject(napi_env env);
private:
    static sptr<DistributedSchedule::JsContinuationStateManagerStub> CreateStub(napi_env env, napi_callback_info info);
    static void GetAbilityContext(std::shared_ptr<AbilityRuntime::AbilityContext> &abilityContext,
        napi_env env, napi_value context);
    static napi_value GenerateBusinessError(const napi_env &env, int32_t errCode, const std::string &errMsg);
    static napi_status MakeEnumItem(const napi_env &env, napi_value object, const char* name, int32_t value);
private:
    static std::map<std::string, sptr<DistributedSchedule::JsContinuationStateManagerStub>> callbackStubs_;
};
napi_value JsContinuationStateManagerInit(napi_env env, napi_value exportObj);
} // namespace DistributedSchedule
} // namespace OHOS
#endif //ABILITY_DMSFWK_JS_CONTINUATION_STATE_MANAGER_H

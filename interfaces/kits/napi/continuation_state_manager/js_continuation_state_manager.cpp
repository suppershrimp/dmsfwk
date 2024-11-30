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

#include "js_continuation_state_manager.h"
#include "js_continuation_state_client.h"

#include "napi_error_code.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace DistributedSchedule {
using namespace OHOS::AbilityRuntime;
using namespace OHOS::AppExecFwk;
namespace {
    const std::string TAG = "JsContinuationStateManager";
    const std::string BIZTYPE_PREPARE_CONTINUE = "prepareContinue";
    const int32_t SUCCESS = 0;
    const int32_t FAILED = 1;
}

napi_value JsContinuationStateManager::ContinueStateCallbackOn(napi_env env, napi_callback_info info)
{
    HILOGI("ContinueStateCallbackOn call");
    napi_value ret = nullptr;
    sptr <DistributedSchedule::JsContinuationStateManagerStub> stub = CreateStub(env, info);
    if (BIZTYPE_PREPARE_CONTINUE != stub.callbackData_.bizType) {
        HILOGE("ContinueStateCallbackOn Unsupported business type: %{public}s",
               stub.callbackData_.bizType.c_str());
        NAPI_CALL(env, napi_get_value_int32(env, ret, FAILED));
        return ret;
    }
    DistributedSchedule::ContinuationStateClient client;
    int32_t result = client.RegisterContinueStateCallback(stub);
    HILOGI("ContinueStateCallbackOn register callback result: %{public}d", result);
    NAPI_CALL(env, napi_get_value_int32(env, ret, result));
    return ret;
}

napi_value JsContinuationStateManager::ContinueStateCallbackOff(napi_env env, napi_callback_info info)
{
    HILOGI("ContinueStateCallbackOff call");
    napi_value ret = nullptr;
    sptr <DistributedSchedule::JsContinuationStateManagerStub> stub = CreateStub(env, info);
    if (BIZTYPE_PREPARE_CONTINUE != stub.callbackData_.bizType) {
        HILOGE("ContinueStateCallbackOff Unsupported business type: %{public}s",
               stub.callbackData_.bizType.c_str());
        NAPI_CALL(env, napi_get_value_int32(env, ret, FAILED));
        return ret;
    }
    DistributedSchedule::ContinuationStateClient client;
    int32_t result = client.UnRegisterContinueStateCallback(stub);
    HILOGI("ContinueStateCallbackOff unregister callback result: %{public}d", result);
    NAPI_CALL(env, napi_get_value_int32(env, ret, result));
    return ret;
}

sptr<DistributedSchedule::JsContinuationStateManagerStub> JsContinuationStateManager::CreateStub(
    napi_env env, napi_callback_info info)
{
    size_t argc = 3;
    napi_value args[3];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    // this.context is 2ed parameter
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext = nullptr;
    GetAbilityContext(abilityContext, env, args[1]);
    if (abilityContext == nullptr) {
        HILOGE("get ability context failed");
        return nullptr;
    }
    std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo = abilityContext->GetAbilityInfo();

    napi_valuetype valuetype;
    NAPI_CALL(env, napi_typeof(env, args[2], &valuetype));
    if (valuetype != napi_function) {
        return nullptr;
    }
    napi_ref callbackRef = nullptr;
    napi_create_reference(env, args[2], 1, &callbackRef);

    sptr <DistributedSchedule::JsContinuationStateManagerStub> stub(
            new DistributedSchedule::JsContinuationStateManagerStub());
    DistributedSchedule::JsContinuationStateManagerStub::StateCallbackData callbackData;
    napi_get_value_string(env, args[0], &callbackData.bizType);
    callbackData.bundleName = abilityContext->GetBundleName();
    callbackData.abilityName = abilityInfo->name;
    callbackData.callbackRef = callbackRef;
    callbackData.env = env;
    stub->callbackData_ = callbackData;
    return stub;
}

void JsContinuationStateManager::GetAbilityContext(
    std::shared_ptr<AbilityRuntime::AbilityContext> &abilityContext, napi_env env, napi_value context)
{
    bool stageMode = false;
    napi_status status = OHOS::AbilityRuntime::IsStageContext(env, context, stageMode);
    if (status != napi_ok || !stageMode) {
        auto ability = OHOS::AbilityRuntime::GetCurrentAbility(env);
        if (!ability) {
            HILOGE("get ability info failed");
            return;
        }
        std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext = ability->GetAbilityContext();
        if (!abilityContext) {
            HILOGE("get ability context failed");
            return;
        }
    } else {
        auto modeContext = AbilityRuntime::GetStageModeContext(env, context);
        if (!modeContext) {
            HILOGE("get stageMode ability info failed");
            return;
        }
        std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext = AbilityRuntime::Context::ConvertTo<AbilityRuntime::AbilityContext>(
                modeContext);
        if (!abilityContext) {
            HILOGE("get stageMode ability context failed");
            return;
        }
    }
}

napi_value JsContinuationStateManagerInit(napi_env env, napi_value exportObj)
{
    static napi_property_descriptor desc[] = {
            DECLARE_NAPI_FUNCTION("on", JsContinuationStateManager::ContinueStateCallbackOn),
            DECLARE_NAPI_FUNCTION("off", JsContinuationStateManager::ContinueStateCallbackOff),
    };
    NAPI_CALL(env, napi_define_properties(env, exportObj, sizeof(desc) / sizeof(desc[0]), desc));
    return exportObj;
}

static napi_module continuationStateManagerModule = {
        .nm_filename = "continuation/libcontinuationstatemanager_napi.so/continuationstatemanager.js",
        .nm_register_func = OHOS::DistributedSchedule::JsContinuationStateManagerInit,
        .nm_modname = "continuation.continuationStateManager",
};

extern "C" __attribute__((constructor)) void ContinuationStateManagerModuleRegister()
{
    napi_module_register(&continuationStateManagerModule);
}
} // namespace DistributedSchedule
} // namespace OHOS
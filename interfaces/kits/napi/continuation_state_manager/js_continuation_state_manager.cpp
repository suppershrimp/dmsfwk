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

namespace OHOS{
    namespace DistributedSchedule{
        using namespace OHOS::AbilityRuntime;
        using namespace OHOS::AppExecFwk;
        namespace {
        const std::string TAG = "JsContinuationStateManager";
        }

        napi_value JsContinuationStateManager::ContinueStateCallbackOn(napi_env env, napi_callback_info info){
            sptr<DistributedSchedule::JsContinuationStateManagerStub> stub = CreateStub(env, info);
            DistributedSchedule::ContinuationStateClient client;
            int32_t result = client.RegisterContinueStateCallback(stub);
            napi_value ret = 0;
            NAPI_CALL(env, napi_get_null(env, &ret));
            return ret;
        }

        napi_value JsContinuationStateManager::ContinueStateCallbackOff(napi_env env, napi_callback_info info){
            sptr<DistributedSchedule::JsContinuationStateManagerStub> stub = CreateStub(env, info);
            DistributedSchedule::ContinuationStateClient client;
            int32_t result = client.UnRegisterContinueStateCallback(stub);
            napi_value ret = 0;
            NAPI_CALL(env, napi_get_null(env, &ret));
            return ret;
        }

        sptr<DistributedSchedule::JsContinuationStateManagerStub> JsContinuationStateManager::CreateStub(napi_env env, napi_callback_info info){
            size_t argc = 2;
            napi_value args[2];
            napi_value thisArg = nullptr;
            NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));
            bool stageMode = false;
            napi_status status = OHOS::AbilityRuntime::IsStageContext(env, args[0], stageMode);
            std::string bundleName;
            std::string abilityName;
            if(status != napi_ok || !stageMode){
                auto ability = OHOS::AbilityRuntime::GetCurrentAbility(env);
                if(!ability){
                    return nullptr;
                }
                std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext = ability->GetAbilityContext();
                if(!abilityContext){
                    return nullptr;
                }
                bundleName = abilityContext->GetBundleName();
                std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo = abilityContext->GetAbilityInfo();
                abilityName = abilityInfo->name;
            }else{
                auto context = AbilityRuntime::GetStageModeContext(env, args[0]);
                if(!context){
                    return nullptr;
                }
                std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext = AbilityRuntime::Context::ConvertTo<AbilityRuntime::AbilityContext>(context);
                if(!abilityContext){
                    return nullptr;
                }
                bundleName = abilityContext->GetBundleName();
                std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo = abilityContext->GetAbilityInfo();
                abilityName = abilityInfo->name;
            }

            napi_valuetype valuetype;
            NAPI_CALL(env, napi_typeof(env, args[1], &valuetype));
            if(valuetype != napi_function){
                napi_throw_type_error(env, nullptr, "Callback function expected.");
                return NULL;
            }
            napi_ref callbackRef= nullptr;
            napi_create_reference(env, args[1], 1, &callbackRef);

            sptr<DistributedSchedule::JsContinuationStateManagerStub> stub(new DistributedSchedule::JsContinuationStateManagerStub());
            DistributedSchedule::JsContinuationStateManagerStub::StateCallbackData callbackData;
            callbackData.bundleName = bundleName;
            callbackData.abilityName = abilityName;
            callbackData.callbackRef = callbackRef;
            callbackData.env = env;
            stub->callbackData_ = callbackData;

            return stub;
        }

        napi_value JsContinuationStateManagerInit(napi_env env, napi_value exportObj){
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

        extern "C" __attribute__((constructor)) void ContinuationStateManagerModuleRegister(){
            napi_module_register(&continuationStateManagerModule);
        }
    } // namespace DistributedSchedule
} // namespace OHOS
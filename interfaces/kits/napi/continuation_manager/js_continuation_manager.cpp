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

#include "js_continuation_manager.h"

#include <memory>

#include "device_connect_status.h"
#include "distributed_ability_manager_client.h"
#include "dtbschedmgr_log.h"
#include "js_runtime_utils.h"
#include "napi_common_util.h"
#include "napi_error_code.h"

namespace OHOS {
namespace DistributedSchedule {
using namespace OHOS::AbilityRuntime;
using namespace OHOS::AppExecFwk;
namespace {
const std::string TAG = "JsContinuationManager";
const std::string CODE_KEY_NAME = "code";
constexpr int32_t ERR_NOT_OK = -1;
constexpr int32_t ARG_COUNT_ONE = 1;
constexpr int32_t ARG_COUNT_TWO = 2;
constexpr int32_t ARG_COUNT_THREE = 3;
constexpr uint32_t MAX_JSPROCOUNT = 1000000;
constexpr int32_t ARG_COUNT_FOUR = 4;
}

void JsContinuationManager::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    HILOGI("JsContinuationManager::Finalizer is called");
    JsContinuationManager* jsContinuationManager = static_cast<JsContinuationManager*>(data);
    if (jsContinuationManager != nullptr) {
        delete jsContinuationManager;
        jsContinuationManager = nullptr;
    }
}

NativeValue* JsContinuationManager::Register(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsContinuationManager* me = CheckParamsAndGetThis<JsContinuationManager>(engine, info);
    return (me != nullptr) ? me->OnRegister(*engine, *info) : nullptr;
}

NativeValue* JsContinuationManager::Unregister(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsContinuationManager* me = CheckParamsAndGetThis<JsContinuationManager>(engine, info);
    return (me != nullptr) ? me->OnUnregister(*engine, *info) : nullptr;
}

NativeValue* JsContinuationManager::RegisterDeviceSelectionCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsContinuationManager* me = CheckParamsAndGetThis<JsContinuationManager>(engine, info);
    return (me != nullptr) ? me->OnRegisterDeviceSelectionCallback(*engine, *info) : nullptr;
}

NativeValue* JsContinuationManager::UnregisterDeviceSelectionCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsContinuationManager* me = CheckParamsAndGetThis<JsContinuationManager>(engine, info);
    return (me != nullptr) ? me->OnUnregisterDeviceSelectionCallback(*engine, *info) : nullptr;
}

NativeValue *JsContinuationManager::UpdateConnectStatus(NativeEngine *engine, NativeCallbackInfo *info)
{
    JsContinuationManager *me = CheckParamsAndGetThis<JsContinuationManager>(engine, info);
    return (me != nullptr) ? me->OnUpdateConnectStatus(*engine, *info) : nullptr;
}

NativeValue *JsContinuationManager::StartDeviceManager(NativeEngine *engine, NativeCallbackInfo *info)
{
    JsContinuationManager *me = CheckParamsAndGetThis<JsContinuationManager>(engine, info);
    return (me != nullptr) ? me->OnStartDeviceManager(*engine, *info) : nullptr;
}

NativeValue *JsContinuationManager::InitDeviceConnectStateObject(NativeEngine *engine, NativeCallbackInfo *info)
{
    JsContinuationManager *me = CheckParamsAndGetThis<JsContinuationManager>(engine, info);
    return (me != nullptr) ? me->OnInitDeviceConnectStateObject(*engine, *info) : nullptr;
}

NativeValue *JsContinuationManager::InitContinuationModeObject(NativeEngine *engine, NativeCallbackInfo *info)
{
    JsContinuationManager *me = CheckParamsAndGetThis<JsContinuationManager>(engine, info);
    return (me != nullptr) ? me->OnInitContinuationModeObject(*engine, *info) : nullptr;
}

NativeValue* JsContinuationManager::RegisterContinuation(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsContinuationManager* me = CheckParamsAndGetThis<JsContinuationManager>(engine, info);
    return (me != nullptr) ? me->OnRegisterContinuation(*engine, *info) : nullptr;
}

NativeValue* JsContinuationManager::UnregisterContinuation(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsContinuationManager* me = CheckParamsAndGetThis<JsContinuationManager>(engine, info);
    return (me != nullptr) ? me->OnUnregisterContinuation(*engine, *info) : nullptr;
}

NativeValue* JsContinuationManager::UpdateContinuationState(NativeEngine* engine, NativeCallbackInfo *info)
{
    JsContinuationManager *me = CheckParamsAndGetThis<JsContinuationManager>(engine, info);
    return (me != nullptr) ? me->OnUpdateContinuationState(*engine, *info) : nullptr;
}

NativeValue* JsContinuationManager::StartContinuationDeviceManager(NativeEngine* engine, NativeCallbackInfo *info)
{
    JsContinuationManager *me = CheckParamsAndGetThis<JsContinuationManager>(engine, info);
    return (me != nullptr) ? me->OnStartContinuationDeviceManager(*engine, *info) : nullptr;
}

NativeValue* JsContinuationManager::OnRegister(NativeEngine &engine, NativeCallbackInfo &info)
{
    HILOGD("called.");
    int32_t errCode = 0;
    decltype(info.argc) unwrapArgc = 0;
    std::shared_ptr<ContinuationExtraParams> continuationExtraParams = std::make_shared<ContinuationExtraParams>();
    if (info.argc > 0 && info.argv[0]->TypeOf() == NATIVE_OBJECT) {
        HILOGI("register options is used.");
        if (!UnWrapContinuationExtraParams(reinterpret_cast<napi_env>(&engine),
            reinterpret_cast<napi_value>(info.argv[0]), continuationExtraParams)) {
            HILOGE("Parse continuationExtraParams failed");
            errCode = ERR_NOT_OK;
        }
        unwrapArgc++;
    }
    AsyncTask::CompleteCallback complete =
        [continuationExtraParams, unwrapArgc, errCode](NativeEngine &engine, AsyncTask &task, int32_t status) {
        if (errCode != 0) {
            task.Reject(engine, CreateJsError(engine, errCode, "Invalidate params."));
            return;
        }
        int32_t token = -1;
        int32_t ret = (unwrapArgc == 0) ? DistributedAbilityManagerClient::GetInstance().Register(nullptr, token) :
            DistributedAbilityManagerClient::GetInstance().Register(continuationExtraParams, token);
        if (ret == ERR_OK) {
            task.Resolve(engine, engine.CreateNumber(token));
        } else {
            task.Reject(engine, CreateJsError(engine, ret, "Register failed."));
        }
    };

    NativeValue* lastParam = (info.argc <= unwrapArgc) ? nullptr : info.argv[unwrapArgc];
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsContinuationManager::OnRegister",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsContinuationManager::OnRegisterContinuation(NativeEngine &engine, NativeCallbackInfo &info)
{
    HILOGD("called.");
    decltype(info.argc) unwrapArgc = 0;
    std::shared_ptr<ContinuationExtraParams> continuationExtraParams;
    std::string errInfo = [this, &engine, &info, &continuationExtraParams, &unwrapArgc]() -> std::string {
        if (info.argc > ARG_COUNT_TWO) {
            return "Parameter error. The type of \"number of parameters\" must be less than 3";
        }
        if (info.argc > 0 && info.argv[0]->TypeOf() == NATIVE_OBJECT) {
            HILOGI("register options is used.");
            continuationExtraParams = std::make_shared<ContinuationExtraParams>();
            if (!UnWrapContinuationExtraParams(reinterpret_cast<napi_env>(&engine),
                reinterpret_cast<napi_value>(info.argv[0]), continuationExtraParams)) {
                return "Parameter error. The type of \"options\" must be ContinuationExtraParams";
            }
            unwrapArgc++;
        }
        return std::string();
    } ();
    if (!errInfo.empty()) {
        HILOGE("%{public}s", errInfo.c_str());
        napi_throw(reinterpret_cast<napi_env>(&engine),
            GenerateBusinessError(reinterpret_cast<napi_env>(&engine), PARAMETER_CHECK_FAILED, errInfo));
        return engine.CreateUndefined();
    }
    AsyncTask::CompleteCallback complete =
        [this, continuationExtraParams, unwrapArgc](NativeEngine &engine, AsyncTask &task, int32_t status) {
        int32_t token = -1;
        int32_t errCode = (unwrapArgc == 0) ? DistributedAbilityManagerClient::GetInstance().Register(nullptr, token) :
            DistributedAbilityManagerClient::GetInstance().Register(continuationExtraParams, token);
        if (errCode == ERR_OK) {
            task.Resolve(engine, engine.CreateNumber(token));
        } else {
            errCode = ErrorCodeReturn(errCode);
            task.Reject(engine, CreateJsError(engine, errCode, ErrorMessageReturn(errCode)));
        }
    };

    NativeValue* lastParam = (info.argc <= unwrapArgc) ? nullptr : info.argv[unwrapArgc];
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsContinuationManager::OnRegisterContinuation",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsContinuationManager::OnUnregister(NativeEngine &engine, NativeCallbackInfo &info)
{
    HILOGD("called.");
    int32_t errCode = 0;
    if (info.argc == 0) {
        HILOGE("Params not match");
        errCode = ERR_NOT_OK;
    }
    int32_t token = -1;
    if (!errCode && !ConvertFromJsValue(engine, info.argv[0], token)) {
        HILOGE("Parse token failed");
        errCode = ERR_NOT_OK;
    }
    AsyncTask::CompleteCallback complete =
        [token, errCode](NativeEngine &engine, AsyncTask &task, int32_t status) {
        if (errCode != 0) {
            task.Reject(engine, CreateJsError(engine, errCode, "Invalidate params."));
            return;
        }
        int32_t ret = DistributedAbilityManagerClient::GetInstance().Unregister(token);
        if (ret == ERR_OK) {
            task.Resolve(engine, engine.CreateUndefined());
        } else {
            task.Reject(engine, CreateJsError(engine, ret, "Unregister failed."));
        }
    };

    NativeValue* lastParam = (info.argc <= ARG_COUNT_ONE) ? nullptr : info.argv[ARG_COUNT_ONE];
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsContinuationManager::OnUnregister",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsContinuationManager::OnUnregisterContinuation(NativeEngine &engine, NativeCallbackInfo &info)
{
    HILOGD("called.");
    int32_t token = -1;
    std::string errInfo = [this, &engine, &info, &token]() -> std::string {
        if (info.argc == 0 || info.argc > ARG_COUNT_TWO) {
            return "Parameter error. The type of \"number of parameters\" must be greater than 0 and less than 3";
        }
        if (!ConvertFromJsValue(engine, info.argv[0], token)) {
            return "Parameter error. The type of \"token\" must be number";
        }
        return std::string();
    } ();
    if (!errInfo.empty()) {
        HILOGE("%{public}s", errInfo.c_str());
        napi_throw(reinterpret_cast<napi_env>(&engine),
            GenerateBusinessError(reinterpret_cast<napi_env>(&engine), PARAMETER_CHECK_FAILED, errInfo));
        return engine.CreateUndefined();
    }
    AsyncTask::CompleteCallback complete =
        [this, token](NativeEngine &engine, AsyncTask &task, int32_t status) {
        int32_t errCode = DistributedAbilityManagerClient::GetInstance().Unregister(token);
        if (errCode == ERR_OK) {
            task.Resolve(engine, engine.CreateNull());
        } else {
            errCode = ErrorCodeReturn(errCode);
            task.Reject(engine, CreateJsError(engine, errCode, ErrorMessageReturn(errCode)));
        }
    };

    NativeValue* lastParam = (info.argc == ARG_COUNT_ONE) ? nullptr : info.argv[ARG_COUNT_ONE];
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsContinuationManager::OnUnregisterContinuation",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

std::string JsContinuationManager::OnRegisterDeviceSelectionCallbackParameterCheck(NativeEngine &engine,
    NativeCallbackInfo &info, std::string &cbType, int32_t &token, NativeValue** jsListenerObj)
{
    if (info.argc != ARG_COUNT_THREE) {
        return "Parameter error. The type of \"number of parameters\" must be 3";
    }
    if (!ConvertFromJsValue(engine, info.argv[0], cbType)) {
        return "Parameter error. The type of \"type\" must be string";
    }
    if (cbType != EVENT_CONNECT && cbType != EVENT_DISCONNECT) {
        return "Parameter error. The type of \"type\" must be " +
            std::string(EVENT_CONNECT) + " or " + std::string(EVENT_DISCONNECT);
    }
    if (!ConvertFromJsValue(engine, info.argv[ARG_COUNT_ONE], token)) {
        return "Parameter error. The type of \"token\" must be number";
    }
    *jsListenerObj = info.argv[ARG_COUNT_TWO];
    if (!IsCallbackValid(*jsListenerObj)) {
        return "Parameter error. The type of \"callback\" must be Callback<Array<ContinuationResult>>";
    }
    return std::string();
}

NativeValue* JsContinuationManager::OnRegisterDeviceSelectionCallback(NativeEngine &engine, NativeCallbackInfo &info)
{
    HILOGD("called.");
    std::string cbType;
    int32_t token = -1;
    int32_t errCode = PARAMETER_CHECK_FAILED;
    NativeValue* jsListenerObj = nullptr;
    std::string errInfo = OnRegisterDeviceSelectionCallbackParameterCheck(engine, info, cbType, token, &jsListenerObj);
    if (errInfo.empty()) {
        errInfo = [this, &engine, &info, &cbType, &token, &jsListenerObj, &errCode]() -> std::string {
            std::lock_guard<std::mutex> jsCbMapLock(jsCbMapMutex_);
            if (IsCallbackRegistered(token, cbType)) {
                errCode = REPEATED_REGISTRATION;
                return ErrorMessageReturn(errCode);
            }
            std::unique_ptr<NativeReference> callbackRef;
            callbackRef.reset(engine.CreateReference(jsListenerObj, 1));
            sptr<JsDeviceSelectionListener> deviceSelectionListener = new JsDeviceSelectionListener(&engine);
            if (deviceSelectionListener == nullptr) {
                HILOGE("deviceSelectionListener is nullptr!");
                errCode = SYSTEM_WORK_ABNORMALLY;
                return ErrorMessageReturn(errCode);
            }
            errCode = DistributedAbilityManagerClient::GetInstance().RegisterDeviceSelectionCallback(
                token, cbType, deviceSelectionListener);
            if (errCode == ERR_OK) {
                deviceSelectionListener->AddCallback(cbType, jsListenerObj);
                CallbackPair callbackPair = std::make_pair(std::move(callbackRef), deviceSelectionListener);
                jsCbMap_[token][cbType] = std::move(callbackPair); // move assignment
                HILOGI("RegisterDeviceSelectionListener success");
            } else {
                deviceSelectionListener = nullptr;
                errCode = ErrorCodeReturn(errCode);
                return ErrorMessageReturn(errCode);
            }
            return std::string();
        }();
    }
    if (!errInfo.empty()) {
        HILOGE("%{public}s", errInfo.c_str());
        napi_throw(reinterpret_cast<napi_env>(&engine),
            GenerateBusinessError(reinterpret_cast<napi_env>(&engine), errCode, errInfo));
    }
    return engine.CreateUndefined();
}

NativeValue* JsContinuationManager::OnUnregisterDeviceSelectionCallback(NativeEngine &engine, NativeCallbackInfo &info)
{
    HILOGD("called.");
    std::string cbType;
    int32_t token = -1;
    int32_t errCode = PARAMETER_CHECK_FAILED;
    std::string errInfo = [this, &engine, &info, &cbType, &token, &errCode]() -> std::string {
        if (info.argc != ARG_COUNT_TWO) {
            return "Parameter error. The type of \"number of parameters\" must be 2";
        }
        if (!ConvertFromJsValue(engine, info.argv[0], cbType)) {
            return "Parameter error. The type of \"type\" must be string";
        }
        if (cbType != EVENT_CONNECT && cbType != EVENT_DISCONNECT) {
            return "Parameter error. The type of \"type\" must be " +
                std::string(EVENT_CONNECT) + " or " + std::string(EVENT_DISCONNECT);
        }
        if (!ConvertFromJsValue(engine, info.argv[ARG_COUNT_ONE], token)) {
            return "Parameter error. The type of \"token\" must be number";
        }
        {
            std::lock_guard<std::mutex> jsCbMapLock(jsCbMapMutex_);
            if (!IsCallbackRegistered(token, cbType)) {
                errCode = CALLBACK_TOKEN_UNREGISTERED;
                return ErrorMessageReturn(errCode);
            }
            errCode = DistributedAbilityManagerClient::GetInstance().UnregisterDeviceSelectionCallback(token, cbType);
            if (errCode == ERR_OK) {
                CallbackPair& callbackPair = jsCbMap_[token][cbType];
                callbackPair.second->RemoveCallback(cbType);
                jsCbMap_[token].erase(cbType);
                if (jsCbMap_[token].empty()) {
                    jsCbMap_.erase(token);
                }
                HILOGI("UnregisterDeviceSelectionCallback success");
            } else {
                errCode = ErrorCodeReturn(errCode);
                return ErrorMessageReturn(errCode);
            }
        }
        return std::string();
    } ();
    if (!errInfo.empty()) {
        HILOGE("%{public}s", errInfo.c_str());
        napi_throw(reinterpret_cast<napi_env>(&engine),
            GenerateBusinessError(reinterpret_cast<napi_env>(&engine), errCode, errInfo));
    }
    return engine.CreateUndefined();
}

NativeValue *JsContinuationManager::OnUpdateConnectStatus(NativeEngine &engine, NativeCallbackInfo &info)
{
    HILOGD("called.");
    int32_t errCode = 0;
    if (info.argc < ARG_COUNT_THREE) {
        HILOGE("Params not match");
        errCode = ERR_NOT_OK;
    }
    int32_t token = -1;
    if (!errCode && !ConvertFromJsValue(engine, info.argv[0], token)) {
        HILOGE("Parse token failed");
        errCode = ERR_NOT_OK;
    }
    std::string deviceId;
    if (!errCode && !ConvertFromJsValue(engine, info.argv[ARG_COUNT_ONE], deviceId)) {
        HILOGE("Parse deviceId failed");
        errCode = ERR_NOT_OK;
    }
    DeviceConnectStatus deviceConnectStatus = DeviceConnectStatus::IDLE;
    if (!errCode && !ConvertFromJsValue(engine, info.argv[ARG_COUNT_TWO], deviceConnectStatus)) {
        HILOGE("Parse device connect status failed");
        errCode = ERR_NOT_OK;
    }
    AsyncTask::CompleteCallback complete =
        [token, deviceId, deviceConnectStatus, errCode](NativeEngine &engine, AsyncTask &task, int32_t status) {
        if (errCode != 0) {
            task.Reject(engine, CreateJsError(engine, errCode, "Invalidate params."));
            return;
        }
        int32_t ret = DistributedAbilityManagerClient::GetInstance().UpdateConnectStatus(
            token, deviceId, deviceConnectStatus);
        if (ret == ERR_OK) {
            task.Resolve(engine, engine.CreateUndefined());
        } else {
            task.Reject(engine, CreateJsError(engine, ret, "UpdateConnectStatus failed."));
        }
    };

    NativeValue* lastParam = (info.argc <= ARG_COUNT_THREE) ? nullptr : info.argv[ARG_COUNT_THREE];
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsContinuationManager::OnUpdateConnectStatus",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsContinuationManager::OnUpdateContinuationState(NativeEngine &engine, NativeCallbackInfo &info)
{
    HILOGD("called.");
    int32_t token = -1;
    std::string deviceId;
    DeviceConnectStatus deviceConnectStatus;
    std::string errInfo = [this, &engine, &info, &token, &deviceId, &deviceConnectStatus, &errInfo]() -> std::string {
        if (info.argc != ARG_COUNT_THREE && info.argc != ARG_COUNT_FOUR) {
            return "Parameter error. The type of \"number of parameters\" must be 3 or 4";
        }
        if (!ConvertFromJsValue(engine, info.argv[0], token)) {
            return "Parameter error. The type of \"token\" must be number";
        }
        if (!ConvertFromJsValue(engine, info.argv[ARG_COUNT_ONE], deviceId) || deviceId.empty()) {
            return "Parameter error. The type of \"deviceId\" must be string and not empty";
        }
        deviceConnectStatus = DeviceConnectStatus::IDLE;
        if (!ConvertFromJsValue(engine, info.argv[ARG_COUNT_TWO], deviceConnectStatus)) {
            return "Parameter error. The type of \"status\" must be DeviceConnectState";
        }
        if (static_cast<int32_t>(deviceConnectStatus) < static_cast<int32_t>(DeviceConnectStatus::IDLE) ||
            static_cast<int32_t>(deviceConnectStatus) > static_cast<int32_t>(DeviceConnectStatus::DISCONNECTING)) {
            HILOGE("deviceConnectStatus is invalid");
            return "Parameter error. The type of \"status\" must be DeviceConnectState";
        }
        return std::string();
    } ();
    if (!errInfo.empty()) {
        HILOGE("%{public}s", errInfo.c_str());
        napi_throw(reinterpret_cast<napi_env>(&engine),
            GenerateBusinessError(reinterpret_cast<napi_env>(&engine), PARAMETER_CHECK_FAILED, errInfo));
        return engine.CreateUndefined();
    }
    AsyncTask::CompleteCallback complete =
        [this, token, deviceId, deviceConnectStatus](NativeEngine &engine, AsyncTask &task, int32_t status) {
        int32_t errCode = DistributedAbilityManagerClient::GetInstance().UpdateConnectStatus(
            token, deviceId, deviceConnectStatus);
        if (errCode == ERR_OK) {
            task.Resolve(engine, engine.CreateNull());
        } else {
            errCode = ErrorCodeReturn(errCode);
            task.Reject(engine, CreateJsError(engine, errCode, ErrorMessageReturn(errCode)));
        }
    };

    NativeValue* lastParam = (info.argc == ARG_COUNT_THREE) ? nullptr : info.argv[ARG_COUNT_THREE];
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsContinuationManager::OnUpdateContinuationState",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue *JsContinuationManager::OnStartDeviceManager(NativeEngine &engine, NativeCallbackInfo &info)
{
    HILOGD("called.");
    int32_t errCode = 0;
    if (info.argc < ARG_COUNT_ONE) {
        HILOGE("Params not match");
        errCode = ERR_NOT_OK;
    }
    int32_t token = -1;
    if (!errCode && !ConvertFromJsValue(engine, info.argv[0], token)) {
        HILOGE("Parse token failed");
        errCode = ERR_NOT_OK;
    }
    decltype(info.argc) unwrapArgc = ARG_COUNT_ONE;
    std::shared_ptr<ContinuationExtraParams> continuationExtraParams = std::make_shared<ContinuationExtraParams>();
    if (info.argc > ARG_COUNT_ONE && info.argv[ARG_COUNT_ONE]->TypeOf() == NATIVE_OBJECT) {
        HILOGI("startDeviceManager options is used.");
        if (!UnWrapContinuationExtraParams(reinterpret_cast<napi_env>(&engine),
            reinterpret_cast<napi_value>(info.argv[ARG_COUNT_ONE]), continuationExtraParams)) {
            HILOGE("Parse continuationExtraParams failed");
            errCode = ERR_NOT_OK;
        }
        unwrapArgc++;
    }
    AsyncTask::CompleteCallback complete =
        [token, continuationExtraParams, unwrapArgc, errCode](NativeEngine &engine, AsyncTask &task, int32_t status) {
        if (errCode != 0) {
            task.Reject(engine, CreateJsError(engine, errCode, "Invalidate params."));
            return;
        }
        int32_t ret = (unwrapArgc == ARG_COUNT_ONE) ?
            DistributedAbilityManagerClient::GetInstance().StartDeviceManager(token) :
            DistributedAbilityManagerClient::GetInstance().StartDeviceManager(token, continuationExtraParams);
        if (ret == ERR_OK) {
            task.Resolve(engine, engine.CreateUndefined());
        } else {
            task.Reject(engine, CreateJsError(engine, ret, "StartDeviceManager failed."));
        }
    };

    NativeValue* lastParam = (info.argc <= unwrapArgc) ? nullptr : info.argv[unwrapArgc];
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsContinuationManager::OnStartDeviceManager",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsContinuationManager::OnStartContinuationDeviceManager(NativeEngine &engine, NativeCallbackInfo &info)
{
    HILOGD("called.");
    int32_t token = -1;
    decltype(info.argc) unwrapArgc = ARG_COUNT_ONE;
    std::shared_ptr<ContinuationExtraParams> continuationExtraParams;
    std::string errInfo = [this, &engine, &info, &token, & unwrapArgc, &continuationExtraParams]() -> std::string {
        if (info.argc < ARG_COUNT_ONE || info.argc > ARG_COUNT_THREE) {
            return "Parameter error. The type of \"number of parameters\" must be greater than 1 and less than 4";
        }
        if (!ConvertFromJsValue(engine, info.argv[0], token)) {
            return "Parameter error. The type of \"token\" must be number";
        }
        continuationExtraParams = std::make_shared<ContinuationExtraParams>();
        if (info.argc > ARG_COUNT_ONE && info.argv[ARG_COUNT_ONE]->TypeOf() == NATIVE_OBJECT) {
            HILOGI("StartContinuationDeviceManager options is used.");
            if (!UnWrapContinuationExtraParams(reinterpret_cast<napi_env>(&engine),
                reinterpret_cast<napi_value>(info.argv[ARG_COUNT_ONE]), continuationExtraParams)) {
                return "Parameter error. The type of \"options\" must be ContinuationExtraParams";
            }
            unwrapArgc++;
        }
        return std::string();
    } ();
    if (!errInfo.empty()) {
        HILOGE("%{public}s", errInfo.c_str());
        napi_throw(reinterpret_cast<napi_env>(&engine),
            GenerateBusinessError(reinterpret_cast<napi_env>(&engine), PARAMETER_CHECK_FAILED, errInfo));
        return engine.CreateUndefined();
    }
    AsyncTask::CompleteCallback complete =
        [this, token, continuationExtraParams, unwrapArgc](NativeEngine &engine, AsyncTask &task, int32_t status) {
        int32_t errCode = (unwrapArgc == ARG_COUNT_ONE) ?
            DistributedAbilityManagerClient::GetInstance().StartDeviceManager(token) :
            DistributedAbilityManagerClient::GetInstance().StartDeviceManager(token, continuationExtraParams);
        if (errCode == ERR_OK) {
            task.Resolve(engine, engine.CreateNull());
        } else {
            errCode = ErrorCodeReturn(errCode);
            task.Reject(engine, CreateJsError(engine, errCode, ErrorMessageReturn(errCode)));
        }
    };

    NativeValue* lastParam = (info.argc <= unwrapArgc) ? nullptr : info.argv[unwrapArgc];
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsContinuationManager::OnStartContinuationDeviceManager",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue *JsContinuationManager::OnInitDeviceConnectStateObject(NativeEngine &engine, NativeCallbackInfo &info)
{
    napi_value object;
    napi_env env = reinterpret_cast<napi_env>(&engine);
    NAPI_CALL(env, napi_create_object(env, &object));

    NAPI_CALL(env, SetEnumItem(env, object, "IDLE",
        static_cast<int32_t>(DeviceConnectStatus::IDLE)));
    NAPI_CALL(env, SetEnumItem(env, object, "CONNECTING",
        static_cast<int32_t>(DeviceConnectStatus::CONNECTING)));
    NAPI_CALL(env, SetEnumItem(env, object, "CONNECTED",
        static_cast<int32_t>(DeviceConnectStatus::CONNECTED)));
    NAPI_CALL(env, SetEnumItem(env, object, "DISCONNECTING",
        static_cast<int32_t>(DeviceConnectStatus::DISCONNECTING)));

    return reinterpret_cast<NativeValue*>(object);
}

NativeValue *JsContinuationManager::OnInitContinuationModeObject(NativeEngine &engine, NativeCallbackInfo &info)
{
    napi_value object;
    napi_env env = reinterpret_cast<napi_env>(&engine);
    NAPI_CALL(env, napi_create_object(env, &object));

    NAPI_CALL(env, SetEnumItem(env, object, "COLLABORATION_SINGLE",
        static_cast<int32_t>(ContinuationMode::COLLABORATION_SINGLE)));
    NAPI_CALL(env, SetEnumItem(env, object, "COLLABORATION_MULTIPLE",
        static_cast<int32_t>(ContinuationMode::COLLABORATION_MUTIPLE)));

    return reinterpret_cast<NativeValue*>(object);
}

napi_status JsContinuationManager::SetEnumItem(const napi_env& env, napi_value object, const char* name, int32_t value)
{
    napi_status status;
    napi_value itemName;
    napi_value itemValue;

    NAPI_CALL_BASE(env, status = napi_create_string_utf8(env, name, NAPI_AUTO_LENGTH, &itemName), status);
    NAPI_CALL_BASE(env, status = napi_create_int32(env, value, &itemValue), status);
    NAPI_CALL_BASE(env, status = napi_set_property(env, object, itemName, itemValue), status);
    NAPI_CALL_BASE(env, status = napi_set_property(env, object, itemValue, itemName), status);

    return napi_ok;
}

bool JsContinuationManager::IsCallbackValid(NativeValue* listenerObj)
{
    if (listenerObj == nullptr) {
        HILOGE("listenerObj is nullptr");
        return false;
    }
    if (!listenerObj->IsCallable()) {
        HILOGE("listenerObj is not callable");
        return false;
    }
    return true;
}

bool JsContinuationManager::IsCallbackRegistered(int32_t token, const std::string& cbType)
{
    if (jsCbMap_.empty() || jsCbMap_.find(token) == jsCbMap_.end()) {
        HILOGE("token %{public}d not registered callback!", token);
        return false;
    }
    if (jsCbMap_[token].empty() || jsCbMap_[token].find(cbType) == jsCbMap_[token].end()) {
        HILOGE("cbType %{public}s not registered callback!", cbType.c_str());
        return false;
    }
    HILOGI("callback already registered, token: %{public}d, cbType %{public}s", token, cbType.c_str());
    return true;
}

bool JsContinuationManager::UnWrapContinuationExtraParams(const napi_env& env, const napi_value& options,
    std::shared_ptr<ContinuationExtraParams>& continuationExtraParams)
{
    HILOGD("called.");
    if (!IsTypeForNapiValue(env, options, napi_object)) {
        HILOGE("options is invalid.");
        return false;
    }
    std::vector<std::string> deviceTypes;
    if (UnwrapStringArrayByPropertyName(env, options, "deviceType", deviceTypes)) {
        continuationExtraParams->SetDeviceType(deviceTypes);
    }
    std::string targetBundle("");
    if (UnwrapStringByPropertyName(env, options, "targetBundle", targetBundle)) {
        continuationExtraParams->SetTargetBundle(targetBundle);
    }
    std::string description("");
    if (UnwrapStringByPropertyName(env, options, "description", description)) {
        continuationExtraParams->SetDescription(description);
    }
    nlohmann::json filter;
    if (!UnwrapJsonByPropertyName(env, options, "filter", filter)) {
        return false;
    }
    continuationExtraParams->SetFilter(filter.dump());
    int32_t continuationMode = 0;
    if (UnwrapInt32ByPropertyName(env, options, "continuationMode", continuationMode)) {
        continuationExtraParams->SetContinuationMode(static_cast<ContinuationMode>(continuationMode));
    }
    nlohmann::json authInfo;
    if (UnwrapJsonByPropertyName(env, options, "authInfo", authInfo)) {
        continuationExtraParams->SetAuthInfo(authInfo.dump());
    }
    return true;
}

bool JsContinuationManager::UnwrapJsonByPropertyName(const napi_env& env, const napi_value& param,
    const std::string& field, nlohmann::json& jsonObj)
{
    HILOGD("called.");
    if (!IsTypeForNapiValue(env, param, napi_object)) {
        HILOGE("param is invalid.");
        return false;
    }
    napi_value jsonField = nullptr;
    napi_get_named_property(env, param, field.c_str(), &jsonField);
    napi_valuetype jsonFieldType = napi_undefined;
    napi_typeof(env, jsonField, &jsonFieldType);
    if (jsonFieldType != napi_object && jsonFieldType != napi_undefined) {
        HILOGE("field: %{public}s is invalid json.", field.c_str());
        return false;
    }
    napi_value jsProNameList = nullptr;
    uint32_t jsProCount = 0;
    napi_get_property_names(env, jsonField, &jsProNameList);
    napi_get_array_length(env, jsProNameList, &jsProCount);
    if (!PraseJson(env, jsonField, jsProNameList, jsProCount, jsonObj)) {
        HILOGE("PraseJson failed.");
        return false;
    }
    return true;
}

bool JsContinuationManager::PraseJson(const napi_env& env, const napi_value& jsonField,
    const napi_value& jsProNameList, uint32_t jsProCount, nlohmann::json& jsonObj)
{
    napi_value jsProName = nullptr;
    napi_value jsProValue = nullptr;
    napi_valuetype jsValueType = napi_undefined;
    if (jsProCount > MAX_JSPROCOUNT) {
        HILOGE("value of jsProCount is larger than MAX_JSPROCOUNT");
        return false;
    }
    for (uint32_t index = 0; index < jsProCount; index++) {
        napi_get_element(env, jsProNameList, index, &jsProName);
        std::string strProName = UnwrapStringFromJS(env, jsProName);
        napi_get_named_property(env, jsonField, strProName.c_str(), &jsProValue);
        napi_typeof(env, jsProValue, &jsValueType);
        switch (jsValueType) {
            case napi_string: {
                std::string elementValue = UnwrapStringFromJS(env, jsProValue);
                HILOGI("Property name=%{public}s, string, value=%{public}s", strProName.c_str(), elementValue.c_str());
                jsonObj[strProName] = elementValue;
                break;
            }
            case napi_boolean: {
                bool elementValue = false;
                napi_get_value_bool(env, jsProValue, &elementValue);
                HILOGI("Property name=%{public}s, boolean, value=%{public}d.", strProName.c_str(), elementValue);
                jsonObj[strProName] = elementValue;
                break;
            }
            case napi_number: {
                int32_t elementValue = 0;
                if (napi_get_value_int32(env, jsProValue, &elementValue) != napi_ok) {
                    HILOGE("Property name=%{public}s, Property int32_t parse error", strProName.c_str());
                } else {
                    HILOGI("Property name=%{public}s, number, value=%{public}d.", strProName.c_str(), elementValue);
                    jsonObj[strProName] = elementValue;
                }
                break;
            }
            default: {
                HILOGE("Property name=%{public}s, value type not support.", strProName.c_str());
                break;
            }
        }
    }
    return true;
}

int32_t JsContinuationManager::ErrorCodeReturn(int32_t code)
{
    switch (code) {
        case DMS_PERMISSION_DENIED:
            return PERMISSION_DENIED;
        case ERR_NULL_OBJECT:
            return SYSTEM_WORK_ABNORMALLY;
        case ERR_FLATTEN_OBJECT:
            return SYSTEM_WORK_ABNORMALLY;
        case CONNECT_ABILITY_FAILED:
            return SYSTEM_WORK_ABNORMALLY;
        case INVALID_CONTINUATION_MODE:
            return PARAMETER_CHECK_FAILED;
        case UNKNOWN_CALLBACK_TYPE:
            return PARAMETER_CHECK_FAILED;
        case INVALID_CONNECT_STATUS:
            return PARAMETER_CHECK_FAILED;
        case CALLBACK_HAS_NOT_REGISTERED:
            return CALLBACK_TOKEN_UNREGISTERED;
        case TOKEN_HAS_NOT_REGISTERED:
            return CALLBACK_TOKEN_UNREGISTERED;
        case REGISTER_EXCEED_MAX_TIMES:
            return OVER_MAX_REGISTERED_TIMES;
        case CALLBACK_HAS_REGISTERED:
            return REPEATED_REGISTRATION;
        default:
            return SYSTEM_WORK_ABNORMALLY;
    };
}

std::string JsContinuationManager::ErrorMessageReturn(int32_t code)
{
    switch (code) {
        case PARAMETER_CHECK_FAILED:
            return "The parameter check failed.";
        case SYSTEM_WORK_ABNORMALLY:
            return "The system ability works abnormally.";
        case CALLBACK_TOKEN_UNREGISTERED:
            return "The specified token or callback is not registered.";
        case OVER_MAX_REGISTERED_TIMES:
            return "The number of token registration times has reached the upper limit.";
        case REPEATED_REGISTRATION:
            return "The specified callback has been registered.";
        default:
            return "The system ability works abnormally.";
    };
}

napi_value JsContinuationManager::GenerateBusinessError(const napi_env &env, int32_t errCode, const std::string &errMsg)
{
    napi_value code = nullptr;
    napi_create_int32(env, errCode, &code);
    napi_value msg = nullptr;
    napi_create_string_utf8(env, errMsg.c_str(), NAPI_AUTO_LENGTH, &msg);
    napi_value businessError = nullptr;
    napi_create_error(env, nullptr, msg, &businessError);
    napi_set_named_property(env, businessError, CODE_KEY_NAME.c_str(), code);
    return businessError;
}

NativeValue* JsContinuationManagerInit(NativeEngine* engine, NativeValue* exportObj)
{
    HILOGD("called.");
    if (engine == nullptr || exportObj == nullptr) {
        HILOGE("Invalid input parameters");
        return nullptr;
    }

    NativeObject* object = ConvertNativeValueTo<NativeObject>(exportObj);
    if (object == nullptr) {
        HILOGE("convertNativeValueTo result is nullptr.");
        return nullptr;
    }

    JsContinuationManager* jsContinuationManager = new JsContinuationManager();
    object->SetNativePointer(jsContinuationManager, JsContinuationManager::Finalizer, nullptr);

    const char *moduleName = "JsContinuationManager";
    BindNativeFunction(*engine, *object, "register", moduleName, JsContinuationManager::Register);
    BindNativeFunction(*engine, *object, "unregister", moduleName, JsContinuationManager::Unregister);
    BindNativeFunction(*engine, *object, "on", moduleName, JsContinuationManager::RegisterDeviceSelectionCallback);
    BindNativeFunction(*engine, *object, "off", moduleName, JsContinuationManager::UnregisterDeviceSelectionCallback);
    BindNativeFunction(*engine, *object, "updateConnectStatus", moduleName, JsContinuationManager::UpdateConnectStatus);
    BindNativeFunction(*engine, *object, "startDeviceManager", moduleName, JsContinuationManager::StartDeviceManager);
    BindNativeProperty(*object, "DeviceConnectState", JsContinuationManager::InitDeviceConnectStateObject);
    BindNativeProperty(*object, "ContinuationMode", JsContinuationManager::InitContinuationModeObject);
    BindNativeFunction(*engine, *object, "registerContinuation", moduleName,
        JsContinuationManager::RegisterContinuation);
    BindNativeFunction(*engine, *object, "unregisterContinuation", moduleName,
        JsContinuationManager::UnregisterContinuation);
    BindNativeFunction(*engine, *object, "updateContinuationState", moduleName,
        JsContinuationManager::UpdateContinuationState);
    BindNativeFunction(*engine, *object, "startContinuationDeviceManager", moduleName,
        JsContinuationManager::StartContinuationDeviceManager);

    return engine->CreateUndefined();
}
}  // namespace DistributedSchedule
}  // namespace OHOS
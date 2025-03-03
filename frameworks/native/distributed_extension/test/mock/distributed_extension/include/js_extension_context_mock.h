/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#ifndef OHOS_JS_EXTENSION_CONTEXT_MOCK_H
#define OHOS_JS_EXTENSION_CONTEXT_MOCK_H

#include <gmock/gmock.h>

#include "distributed_extension_context_js.h"

namespace OHOS::DistributedSchedule {
class TJsExtensionContext {
public:
    virtual ~TJsExtensionContext() = default;
public:
    virtual napi_value CreateJsExtensionContext(napi_env env,
        const std::shared_ptr<AbilityRuntime::ExtensionContext>& context,
        std::shared_ptr<OHOS::AppExecFwk::AbilityInfo> abilityInfo) = 0;
public:
    static inline std::shared_ptr<TJsExtensionContext> tJsExtContext = nullptr;
};

class JsExtensionContextMock : public TJsExtensionContext {
public:
    MOCK_METHOD(napi_value, CreateJsExtensionContext, (napi_env,
        const std::shared_ptr<AbilityRuntime::ExtensionContext>&,
        std::shared_ptr<OHOS::AppExecFwk::AbilityInfo>));
};
} // namespace OHOS::DistributedSchedule
#endif // OHOS_JS_EXTENSION_CONTEXT_MOCK_H

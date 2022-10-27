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

#ifndef SERVICES_DTBSCHEDMGR_NAPI_ERROR_CODE_H_
#define SERVICES_DTBSCHEDMGR_NAPI_ERROR_CODE_H_

namespace OHOS {
namespace DistributedSchedule {
enum {
    /**
     * Result(201) for permission denied.
     */
    PERMISSION_DENIED = 201,
    /**
     * Result(401) for parameter check failed.
     */
    PARAMETER_CHECK_FAILED = 401,
    /**
     * Result(16600001) for the system ability work abnormally.
     */
    SYSTEM_WORK_ABNORMALLY = 16600001,
    /**
     * Result(16600002) for token or callback not registered.
     */
    CALLBACK_TOKEN_UNREGISTERED = 16600002,
    /**
     * Result(16600003) for token register max times.
     */
    OVER_MAX_REGISTERED_TIMES = 16600003,
    /**
     * Result(16600004) for token has been registered.
     */
    REPEATED_REGISTRATION = 16600004,
};
} // namespace DistributedSchedule
} // namespace OHOS
#endif // SERVICES_DTBSCHEDMGR_NAPI_ERROR_CODE_H_
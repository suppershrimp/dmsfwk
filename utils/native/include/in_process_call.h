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

#ifndef FOUNDATION_DISTRIBUTEDSCHEDULE_UTILS_NATIVE_INCLUDE_IN_PROCESS_CALL_H
#define FOUNDATION_DISTRIBUTEDSCHEDULE_UTILS_NATIVE_INCLUDE_IN_PROCESS_CALL_H

#include "ipc_skeleton.h"

#define IN_PROCESS_CALL(theCall)                                     \
    ([&]() {                                                         \
        std::string identity = IPCSkeleton::ResetCallingIdentity();  \
        auto retVal = theCall;                                       \
        IPCSkeleton::SetCallingIdentity(identity);                   \
        return retVal;                                               \
    }())

#endif // FOUNDATION_DISTRIBUTEDSCHEDULE_UTILS_NATIVE_INCLUDE_IN_PROCESS_CALL_H
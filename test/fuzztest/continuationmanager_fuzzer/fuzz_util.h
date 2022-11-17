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

#ifndef MOCK_PERMISSION_H
#define MOCK_PERMISSION_H

#include "iservice_registry.h"

namespace OHOS {
namespace DistributedSchedule {
class FuzzUtil {
public:
    static void MockPermission();
    static void MockProcessAndPermission(const char* processName,
        const char *perms[], int32_t permsNum);
};
}
} // namespace OHOS
#endif // SAMGR_SERVICES_SAMGR_MOCK_PERMISSION_H
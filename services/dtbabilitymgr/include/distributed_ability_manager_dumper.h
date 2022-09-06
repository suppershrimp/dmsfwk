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

#ifndef OHOS_DISTRIBUTED_ABILITY_MANAGER_DUMPER_H
#define OHOS_DISTRIBUTED_ABILITY_MANAGER_DUMPER_H

#include <string>
#include <vector>

#include "dms_dumper.h"

namespace OHOS {
namespace DistributedSchedule {
class DistributedAbilityManagerDumper {
public:
    static bool Dump(const sptr<DmsDumper>& dmsDumper, const std::vector<std::string>& args, std::string& result);
private:
    DistributedAbilityManagerDumper() = default;
    ~DistributedAbilityManagerDumper() = default;

    static void ShowHelp(std::string& result);
    static void IllegalInput(std::string& result);
    static bool CanDump();
    static bool ProcessDistributedSchedDump(const sptr<DmsDumper>& dmsDumper,
        const std::vector<std::string>& args, std::string& result);
};
} // namespace DistributedSchedule
} // namespace OHOS
#endif // OHOS_DISTRIBUTED_ABILITY_MANAGER_DUMPER_H
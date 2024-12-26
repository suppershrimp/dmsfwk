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

#ifndef ABILITY_DMSFWK_1_BLUETOOTH_STATE_ADAPTER_H
#define ABILITY_DMSFWK_1_BLUETOOTH_STATE_ADAPTER_H

#include <string>
#include "single_instance.h"
#include <atomic>

namespace OHOS {
namespace DistributedSchedule {
class BluetoothStateAdapter {
public:
DECLARE_SINGLE_INSTANCE_BASE(BluetoothStateAdapter);

public:
    bool IsBluetoothActive();

    void UpdateBTState(bool isBTActive);

    void UpdateBLEState(bool isBLEActive);

private:
    BluetoothStateAdapter() {}

    std::atomic<bool> isBTActive_;
    std::atomic<bool> isBLEActive_;
};
} // namespace DistributedSchedule
} // namespace OHOS
#endif //ABILITY_DMSFWK_1_BLUETOOTH_STATE_ADAPTER_H

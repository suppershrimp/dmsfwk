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

#ifndef OHOS_DISTRIBUTED_ABILITY_CONNECTION_TRANS_STREAM_H
#define OHOS_DISTRIBUTED_ABILITY_CONNECTION_TRANS_STREAM_H

#include "av_sender_engine.h"

namespace OHOS {
namespace DistributedCollab {

class AbilityConnectionTransStream {
public:
    AbilityConnectionTransStream(int32_t streamId, const StreamParam& param);
    ~AbilityConnectionTransStream();

    int32_t InitEngine();
    int32_t ConfigEngineParam(const SurfaceParam& param);
    int32_t 

private:
    int32_t streamId_;
    StreamParam param;

    std::shared_ptr<AVSenderEngine> engine_ = nullptr;
};
} // namespace DistributedCollab
} // namespace OHOS
#endif //OHOS_DISTRIBUTED_ABILITY_CONNECTION_SESSION_H
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

#ifndef DISTRIBUTED_COLLAB_AV_TRANS_STREAM_DATA_TEST_H
#define DISTRIBUTED_COLLAB_AV_TRANS_STREAM_DATA_TEST_H

#include "gtest/gtest.h"
#include "av_trans_stream_data.h"

namespace OHOS {
namespace DistributedCollab {
    class AVTransStreamDataTest : public testing::Test {
    public:
        static void SetUpTestCase();
        static void TearDownTestCase();
        void SetUp();
        void TearDown();
        std::shared_ptr<AVTransStreamData> streamData_;
        AVTransStreamDataExt extData_;
    };
} // namespace DistributedCollab
} // namespace OHOS
#endif
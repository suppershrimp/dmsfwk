/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "surface_decoder_adapter_test.h"
#include "test_log.h"
#include "media_description.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedCollab {
namespace {
    using AVBufferQueue = Media::AVBufferQueue;
    using AVBuffer = Media::AVBuffer;
    using Status = Media::Status;
    static const std::string TEST_MIME = "testMime";
    static constexpr int32_t NUM_1 = 1;
}

void SurfaceDecoderAdapterTest::SetUpTestCase()
{
    DTEST_LOG << "SurfaceDecoderAdapterTest::SetUpTestCase" << std::endl;
}

void SurfaceDecoderAdapterTest::TearDownTestCase()
{
    DTEST_LOG << "SurfaceDecoderAdapterTest::TearDownTestCase" << std::endl;
}

void SurfaceDecoderAdapterTest::TearDown()
{
    DTEST_LOG << "SurfaceDecoderAdapterTest::TearDown" << std::endl;
}

void SurfaceDecoderAdapterTest::SetUp()
{
    DTEST_LOG << "SurfaceDecoderAdapterTest::SetUp" << std::endl;
}

/**
 * @tc.name: Init_001
 * @tc.desc: SurfaceDecoderAdapter Init
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, Init_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest Init_001 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    EXPECT_EQ(adapter.Init(TEST_MIME), Status::ERROR_UNKNOWN);
    DTEST_LOG << "SurfaceDecoderAdapterTest Init_001 end" << std::endl;
}

/**
 * @tc.name: Configure_001
 * @tc.desc: SurfaceDecoderAdapter Configure Status::ERROR_UNKNOWN
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, Configure_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest Configure_001 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    MediaAVCodec::Format format;
    adapter.codecServer_ = nullptr;
    Status relult = adapter.Configure(format);
    EXPECT_EQ(relult, Status::ERROR_UNKNOWN);
    DTEST_LOG << "SurfaceDecoderAdapterTest Configure_001 end" << std::endl;
}

/**
 * @tc.name: GetInputBufferQueue_001
 * @tc.desc: SurfaceDecoderAdapter GetInputBufferQueue
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, GetInputBufferQueue_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest GetInputBufferQueue_001 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    adapter.inputBufferQueue_ = AVBufferQueue::Create(NUM_1,
        Media::MemoryType::UNKNOWN_MEMORY, "inputBufferQueue", true);
    EXPECT_EQ(adapter.GetInputBufferQueue(), nullptr);
    DTEST_LOG << "SurfaceDecoderAdapterTest GetInputBufferQueue_001 end" << std::endl;
}

/**
 * @tc.name: Start_001
 * @tc.desc: SurfaceDecoderAdapter Start returns ERROR_UNKNOWN
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, Start_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest Start_001 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    adapter.codecServer_ = nullptr;
    EXPECT_EQ(adapter.Start(), Status::ERROR_UNKNOWN);
    DTEST_LOG << "SurfaceDecoderAdapterTest Start_001 end" << std::endl;
}

/**
 * @tc.name: Stop_001
 * @tc.desc: SurfaceDecoderAdapter when releaseBufferTask_ is null then Stop returns Status::OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, Stop_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest Stop_001 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    adapter.releaseBufferTask_ = nullptr;
    adapter.codecServer_ = nullptr;
    EXPECT_EQ(adapter.Stop(), Status::OK);
    DTEST_LOG << "SurfaceDecoderAdapterTest Stop_001 end" << std::endl;
}

/**
 * @tc.name: Pause_001
 * @tc.desc: SurfaceDecoderAdapter Pause returns Status::OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, Pause_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest Pause_001 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    Status result= adapter.Pause();
    EXPECT_EQ(result, Status::OK);
    DTEST_LOG << "SurfaceDecoderAdapterTest Pause_001 end" << std::endl;
}

/**
 * @tc.name: Resume_001
 * @tc.desc: SurfaceDecoderAdapter Resume returns Status::OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, Resume_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest Resume_001 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    Status result= adapter.Resume();
    EXPECT_EQ(result, Status::OK);
    DTEST_LOG << "SurfaceDecoderAdapterTest Resume_001 end" << std::endl;
}

/**
 * @tc.name: Flush_001
 * @tc.desc: SurfaceDecoderAdapter Flush returns Status::ERROR_UNKNOWN
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, Flush_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest Flush_001 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    adapter.codecServer_ = nullptr;
    Status result= adapter.Flush();
    EXPECT_EQ(result, Status::ERROR_UNKNOWN);
    DTEST_LOG << "SurfaceDecoderAdapterTest Flush_001 end" << std::endl;
}

/**
 * @tc.name: Release_001
 * @tc.desc: SurfaceDecoderAdapter Release returns Status::OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, Release_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest Release_001 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    adapter.codecServer_ = nullptr;
    Status status= adapter.Release();
    EXPECT_EQ(status, Status::OK);
    DTEST_LOG << "SurfaceDecoderAdapterTest Release_001 end" << std::endl;
}

/**
 * @tc.name: Release_002
 * @tc.desc: SurfaceDecoderAdapter Release returns Status::OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, Release_002, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest Release_002 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    adapter.codecServer_ = nullptr;
    Status status= adapter.Release();
    EXPECT_EQ(status, Status::OK);
    DTEST_LOG << "SurfaceDecoderAdapterTest Release_002 end" << std::endl;
}

/**
 * @tc.name: SetParameter_001
 * @tc.desc: SurfaceDecoderAdapter SetParameter returns Status::ERROR_UNKNOWN
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, SetParameter_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest SetParameter_001 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    MediaAVCodec::Format format;
    adapter.codecServer_ = nullptr;
    Status status= adapter.SetParameter(format);
    EXPECT_EQ(status, Status::ERROR_UNKNOWN);
    DTEST_LOG << "SurfaceDecoderAdapterTest SetParameter_001 end" << std::endl;
}

/**
 * @tc.name: OnInputBufferAvailable_001
 * @tc.desc: SurfaceDecoderAdapter OnInputBufferAvailable when buffer is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, OnInputBufferAvailable_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest OnInputBufferAvailable_001 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    uint32_t index = NUM_1;
    std::shared_ptr<AVBuffer> buffer = nullptr;
    adapter.OnInputBufferAvailable(index, buffer);
    DTEST_LOG << "SurfaceDecoderAdapterTest OnInputBufferAvailable_001 end" << std::endl;
}

/**
 * @tc.name: OnInputBufferAvailable_002
 * @tc.desc: SurfaceDecoderAdapter OnInputBufferAvailable when inputBufferQueueConsumer_ is null
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, OnInputBufferAvailable_002, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest OnInputBufferAvailable_002 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    uint32_t index = NUM_1;
    std::shared_ptr<AVBuffer> buffer = std::shared_ptr<AVBuffer>();
    adapter.OnInputBufferAvailable(index, buffer);
    DTEST_LOG << "SurfaceDecoderAdapterTest OnInputBufferAvailable_002 end" << std::endl;
}
}  // namespace DistributedCollab
}  // namespace OHOS
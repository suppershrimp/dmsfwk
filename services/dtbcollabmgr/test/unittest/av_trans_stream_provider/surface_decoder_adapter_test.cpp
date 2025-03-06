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
#include "dtbcollabmgr_log.h"
#include "media_description.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedCollab {
namespace {
    static constexpr int32_t NUM_1 = 1;
    static constexpr std::vector<uint32_t> INDEXS ={1, 2, 3};
}

void SurfaceDecoderAdapterTest::SetUpTestCase()
{
    HILOGI("SurfaceDecoderAdapterTest::SetUpTestCase");
}

void SurfaceDecoderAdapterTest::TearDownTestCase()
{
    HILOGI("SurfaceDecoderAdapterTest::TearDownTestCase");
}

void SurfaceDecoderAdapterTest::TearDown()
{
    HILOGI("SurfaceDecoderAdapterTest::TearDown");
}

void SurfaceDecoderAdapterTest::SetUp()
{
    HILOGI("SurfaceDecoderAdapterTest::SetUp");
}

/**
 * @tc.name: SurfaceDecoderAdapter_001
 * @tc.desc: SurfaceDecoderAdapter
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, SurfaceDecoderAdapter_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest SurfaceDecoderAdapter_001 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    EXPECT_STREQ("encoder adapter create\n",HILOGI.str().c_str());
    DTEST_LOG << "SurfaceDecoderAdapterTest SurfaceDecoderAdapter_001 end" << std::endl;
}

/**
 * @tc.name: SurfaceDecoderAdapterRelease_001
 * @tc.desc: SurfaceDecoderAdapter release
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, SurfaceDecoderAdapterRelease_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest SurfaceDecoderAdapterRelease_001 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    adapter.~SurfaceDecoderAdapter();
    EXPECT_STREQ("encoder adapter destroy\n",HILOGI.str().c_str());
    DTEST_LOG << "SurfaceDecoderAdapterTest SurfaceDecoderAdapterRelease_001 end" << std::endl;
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
    adapter.Init(TEST_MIME);
    EXPECT_TRUE(adapter.releaseBufferTask_);
    DTEST_LOG << "SurfaceDecoderAdapterTest Init_001 end" << std::endl;
}

/**
 * @tc.name: Init_002
 * @tc.desc: SurfaceDecoderAdapter Init
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, Init_002, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest Init_002 begin" << std::endl;
    MediaAVCodec::VideoDecoder* codecServer = MediaAVCodec::VideoDecoderFactory::CreateByMime(mime);
    EXPECT_NE(nullptr,codecServer);
    SurfaceDecoderAdapter adapter;
    adapter.Init(TEST_MIME);
    EXPECT_EQ(codecServer,adapter.codecServer_);
    delete codecServer;
    DTEST_LOG << "SurfaceDecoderAdapterTest Init_002 end" << std::endl;
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
 * @tc.name: Configure_002
 * @tc.desc: SurfaceDecoderAdapter Configure returns Status::OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, Configure_002, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest Configure_002 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    MediaAVCodec::Format format;
    adapter.codecServer_ = std::make_shared<MediaAVCodec>();
    EXPECT_CALL(*adapter.codecServer_, Configure(format).WillOnce(Return(ERR_OK)));
    Status relult = adapter.Configure(format);
    EXPECT_EQ(relult, Status::OK);
    DTEST_LOG << "SurfaceDecoderAdapterTest Configure_002 end" << std::endl;
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
    adapter.inputBufferQueue_ = nullptr;
    EXPECT_EQ(adapter.GetInputBufferQueue(), nullptr);
    DTEST_LOG << "SurfaceDecoderAdapterTest GetInputBufferQueue_001 end" << std::endl;
}

/**
 * @tc.name: GetInputBufferQueue_002
 * @tc.desc: SurfaceDecoderAdapter GetInputBufferQueue
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, GetInputBufferQueue_002, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest GetInputBufferQueue_002 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    adapter.inputBufferQueue_ = AVBufferQueue::Create(0,
        Media::MemoryType::UNKNOWN_MEMORY, "inputBufferQueue", true);
    EXPECT_NE(adapter.GetInputBufferQueue(), nullptr);
    DTEST_LOG << "SurfaceDecoderAdapterTest GetInputBufferQueue_002 end" << std::endl;
}

/**
 * @tc.name: SetOutputSurface_001
 * @tc.desc: SurfaceDecoderAdapter SetOutputSurface return ERROR_UNKNOWN
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, SetOutputSurface_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest SetOutputSurface_001 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    sptr<Surface> surface = new Surface();
    adapter.codecServer_ = nullptr;
    Status status = adapter.SetOutputSurface(surface);
    EXPECT_EQ(status, Status::ERROR_UNKNOWN);
    DTEST_LOG << "SurfaceDecoderAdapterTest SetOutputSurface_001 end" << std::endl;
}

/**
 * @tc.name: SetOutputSurface_002
 * @tc.desc: SurfaceDecoderAdapter SetOutputSurface succeeds then return OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, SetOutputSurface_002, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest SetOutputSurface_002 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    sptr<Surface> surface = new Surface();
    adapter.codecServer_ = new CodeServer();
    EXPECT_CALL(*adapter.codecServer_, SetOutputSurface(surface).WillOnce(Return(ERR_OK)));
    Status status = adapter.SetOutputSurface(surface);
    EXPECT_EQ(status, Status::OK);
    DTEST_LOG << "SurfaceDecoderAdapterTest SetOutputSurface_002 end" << std::endl;
}

/**
 * @tc.name: SetDecoderAdapterCallback_001
 * @tc.desc: SurfaceDecoderAdapter SetDecoderAdapterCallback when codecServer is null
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, SetDecoderAdapterCallback_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest SetDecoderAdapterCallback_001 begin" << std::endl;
    std::shared_ptr<DecoderAdapterCallback> decoderAdapterCallback = std::make_shared<DecoderAdapterCallback>();
    SurfaceDecoderAdapter adapter;
    adapter.codecServer_ = nullptr;
    Status status = adapter.SetDecoderAdapterCallback(decoderAdapterCallback);
    EXPECT_EQ(status, Status::ERROR_UNKNOWN);
    DTEST_LOG << "SurfaceDecoderAdapterTest SetDecoderAdapterCallback_001 end" << std::endl;
}

/**
 * @tc.name: SetDecoderAdapterCallback_002
 * @tc.desc: SurfaceDecoderAdapter SetDecoderAdapterCallback return ERROR_UNKNOWN
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, SetDecoderAdapterCallback_002, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest SetDecoderAdapterCallback_002 begin" << std::endl;
    std::shared_ptr<DecoderAdapterCallback> decoderAdapterCallback = std::make_shared<DecoderAdapterCallback>();
    SurfaceDecoderAdapter adapter;
    adapter.codecServer_ = std::make_shared<MediaAVCodec::MediaCodecCallback>();
    ON_CALL(*adapter.codecServer_, SetCallback(_)).WillByDefault(Return(ERR_OK));
    Status status = adapter.SetDecoderAdapterCallback(decoderAdapterCallback);
    EXPECT_EQ(status, Status::OK);
    DTEST_LOG << "SurfaceDecoderAdapterTest SetDecoderAdapterCallback_002 end" << std::endl;
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
 * @tc.name: Start_002
 * @tc.desc: SurfaceDecoderAdapter Start returns Status::OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, Start_002, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest Start_002 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    adapter.codecServer_ = new CoderServer();
    EXPECT_CALL(*adapter.codecServer_, Prepare()).WillOnce(Return(ERR_OK));
    EXPECT_EQ(adapter.Start(), Status::OK);
    DTEST_LOG << "SurfaceDecoderAdapterTest Start_002 end" << std::endl;
}

/**
 * @tc.name: Start_003
 * @tc.desc: SurfaceDecoderAdapter when start succeeds then Start returns Status::OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, Start_003, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest Start_003 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    adapter.codecServer_ = new CoderServer();
    EXPECT_CALL(*adapter.codecServer_, Prepare()).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*adapter.codecServer_, Start()).WillOnce(Return(ERR_OK));
    EXPECT_EQ(adapter.Start(), Status::OK);
    DTEST_LOG << "SurfaceDecoderAdapterTest Start_003 end" << std::endl;
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
 * @tc.name: Stop_002
 * @tc.desc: SurfaceDecoderAdapter when Stop succeeds then Stop returns Status::OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, Stop_002, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest Stop_002 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    adapter.releaseBufferTask_ = nullptr;
    adapter.codecServer_ = new CoderServer();
    EXPECT_CALL(*adapter.codecServer_, Stop()).WillOnce(Return(ERR_OK));
    // ON_CALL(*adapter.codecServer_, Stop()).WillByDefault(Return(ERR_OK));
    EXPECT_EQ(adapter.Stop(), Status::OK);
    DTEST_LOG << "SurfaceDecoderAdapterTest Stop_002 end" << std::endl;
}

/**
 * @tc.name: Stop_003
 * @tc.desc: SurfaceDecoderAdapter when Stop succeeds then Stop returns ERROR_UNKNOWN
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, Stop_003, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest Stop_003 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    adapter.releaseBufferTask_ = nullptr;
    adapter.codecServer_ = new CoderServer();
    EXPECT_CALL(*adapter.codecServer_, Stop()).WillOnce(Return(ERROR_UNKNOWN));
    // ON_CALL(*adapter.codecServer_, Stop()).WillByDefault(Return(ERROR_UNKNOWN));
    EXPECT_EQ(adapter.Stop(), Status::ERROR_UNKNOWN);
    DTEST_LOG << "SurfaceDecoderAdapterTest Stop_003 end" << std::endl;
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
 * @tc.name: Flush_002
 * @tc.desc: SurfaceDecoderAdapter Flush returns Status::OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, Flush_002, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest Flush_002 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    adapter.codecServer_ = new CodecServer();
    EXPECT_CALL(*adapter.codecServer_, Flush()).WillOnce(Return(ERR_OK));
    Status result= adapter.Flush();
    EXPECT_EQ(result, Status::OK);
    DTEST_LOG << "SurfaceDecoderAdapterTest Flush_002 end" << std::endl;
}

/**
 * @tc.name: Flush_003
 * @tc.desc: SurfaceDecoderAdapter Flush returns Status::ERROR_UNKNOWN
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, Flush_003, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest Flush_003 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    adapter.codecServer_ = new CodecServer();
    EXPECT_CALL(*adapter.codecServer_, Flush()).WillOnce(Return(-1));
    Status result= adapter.Flush();
    EXPECT_EQ(result, Status::ERROR_UNKNOWN);
    DTEST_LOG << "SurfaceDecoderAdapterTest Flush_003 end" << std::endl;
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
    MediaAVCodec::Format format
    adapter.codecServer_ = nullptr;
    Status result= adapter.SetParameter(format);
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

/**
 * @tc.name: OnOutputBufferAvailable_001
 * @tc.desc: SurfaceDecoderAdapter OnOutputBufferAvailable 
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, OnOutputBufferAvailable_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest OnOutputBufferAvailable_001 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    uint32_t index = NUM_1;
    std::shared_ptr<AVBuffer> buffer = std::shared_ptr<AVBuffer>();
    buffer->flag_ = NUM_1;
    adapter.OnOutputBufferAvailable(index, buffer);
    EXPECT_EQ(adapter.dropIndexs_.back(), index);
    DTEST_LOG << "SurfaceDecoderAdapterTest OnOutputBufferAvailable_001 end" << std::endl;
}

/**
 * @tc.name: AcquireAvailableInputBuffer_001
 * @tc.desc: SurfaceDecoderAdapter AcquireAvailableInputBuffer 
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, AcquireAvailableInputBuffer_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest AcquireAvailableInputBuffer_001 begin" << std::endl;
    SurfaceDecoderAdapter adapter;
    EXPECT_CALL(*adapter.inputBufferQueueConsumer_, AcquireBuffer(_)).WillOnce(Return(Status::ERROR_UNKNOWN));
    adapter.AcquireAvailableInputBuffer();
    EXPECT_EQ(adapter.buffer_, nullptr);
    DTEST_LOG << "SurfaceDecoderAdapterTest AcquireAvailableInputBuffer_001 end" << std::endl;
}

/**
 * @tc.name: ReleaseBuffer_001
 * @tc.desc: SurfaceDecoderAdapter ReleaseBuffer 
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDecoderAdapterTest, ReleaseBuffer_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceDecoderAdapterTest ReleaseBuffer_001 begin" << std::endl;
    std::vector<uint32_t> indexs =INDEXS;
    std::vector<uint32_t> dropIndexs;
    SurfaceDecoderAdapter adapter;
    adapter.indexs_ = indexs;  
    adapter.isThreadExit_ = false;
    adapter.ReleaseBuffer();
    EXPECT_TRUE(adapter.indexs_.empty());
    EXPECT_TRUE(adapter.dropIndexs_.empty());
    DTEST_LOG << "SurfaceDecoderAdapterTest ReleaseBuffer_001 end" << std::endl;
}

}  // namespace DistributedCollab
}  // namespace OHOS
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

#include "surface_encoder_filter_test.h"
#include "test_log.h"
#include "dtbcollabmgr_log.h"
#include "media_description.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedCollab {
namespace {
    static constexpr int32_t TEST_APPPID = 1234;
    static constexpr int32_t TEST_APPUID = 5678;
    static constexpr int32_t TEST_INSTANCEID = 12345678;
    static constexpr std::vector<uint32_t> INDEXS ={1, 2, 3};
    const std::string CONTINUE_SWITCH_OFF = "0";
    const std::string CONTINUE_SWITCH_ON = "1";
}

void SurfaceEncoderFilterTest::SetUpTestCase()
{
    HILOGI("SurfaceEncoderFilterTest::SetUpTestCase");
}

void SurfaceEncoderFilterTest::TearDownTestCase()
{
    HILOGI("SurfaceEncoderFilterTest::TearDownTestCase");
}

void SurfaceEncoderFilterTest::TearDown()
{
    HILOGI("SurfaceEncoderFilterTest::TearDown");
}

void SurfaceEncoderFilterTest::SetUp()
{
    HILOGI("SurfaceEncoderFilterTest::SetUp");
}

/**
 * @tc.name: SetCodecFormat_001
 * @tc.desc: SurfaceEncoderFilter SetCodecFormat return OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, SetCodecFormat_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest SetCodecFormat_001 begin" << std::endl;
    std::shared_ptr<Meta> format = std::maked_shared<Meta>();
    format->Set<Tag::MIME_TYPE>("video/mp4");
    SurfaceEncoderFilter filter;
    EXPECT_EQ(filter.SetCodecFormat(format), Status::OK);
    DTEST_LOG << "SurfaceEncoderFilterTest SetCodecFormat_001 end" << std::endl;
}

/**
 * @tc.name: SetCodecFormat_002
 * @tc.desc: SurfaceEncoderFilter SetCodecFormat return ERROR_INVALID_PARAMETER
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, SetCodecFormat_002, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest SetCodecFormat_002 begin" << std::endl;
    std::shared_ptr<Meta> format = std::maked_shared<Meta>();
    SurfaceEncoderFilter filter;
    EXPECT_EQ(filter.SetCodecFormat(format), Status::ERROR_INVALID_PARAMETER);
    DTEST_LOG << "SurfaceEncoderFilterTest SetCodecFormat_002 end" << std::endl;
}

/**
 * @tc.name: Init_001
 * @tc.desc: SurfaceEncoderFilter Init
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, Init_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest Init_001 begin" << std::endl;
    std::shared_ptr<EventReceiver> receiver = std::maked_shared<EventReceiver>();
    std::shared_ptr<FilterCallback> callback = std::maked_shared<FilterCallback>();
    SurfaceEncoderFilter filter;
    filter.Init(receiver, callback);
    EXPECT_EQ(filter.eventReceiver_, receiver);
    EXPECT_EQ(filter.filterCallback_, callback);
    EXPECT_NE(filter.filterCallback_, nullptr);
    EXPECT_EQ(filter.isUpdateCodecNeeded_, false);
    DTEST_LOG << "SurfaceEncoderFilterTest Init_001 end" << std::endl;
}

/**
 * @tc.name: Init_002
 * @tc.desc: SurfaceEncoderFilter Init
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, Init_002, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest Init_002 begin" << std::endl;
    std::shared_ptr<EventReceiver> receiver = std::maked_shared<EventReceiver>();
    std::shared_ptr<FilterCallback> callback = std::maked_shared<FilterCallback>();
    SurfaceEncoderFilter filter;
    filter.Init(receiver, callback);
    EXPECT_EQ(filter.eventReceiver_, receiver);
    EXPECT_EQ(filter.filterCallback_, callback);
    EXPECT_EQ(filter.filterCallback_, nullptr);
    EXPECT_EQ(filter.isUpdateCodecNeeded_, false);
    DTEST_LOG << "SurfaceEncoderFilterTest Init_002 end" << std::endl;
}

/**
 * @tc.name: Configure_001
 * @tc.desc: SurfaceEncoderFilter Configure Status::ERROR_UNKNOWN
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, Configure_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest Configure_001 begin" << std::endl;
    SurfaceEncoderFilter filter;
    std::shared_ptr<Meta> parameter = std::maked_shared<Meta>();
    filter.codecAdapter_ = nullptr;
    EXPECT_EQ(filter.Configure(parameter), Status::ERROR_UNKNOWN);
    DTEST_LOG << "SurfaceEncoderFilterTest Configure_001 end" << std::endl;
}

/**
 * @tc.name: Configure_002
 * @tc.desc: SurfaceEncoderFilter Configure returns Status::OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, Configure_002, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest Configure_002 begin" << std::endl;
    SurfaceEncoderFilter filter;
    std::shared_ptr<Meta> parameter = std::maked_shared<Meta>();
    filter.codecAdapter_ = std::maked_shared<SurfaceEncoderAdapter>();
    EXPECT_EQ(filter.Configure(parameter), Status::OK);
    EXPECT_EQ(relult, Status::OK);
    DTEST_LOG << "SurfaceEncoderFilterTest Configure_002 end" << std::endl;
}

/**
 * @tc.name: SetInputSurface_001
 * @tc.desc: SurfaceEncoderFilter SetInputSurface return OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, SetInputSurface_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest SetInputSurface_001 begin" << std::endl;
    SurfaceEncoderFilter filter;
    sptr<Surface> surface = new Surface();
    EXPECT_EQ(adapter.SetInputSurface(surface), Status::OK);
    DTEST_LOG << "SurfaceEncoderFilterTest SetInputSurface_001 end" << std::endl;
}

/**
 * @tc.name: SetInputSurface_002
 * @tc.desc: SurfaceEncoderFilter SetInputSurface return OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, SetInputSurface_002, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest SetInputSurface_002 begin" << std::endl;
    SurfaceEncoderFilter filter;
    sptr<Surface> surface = nullptr;
    EXPECT_EQ(adapter.SetInputSurface(surface), Status::OK);
    DTEST_LOG << "SurfaceEncoderFilterTest SetInputSurface_002 end" << std::endl;
}

/**
 * @tc.name: SetTransCoderMode_001
 * @tc.desc: SurfaceEncoderFilter SetTransCoderMode return OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, SetTransCoderMode_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest SetTransCoderMode_001 begin" << std::endl;
    SurfaceEncoderFilter filter;
    Status status = adapter.SetTransCoderMode();
    EXPECT_EQ(status, Status::OK);
    DTEST_LOG << "SurfaceEncoderFilterTest SetTransCoderMode_001 end" << std::endl;
}

/**
 * @tc.name: GetInputSurface_001
 * @tc.desc: SurfaceEncoderFilter GetInputSurface return the surface
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, GetInputSurface_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest GetInputSurface_001 begin" << std::endl;
    SurfaceEncoderFilter filter;
    sptr<Surface> surface = new Surface();
    filter.surface_ = surface;
    EXPECT_EQ(filter.GetInputSurface(), surface);
    DTEST_LOG << "SurfaceEncoderFilterTest GetInputSurface_001 end" << std::endl;
}

/**
 * @tc.name: GetInputSurface_002
 * @tc.desc: SurfaceEncoderFilter GetInputSurface return the surface from codecAdapter
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, GetInputSurface_002, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest GetInputSurface_002 begin" << std::endl;
    SurfaceEncoderFilter filter;
    sptr<CodecAdapter> codecAdapter = new CodecAdapter();
    sptr<Surface> surface = new Surface();
    codecAdapter->.GetInputSurface = [](){return surface;};
    filter.codecAdapter_ = codecAdapter;
    EXPECT_EQ(filter.GetInputSurface(), surface);
    DTEST_LOG << "SurfaceEncoderFilterTest GetInputSurface_002 end" << std::endl;
}

/**
 * @tc.name: GetInputSurface_003
 * @tc.desc: SurfaceEncoderFilter GetInputSurface return null
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, GetInputSurface_003, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest GetInputSurface_003 begin" << std::endl;
    SurfaceEncoderFilter filter;
    EXPECT_EQ(filter.GetInputSurface(), nullptr);
    DTEST_LOG << "SurfaceEncoderFilterTest GetInputSurface_003 end" << std::endl;
}

/**
 * @tc.name: DoPrepare_001
 * @tc.desc: SurfaceEncoderFilter DoPrepare return the OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, DoPrepare_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest DoPrepare_001 begin" << std::endl;
    SurfaceEncoderFilter filter;
    Status result = filter.DoPrepare();
    EXPECT_EQ(result, Status::OK);
    DTEST_LOG << "SurfaceEncoderFilterTest DoPrepare_001 end" << std::endl;
}

/**
 * @tc.name: DoStart_001
 * @tc.desc: SurfaceEncoderFilter DoStart return the ERROR_UNKNOWN
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, DoStart_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest DoStart_001 begin" << std::endl;
    SurfaceEncoderFilter filter;
    filter.codecAdapter_ = nullptr;
    EXPECT_EQ(filter.DoStart(), Status::ERROR_UNKNOWN);
    DTEST_LOG << "SurfaceEncoderFilterTest DoStart_001 end" << std::endl;
}

/**
 * @tc.name: DoStart_002
 * @tc.desc: SurfaceEncoderFilter DoStart return the OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, DoStart_002, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest DoStart_002 begin" << std::endl;
    SurfaceEncoderFilter filter;
    filter.codecAdapter_ = new CodecAdapter();
    EXPECT_EQ(filter.DoStart(), Status::OK);
    DTEST_LOG << "SurfaceEncoderFilterTest DoStart_002 end" << std::endl;
}

/**
 * @tc.name: DoPause_001
 * @tc.desc: SurfaceEncoderFilter DoPause return the ERROR_UNKNOWN
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, DoPause_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest DoPause_001 begin" << std::endl;
    SurfaceEncoderFilter filter;
    filter.codecAdapter_ = nullptr;
    EXPECT_EQ(filter.DoPause(), Status::ERROR_UNKNOWN);
    DTEST_LOG << "SurfaceEncoderFilterTest DoPause_001 end" << std::endl;
}

/**
 * @tc.name: DoPause_002
 * @tc.desc: SurfaceEncoderFilter DoPause return the OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, DoPause_002, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest DoPause_002 begin" << std::endl;
    SurfaceEncoderFilter filter;
    filter.codecAdapter_ = new CodecAdapter();
    EXPECT_EQ(filter.DoPause(), Status::OK);
    DTEST_LOG << "SurfaceEncoderFilterTest DoPause_002 end" << std::endl;
}

/**
 * @tc.name: DoResume_001
 * @tc.desc: SurfaceEncoderFilter DoResume return the ERROR_UNKNOWN
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, DoResume_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest DoResume_001 begin" << std::endl;
    SurfaceEncoderFilter filter;
    filter.codecAdapter_ = nullptr;
    EXPECT_EQ(filter.DoResume(), Status::ERROR_UNKNOWN);
    DTEST_LOG << "SurfaceEncoderFilterTest DoResume_001 end" << std::endl;
}

/**
 * @tc.name: DoResume_002
 * @tc.desc: SurfaceEncoderFilter DoResume return the OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, DoResume_002, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest DoResume_002 begin" << std::endl;
    SurfaceEncoderFilter filter;
    filter.codecAdapter_ = new CodecAdapter();
    EXPECT_EQ(filter.DoResume(), Status::OK);
    DTEST_LOG << "SurfaceEncoderFilterTest DoResume_002 end" << std::endl;
}

/**
 * @tc.name: DoStop_001
 * @tc.desc: SurfaceEncoderFilter DoStop return the OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, DoStop_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest DoStop_001 begin" << std::endl;
    SurfaceEncoderFilter filter;
    filter.codecAdapter_ = nullptr;
    EXPECT_EQ(filter.DoStop(), Status::OK);
    DTEST_LOG << "SurfaceEncoderFilterTest DoStop_001 end" << std::endl;
}

/**
 * @tc.name: Reset_001
 * @tc.desc: SurfaceEncoderFilter Reset return the OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, Reset_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest Reset_001 begin" << std::endl;
    SurfaceEncoderFilter filter;
    filter.codecAdapter_ = nullptr;
    EXPECT_EQ(filter.Reset(), Status::OK);
    DTEST_LOG << "SurfaceEncoderFilterTest Reset_001 end" << std::endl;
}

/**
 * @tc.name: DoRelease_001
 * @tc.desc: SurfaceEncoderFilter DoRelease return the OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, DoRelease_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest DoRelease_001 begin" << std::endl;
    SurfaceEncoderFilter filter;
    filter.codecAdapter_ = nullptr;
    EXPECT_EQ(filter.DoRelease(), Status::OK);
    DTEST_LOG << "SurfaceEncoderFilterTest DoRelease_001 end" << std::endl;
}

/**
 * @tc.name: DoRelease_002
 * @tc.desc: SurfaceEncoderFilter DoRelease return the OK
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, DoRelease_002, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest DoRelease_002 begin" << std::endl;
    SurfaceEncoderFilter filter;
    CodecAdapter* codecAdapter = new CodecAdapter();
    filter.codecAdapter_ = codecAdapter;
    EXPECT_EQ(filter.DoRelease(), codecAdapter_->Reset());
    DTEST_LOG << "SurfaceEncoderFilterTest DoRelease_002 end" << std::endl;
}

/**
 * @tc.name: SetParameter_001
 * @tc.desc: SurfaceEncoderFilter SetParameter return the nullptr
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, SetParameter_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest SetParameter_001 begin" << std::endl;
    SurfaceEncoderFilter filter;
    std::shared_ptr<Meta> parameter = std::make_shared<Meta>();
    filter.SetParameter(parameter);
    EXPECT_EQ(filter.codecAdapter_, nullptr);
    DTEST_LOG << "SurfaceEncoderFilterTest SetParameter_001 end" << std::endl;
}

/**
 * @tc.name: SetCallingInfo_001
 * @tc.desc: SurfaceEncoderFilter SetCallingInfo with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, SetCallingInfo_001, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest SetCallingInfo_001 begin" << std::endl;
    SurfaceEncoderFilter filter;
    int32_t appUid = TEST_APPUID;
    int32_t appPid = TEST_APPPID;
    std::string bundleName = "com.ohos.testBundle";
    uint64_t instanceId = TEST_INSTANCEID;

    filter.SetCallingInfo(appUid, appPid, bundleName, instanceId);

    EXPECT_EQ(filter.appUid_, appUid);
    EXPECT_EQ(filter.appPid_, appPid);
    EXPECT_EQ(filter.bundleName_, bundleName);
    EXPECT_EQ(filter.instanceId_, instanceId);
    DTEST_LOG << "SurfaceEncoderFilterTest SetCallingInfo_001 end" << std::endl;
}

/**
 * @tc.name: SetCallingInfo_002
 * @tc.desc: SurfaceEncoderFilter SetCallingInfo when codecAdapter_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceEncoderFilterTest, SetCallingInfo_002, TestSize.Level3)
{
    DTEST_LOG << "SurfaceEncoderFilterTest SetCallingInfo_002 begin" << std::endl;
    SurfaceEncoderFilter filter;
    int32_t appUid = TEST_APPUID;
    int32_t appPid = TEST_APPPID;
    std::string bundleName = "com.ohos.testBundle";
    uint64_t instanceId = TEST_INSTANCEID;

    filter.codecAdapter_ = nullptr;
    filter.SetCallingInfo(appUid, appPid, bundleName, instanceId);

    EXPECT_EQ(filter.appUid_, appUid);
    EXPECT_EQ(filter.appPid_, appPid);
    EXPECT_EQ(filter.bundleName_, bundleName);
    EXPECT_EQ(filter.instanceId_, instanceId);
    DTEST_LOG << "SurfaceEncoderFilterTest SetCallingInfo_002 end" << std::endl;
}

}  // namespace DistributedCollab
}  // namespace OHOS
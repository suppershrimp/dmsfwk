#include "av_trans_stream_data_test.h"
#include "dtbcollabmgr_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedCollab {
    namespace {
        static const std::string TAG = "DSchedCollabAVTransStreamDataTest";
    }
    void AVTransStreamDataTest::SetUpTestCase()
    {
        HILOGI("AVTransStreamDataTest::SetUpTestCase");
    }

    void AVTransStreamDataTest::TearDownTestCase()
    {
        HILOGI("AVTransStreamDataTest::TearDownTestCase");
    }

    void AVTransStreamDataTest::SetUp()
    {
        HILOGI("AVTransStreamDataTest::SetUp");
        auto buffer = std::make_shared<AVTransDataBuffer>(1024); // 设置容量
        extData_ = { AvCodecBufferFlag::AVCODEC_BUFFER_FLAG_SYNC_FRAME, 10, 1000, 2000, 3000, 4000 };
        streamData_ = std::make_shared<AVTransStreamData>(buffer, extData_);
    }

    void AVTransStreamDataTest::TearDown()
    {
        HILOGI("AVTransStreamDataTest::TearDown");
        streamData_.reset();
    }

    /**
     * @tc.name: SerializeStreamDataExt_001
     * @tc.desc: test serialize stream data ext
     * @tc.type: FUNC
     */
    HWTEST_F(AVTransStreamDataTest, SerializeStreamDataExt_001, TestSize.Level1)
    {
        cJSON* jsonData = streamData_->SerializeStreamDataExt();
        ASSERT_NE(jsonData, nullptr);

        cJSON* flag = cJSON_GetObjectItem(jsonData, "flag");
        ASSERT_NE(flag, nullptr);
        EXPECT_EQ(flag->valueint, static_cast<int>(extData_.flag_));

        cJSON* index = cJSON_GetObjectItem(jsonData, "index");
        ASSERT_NE(index, nullptr);
        EXPECT_EQ(index->valueint, extData_.index_);

        cJSON_Delete(jsonData);
    }

    /**
     * @tc.name: DeserializeStreamDataExt_001
     * @tc.desc: DeserializeStreamDataExt
     * @tc.type: FUNC
     */
    HWTEST_F(AVTransStreamDataTest, DeserializeStreamDataExt_001, TestSize.Level1)
    {
        cJSON* jsonData = streamData_->SerializeStreamDataExt();
        int32_t indexVal = 10;
        char* jsonString = cJSON_PrintUnformatted(jsonData);

        int32_t result = streamData_->DeserializeStreamDataExt(jsonString);
        EXPECT_EQ(result, ERR_OK);
        EXPECT_EQ(indexVal, streamData_->GetStreamDataExt().index_);
        cJSON_Delete(jsonData);
        free(jsonString);
    }

    /**
     * @tc.name: DeserializeStreamDataExt_002
     * @tc.desc: DeserializeStreamDataExt when nullptr
     * @tc.type: FUNC
     */
    HWTEST_F(AVTransStreamDataTest, DeserializeStreamDataExt_002, TestSize.Level1)
    {
        int32_t result = streamData_->DeserializeStreamDataExt(nullptr);
        EXPECT_EQ(result, PARSE_AV_TRANS_STREAM_EXT_FAILED); 
    }
} // namespace DistributedCollab
}
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
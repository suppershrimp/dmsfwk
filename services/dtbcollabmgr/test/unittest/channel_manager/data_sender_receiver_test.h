#ifndef DISTRIBUTED_COLLAB_DATA_SENDER_RECEIVER_TEST_H
#define DISTRIBUTED_COLLAB_DATA_SENDER_RECEIVER_TEST_H

#include "data_sender_receiver.h"
#include "socket.h"
#include "softbus_mock.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace OHOS {
namespace DistributedCollab {
    class DataSenderReceiverTest : public testing::Test {
    public:
        SoftbusMock mockSoftbus;
        int32_t socketId = 1234;
        std::shared_ptr<AVTransDataBuffer> buffer = std::make_shared<AVTransDataBuffer>(100);
        AVTransStreamDataExt ext { AvCodecBufferFlag::AVCODEC_BUFFER_FLAG_SYNC_FRAME, 1, 12345, 67890, 123456, 7891011 };
        std::shared_ptr<AVTransStreamData> streamData = std::make_shared<AVTransStreamData>(buffer, ext);
        DataSenderReceiver dataSenderReceiver = DataSenderReceiver(socketId);
        
        static void SetUpTestCase();
        static void TearDownTestCase();
        void SetUp();
        void TearDown();
    };
} // namespace DistributedCollab
} // namespace OHOS
#endif
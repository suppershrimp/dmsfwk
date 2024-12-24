#ifndef DISTRIBUTED_COLLAB_SESSION_DATA_HEADER_TEST_H
#define DISTRIBUTED_COLLAB_SESSION_DATA_HEADER_TEST_H

#include "session_data_header.h"
#include "gtest/gtest.h"

namespace OHOS {
namespace DistributedCollab {
    class SessionDataHeaderTest : public testing::Test {
    public:
        static void SetUpTestCase();
        static void TearDownTestCase();
        void SetUp();
        void TearDown();

        uint16_t version;
        FRAG_TYPE fragFlag;
        uint32_t dataType;
        uint32_t seqNum;
        uint32_t totalLen;
        uint32_t packetLen;
        uint32_t payloadLen;
        uint16_t subSeq;

        std::unique_ptr<SessionDataHeader> header;
    };

} // namespace DistributedCollab
} // namespace OHOS
#endif
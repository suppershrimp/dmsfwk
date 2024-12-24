#include "av_receiver_filter_listener.h"
#include "dtbcollabmgr_log.h"

namespace OHOS {
namespace DistributedCollab {
    namespace {
        static const std::string TAG = "AVReceiverFilterListener";
    }

    AVReceiverFilterListener::~AVReceiverFilterListener() {
        HILOGI("AVReceiverFilterListener destroyed");
    }

    void AVReceiverFilterListener::OnConnect(const int32_t channelId) const { }

    void AVReceiverFilterListener::OnDisConnect(const int32_t channelId) const { }

    void AVReceiverFilterListener::OnMessage(const int32_t channelId, const std::shared_ptr<AVTransDataBuffer>& buffer) const { }

    void AVReceiverFilterListener::OnError(const int32_t channelId, const int32_t errorCode) const
    {
        if (auto ptr = filter_.lock()) {
            ptr->OnError(errorCode);
        }
    }

    void AVReceiverFilterListener::OnStream(const int32_t channelId, const std::shared_ptr<AVTransStreamData>& stream) const
    {
        if (auto ptr = filter_.lock()) {
            ptr->OnStream(stream);
        }
    }

    void AVReceiverFilterListener::OnBytes(const int32_t channelId, const std::shared_ptr<AVTransDataBuffer>& buffer) const
    {
        if (auto ptr = filter_.lock()) {
            ptr->OnBytes(buffer);
        }
    }

} // namespace DistributedCollab
} // namespace OHOS
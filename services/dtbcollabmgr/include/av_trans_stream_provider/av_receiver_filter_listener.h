#ifndef OHOS_AV_TRANS_STREAM_AV_RECEIVER_FILTER_LISTENER_H
#define OHOS_AV_TRANS_STREAM_AV_RECEIVER_FILTER_LISTENER_H

#include "av_receiver_filter.h"
#include "ichannel_listener.h"

namespace OHOS {
namespace DistributedCollab {
    class AVReceiverFilterListener : public IChannelListener, public std::enable_shared_from_this<AVReceiverFilterListener> {
    public:
        AVReceiverFilterListener(const std::shared_ptr<AVReceiverFilter>& filter)
            : filter_(filter) {};
        ~AVReceiverFilterListener();
        void OnConnect(const int32_t channelId) const override;
        void OnDisConnect(const int32_t channelId) const override;
        void OnStream(const int32_t channelId, const std::shared_ptr<AVTransStreamData>& stream) const override;
        void OnBytes(const int32_t channelId, const std::shared_ptr<AVTransDataBuffer>& buffer) const override;
        void OnMessage(const int32_t channelId, const std::shared_ptr<AVTransDataBuffer>& buffer) const override;
        void OnError(const int32_t channelId, const int32_t errorCode) const override;

    private:
        std::weak_ptr<AVReceiverFilter> filter_;
    };

} // namespace DistributedCollab
} // namespace OHOS
#endif
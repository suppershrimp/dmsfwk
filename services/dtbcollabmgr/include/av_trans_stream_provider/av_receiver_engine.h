#ifndef OHOS_AV_TRANS_STREAM_AV_RECEIVER_ENGINE_H
#define OHOS_AV_TRANS_STREAM_AV_RECEIVER_ENGINE_H

#include "av_receiver_filter.h"
#include "av_stream_common.h"
#include "av_stream_param.h"
#include "common/status.h"
#include "meta/meta.h"
#include "pipeline/pipeline.h"
#include "refbase.h"
#include "surface.h"
#include "surface_decoder_filter.h"
#include "iengine_listener.h"
#ifdef ENABLE_SURFACE_BUFFER_CACHE
#include "av_surface_buffer_cache.h"
#endif
#include "cJSON.h"
#include <memory>
#include <string>

namespace OHOS {
namespace DistributedCollab {
    class AVReceiverEngine : public std::enable_shared_from_this<AVReceiverEngine> {
    public:
        explicit AVReceiverEngine();
        ~AVReceiverEngine();
        void Init();
        void SetVideoSource(const VideoSourceType source);
        int32_t Configure(const StreamParam& recParam);
        int32_t SetVideoSurface(uint64_t surfaceId);
        void SetChannelListener(int32_t channelId);
        cJSON* GetVideoCodecAbility();
        int32_t Prepare();
        int32_t Start();
        int32_t Pause();
        int32_t Resume();
        int32_t Stop(bool isDrainAll);
        int32_t Reset();
        void OnEvent(const Media::Event& event);
        Media::Status OnCallback(
            const std::shared_ptr<Media::Pipeline::Filter>& filter,
            Media::Pipeline::FilterCallBackCommand cmd,
            Media::Pipeline::StreamType outType);
        void SetEngineListener(const std::shared_ptr<IEngineListener>& listener);
#ifdef DSCH_COLLAB_AV_TRANS_TEST_DEMO
        std::shared_ptr<IChannelListener> GetChannelListener();
#endif

    private:
        void ChangeState(const EngineState state);
        int32_t InitRecvHeaderFilter();
        bool isVideoParam(const StreamParam& recParam);

    private:
        std::shared_ptr<Media::Pipeline::Pipeline> pipeline_ = nullptr;
        std::shared_ptr<SurfaceDecoderFilter> videoDecoderFilter_ = nullptr;
        std::shared_ptr<Media::Meta> videoDecFormat_ = std::make_shared<Media::Meta>();
        std::shared_ptr<AVReceiverFilter> receiverFilter_ = nullptr;
        sptr<Surface> surface_ { nullptr };
#ifdef ENABLE_SURFACE_BUFFER_CACHE
        std::shared_ptr<AVSurfaceBufferCache> bufferCache_ = nullptr;
#endif
        std::shared_ptr<Media::Pipeline::EventReceiver> engineEventReceiver_= nullptr;
        std::shared_ptr<Media::Pipeline::FilterCallback> engineFilterCallback_= nullptr;
        std::string engineId_;

        VideoSourceType sourceType_ = VideoSourceType::NV21;
        EngineState curState_ = EngineState::EMPTY;
    };
} // namespace DistributedCollab
} // namespace OHOS
#endif
#ifndef OHOS_AV_TRANS_STREAM_COMMON_DEFINITION_H
#define OHOS_AV_TRANS_STREAM_COMMON_DEFINITION_H

#include <cstdint>
#include <string>

namespace OHOS {
namespace DistributedCollab {
    enum class VideoSourceType {
        YUV420P,
        YUVI420,
        NV12,
        NV21,
        SURFACE_FORMAT,
        RGBA,
    };

    enum class EngineState : uint32_t {
        EMPTY = 0,
        INIT,
        SETTING_PARAM,
        PREPARE,
        READY,
        PAUSE,
        RUNNING,
        ERROR,
    };

    enum class OutputFormatType : int32_t {
        /** Default format */
        FORMAT_DEFAULT = 0,
        /** MPEG4 format */
        FORMAT_MPEG_4 = 2,
        /** M4A format */
        FORMAT_M4A = 6,
        /** WAV format */
        FORMAT_WAV = 10,
    };

    enum class VideoCodecFormat : int32_t {
        /** Default format */
        VIDEO_DEFAULT,
        /** H.264 */
        H264,
        /** MPEG4 */
        MPEG4,
        /** H.265 */
        H265,
    };
    
    const std::string VIDEO_CODEC_LIST_KEY = "VideoCodecList";
} // namespace DistributedCollab
} // namespace OHOS
#endif
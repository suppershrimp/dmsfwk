#include "av_stream_param.h"
#include "media_description.h"

namespace OHOS {
namespace DistributedCollab {
    namespace {
        using MDKey = MediaAVCodec::MediaDescriptionKey;

    }
    void VidEnc::Configure(std::shared_ptr<Media::Meta> meta, const ConfigureMode mode) const
    {
        switch (mode) {
        case ConfigureMode::Encode:
            ConfigureEncode(meta);
            break;
        case ConfigureMode::Decode:
            ConfigureDecode(meta);
            break;
        default:
            break;
        }
    }

    void VidEnc::ConfigureEncode(std::shared_ptr<Media::Meta>& meta) const
    {
        switch (encFmt) {
        case VideoCodecFormat::VIDEO_DEFAULT:
        case VideoCodecFormat::H264: {
            meta->Set<Media::Tag::MIME_TYPE>(Media::Plugins::MimeType::VIDEO_AVC);
            meta->Set<Media::Tag::VIDEO_H264_PROFILE>(Media::Plugins::VideoH264Profile::BASELINE);
            meta->Set<Media::Tag::VIDEO_H264_LEVEL>(32);
            break;
        }
        case VideoCodecFormat::MPEG4: {
            meta->Set<Media::Tag::MIME_TYPE>(Media::Plugins::MimeType::VIDEO_MPEG4);
            break;
        }
        case VideoCodecFormat::H265: {
            meta->Set<Media::Tag::MIME_TYPE>(Media::Plugins::MimeType::VIDEO_HEVC);
            break;
        }
        default:
            break;
        }
    }

    void VidEnc::ConfigureDecode(std::shared_ptr<Media::Meta>& meta) const
    {
        switch (encFmt) {
        case VideoCodecFormat::VIDEO_DEFAULT:
        case VideoCodecFormat::H264: {
            meta->SetData(std::string(MDKey::MD_KEY_CODEC_MIME), Media::Plugins::MimeType::VIDEO_AVC);
            break;
        }
        case VideoCodecFormat::MPEG4: {
            meta->SetData(std::string(MDKey::MD_KEY_CODEC_MIME), Media::Plugins::MimeType::VIDEO_MPEG4);
            break;
        }
        case VideoCodecFormat::H265: {
            meta->SetData(std::string(MDKey::MD_KEY_CODEC_MIME), Media::Plugins::MimeType::VIDEO_HEVC);
            break;
        }
        default:
            break;
        }
    }

    void VidRectangle::Configure(std::shared_ptr<Media::Meta> meta, const ConfigureMode mode) const
    {
        switch (mode) {
        case ConfigureMode::Encode:
            ConfigureEncode(meta);
            break;
        case ConfigureMode::Decode:
            ConfigureDecode(meta);
            break;
        default:
            break;
        }
    }

    void VidRectangle::ConfigureEncode(std::shared_ptr<Media::Meta>& meta) const
    {
        meta->Set<Media::Tag::VIDEO_WIDTH>(width);
        meta->Set<Media::Tag::VIDEO_HEIGHT>(height);
    }

    void VidRectangle::ConfigureDecode(std::shared_ptr<Media::Meta>& meta) const
    {
        meta->SetData(std::string(MDKey::MD_KEY_WIDTH), width);
        meta->SetData(std::string(MDKey::MD_KEY_HEIGHT), height);
    }

    void VidBitRate::Configure(std::shared_ptr<Media::Meta> meta, const ConfigureMode mode) const
    {
        switch (mode) {
        case ConfigureMode::Encode:
            ConfigureEncode(meta);
            break;
        case ConfigureMode::Decode:
            ConfigureDecode(meta);
            break;
        default:
            break;
        }
    }

    void VidBitRate::ConfigureEncode(std::shared_ptr<Media::Meta>& meta) const
    {
        meta->Set<Media::Tag::MEDIA_BITRATE>(bitRate);
    }

    void VidBitRate::ConfigureDecode(std::shared_ptr<Media::Meta>& meta) const
    {
        meta->SetData(std::string(MDKey::MD_KEY_BITRATE), bitRate);
    }

    void VidFrameRate::Configure(std::shared_ptr<Media::Meta> meta, const ConfigureMode mode) const
    {
        switch (mode) {
        case ConfigureMode::Encode:
            ConfigureEncode(meta);
            break;
        case ConfigureMode::Decode:
            ConfigureDecode(meta);
            break;
        default:
            break;
        }
    }

    void VidFrameRate::ConfigureEncode(std::shared_ptr<Media::Meta>& meta) const
    {
        meta->Set<Media::Tag::VIDEO_FRAME_RATE>(frameRate);
    }

    void VidFrameRate::ConfigureDecode(std::shared_ptr<Media::Meta>& meta) const
    {
        meta->SetData(std::string(MDKey::MD_KEY_FRAME_RATE), frameRate);
    }

    void VidIsHdr::Configure(std::shared_ptr<Media::Meta> meta, const ConfigureMode mode) const
    {
        switch (mode) {
        case ConfigureMode::Encode:
            ConfigureEncode(meta);
            break;
        case ConfigureMode::Decode:
            ConfigureDecode(meta);
            break;
        default:
            break;
        }
    }

    void VidIsHdr::ConfigureEncode(std::shared_ptr<Media::Meta>& meta) const
    {
        if (isHdr) {
            meta->Set<Media::Tag::VIDEO_H265_PROFILE>(Media::Plugins::HEVCProfile::HEVC_PROFILE_MAIN_10);
        }
    }

    void VidIsHdr::ConfigureDecode(std::shared_ptr<Media::Meta>& meta) const
    {
        if (isHdr) {
            meta->SetData(std::string(MDKey::MD_KEY_PROFILE), Media::Plugins::HEVCProfile::HEVC_PROFILE_MAIN_10);
        }
    }

    void VidEnableTemporalScale::Configure(std::shared_ptr<Media::Meta> meta, const ConfigureMode mode) const
    {
        switch (mode) {
        case ConfigureMode::Encode:
            ConfigureEncode(meta);
            break;
        case ConfigureMode::Decode:
            ConfigureDecode(meta);
            break;
        default:
            break;
        }
    }

    void VidEnableTemporalScale::ConfigureEncode(std::shared_ptr<Media::Meta>& meta) const
    {
        if (enableTemporalScale) {
            meta->Set<Media::Tag::VIDEO_ENCODER_ENABLE_TEMPORAL_SCALABILITY>(
                enableTemporalScale);
        }
    }

    void VidEnableTemporalScale::ConfigureDecode(std::shared_ptr<Media::Meta>& meta) const
    {
        if (enableTemporalScale) {
            meta->SetData(std::string(MDKey::OH_MD_KEY_VIDEO_ENCODER_ENABLE_TEMPORAL_SCALABILITY),
                1);
        }
    }
}
}
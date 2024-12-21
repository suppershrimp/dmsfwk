/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_AV_TRANS_STREAM_DATA_H
#define OHOS_AV_TRANS_STREAM_DATA_H

#include "av_trans_data_buffer.h"
#include "cJSON.h"
#include <memory>
#include <optional>

namespace OHOS {
namespace DistributedCollab {
    enum class AvCodecBufferFlag : uint32_t {
        AVCODEC_BUFFER_FLAG_NONE = 0,
        AVCODEC_BUFFER_FLAG_EOS,
        AVCODEC_BUFFER_FLAG_SYNC_FRAME,
        AVCODEC_BUFFER_FLAG_PARTIAL_FRAME,
        AVCODEC_BUFFER_FLAG_CODEC_DATA,
        AVCODEC_BUFFER_FLAG_PIXEL_MAP,
    };

    struct PixelMapPackOption {
        uint8_t quality = 100;
    };

    struct AVTransStreamDataExt {
        AvCodecBufferFlag flag_;
        uint32_t index_;
        uint64_t pts_;
        uint64_t startEncodeT_;
        uint64_t finishEncodeT_;
        uint64_t sendEncodeT_;
        PixelMapPackOption pixelMapOption_;
    };

    class AVTransStreamData {
    public:
        explicit AVTransStreamData(
            const std::shared_ptr<AVTransDataBuffer>& buffer,
            const AVTransStreamDataExt ext)
            : data_(buffer)
            , ext_(ext) {};
        ~AVTransStreamData() = default;

        std::shared_ptr<AVTransDataBuffer> StreamData();
        const AVTransStreamDataExt& GetStreamDataExt() const;
        cJSON* SerializeStreamDataExt() const;
        int32_t DeserializeStreamDataExt(const char* data);
        void DeserializeExtFromJson(const cJSON* root);

    private:
        std::shared_ptr<AVTransDataBuffer> data_;
        AVTransStreamDataExt ext_;

    private:
        void WriteExtFlagToJson(cJSON* root) const;
        void WriteExtIndexToJson(cJSON* root) const;
        void WriteExtPtsToJson(cJSON* root) const;
        void WriteExtStartEncodeTToJson(cJSON* root) const;
        void WriteExtFinishEncodeTToJson(cJSON* root) const;
        void WriteExtSendEncodeTToJson(cJSON* root) const;
        void WriteExtPixelMapPackOptionToJson(cJSON* root) const;

        void ReadExtFlagFromJson(AVTransStreamDataExt& dataExt, const cJSON* root);
        void ReadExtIndexFromJson(AVTransStreamDataExt& dataExt, const cJSON* root);
        void ReadExtPtsFromJson(AVTransStreamDataExt& dataExt, const cJSON* root);
        void ReadExtStartEncodeTFromJson(AVTransStreamDataExt& dataExt, const cJSON* root);
        void ReadExtFinishEncodeTFromJson(AVTransStreamDataExt& dataExt, const cJSON* root);
        void ReadExtSendEncodeTFromJson(AVTransStreamDataExt& dataExt, const cJSON* root);
        void ReadExtPixelMapPackOptionToJson(AVTransStreamDataExt& dataExt, const cJSON* root);
    };
} // namespace DistributedSchedule
} // namespace OHOS
#endif // OHOS_AV_TRANS_DATA_BUFFER_H

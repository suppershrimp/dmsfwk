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
#include "av_receiver_engine.h"
#include "av_receiver_filter_listener.h"
#include "av_sender_engine.h"
#include "av_trans_data_buffer.h"
#include "cJSON.h"
#include "display_type.h"
#include "dtbcollabmgr_log.h"
#include "ibuffer_consumer_listener.h"
#include "iconsumer_surface.h"
#include "surface_type.h"
#include "surface_utils.h"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#ifdef DSCH_COLLAB_AV_TRANS_PICTURE_TEST_DEMO
#include "iengine_listener.h"
#include "image_packer.h"
#include "image_source.h"
#include "image_type.h"
#endif

namespace OHOS {
namespace DistributedCollab {
    static const std::string TAG = "AVStreamTransProviderDemo";
    static constexpr int32_t width = 640;
    static constexpr int32_t height = 480;
    static constexpr int32_t frameSize = width * height + (width * height) / 2;
    static constexpr int32_t bitRate = 640000;
    static constexpr int32_t frameRate = 30;
    static constexpr int32_t READ_FRAME_NUM = 30;
    static const VideoSourceType sourceType = VideoSourceType::NV21;
    static const VideoCodecFormat codecFormat = VideoCodecFormat::H264;
    static const std::string yuvReadFilePath = "/data/tmp/preview.yuv";
    static const std::string yuvWriteFilePath = "/data/tmp/decode_output.yuv";
    static const std::string jpegReadFilePath = "/data/tmp/test_pic.jpeg";
    static const std::string jpegWriteFilePath = "/data/tmp/test_pic_write.jpeg";

    std::shared_ptr<AVSenderEngine> InitSender()
    {
        int32_t appUid = 0;
        int32_t appPid = 0;
        std::string bundleName = "stream_demo";
        uint64_t instanceId = 0;
        std::shared_ptr<AVSenderEngine> engine = std::make_shared<AVSenderEngine>(appUid, appPid,
            bundleName, instanceId);
        HILOGI("init sender engine");
        engine->Init();
        return engine;
    }

    template <typename T>
    int32_t ConfigEngineParam(std::shared_ptr<T>& engine)
    {
        static_assert(std::is_member_function_pointer<decltype(&T::Configure)>::value, "T must have Configure method");
        static_assert(std::is_member_function_pointer<decltype(&T::SetVideoSource)>::value,
            "T must have SetVideoSource method");
        engine->SetVideoSource(sourceType);

        int32_t ret = ERR_OK;
        ret = engine->Configure(VidEnc(codecFormat));
        if (ret != ERR_OK) {
            HILOGE("configure videnc failed");
            return ret;
        }
        ret = engine->Configure(VidRectangle(width, height));
        if (ret != ERR_OK) {
            HILOGE("configure VidRectangle failed");
            return ret;
        }
        ret = engine->Configure(VidBitRate(bitRate));
        if (ret != ERR_OK) {
            HILOGE("configure VidBitRate failed");
            return ret;
        }
        ret = engine->Configure(VidFrameRate(frameRate));
        if (ret != ERR_OK) {
            HILOGE("configure VidFrameRate failed");
            return ret;
        }
        ret = engine->Configure(VidIsHdr(false));
        if (ret != ERR_OK) {
            HILOGE("configure VidIsHdr failed");
            return ret;
        }
        ret = engine->Configure(VidEnableTemporalScale(false));
        if (ret != ERR_OK) {
            HILOGE("configure VidEnableTemporalScale failed");
            return ret;
        }
        return ERR_OK;
    }

    sptr<SurfaceBuffer> GetInputBuffer(const sptr<Surface>& surface)
    {
        BufferRequestConfig config;
        config.width = width;
        config.height = height;
        config.usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA;
        config.timeout = 0;
        config.strideAlignment = 8;
        switch (sourceType) {
            case VideoSourceType::NV12:
                config.format = PixelFormat::PIXEL_FMT_YCBCR_420_SP;
                break;
            case VideoSourceType::NV21:
                config.format = PixelFormat::PIXEL_FMT_YCRCB_420_SP;
                break;
            default:
                HILOGE("The current pixel format does not support encoding.");
                return nullptr;
        }
        sptr<SurfaceBuffer> surfacebuffer = nullptr;
        int32_t flushFence = -1;
        GSError err = surface->RequestBuffer(surfacebuffer, flushFence, config);
        if (err != GSERROR_OK || surfacebuffer == nullptr) {
            HILOGE("Request encoder input producer surface buffer failed, error code: %{public}d.", err);
        }
        return surfacebuffer;
    }

    // 读取指定帧数据到SurfaceBuffer
    bool ReadFrameToSurfaceBuffer(uint32_t frameIndex, sptr<SurfaceBuffer> surfaceBuffer)
    {
        HILOGI("ReadFrameToSurfaceBuffer enter, frameIndex=%{public}d", frameIndex);
        std::ifstream yuvFile(yuvReadFilePath, std::ios::binary);
        if (!yuvFile) {
            HILOGE("Failed to open YUV file!");
            return false;
        }
        void* bufferAddr = surfaceBuffer->GetVirAddr();
        size_t bufferSize = surfaceBuffer->GetSize();
        if (bufferSize < frameSize) {
            HILOGE("Buffer size is smaller than frame size!");
            return false;
        }
        // 计算需要跳过的字节数，跳过前frameIndex帧数据
        size_t skipBytes = frameIndex * frameSize;
        yuvFile.seekg(skipBytes, std::ios::beg);
        yuvFile.read(reinterpret_cast<char*>(bufferAddr), frameSize);

        // 检查是否读取到足够的数据
        if (yuvFile.gcount() != frameSize) {
            if (yuvFile.eof()) {
                HILOGE("Reached end of file before reading the full frame.");
            } else if (yuvFile.fail()) {
                HILOGE("An error occurred during the read operation.");
            }
            return false;
        }
        return true;
    }

    inline int64_t GetEncoderTimeStamp()
    {
        std::chrono::microseconds nowUs = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch());
        return nowUs.count();
    }

    int32_t FlushBuffer(sptr<SurfaceBuffer> surfaceBuffer, const sptr<Surface>& surface)
    {
        HILOGI("FlushBuffer to surface");
        int64_t inputTimeStampUs = GetEncoderTimeStamp();
        GraphicPresentTimestamp curTime = {
            GraphicPresentTimestampType::GRAPHIC_DISPLAY_PTS_TIMESTAMP,
            inputTimeStampUs,
        };
        uint32_t seqNum = surfaceBuffer->GetSeqNum();
        surface->SetPresentTimestamp(seqNum, curTime);
        HILOGI("now time=%{public}ld, seqNum=%{public}u", inputTimeStampUs, seqNum);

        BufferFlushConfig flushConfig = { { 0, 0, width, height }, 0 };
        SurfaceError ret = surface->FlushBuffer(surfaceBuffer, -1, flushConfig);
        if (ret != SURFACE_ERROR_OK) {
            HILOGE("Flush encoder input producer surface buffer failed");
            return -3;
        }
        return ERR_OK;
    }

    int32_t Start(const sptr<Surface>& senderSurface)
    {
        // read file and flush buffer
        int32_t idx = 0;
        while (idx < READ_FRAME_NUM) {
            sptr<SurfaceBuffer> sendBuffer = GetInputBuffer(senderSurface);
            if (sendBuffer == nullptr) {
                HILOGE("get surface buffer failed, %{public}d", idx);
                return -2;
            }
            if (!ReadFrameToSurfaceBuffer(idx, sendBuffer)) {
                HILOGE("read frame to buffer failed");
                return -1;
            }
            int32_t ret = FlushBuffer(sendBuffer, senderSurface);
            if (ret != ERR_OK) {
                return -1;
            }
            idx++;
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
        return ERR_OK;
    }

    void WriteSurfaceBufferToFile(const sptr<SurfaceBuffer>& surfaceBuffer)
    {
        HILOGI("WriteSurfaceBufferToFile enter");
        void* dataPtr = surfaceBuffer->GetVirAddr();
        int32_t dataSize = static_cast<int32_t>(surfaceBuffer->GetSize());
        if ((dataPtr == nullptr) || (dataSize <= 0)) {
            HILOGE("buffer invalid params, size: %{public}d", dataSize);
            return;
        }

        std::ofstream outFile(yuvWriteFilePath, std::ios::app);
        if (!outFile.is_open()) {
            HILOGE("open write file failed");
            return;
        }
        outFile.write(reinterpret_cast<const char*>(dataPtr), dataSize);
        if (!outFile) {
            HILOGE("Error occurred while writing to file.");
        }
        outFile.close();
    }

    class SurfaceListener : public IBufferConsumerListener {
    public:
        SurfaceListener(const sptr<Surface>& surface)
            : surface_(surface) {};
        void OnBufferAvailable() override
        {
            HILOGI("output surface buffer avaliable");
            if (auto ptr = surface_.promote()) {
                int32_t flushFence = 0;
                int64_t timestamp = 0;
                Rect damage;
                sptr<SurfaceBuffer> buffer = nullptr;
                ptr->AcquireBuffer(buffer, flushFence, timestamp, damage);
                if (buffer == nullptr) {
                    HILOGE("SurfaceListener AcquireBuffer failed");
                    return;
                }
                WriteSurfaceBufferToFile(buffer);
                ptr->ReleaseBuffer(buffer, -1);
            }
        };

    private:
        wptr<Surface> surface_;
    };

#ifdef DSCH_COLLAB_AV_TRANS_PICTURE_TEST_DEMO
    class PixelMapListener : public IEngineListener {
    public:
        void OnRecvPixelMap(const std::shared_ptr<Media::PixelMap>& pixelMap) override
        {
            HILOGI("PixelMapListener OnRecvPixelMap enter");
            std::unique_ptr<AVTransDataBuffer> buffer = nullptr;
            int32_t ret = PackPixelMap(pixelMap, buffer);
            if (ret != ERR_OK) {
                return;
            }
            WriteJpegToFile(buffer);
        }

        int32_t PackPixelMap(const std::shared_ptr<Media::PixelMap>& pixelMap,
            std::unique_ptr<AVTransDataBuffer>& buffer)
        {
            HILOGI("PixelMapListener PackPixelMap enter");
            Media::PackOption option = {
                .format = "image/jpeg",
                .quality = 100,
            };
            int32_t dataSize = pixelMap->GetByteCount();
            buffer = std::make_unique<AVTransDataBuffer>(dataSize);
            Media::ImagePacker packer;
            uint32_t ret = packer.StartPacking(buffer->Data(), buffer->Size(), option);
            if (ret != ERR_OK) {
                HILOGE("start packing failed, %{public}d", ret);
                return ret;
            }
            ret = packer.AddImage(*pixelMap.get());
            if (ret != ERR_OK) {
                HILOGE("add image failed, %{public}d", ret);
                return ret;
            }
            int64_t packSize = 0;
            ret = packer.FinalizePacking(packSize);
            if (ret != ERR_OK) {
                HILOGE("pack image failed, %{public}d", ret);
                return ret;
            }
            buffer->SetRange(0, packSize);
            return ERR_OK;
        }

        void WriteJpegToFile(const std::unique_ptr<AVTransDataBuffer>& buffer)
        {
            HILOGI("PixelMapListener WriteJpegToFile enter");
            void* dataPtr = buffer->Data();
            int32_t dataSize = static_cast<int32_t>(buffer->Size());
            if ((dataPtr == nullptr) || (dataSize <= 0)) {
                HILOGE("buffer invalid params, size: %{public}d", dataSize);
                return;
            }

            std::ofstream outFile(jpegWriteFilePath, std::ios::app);
            if (!outFile.is_open()) {
                HILOGE("open write file failed");
                return;
            }
            outFile.write(reinterpret_cast<const char*>(dataPtr), dataSize);
            if (!outFile) {
                HILOGE("Error occurred while writing to file.");
            }
            outFile.close();
        }

        void OnRecvSurfaceParam(const SurfaceParam& param) override 
        {}
    };

    void ReadJpegToBuffer(std::unique_ptr<Media::PixelMap>& pixelMap)
    {
        HILOGI("ReadJpegToBuffer enter");
        Media::SourceOptions sourceOption;
        uint32_t err = ERR_OK;
        std::unique_ptr<Media::ImageSource> imageSource =
            Media::ImageSource::CreateImageSource(jpegReadFilePath, sourceOption, err);
        if (err != ERR_OK) {
            HILOGE("create image source failed");
            return;
        }
        Media::DecodeOptions decodeOptions;
        decodeOptions.desiredSize = {
            .width = 960,
            .height = 480,
        };
        pixelMap = imageSource->CreatePixelMap(decodeOptions, err);
        if (err != ERR_OK) {
            HILOGE("decode pixelmap failed");
            return;
        }
    }
#endif

    int32_t Execute(std::shared_ptr<AVSenderEngine>& senderEngine, std::shared_ptr<AVReceiverEngine>& recvEngine)
    {
        // clear last result file
        if (std::filesystem::exists(yuvWriteFilePath)) {
            if (!std::filesystem::remove(yuvWriteFilePath)) {
                HILOGE("delete output file failed");
                return -1;
            }
        }

        // init sender engine
        senderEngine = InitSender();
        int32_t ret = ConfigEngineParam(senderEngine);
        if (ret != ERR_OK) {
            return ret;
        }
        senderEngine->SetTransChannel(0, OHOS::DistributedCollab::ChannelDataType::BYTES);
        cJSON* ecoCodecList = senderEngine->GetVideoCodecAbility();
        HILOGI("encode codec list=%{public}s", cJSON_PrintUnformatted(ecoCodecList));

        // init recv engine
        recvEngine = std::make_shared<AVReceiverEngine>();
        recvEngine->Init();
        cJSON* dcoCodecList = recvEngine->GetVideoCodecAbility();
        HILOGI("decode codec list=%{public}s", cJSON_PrintUnformatted(dcoCodecList));
        ret = ConfigEngineParam(recvEngine);
        if (ret != ERR_OK) {
            return ret;
        }
        sptr<Surface> recvSurface = IConsumerSurface::Create();
        if (recvSurface == nullptr) {
            HILOGE("error create recvSurface");
            return -1;
        }
        uint64_t id = recvSurface->GetUniqueId();
        SurfaceError err = SurfaceUtils::GetInstance()->Add(id, recvSurface);
        if (err != SURFACE_ERROR_OK) {
            HILOGE("register surface failed");
            return -1;
        }

        recvSurface->SetDefaultWidthAndHeight(width, height);
        recvSurface->SetDefaultUsage(
            SurfaceBufferUsage::BUFFER_USAGE_MEM_MMZ_CACHE | SurfaceBufferUsage::BUFFER_USAGE_CPU_READ);
        recvEngine->SetVideoSurface(id);
        recvEngine->SetChannelListener(0);

        // prepare
        ret = senderEngine->Prepare();
        if (ret != ERR_OK) {
            HILOGE("error prepare senderEngine");
            return ret;
        }
        ret = recvEngine->Prepare();
        if (ret != ERR_OK) {
            HILOGE("error prepare recvEngine");
            return ret;
        }

        // register Listener
#ifdef DSCH_COLLAB_AV_TRANS_TEST_DEMO
        std::shared_ptr<IChannelListener> listener = recvEngine->GetChannelListener();
        senderEngine->SetChannelListener(listener);
        sptr<IBufferConsumerListener> surfaceListener = sptr<SurfaceListener>::MakeSptr(recvSurface);
        recvSurface->RegisterConsumerListener(surfaceListener);
#endif

        // Get surface before start
        uint64_t surfaceId = senderEngine->GetSurface();
        if (surfaceId == 0) {
            HILOGE("get surface failed");
            return -1;
        }
        auto senderSurface = SurfaceUtils::GetInstance()->GetSurface(surfaceId);

#ifdef DSCH_COLLAB_AV_TRANS_PICTURE_TEST_DEMO
        std::unique_ptr<Media::PixelMap> pixelMap = nullptr;
        ReadJpegToBuffer(pixelMap);
        std::shared_ptr<IEngineListener> engineListener = std::make_shared<PixelMapListener>();
        recvEngine->SetEngineListener(engineListener);
        auto pixel = std::shared_ptr<Media::PixelMap>(std::move(pixelMap));
        senderEngine->SendPixelMap(pixel);
        std::this_thread::sleep_for(std::chrono::minutes(1));
        return 0;
#endif

        // Start
        ret = recvEngine->Start();
        if (ret != ERR_OK) {
            HILOGE("error start recvEngine");
            return ret;
        }
        ret = senderEngine->Start();
        if (ret != ERR_OK) {
            HILOGE("error start senderEngine");
            return ret;
        }
        return Start(senderSurface);
    }
}
}

int main(int argc, char* argv[])
{
    std::shared_ptr<OHOS::DistributedCollab::AVSenderEngine> senderEngine;
    std::shared_ptr<OHOS::DistributedCollab::AVReceiverEngine> recvEngine;
    OHOS::DistributedCollab::Execute(senderEngine, recvEngine);
    std::this_thread::sleep_for(std::chrono::minutes(1));
    senderEngine->Stop(false);
    recvEngine->Stop(false);
    return 0;
}

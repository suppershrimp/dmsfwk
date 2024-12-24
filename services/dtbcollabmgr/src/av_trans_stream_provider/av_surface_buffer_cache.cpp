#include "av_surface_buffer_cache.h"
#include "buffer_extra_data.h"
#include "dtbcollabmgr_log.h"

namespace OHOS {
namespace DistributedCollab {
    namespace {
        static constexpr uint32_t MAX_CACHE_SIZE = 3;
        static const std::string TAG = "AVSurfaceBufferCache";
    }

    AVSurfaceBufferCache::~AVSurfaceBufferCache()
    {
        Stop();
    }

    void AVSurfaceBufferCache::Init()
    {
        HILOGI("AVSurfaceBufferCache::Init enter");
        if (outputSurface_ == nullptr) {
            HILOGE("output surface empty");
            return;
        }
        sptr<IBufferProducer> producer = outputSurface_->GetProducer();
        outputSurface_ = Surface::CreateSurfaceAsProducer(producer);
    }

    int32_t AVSurfaceBufferCache::GetSurface(sptr<Surface>& surface)
    {
        HILOGI("AVSurfaceBufferCache::GetSurface enter");
        innerSurface_ = IConsumerSurface::Create();
        if (innerSurface_ == nullptr) {
            HILOGE("create inner surface failed");
            return CREATE_SURFACE_FAILED;
        }
        surface = innerSurface_;
        // surface->SetDefaultUsage(BUFFER_USAGE_MEM_MMZ_CACHE | BUFFER_USAGE_CPU_READ);
        surface->SetDefaultUsage(BUFFER_USAGE_CPU_READ & (~BUFFER_USAGE_VIDEO_DECODER));
        return ERR_OK;
    }

    void AVSurfaceBufferCache::SurfaceListener::OnBufferAvailable()
    {
        HILOGI("AVSurfaceBufferCache::SurfaceListener::OnBufferAvailable enter");
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
            if (auto cachePtr = cache_.lock()) {
                cachePtr->AddSurfaceBuffer(buffer);
            }
            ptr->ReleaseBuffer(buffer, -1);
        }
    };

    int32_t AVSurfaceBufferCache::AddSurfaceBuffer(const sptr<SurfaceBuffer>& buffer)
    {
        HILOGI("AVSurfaceBufferCache::AddSurfaceBuffer enter");
        if (buffer == nullptr) {
            HILOGE("empty surface buffer");
            return INVALID_PARAMETERS_ERR;
        }
        BufferRequestConfig config = buffer->GetBufferRequestConfig();
        config.usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA;
        HILOGI("decoder buffer, w=%{public}d,h=%{public}d,stride=%{public}d,format=%{public}d,usage=%{public}lu,size=%{public}u",
            buffer->GetWidth(),
            buffer->GetHeight(),
            buffer->GetStride(),
            buffer->GetFormat(),
            buffer->GetUsage(),
            buffer->GetSize());
        void* dataPtr = buffer->GetVirAddr();
        int32_t dataSize = static_cast<int32_t>(buffer->GetSize());
        if ((dataPtr == nullptr) || (dataSize <= 0)) {
            HILOGE("buffer invalid params, size: %{public}d", dataSize);
            return INVALID_PARAMETERS_ERR;
        }
        std::unique_ptr<AVTransDataBuffer> data = std::make_unique<AVTransDataBuffer>(dataSize);
        int32_t ret = memcpy_s(data->Data(), data->Size(), dataPtr, dataSize);
        if (ret != ERR_OK) {
            HILOGE("write buffer data failed");
            return READ_SURFACE_BUFFER_FAILED;
        }
        AddCache(data, config);
        cv_.notify_one();
        return ERR_OK;
    }

    void AVSurfaceBufferCache::AddCache(std::unique_ptr<AVTransDataBuffer>& data, BufferRequestConfig& config)
    {
        HILOGI("AVSurfaceBufferCache::AddCache enter");
        std::lock_guard<std::mutex> lock(queueMutex_);
        lastIndex_++;
        std::unique_ptr<Cache> cache = std::make_unique<Cache>();
        cache->data_ = std::move(data);
        cache->index_ = lastIndex_;
        cache->config_ = std::move(config);
        cacheQueue_.push(std::move(cache));
    }

    void AVSurfaceBufferCache::Start()
    {
        HILOGI("AVSurfaceBufferCache::Start enter");
        if (innerSurface_ == nullptr) {
            HILOGE("inner surface should be created first");
            return;
        }
        sptr<IBufferConsumerListener> surfaceListener = sptr<SurfaceListener>::MakeSptr(innerSurface_, shared_from_this());
        innerSurface_->RegisterConsumerListener(surfaceListener);
        isRunning_ = true;
        processingThread_ = std::thread(&AVSurfaceBufferCache::Process, this);
    }

    void AVSurfaceBufferCache::Stop()
    {
        HILOGI("AVSurfaceBufferCache::Stop enter");
        isRunning_ = false;
        cv_.notify_all();
        if (processingThread_.joinable()) {
            processingThread_.join();
        }
    }

    void AVSurfaceBufferCache::Process()
    {
        HILOGI("AVSurfaceBufferCache::Process enter");
        while (isRunning_) {
            std::unique_lock<std::mutex> lock(queueMutex_);
            cv_.wait(lock, [this] {
                return !isRunning_ || !cacheQueue_.empty();
            });
            if (!isRunning_) {
                HILOGI("exit running process thread");
                return;
            }
            // first need MAX_CACHE_SIZE frame saved
            if (lastIndex_ < MAX_CACHE_SIZE) {
                continue;
            }
            while (!cacheQueue_.empty()) {
                auto& ptr = cacheQueue_.top();
                WriteDataToSurface(ptr);
                cacheQueue_.pop();
            }
        }
    }

    void AVSurfaceBufferCache::WriteDataToSurface(const std::unique_ptr<Cache>& cacheData)
    {
        HILOGD("AVSurfaceBufferCache::WriteDataToSurface enter");
        if (outputSurface_ == nullptr) {
            HILOGE("outputSurface not init");
            return;
        }
        BufferRequestConfig& config = cacheData->config_;
        sptr<SurfaceBuffer> surfaceBuffer = nullptr;
        int32_t flushFence = -1;
        GSError err = outputSurface_->RequestBuffer(surfaceBuffer, flushFence, config);
        if (err != GSERROR_OK || surfaceBuffer == nullptr) {
            HILOGE("Request output surface buffer failed, error code: %{public}d", err);
            surfaceBuffer = nullptr;
            return;
        }
        auto& dataPtr = cacheData->data_;
        void* bufferAddr = surfaceBuffer->GetVirAddr();
        size_t bufferSize = surfaceBuffer->GetSize();
        if (bufferSize < dataPtr->Size()) {
            HILOGE("Buffer size is smaller than frame size!");
            return;
        }
        int32_t ret = memcpy_s(bufferAddr, bufferSize, dataPtr->Data(), dataPtr->Size());
        if (ret != ERR_OK) {
            HILOGE("write buffer to surface failed");
            return;
        }
        BufferFlushConfig flushConfig = { { 0, 0, config.width, config.height }, 0 };
        SurfaceError flushRet = outputSurface_->FlushBuffer(surfaceBuffer, -1, flushConfig);
        if (flushRet != SURFACE_ERROR_OK) {
            HILOGE("Flush encoder input producer surface buffer failed");
        }
    }
} // namespace DistributedCollab
} // namespace OHOS
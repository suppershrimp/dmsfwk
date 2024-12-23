#ifndef OHOS_DSCHED_ABILITY_CONNECTION_INFO_H
#define OHOS_DSCHED_ABILITY_CONNECTION_INFO_H

#include <map>
#include <string>

#include "ability_connection_manager_listener.h"
#include "av_trans_data_buffer.h"
#include "distributed_sched_utils.h"
#include "parcel.h"
#include "refbase.h"
#include "want_params.h"

namespace OHOS {
namespace DistributedCollab {
enum class StartOption : int32_t {
    START_TO_FOREGROUND,
    START_TO_BACKGROUND
};

enum class StreamRole : int32_t {
    SOURCE = 0,
    SINK = 1,
};

enum class FlipOption : int32_t {
    HORIZONTAL = 0,
    VERTICAL = 1,
};

enum class VideoPixelFormat : int32_t {
    UNKNOWN = -1,
    NV12 = 0,
    NV21 = 1,
};

enum class DisconnectReason : int32_t {
    UNKNOW = -1,
    PEER_APP_EXIT = 0,
    NETWORK_DISCONNECTED = 1,
};

struct PeerInfo : public Parcelable {
    std::string deviceId;
    std::string bundleName;
    std::string moduleName;
    std::string abilityName;
    std::string serverId;

    PeerInfo() = default;
    PeerInfo(const std::string& deviceId, const std::string& bundleName,
        const std::string& moduleName, const std::string& abilityName, const std::string& serverId)
        : deviceId(deviceId), bundleName(bundleName), moduleName(moduleName),
        abilityName(abilityName), serverId(serverId) {}

    bool ReadFromParcel(Parcel &parcel);
    bool Marshalling(Parcel &parcel) const override;
    static PeerInfo *Unmarshalling(Parcel &parcel);

    bool operator == (const PeerInfo &index) const
    {
        std::string compareInfo = this->deviceId + this->bundleName + this->moduleName +
            this->abilityName + this->serverId;
        std::string otherCompareInfo = index.deviceId + index.bundleName + index.moduleName +
            index.abilityName + index.serverId;
        return compareInfo.compare(otherCompareInfo) == 0;
    }

    bool operator < (const PeerInfo &index) const
    {
        std::string compareInfo = this->deviceId + this->bundleName + this->moduleName +
            this->abilityName + this->serverId;
        std::string otherCompareInfo = index.deviceId + index.bundleName + index.moduleName +
            index.abilityName + index.serverId;
        return compareInfo < otherCompareInfo;
    }

    std::string toString() const
    {
        return "deviceId: " + DistributedSchedule::GetAnonymStr(deviceId) + " " +
            "bundleName: " + bundleName + " " +
            "moduleName: "+ moduleName + " " +
            "abilityName: " + abilityName + " " +
            "serverId: " + serverId;
    }
};

struct ConnectOption : public Parcelable {
    bool needSendBigData;
    bool needSendStream;
    bool needReceiveStream;
    // WantParams startOption;
    AAFwk::WantParams parameters;
    bool ReadFromParcel(Parcel &parcel);
    bool Marshalling(Parcel &parcel) const override;
    static ConnectOption *Unmarshalling(Parcel &parcel);
};

struct StreamParams {
    std::string name;
    StreamRole role;
};

struct SurfaceParam {
    int32_t width;
    int32_t height;
    VideoPixelFormat format;
    int32_t rotation;
    FlipOption flip;
};

struct EventCallbackInfo {
    int32_t sessionId;
    DisconnectReason reason = DisconnectReason::UNKNOW;
    std::string msg;
    std::shared_ptr<AVTransDataBuffer> data = nullptr;
};
} // namespace DistributedCollab
} // namespace OHOS
#endif //OHOS_DSCHED_ABILITY_CONNECTION_INFO_H
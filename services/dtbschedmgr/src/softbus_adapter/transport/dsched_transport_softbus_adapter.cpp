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

#include "dsched_transport_softbus_adapter.h"

#include "adapter/dnetwork_adapter.h"
#include "dtbschedmgr_device_info_storage.h"
#include "dtbschedmgr_log.h"
#include "softbus_bus_center.h"
#include "softbus_common.h"
#include "softbus_error_code.h"

namespace OHOS {
namespace DistributedSchedule {
namespace {
const std::string TAG = "DSchedTransportSoftbusAdapter";
}

IMPLEMENT_SINGLE_INSTANCE(DSchedTransportSoftbusAdapter);

static QosTV g_qosInfo[] = {
    { .qos = QOS_TYPE_MIN_BW, .value = 64 * 1024 },
    { .qos = QOS_TYPE_MAX_LATENCY, .value = 10000 },
    { .qos = QOS_TYPE_MIN_LATENCY, .value = 5000 }
};
static uint32_t g_QosTV_Param_Index = static_cast<uint32_t>(sizeof(g_qosInfo) / sizeof(QosTV));

static void OnBind(int32_t socket, PeerSocketInfo info)
{
    HILOGD("called, sessionId: %d", socket);
    DSchedTransportSoftbusAdapter::GetInstance().CreateSessionRecord(sessionId, peerDeviceId, false);
}

static void OnShutdown(int32_t socket, ShutdownReason reason)
{
    HILOGD("called, sessionId: %d, reason: %d", reason);
    DSchedTransportSoftbusAdapter::GetInstance().OnShutdown(socket, reason);
}

static void OnBytes(int32_t socket, const void *data, uint32_t dataLen)
{
    DSchedTransportSoftbusAdapter::GetInstance().OnBytes(socket, data, dataLen);
}

ISocketListener iSocketListener = {
    .OnBind = OnBind,
    .OnShutdown = OnShutdown,
    .OnBytes = OnBytes
};

DSchedTransportSoftbusAdapter::DSchedTransportSoftbusAdapter()
{
}

DSchedTransportSoftbusAdapter::~DSchedTransportSoftbusAdapter()
{
}

int32_t DSchedTransportSoftbusAdapter::InitChannel()
{
    HILOGI("start");
    int32_t serverSocket_ = CreateServerSocket();
    if (serverSocket_ <= 0) {
        HILOGE("create socket failed, ret: %d", serverSocket_);
        return serverSocket_;
    }

    int32_t ret = Listen(serverSocket_, g_qosInfo, g_QosTV_Param_Index, &iSocketListener);
    if (ret != ERR_OK) {
        HILOGE("service listen failed, ret: %d", ret);
        return ret;
    }
    HILOGI("end");
    return ERR_OK;
}

int32_t DSchedTransportSoftbusAdapter::CreateServerSocket()
{
    HILOGI("start");
    localSessionName_ = SOCKET_CONTINUE_SERVER_NAME;
    SocketInfo info = {
        .name = const_cast<char*>(localSessionName_.c_str()),
        .pkgName = const_cast<char*>(SOCKET_DMS_PKG_NAME.c_str()),
        .dataType = DATA_TYPE_BYTES
    };
    int32_t socket = Socket(info);
    HILOGI("finish, socket session id: %d", socket);
    return socket;
}

int32_t DSchedTransportSoftbusAdapter::ConnectDevice(std::string &peerDeviceId)
{
    HILOGI("try to connect peer: %s", DnetworkAdapter::AnonymizeNetworkId(peerDeviceId).c_str());
    int32_t sessionId = 0;
    if (!GetSessionIdByDeviceId(peerDeviceId, sessionId)) {
        HILOGI("peer device already connected");
        std::lock_guard<std::mutex> sessionLock(sessionMutex_);
        sessions_[sessionId]->OnConnect();
        return sessionId;
    }

    sessionId = CreateClientSocket(peerDeviceId);
    if (sessionId <= 0) {
        HILOGE("create socket failed, ret: %d", sessionId);
        return -1;
    }
    
    int32_t ret = Bind(sessionId, serverQos, &listeners_[1]);
    if (ret != ERR_OK) {
        HILOGE("client bind failed, ret: %d", ret);
        Shutdown(sessionId);
        return -1;
    }
    return CreateSessionRecord(sessionId, peerDeviceId, false);
}

int32_t DSchedTransportSoftbusAdapter::CreateClientSocket(std::string &peerDeviceId)
{
    HILOGI("start");
    SocketInfo info = {
        .name = const_cast<char*>(SOCKET_CONTINUE_CLIENT_NAME.c_str()),
        .peerName = const_cast<char*>(SOCKET_CONTINUE_SERVER_NAME.c_str()),
        .peerNetworkId = const_cast<char*>(peerDeviceId.c_str()),
        .pkgName = const_cast<char*>(SOCKET_DMS_PKG_NAME.c_str()),
        .dataType = DATA_TYPE_BYTES
    };
    int32_t sessionId = Socket(info);
    HILOGI("finish, socket session id: %d", sessionId);
    return sessionId;
}

int32_t DSchedTransportSoftbusAdapter::CreateSessionRecord(int32_t sessionId, std::string &peerDeviceId, bool isServer)
{
    std::string localDeviceId;
    if (!DtbschedmgrDeviceInfoStorage::GetInstance().GetLocalDeviceId(&localDeviceId)) {
        HILOGE("GetLocalDeviceId failed");
        Shutdown(sessionId);
        return -1;
    }
    {
        std::lock_guard<std::mutex> sessionLock(sessionMutex_);
        std::string sessionName = SOCKET_CONTINUE_CLIENT_NAME;
        if (isServer) {
            sessionName = SOCKET_CONTINUE_SERVER_NAME;
        }
        SessionInfo info = { sessionId, localDeviceId, peerDeviceId, sessionName, isServer };
        auto session = std::make_shared<DSchedSoftbusSession>(info);
        sessions_[sessionId] = session;
    }
    return ERR_OK;
}

int32_t DSchedTransportSoftbusAdapter::DisconnectDevice(std::string &peerDeviceId)
{
    HILOGI("try to disconnect peer: %s", DnetworkAdapter::AnonymizeNetworkId(peerDeviceId).c_str());
    int32_t sessionId = 0;
    if (!GetSessionIdByDeviceId(peerDeviceId, sessionId)) {
        HILOGI("peer device is not connected");
        return ERR_OK;
    }
    {
        std::lock_guard<std::mutex> sessionLock(sessionMutex_);
        sessions_[sessionId]->DecreaseRefCount();
    }
    HILOGI("finish, socket session id: %d", sessionId);
    return ERR_OK;
}

bool DSchedTransportSoftbusAdapter::GetSessionIdByDeviceId(std::string &peerDeviceId, int32_t &sessionId) {
    std::lock_guard<std::mutex> sessionLock(sessionMutex_);
    for (auto iter = sessions_.begin(); iter != sessions_.end(); iter++) {
        if (iter->second != nullptr && peerDeviceId == iter->second->GetPeerDeviceId()) {
            sessionId = iter->first;
            return true;
        }
    }
    return false;
}

void DSchedTransportSoftbusAdapter::OnShutdown(int32_t sessionId)
{
    std::lock_guard<std::mutex> sessionLock(sessionMutex_);
    if (!sessions_.count(sessionId)) {
        HILOGE("error, invalid sessionId %{public}d", sessionId);
    }
    HILOGI("shutdown client: %s, socket session id: %d",
        DnetworkAdapter::AnonymizeNetworkId(sessions_[sessionId]->GetPeerDeviceId()).c_str(), sessionId);
    Shutdown(sessionId);
    sessions_.erase(sessionId);
}

int32_t DSchedTransportSoftbusAdapter::ReleaseChannel()
{
    HILOGI("start");
    {
        std::lock_guard<std::mutex> sessionLock(sessionMutex_);
        for (auto iter = sessions_.begin(); iter != sessions_.end(); iter++) {
            HILOGI("shutdown client: %s, socket session id: %d",
                DnetworkAdapter::AnonymizeNetworkId(iter->second->GetPeerDeviceId()).c_str(), iter->first);
            Shutdown(iter->first);
        }
        sessions_.clear();
    }
    HILOGI("shutdown server: %s, socket session id: %d", serverSocket_);
    Shutdown(serverSocket_);
    serverSocket_ = 0;
    return ERR_OK;
}

int32_t DSchedTransportSoftbusAdapter::SendData(int32_t sessionId, std::shared_ptr<DSchedDataBuffer> dataBuffer)
{
    std::lock_guard<std::mutex> sessionLock(sessionMutex_);
    if (!sessions_.count(sessionId)) {
        HILOGE("error, invalid session id %{public}d", sessionId);
    }
    return sessions_[sessionId]->SendData(dataBuffer);
}

int32_t DSchedTransportSoftbusAdapter::SendBytes(int32_t sessionId, std::shared_ptr<DSchedDataBuffer> dataBuffer)
{
    return SendBytes(sessionId, buffer->Data(), buffer->Size());
}

void DSchedTransportSoftbusAdapter::OnBytes(int32_t sessionId, const void *data, uint32_t dataLen)
{
    if (dataLen == 0 || dataLen > DSCHED_MAX_RECV_DATA_LEN || data == nullptr) {
        HILOGE("error, dataLen: %{public}d, session id: %{public}d", dataLen, sessionId);
        return;
    }
    HILOGD("start, sessionId: %{public}d", sessionId);
    {
        std::lock_guard<std::mutex> sessionLock(sessionMutex_);
        if (!sessions_.count(sessionId)) {
            HILOGE("invalid session id %{public}d", sessionId);
            return;
        }
        std::shared_ptr<DSchedDataBuffer> buffer = std::make_shared<DSchedDataBuffer>(dataLen);
        ret = memcpy_s(buffer->Data(), buffer->Capacity(), data, dataLen);
        if (ret != ERR_OK) {
            HILOGE("memcpy_s failed ret: %{public}d", ret);
            return;
        }
        sessions_[sessionId]->OnBytesReceived(buffer);
    }
    HILOGD("end, session id: %{public}d", sessionId);
    return;
}

void DSchedTransportSoftbusAdapter::OnDataReady(int32_t sessionId, std::shared_ptr<DSchedDataBuffer> dataBuffer)
{
    std::lock_guard<std::mutex> listenerMapLock(listenerMutex_);
    auto iterItem = listeners_.find(onType_);
    if (iterItem == listeners_.end()) {
        HILOGE("get iterItem failed from listeners_, sessionId: %{public}d", sessionId);
        return INVALID_PARAMETERS_ERR;
    }
    std::vector<std::shared_ptr<ISocketListener>> objs = iterItem->second;
    for (auto iter : objs) {
        iter->HandleBytesReceived(sessionId, data, dataLen);
    }
    return;
}

void DSchedTransportSoftbusAdapter::RegisterListener(int32_t serviceType, std::shared_ptr<IDataListener> listener)
{
    HILOGI("start, service type: %{public}d", serviceType);
    if (listener == nullptr) {
        HILOGE("listener is null, type: %{public}d", serviceType);
        return INVALID_PARAMETERS_ERR;
    }
    std::lock_guard<std::mutex> listenerMapLock(listenerMutex_);
    auto iterItem = listeners_.find(serviceType);
    if (iterItem == listeners_.end()) {
        HILOGD("service type %{public}d does not exist in the listeners, adding", serviceType);
        std::vector<std::shared_ptr<ISocketListener>> newListners;
        newListners.emplace_back(listener);
        listeners_[serviceType] = newListners;
        HILOGI("listener register success");
        return ERR_OK;
    }
    for (auto iter : iterItem->second) {
        if (iter == listener) {
            HILOGI("listener already registed");
            return ERR_OK;
        }
    }
    iterItem->second.emplace_back(listener);
    HILOGI("listener register success");
    return ERR_OK;
}
}  // namespace DistributedSchedule
}  // namespace OHOS
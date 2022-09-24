/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_DISTRIBUTED_DATA_STORAGE_H
#define OHOS_DISTRIBUTED_DATA_STORAGE_H

#include <atomic>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <set>
#include "deviceManager/dms_device_info.h"
#include "distributed_data_change_listener.h"
#include "distributed_kv_data_manager.h"
#include "event_handler.h"
#include "kvstore_death_recipient.h"

namespace OHOS {
namespace DistributedSchedule {
class DistributedDataStorage {
public:
    DistributedDataStorage();
    ~DistributedDataStorage() = default;

    /**
     * Init DistributedDataStorage.
     *
     * @return Returns true if Init successfully.
     */
    bool Init();

    /**
     * Stop DistributedDataStorage.
     *
     * @return Returns true if Stop successfully.
     */
    bool Stop();

    /**
     * Insert networkId + missionId to kvStore.
     *
     * @param networkId, the networkId to insert
     * @param missionId, the missionId to insert
     * @param byteStream, byte stream for file conversion
     * @param len, length of the byte stream
     * @return Returns true if Insert successfully.
     */
    bool Insert(const std::string& networkId, int32_t missionId, const uint8_t* byteStream, size_t len);

    /**
     * Delete networkId + missionId in kvStore.
     *
     * @param networkId, the networkId to delete
     * @param missionId, the missionId to delete
     * @return Returns true if Delete successfully.
     */
    bool Delete(const std::string& networkId, int32_t missionId);

    /**
     * FuzzyDelete networkId in kvStore.
     *
     * @param networkId, the networkId to delete
     * @return Returns true if Delete successfully.
     */
    bool FuzzyDelete(const std::string& networkId);

    /**
     * Query networkId + missionId in kvStore.
     *
     * @param networkId, the networkId to query
     * @param missionId, the missionId to query
     * @param value, if success return the value
     * @return Returns true if query successfully.
     */
    bool Query(const std::string& networkId, int32_t missionId, DistributedKv::Value& value) const;

    void NotifyRemoteDied(const wptr<IRemoteObject>& remote);

private:
    bool InitKvDataService();
    bool WaitKvDataService();
    void InitDistributedDataStorage();
    bool TryGetKvStore();
    DistributedKv::Status GetKvStore();
    void SubscribeDistributedDataStorage();
    bool UninitDistributedDataStorage();
    bool InsertInnerLocked(const std::string& uuid, int32_t missionId, const uint8_t* byteStream, size_t len);
    bool DeleteInnerLocked(const std::string& uuid, int32_t missionId);
    bool FuzzyDeleteInnerLocked(const std::string& networkId);
    bool QueryInnerLocked(const std::string& uuid, int32_t missionId, DistributedKv::Value& value) const;
    static void GenerateKey(const std::string& uuid, int32_t missionId, DistributedKv::Key& key);
    static void GenerateValue(const uint8_t* byteStream, size_t len, DistributedKv::Value& value);

    mutable std::shared_mutex initLock_;
    std::shared_ptr<AppExecFwk::EventHandler> dmsDataStorageHandler_;
    DistributedKv::AppId appId_;
    DistributedKv::StoreId storeId_;
    DistributedKv::DistributedKvDataManager dataManager_;
    std::shared_ptr<DistributedKv::SingleKvStore> kvStorePtr_; // protected by initLock_
    std::unique_ptr<DistributedDataChangeListener> distributedDataChangeListener_;
    sptr<IRemoteObject::DeathRecipient> kvStoreDeathRecipient_;
};
} // DistributedSchedule
} // OHOS

#endif // OHOS_DISTRIBUTED_DATA_STORAGE_H
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

#ifndef DISTRIBUTED_BM_STORAGE_H
#define DISTRIBUTED_BM_STORAGE_H

#include <map>
#include <mutex>
#include <shared_mutex>

#include "bundle_info.h"
#include "bundle_mgr_interface.h"
#include "distributed_kv_data_manager.h"
#include "deviceManager/dms_device_info.h"
#include "distributed_data_change_listener.h"
#include "distributed_kv_data_manager.h"
#include "mission/distributed_bundle_info.h"
#include "kvstore_death_recipient.h"

namespace OHOS {
namespace DistributedSchedule {
namespace {
// distributed bm
static std::string DMS_BM_APP_ID = "DistributedSchedule";
static std::string DISTRIBUTE_BM_STORE_ID = "dms_bundle_datas";
}
class DmsBmStorage {
public:
    DmsBmStorage();
    ~DmsBmStorage();
    static std::shared_ptr<DmsBmStorage> GetInstance();
    OHOS::sptr<OHOS::AppExecFwk::IBundleMgr> GetBundleMgr();
    bool SaveStorageDistributeInfo(const std::string &bundleName, int32_t userId);
    bool DeleteStorageDistributeInfo(const std::string &bundleName, int32_t userId);
    bool GetStorageDistributeInfo(const std::string &networkId, const std::string &bundleName,
        DmsBundleInfo &info);
    bool GetDistributedBundleName(const std::string &networkId,  uint16_t accessTokenId,
        std::string &bundleName);
    bool GetBundleId(std::string bundleName, uint16_t &accessTokenId);
    std::string GetContinueType(const std::string &networkId, std::string &bundleName, uint8_t continueTypeId);
    void UpdateDistributedData(int32_t userId);
    int32_t PullOtherDistributedData();

private:
    std::string DeviceAndNameToKey(const std::string &udid, const std::string &bundleName) const;
    void TryTwice(const std::function<DistributedKv::Status()> &func) const;
    bool CheckKvStore();
    DistributedKv::Status GetKvStore();
    bool DealGetBundleId(std::string bundleName, uint16_t &accessTokenId);
    bool DealGetBundleName(const std::string &networkId, uint16_t accessTokenId, std::string &bundleName);
    uint16_t CreateBundleId(std::string bundleName);
    DmsBundleInfo ConvertToDistributedBundleInfo(const AppExecFwk::BundleInfo &bundleInfo);
    bool InnerSaveStorageDistributeInfo(const DmsBundleInfo &distributedBundleInfo);
    std::map<std::string, DmsBundleInfo> GetAllOldDistributionBundleInfo(
        const std::vector<std::string> &bundleNames);

private:
    static std::mutex mutex_;
    static std::shared_ptr<DmsBmStorage> instance_;
    const DistributedKv::AppId appId_ {DMS_BM_APP_ID};
    const DistributedKv::StoreId storeId_ {DISTRIBUTE_BM_STORE_ID};
    DistributedKv::DistributedKvDataManager dataManager_;
    std::shared_ptr<DistributedKv::SingleKvStore> kvStorePtr_;
    mutable std::mutex kvStorePtrMutex_;
    OHOS::sptr<OHOS::AppExecFwk::IBundleMgr> bundleMgr_;
    std::vector<bool> bundleNameIdTables;
};
}  // namespace DistributedSchedule
}  // namespace OHOS
#endif  // DISTRIBUTED_BM_STORAGE_H
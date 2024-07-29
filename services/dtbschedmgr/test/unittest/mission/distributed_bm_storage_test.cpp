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

#include "distributed_bm_storage_test.h"

#include <thread>
#include "distributed_sched_test_util.h"
#include "dtbschedmgr_device_info_storage.h"
#include "test_log.h"

namespace OHOS {
namespace DistributedSchedule {
using namespace std;
using namespace testing;
using namespace testing::ext;
using namespace OHOS::DistributedKv;
using namespace OHOS::DistributedHardware;
namespace {
const std::string BASEDIR = "/data/service/el1/public/database/DistributedSchedule";
constexpr int32_t TASK_ID_1 = 11;
constexpr int32_t TASK_ID_2 = 12;
constexpr size_t BYTESTREAM_LENGTH = 100;
constexpr uint8_t ONE_BYTE = '6';
}

void DistributedBmStorageTest::SetUpTestCase()
{
    mkdir(BASEDIR.c_str(), (S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));
    DTEST_LOG << "DistributedBmStorageTest::SetUpTestCase" << std::endl;
}

void DistributedBmStorageTest::TearDownTestCase()
{
    (void)remove(BASEDIR.c_str());
    DTEST_LOG << "DistributedBmStorageTest::TearDownTestCase" << std::endl;
}

void DistributedBmStorageTest::SetUp()
{
    DistributedSchedUtil::MockPermission();
    dmsBmStorage_ = std::make_shared<DmsBmStorage>();
    DTEST_LOG << "DistributedBmStorageTest::SetUp" << std::endl;
}

void DistributedBmStorageTest::TearDown()
{
    DTEST_LOG << "DistributedBmStorageTest::TearDown" << std::endl;
}


std::shared_ptr<DmsBmStorage> DistributedBmStorageTest::GetDmsBmStorage()
{
    if (dmsBmStorage_ == nullptr) {
        dmsBmStorage_ = std::make_unique<DmsBmStorage>();
    }
    return dmsBmStorage_;
}

/**
 * @tc.name: InitTest_001
 * @tc.desc: test init DistributedBmStorage
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, SaveStorageDistributeInfo_001, TestSize.Level0)
{
    DTEST_LOG << "DistributedBmStorageTest SaveStorageDistributeInfo_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        const std::string bundleName = "NonexistentName";
        bool ret = dmsBmStorage_->GetInstance()->SaveStorageDistributeInfo(bundleName);
        EXPECT_EQ(ret, false);
    }
    DTEST_LOG << "DistributedBmStorageTest SaveStorageDistributeInfo_001 end" << std::endl;
}

/**
 * @tc.name: InnerSaveStorageDistributeInfo_001
 * @tc.desc: test InnerSaveStorageDistributeInfo
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, InnerSaveStorageDistributeInfo_001, TestSize.Level0)
{
    DTEST_LOG << "DistributedBmStorageTest InnerSaveStorageDistributeInfo_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    ASSERT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        DmsBundleInfo info;
        std::string udid = "utest";
        bool ret = dmsBmStorage_->GetInstance()->InnerSaveStorageDistributeInfo(info, udid);
        EXPECT_EQ(ret, false);
        info.bundleName = "NonExistName";
        ret = dmsBmStorage_->GetInstance()->InnerSaveStorageDistributeInfo(info, udid);
        EXPECT_EQ(ret, true);
    }
    DTEST_LOG << "DistributedBmStorageTest InnerSaveStorageDistributeInfo_001 end" << std::endl;
}

/**
 * @tc.name: DelBundleNameId_001
 * @tc.desc: test DelBundleNameId
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, DelBundleNameId_001, TestSize.Level0)
{
    DTEST_LOG << "DistributedBmStorageTest DelBundleNameId_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);

    auto bm = dmsBmStorage_->GetInstance();
    uint16_t id = 9999;
    std::string bundleName = "NonExistName";
    int32_t size = bm->bundleNameIdTables_.size();

    bm->AddBundleNameId(id, bundleName);
    EXPECT_EQ(bm->bundleNameIdTables_.size(), size + 1);

    bm->DelBundleNameId(bundleName);
    EXPECT_EQ(bm->bundleNameIdTables_.size(), size);
    DTEST_LOG << "DistributedBmStorageTest DelBundleNameId_001 end" << std::endl;
}

/**
 * @tc.name: DeleteStorageDistributeInfoTest_001
 * @tc.desc: test delete DistributedBmStorage
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, DeleteStorageDistributeInfoTest_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest DeleteStorageDistributeInfoTest_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        const std::string bundleName = "DeleteStorageDistributeInfoTest_001";
        bool ret = dmsBmStorage_->GetInstance()->DeleteStorageDistributeInfo(bundleName);
        EXPECT_EQ(ret, false);
    }
    DTEST_LOG << "DistributedBmStorageTest DeleteStorageDistributeInfoTest_001 end" << std::endl;
}

/**
 * @tc.name: DeleteStorageDistributeInfoTest_002
 * @tc.desc: test delete DistributedBmStorage
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, DeleteStorageDistributeInfoTest_002, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest DeleteStorageDistributeInfoTest_002 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        DmsBundleInfo info;
        std::string udid = "utest";
        info.bundleName = "DeleteStorageDistributeInfoTest_002";
        bool ret = dmsBmStorage_->GetInstance()->InnerSaveStorageDistributeInfo(info, udid);
        EXPECT_EQ(ret, true);
        ret = dmsBmStorage_->GetInstance()->DeleteStorageDistributeInfo(info.bundleName);
        EXPECT_EQ(ret, true);
    }
    DTEST_LOG << "DistributedBmStorageTest DeleteStorageDistributeInfoTest_002 end" << std::endl;
}

/**
 * @tc.name: GetStorageDistributeInfo_001
 * @tc.desc: test insert DistributedBmStorage
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, GetStorageDistributeInfo_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest GetStorageDistributeInfo_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    ASSERT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        std::string bundleName = "GetStorageDistributeInfo_001";
        DmsBundleInfo info;
        bool ret = dmsBmStorage_->GetInstance()->GetStorageDistributeInfo("", bundleName, info);
        EXPECT_EQ(ret, false);

        std::string localDeviceId;
        ret = DtbschedmgrDeviceInfoStorage::GetInstance().GetLocalDeviceId(localDeviceId);
        ASSERT_EQ(ret, true);
        ret = dmsBmStorage_->GetInstance()->GetStorageDistributeInfo(localDeviceId, bundleName, info);
        EXPECT_EQ(ret, false);
    }
    DTEST_LOG << "DistributedBmStorageTest GetStorageDistributeInfo_001 end" << std::endl;
}

/**
 * @tc.name: DealGetBundleName_001
 * @tc.desc: test DealGetBundleName
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, DealGetBundleName_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest DealGetBundleName_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    ASSERT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        std::string bundleName;
        DmsBundleInfo info;
        bool ret = dmsBmStorage_->GetInstance()->DealGetBundleName("", 0, bundleName);
        EXPECT_EQ(ret, false);

        std::string localDeviceId;
        ret = DtbschedmgrDeviceInfoStorage::GetInstance().GetLocalDeviceId(localDeviceId);
        ASSERT_EQ(ret, true);
        ret = dmsBmStorage_->GetInstance()->DealGetBundleName(localDeviceId, 0, bundleName);
        EXPECT_EQ(ret, false);
    }
    DTEST_LOG << "DistributedBmStorageTest DealGetBundleName_001 end" << std::endl;
}

/**
 * @tc.name: DelReduData_001
 * @tc.desc: test DelReduData
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, DelReduData_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest DelReduData_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    ASSERT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        std::vector<DistributedKv::Entry> entry;
        bool ret = dmsBmStorage_->GetInstance()->DelReduData("", entry);
        EXPECT_EQ(ret, false);

        std::string localDeviceId;
        ret = DtbschedmgrDeviceInfoStorage::GetInstance().GetLocalDeviceId(localDeviceId);
        ASSERT_EQ(ret, true);
        ret = dmsBmStorage_->GetInstance()->DelReduData(localDeviceId, entry);
        EXPECT_EQ(ret, true);
    }
    DTEST_LOG << "DistributedBmStorageTest DelReduData_001 end" << std::endl;
}

/**
 * @tc.name: CheckSyncData_001
 * @tc.desc: test CheckSyncData
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, CheckSyncData_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest CheckSyncData_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    ASSERT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        std::vector<DistributedKv::Entry> entry;
        bool ret = dmsBmStorage_->GetInstance()->CheckSyncData("");
        EXPECT_EQ(ret, false);

        std::string localDeviceId;
        ret = DtbschedmgrDeviceInfoStorage::GetInstance().GetLocalDeviceId(localDeviceId);
        ASSERT_EQ(ret, true);
        ret = dmsBmStorage_->GetInstance()->CheckSyncData(localDeviceId);
        EXPECT_EQ(ret, true);
    }
    DTEST_LOG << "DistributedBmStorageTest CheckSyncData_001 end" << std::endl;
}

/**
 * @tc.name: GetDistributedBundleNameTest_001
 * @tc.desc: test GetDistributedBundleName
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, GetDistributedBundleNameTest_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest GetDistributedBundleNameTest_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        std::string bundleName;
        bool ret = dmsBmStorage_->GetInstance()->GetDistributedBundleName("", 0, bundleName);
        EXPECT_EQ(ret, false);

        std::string localDeviceId;
        ret = DtbschedmgrDeviceInfoStorage::GetInstance().GetLocalDeviceId(localDeviceId);
        ASSERT_EQ(ret, true);
        ret = dmsBmStorage_->GetInstance()->GetDistributedBundleName(localDeviceId, 0, bundleName);
        EXPECT_EQ(ret, false);
    }
    DTEST_LOG << "DistributedBmStorageTest GetDistributedBundleNameTest_001 end" << std::endl;
}

/**
 * @tc.name: GetDistributedBundleNameTest_002
 * @tc.desc: test insert DistributedBmStorage
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, GetDistributedBundleNameTest_002, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest GetDistributedBundleNameTest_002 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        std::string networkId = "";
        std::string bundleName = "";
        uint16_t bundleNameId = 0;
        bool ret = dmsBmStorage_->GetInstance()->GetDistributedBundleName(networkId, bundleNameId, bundleName);
        EXPECT_EQ(ret, false);
    }
    DTEST_LOG << "DistributedBmStorageTest GetDistributedBundleNameTest_002 end" << std::endl;
}

/**
 * @tc.name: GetDistributedBundleNameTest_003
 * @tc.desc: test insert DistributedBmStorage
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, GetDistributedBundleNameTest_003, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest GetDistributedBundleNameTest_003 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        std::string networkId = "";
        std::string bundleName = "browser";
        uint16_t bundleNameId = 3;
        bool ret = dmsBmStorage_->GetInstance()->GetDistributedBundleName(networkId, bundleNameId, bundleName);
        EXPECT_EQ(ret, false);
    }
    DTEST_LOG << "DistributedBmStorageTest GetDistributedBundleNameTest_003 end" << std::endl;
}

/**
 * @tc.name: GetResultSatus_001
 * @tc.desc: test GetResultSatus
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, GetResultSatus_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest GetResultSatus_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    std::promise<OHOS::DistributedKv::Status> signal;
    Status ret = dmsBmStorage_->GetInstance()->GetResultSatus(signal);
    EXPECT_EQ(ret, Status::ERROR);
    DTEST_LOG << "DistributedBmStorageTest GetResultSatus_001 end" << std::endl;
}

/**
 * @tc.name: GetBundleIdTest_001
 * @tc.desc: test GetBundleNameId
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, GetBundleIdTest_001, TestSize.Level0)
{
    DTEST_LOG << "DistributedBmStorageTest GetBundleIdTest_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        const std::string bundleName = "NonexistentName";
        uint16_t bundleNameId = 0;
        bool ret = dmsBmStorage_->GetInstance()->GetBundleNameId(bundleName, bundleNameId);
        EXPECT_EQ(ret, false);
    }
    DTEST_LOG << "DistributedBmStorageTest GetBundleIdTest_001 end" << std::endl;
}

/**
 * @tc.name: GetBundleIdTest_002
 * @tc.desc: test delete DistributedBmStorage
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, GetBundleIdTest_002, TestSize.Level0)
{
    DTEST_LOG << "DistributedBmStorageTest GetBundleIdTest_002 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        const std::string bundleName = "browser";
        uint16_t bundleNameId = 1;
        bool ret = dmsBmStorage_->GetInstance()->GetBundleNameId(bundleName, bundleNameId);
        EXPECT_EQ(ret, false);
    }
    DTEST_LOG << "DistributedBmStorageTest GetBundleIdTest_002 end" << std::endl;
}

/**
 * @tc.name: UpdateDistributedDataTest_001
 * @tc.desc: test delete DistributedBmStorage
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, UpdateDistributedDataTest_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest UpdateDistributedDataTest_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        dmsBmStorage_->GetInstance()->UpdateDistributedData();
    }
    DTEST_LOG << "DistributedBmStorageTest UpdateDistributedDataTest_001 end" << std::endl;
}

/**
 * @tc.name: GetLastBundleNameId_001
 * @tc.desc: test GetLastBundleNameId
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, GetLastBundleNameId_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest GetLastBundleNameId_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    ASSERT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        uint16_t bundleNameId = 0;
        bool ret = dmsBmStorage_->GetInstance()->GetLastBundleNameId(bundleNameId);
        EXPECT_EQ(ret, true);
    }
    DTEST_LOG << "DistributedBmStorageTest GetLastBundleNameId_001 end" << std::endl;
}

/**
 * @tc.name: CreateBundleNameId_001
 * @tc.desc: test CreateBundleNameId
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, CreateBundleNameId_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest CreateBundleNameId_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    ASSERT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        std::string bundleName = "CreateBundleNameId_001";
        uint16_t ret = 0;
        ret = dmsBmStorage_->GetInstance()->CreateBundleNameId(bundleName);
        EXPECT_NE(ret, 0);
    }
    DTEST_LOG << "DistributedBmStorageTest CreateBundleNameId_001 end" << std::endl;
}

/**
 * @tc.name: RebuildLocalData_001
 * @tc.desc: test RebuildLocalData
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, RebuildLocalData_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest RebuildLocalData_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    ASSERT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        bool ret = dmsBmStorage_->GetInstance()->RebuildLocalData();
        EXPECT_EQ(ret, false);
    }
    DTEST_LOG << "DistributedBmStorageTest RebuildLocalData_001 end" << std::endl;
}

/**
 * @tc.name: ConvertToDistributedBundleInfo_001
 * @tc.desc: test ConvertToDistributedBundleInfo
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, ConvertToDistributedBundleInfo_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest ConvertToDistributedBundleInfo_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    ASSERT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        AppExecFwk::BundleInfo info;
        info.name = "ConvertToDistributedBundleInfo";
        auto dInfo = dmsBmStorage_->GetInstance()->ConvertToDistributedBundleInfo(info);
        EXPECT_EQ(dInfo.bundleName, info.name);
    }
    DTEST_LOG << "DistributedBmStorageTest ConvertToDistributedBundleInfo_001 end" << std::endl;
}

/**
 * @tc.name: CloudSyncTest_001
 * @tc.desc: test delete DistributedBmStorage
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, CloudSyncTest_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest CloudSyncTest_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        dmsBmStorage_->GetInstance()->CloudSync();
    }
    DTEST_LOG << "DistributedBmStorageTest CloudSyncTest_001 end" << std::endl;
}

/**
 * @tc.name: GetBundleMgrTest_001
 * @tc.desc: test delete DistributedBmStorage
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, GetBundleMgrTest_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest GetBundleMgrTest_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        auto bundleMgr = dmsBmStorage_->GetInstance()->GetBundleMgr();
        EXPECT_NE(bundleMgr, nullptr);
    }
    DTEST_LOG << "DistributedBmStorageTest GetBundleMgrTest_001 end" << std::endl;
}

/**
 * @tc.name: GetContinueTypeTest_001
 * @tc.desc: test delete DistributedBmStorage
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, GetContinueTypeTest_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest GetContinueTypeTest_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        std::string networkId;
        std::string bundleName;
        uint8_t continueTypeId = 0;
        std::string ret = dmsBmStorage_->GetInstance()->GetContinueType(networkId, bundleName, continueTypeId);
        EXPECT_EQ(ret, "");
    }
    DTEST_LOG << "DistributedBmStorageTest GetContinueTypeTest_001 end" << std::endl;
}

/**
 * @tc.name: GetAbilityNameTest_001
 * @tc.desc: test delete DistributedBmStorage
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, GetAbilityNameTest_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest GetAbilityNameTest_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        std::string networkId;
        std::string bundleName;
        std::string continueType;
        std::string ret = dmsBmStorage_->GetInstance()->GetAbilityName(networkId, bundleName, continueType);
        EXPECT_EQ(ret, "");
    }
    DTEST_LOG << "DistributedBmStorageTest GetAbilityNameTest_001 end" << std::endl;
}

/**
 * @tc.name: GetContinueTypeIdTest_001
 * @tc.desc: test delete DistributedBmStorage
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, GetContinueTypeIdTest_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest GetContinueTypeIdTest_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        std::string bundleName;
        std::string abilityName;
        uint8_t continueTypeId = 0;
        bool ret = dmsBmStorage_->GetInstance()->GetContinueTypeId(bundleName, abilityName, continueTypeId);
        EXPECT_EQ(ret, false);
    }
    DTEST_LOG << "DistributedBmStorageTest GetContinueTypeIdTest_001 end" << std::endl;
}

/**
 * @tc.name: GetContinueEventInfoTest_001
 * @tc.desc: test delete DistributedBmStorage
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, GetContinueEventInfoTest_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest GetContinueEventInfoTest_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        std::string networkId;
        std::string bundleName;
        std::string continueType;
        ContinueEventInfo continueEventInfo;
        bool ret = dmsBmStorage_->GetInstance()->GetContinueEventInfo(networkId, bundleName,
            continueType, continueEventInfo);
        EXPECT_EQ(ret, false);
    }
    DTEST_LOG << "DistributedBmStorageTest GetContinueEventInfoTest_001 end" << std::endl;
}

/**
 * @tc.name: DmsPutBatchTest_001
 * @tc.desc: test delete DistributedBmStorage
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, DmsPutBatchTest_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest DmsPutBatchTest_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        std::vector<DmsBundleInfo> dmsBundleInfos;
        dmsBmStorage_->GetInstance()->DmsPutBatch(dmsBundleInfos);
        DmsBundleInfo info;
        dmsBundleInfos.push_back(info);
        dmsBmStorage_->GetInstance()->DmsPutBatch(dmsBundleInfos);
    }
    DTEST_LOG << "DistributedBmStorageTest DmsPutBatchTest_001 end" << std::endl;
}

/**
 * @tc.name: FindContinueTypeTest_001
 * @tc.desc: test FindContinueType
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, FindContinueTypeTest_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest FindContinueTypeTest_001 start" << std::endl;
    ASSERT_NE(dmsBmStorage_, nullptr);
    auto distributedDataStorage = GetDmsBmStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        DmsBundleInfo info;
        std::string type = dmsBmStorage_->GetInstance()->FindContinueType(info, 0);
        EXPECT_EQ(type, "");
    }
    DTEST_LOG << "DistributedBmStorageTest FindContinueTypeTest_001 end" << std::endl;
}

/**
 * @tc.name: FindAbilityAndModuleNameTest_001
 * @tc.desc: test FindAbilityName and FindModuleName
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, FindAbilityAndModuleNameTest_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest FindAbilityAndModuleNameTest_001 start" << std::endl;
    DmsBundleInfo info;
    std::string result = FindAbilityName(info, "");
    EXPECT_EQ(result, "");

    result = FindModuleName(info, "");
    EXPECT_EQ(result, "");
    DTEST_LOG << "DistributedBmStorageTest FindAbilityAndModuleNameTest_001 end" << std::endl;
}

/**
 * @tc.name: FindContinueTypeIdTest_001
 * @tc.desc: test FindContinueTypeId
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, FindContinueTypeIdTest_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest FindContinueTypeIdTest_001 start" << std::endl;
    DmsBundleInfo info;
    uint8_t ret = FindContinueTypeId(info, "");
    EXPECT_EQ(ret, MAX_CONTINUETYPEID);
    DTEST_LOG << "DistributedBmStorageTest FindContinueTypeIdTest_001 end" << std::endl;
}

/**
 * @tc.name: UpdatePublicRecordsTest_001
 * @tc.desc: test delete DistributedBmStorage
 * @tc.type: FUNC
 */
HWTEST_F(DistributedBmStorageTest, UpdatePublicRecordsTest_001, TestSize.Level1)
{
    DTEST_LOG << "DistributedBmStorageTest UpdatePublicRecordsTest_001 start" << std::endl;
    auto distributedDataStorage = GetDmsBmStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        std::vector<DmsBundleInfo> dmsBundleInfos;
        dmsBmStorage_->GetInstance()->UpdatePublicRecords("");
    }
    DTEST_LOG << "DistributedBmStorageTest UpdatePublicRecordsTest_001 end" << std::endl;
}
} // namespace DistributedSchedule
} // namespace OHOS
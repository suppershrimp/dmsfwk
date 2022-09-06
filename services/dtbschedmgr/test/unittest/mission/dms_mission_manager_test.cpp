/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "dms_mission_manager_test.h"

#include "distributed_sched_proxy.h"
#include "distributed_sched_service.h"
#include "distributed_sched_util.h"
#include "dtbschedmgr_device_info_storage.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "mission/mission_constant.h"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "test_log.h"

#define private public
#include "mission/distributed_sched_mission_manager.h"

using namespace std;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedSchedule {
namespace {
const std::string DEVICE_NAME = "DEVICE_PHONE_001";
const std::string DEVICE_ID = "123456789ABCD";
const std::u16string U16DEVICE_ID = u"123456789ABCD";
const std::string BUNDLE_NAME = "ohos.test.test";
const int32_t NUM_MISSIONS = 100;
const int32_t NORMAL_NUM_MISSIONS = 10;
}
void DMSMissionManagerTest::SetUpTestCase()
{
}

void DMSMissionManagerTest::TearDownTestCase()
{
}

void DMSMissionManagerTest::SetUp()
{
    DistributedSchedUtil::MockPermission();
    string localDeviceId;
    if (!DtbschedmgrDeviceInfoStorage::GetInstance().GetLocalDeviceId(localDeviceId)) {
        DTEST_LOG << "getLocalDevicesId failed!" << std::endl;
        return;
    }
    localDeviceId_ = localDeviceId;
    u16localDeviceId_ = Str8ToStr16(localDeviceId);
}

void DMSMissionManagerTest::TearDown()
{
}

sptr<IDistributedSched> DMSMissionManagerTest::GetDms()
{
    if (proxy_ != nullptr) {
        return proxy_;
    }
    auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_TRUE(sm != nullptr);
    if (sm == nullptr) {
        DTEST_LOG << "DMSMissionManagerTest sm is nullptr" << std::endl;
        return nullptr;
    }
    DTEST_LOG << "DMSMissionManagerTest sm is not nullptr" << std::endl;
    auto distributedObject = sm->GetSystemAbility(DISTRIBUTED_SCHED_SA_ID);
    EXPECT_TRUE(distributedObject != nullptr);
    proxy_ = iface_cast<IDistributedSched>(distributedObject);
    if (proxy_ == nullptr) {
        DTEST_LOG << "DMSMissionManagerTest DistributedSched is nullptr" << std::endl;
    }
    DTEST_LOG << "DMSMissionManagerTest DistributedSched is not nullptr" << std::endl;
    return proxy_;
}

void RemoteMissionListenerTest::NotifyMissionsChanged(const std::string& deviceId)
{
    DTEST_LOG << "NotifyMissionsChanged" << std::endl;
}

void RemoteMissionListenerTest::NotifySnapshot(const std::string& deviceId, int32_t missionId)
{
    DTEST_LOG << "NotifySnapshot" << std::endl;
}

void RemoteMissionListenerTest::NotifyNetDisconnect(const std::string& deviceId, int32_t state)
{
    DTEST_LOG << "NotifyNetDisconnect" << std::endl;
}

/**
 * @tc.name: testGetRemoteMissionInfo001
 * @tc.desc: get remote mission info
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testGetRemoteMissionInfo001, TestSize.Level1)
{
    std::vector<AAFwk::MissionInfo> infos;
    auto ret = DistributedSchedMissionManager::GetInstance().GetMissionInfos(DEVICE_ID, 0, infos);
    EXPECT_TRUE(ret != ERR_NONE);

    ret = DistributedSchedMissionManager::GetInstance().GetMissionInfos(localDeviceId_, 0, infos);
    EXPECT_TRUE(ret != ERR_NONE);

    ret = DistributedSchedMissionManager::GetInstance().GetMissionInfos("", 0, infos);
    EXPECT_TRUE(ret != ERR_NONE);

    ret = DistributedSchedMissionManager::GetInstance().GetMissionInfos(DEVICE_ID, -1, infos);
    EXPECT_TRUE(ret != ERR_NONE);

    ret = DistributedSchedMissionManager::GetInstance().GetMissionInfos(localDeviceId_, -1, infos);
    EXPECT_TRUE(ret != ERR_NONE);
}

/**
 * @tc.name: testGetRemoteMissionInfo002
 * @tc.desc: get remote mission info
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testGetRemoteMissionInfo002, TestSize.Level1)
{
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    std::vector<AAFwk::MissionInfo> infos;
    auto ret = proxy->GetMissionInfos(DEVICE_ID, 0, infos);
    EXPECT_TRUE(ret != ERR_NONE);

    ret = proxy->GetMissionInfos(localDeviceId_, 0, infos);
    EXPECT_TRUE(ret != ERR_NONE);

    ret = proxy->GetMissionInfos("", 0, infos);
    EXPECT_TRUE(ret != ERR_NONE);

    ret = proxy->GetMissionInfos(DEVICE_ID, -1, infos);
    EXPECT_TRUE(ret != ERR_NONE);

    ret = proxy->GetMissionInfos(localDeviceId_, -1, infos);
    EXPECT_TRUE(ret != ERR_NONE);
}

/**
 * @tc.name: testStartSyncRemoteMissions001
 * @tc.desc: prepare and sync missions from remote
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStartSyncRemoteMissions001, TestSize.Level1)
{
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    auto ret = proxy->StartSyncRemoteMissions(DEVICE_ID, false, 0);
    EXPECT_TRUE(ret != ERR_NONE);
}

/**
 * @tc.name: testStartSyncRemoteMissions002
 * @tc.desc: prepare and sync missions from remote
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStartSyncRemoteMissions002, TestSize.Level1)
{
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    auto ret = proxy->StartSyncRemoteMissions("", false, 0);
    EXPECT_TRUE(ret != ERR_NONE);
}

/**
 * @tc.name: testStartSyncRemoteMissions003
 * @tc.desc: prepare and sync missions from remote
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStartSyncRemoteMissions003, TestSize.Level1)
{
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    auto ret = proxy->StartSyncRemoteMissions(localDeviceId_, false, 0);
    EXPECT_TRUE(ret != ERR_NONE);
}

/**
 * @tc.name: testStartSyncRemoteMissions004
 * @tc.desc: prepare and sync missions from remote
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStartSyncRemoteMissions004, TestSize.Level3)
{
    DTEST_LOG << "testStartSyncRemoteMissions004 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().StartSyncRemoteMissions(DEVICE_ID, false, 0);
    EXPECT_TRUE(ret == ERR_NONE);
    DTEST_LOG << "testStartSyncRemoteMissions004 end" << std::endl;
}

/**
 * @tc.name: testStartSyncRemoteMissions005
 * @tc.desc: prepare and sync missions from remote
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStartSyncRemoteMissions005, TestSize.Level3)
{
    DTEST_LOG << "testStartSyncRemoteMissions005 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().StartSyncRemoteMissions("", false, 0);
    EXPECT_TRUE(ret == ERR_NONE);
    DTEST_LOG << "testStartSyncRemoteMissions005 end" << std::endl;
}

/**
 * @tc.name: testStartSyncRemoteMissions006
 * @tc.desc: prepare and sync missions from remote
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStartSyncRemoteMissions006, TestSize.Level3)
{
    DTEST_LOG << "testStartSyncRemoteMissions006 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().StartSyncRemoteMissions(localDeviceId_, false, 0);
    EXPECT_TRUE(ret != ERR_NONE);
    DTEST_LOG << "testStartSyncRemoteMissions006 end" << std::endl;
}

/**
 * @tc.name: testStartSyncRemoteMissions007
 * @tc.desc: prepare and sync missions from remote
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStartSyncRemoteMissions007, TestSize.Level3)
{
    DTEST_LOG << "testStartSyncRemoteMissions007 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().StartSyncRemoteMissions(DEVICE_ID, true, 0);
    EXPECT_TRUE(ret == ERR_NONE);
    DTEST_LOG << "testStartSyncRemoteMissions007 end" << std::endl;
}

/**
 * @tc.name: testStartSyncRemoteMissions008
 * @tc.desc: prepare and sync missions from remote
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStartSyncRemoteMissions008, TestSize.Level3)
{
    DTEST_LOG << "testStartSyncRemoteMissions008 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().StartSyncRemoteMissions("", true, 0);
    EXPECT_TRUE(ret == ERR_NONE);
    DTEST_LOG << "testStartSyncRemoteMissions008 end" << std::endl;
}

/**
 * @tc.name: testStartSyncRemoteMissions009
 * @tc.desc: prepare and sync missions from remote
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStartSyncRemoteMissions009, TestSize.Level3)
{
    DTEST_LOG << "testStartSyncRemoteMissions009 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().StartSyncRemoteMissions(localDeviceId_, true, 0);
    EXPECT_TRUE(ret != ERR_NONE);
    DTEST_LOG << "testStartSyncRemoteMissions009 end" << std::endl;
}

/**
 * @tc.name: testStartSyncRemoteMissions010
 * @tc.desc: prepare and sync missions from remote
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStartSyncRemoteMissions010, TestSize.Level3)
{
    DTEST_LOG << "testStartSyncRemoteMissions010 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().StartSyncRemoteMissions(DEVICE_ID, true, 1);
    EXPECT_TRUE(ret == ERR_NONE);
    DTEST_LOG << "testStartSyncRemoteMissions010 end" << std::endl;
}

/**
 * @tc.name: testStartSyncRemoteMissions011
 * @tc.desc: prepare and sync missions from remote
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStartSyncRemoteMissions011, TestSize.Level3)
{
    DTEST_LOG << "testStartSyncRemoteMissions011 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().StartSyncRemoteMissions("", true, 1);
    EXPECT_TRUE(ret == ERR_NONE);
    DTEST_LOG << "testStartSyncRemoteMissions011 end" << std::endl;
}

/**
 * @tc.name: testStartSyncRemoteMissions012
 * @tc.desc: prepare and sync missions from remote
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStartSyncRemoteMissions012, TestSize.Level3)
{
    DTEST_LOG << "testStartSyncRemoteMissions012 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().StartSyncRemoteMissions(localDeviceId_, true, 1);
    EXPECT_TRUE(ret != ERR_NONE);
    DTEST_LOG << "testStartSyncRemoteMissions012 end" << std::endl;
}

/**
 * @tc.name: testStartSyncRemoteMissions013
 * @tc.desc: prepare and sync missions from remote
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStartSyncRemoteMissions013, TestSize.Level3)
{
    DTEST_LOG << "testStartSyncRemoteMissions013 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().StartSyncRemoteMissions(DEVICE_ID, localDeviceId_);
    EXPECT_TRUE(ret == ERR_NONE);
    DTEST_LOG << "testStartSyncRemoteMissions013 end" << std::endl;
}

/**
 * @tc.name: testStartSyncRemoteMissions014
 * @tc.desc: prepare and sync missions from remote
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStartSyncRemoteMissions014, TestSize.Level3)
{
    DTEST_LOG << "testStartSyncRemoteMissions014 begin" << std::endl;
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    auto ret = DistributedSchedMissionManager::GetInstance().StartSyncRemoteMissions(DEVICE_ID, proxy);
    EXPECT_TRUE(ret != ERR_NONE);
    DTEST_LOG << "testStartSyncRemoteMissions014 end" << std::endl;
}

/**
 * @tc.name: testNeedSyncDevice001
 * @tc.desc: need sync device
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testNeedSyncDevice001, TestSize.Level3)
{
    DTEST_LOG << "testCleanMissionResources001 begin" << std::endl;
    bool ret = DistributedSchedMissionManager::GetInstance().needSyncDevice(DEVICE_ID);
    EXPECT_EQ(false, ret);
    DTEST_LOG << "testNeedSyncDevice001 end" << std::endl;
}

/**
 * @tc.name: testHasSyncListener001
 * @tc.desc: need sync device
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testHasSyncListener001, TestSize.Level3)
{
    DTEST_LOG << "testHasSyncListener001 begin" << std::endl;
    bool ret = DistributedSchedMissionManager::GetInstance().HasSyncListener(DEVICE_ID);
    EXPECT_EQ(false, ret);
    DTEST_LOG << "testHasSyncListener001 end" << std::endl;
}

/**
 * @tc.name: testCleanMissionResources001
 * @tc.desc: prepare and sync missions from remote
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testCleanMissionResources001, TestSize.Level3)
{
    DTEST_LOG << "testCleanMissionResources001 begin" << std::endl;
    DistributedSchedMissionManager::GetInstance().CleanMissionResources(DEVICE_ID);
    DTEST_LOG << "testCleanMissionResources001 end" << std::endl;
}

/**
 * @tc.name: testStopSyncRemoteMissions001
 * @tc.desc: stop sync missions from remote with fake deviceId
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStopSyncRemoteMissions001, TestSize.Level1)
{
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    auto ret = proxy->StopSyncRemoteMissions(DEVICE_ID);
    EXPECT_TRUE(ret != ERR_NONE);
}

/**
 * @tc.name: testStopSyncRemoteMissions002
 * @tc.desc: stop sync missions from remote with local deviceId
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStopSyncRemoteMissions002, TestSize.Level1)
{
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    auto ret = proxy->StopSyncRemoteMissions(localDeviceId_);
    EXPECT_TRUE(ret != ERR_NONE);
}

/**
 * @tc.name: testStopSyncRemoteMissions003
 * @tc.desc: stop sync missions from remote with empty deviceId
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStopSyncRemoteMissions003, TestSize.Level1)
{
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    auto ret = proxy->StopSyncRemoteMissions("");
    EXPECT_TRUE(ret != ERR_NONE);
}

/**
 * @tc.name: testStopSyncRemoteMissions004
 * @tc.desc: stop sync missions from remote with empty deviceId
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStopSyncRemoteMissions004, TestSize.Level3)
{
    DTEST_LOG << "testStopSyncRemoteMissions004 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().StopSyncRemoteMissions("", false, true);
    EXPECT_TRUE(ret != ERR_NONE);
    DTEST_LOG << "testStopSyncRemoteMissions004 end" << std::endl;
}

/**
 * @tc.name: testStopSyncRemoteMissions005
 * @tc.desc: stop sync missions from remote with local deviceId
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStopSyncRemoteMissions005, TestSize.Level3)
{
    DTEST_LOG << "testStopSyncRemoteMissions005 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().StopSyncRemoteMissions(localDeviceId_, false, true);
    EXPECT_TRUE(ret != ERR_NONE);
    DTEST_LOG << "testStopSyncRemoteMissions005 end" << std::endl;
}

/**
 * @tc.name: testStopSyncRemoteMissions006
 * @tc.desc: stop sync missions from remote with deviceId
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStopSyncRemoteMissions006, TestSize.Level3)
{
    DTEST_LOG << "testStopSyncRemoteMissions006 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().StopSyncRemoteMissions(DEVICE_ID, false, true);
    EXPECT_TRUE(ret != ERR_NONE);
    DTEST_LOG << "testStopSyncRemoteMissions006 end" << std::endl;
}

/**
 * @tc.name: testStopSyncRemoteMissions007
 * @tc.desc: stop sync missions from remote with empty deviceId
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStopSyncRemoteMissions007, TestSize.Level3)
{
    DTEST_LOG << "testStopSyncRemoteMissions007 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().StopSyncRemoteMissions("", true, true);
    EXPECT_TRUE(ret == ERR_NONE);
    DTEST_LOG << "testStopSyncRemoteMissions007 end" << std::endl;
}

/**
 * @tc.name: testStopSyncRemoteMissions008
 * @tc.desc: stop sync missions from remote with local deviceId
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStopSyncRemoteMissions008, TestSize.Level3)
{
    DTEST_LOG << "testStopSyncRemoteMissions008 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().StopSyncRemoteMissions(localDeviceId_, true, true);
    EXPECT_TRUE(ret == ERR_NONE);
    DTEST_LOG << "testStopSyncRemoteMissions008 end" << std::endl;
}

/**
 * @tc.name: testStopSyncRemoteMissions009
 * @tc.desc: stop sync missions from remote with deviceId
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStopSyncRemoteMissions009, TestSize.Level3)
{
    DTEST_LOG << "testStopSyncRemoteMissions009 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().StopSyncRemoteMissions(DEVICE_ID, true, true);
    EXPECT_TRUE(ret == ERR_NONE);
    DTEST_LOG << "testStopSyncRemoteMissions009 end" << std::endl;
}

/**
 * @tc.name: testStopSyncRemoteMissions010
 * @tc.desc: stop sync missions from remote with empty deviceId
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStopSyncRemoteMissions010, TestSize.Level3)
{
    DTEST_LOG << "testStopSyncRemoteMissions010 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().StopSyncRemoteMissions("", true, false);
    EXPECT_TRUE(ret == ERR_NONE);
    DTEST_LOG << "testStopSyncRemoteMissions010 end" << std::endl;
}

/**
 * @tc.name: testStopSyncRemoteMissions011
 * @tc.desc: stop sync missions from remote with local deviceId
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStopSyncRemoteMissions011, TestSize.Level3)
{
    DTEST_LOG << "testStopSyncRemoteMissions011 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().StopSyncRemoteMissions(localDeviceId_, true, false);
    EXPECT_TRUE(ret == ERR_NONE);
    DTEST_LOG << "testStopSyncRemoteMissions011 end" << std::endl;
}

/**
 * @tc.name: testStopSyncRemoteMissions012
 * @tc.desc: stop sync missions from remote with deviceId
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStopSyncRemoteMissions012, TestSize.Level3)
{
    DTEST_LOG << "testStopSyncRemoteMissions012 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().StopSyncRemoteMissions(DEVICE_ID, true, false);
    EXPECT_TRUE(ret == ERR_NONE);
    DTEST_LOG << "testStopSyncRemoteMissions012 end" << std::endl;
}

/**
 * @tc.name: testRegisterMissionListener001
 * @tc.desc: register mission listener
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testRegisterMissionListener001, TestSize.Level1)
{
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    auto ret = proxy->RegisterMissionListener(U16DEVICE_ID, nullptr);
    EXPECT_TRUE(ret != ERR_NONE);

    ret = proxy->RegisterMissionListener(u16localDeviceId_, nullptr);
    EXPECT_TRUE(ret != ERR_NONE);

    ret = proxy->RegisterMissionListener(u"", nullptr);
    EXPECT_TRUE(ret != ERR_NONE);
}

/**
 * @tc.name: testRegisterMissionListener002
 * @tc.desc: register mission listener
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testRegisterMissionListener002, TestSize.Level1)
{
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    sptr<IRemoteObject> listener = new RemoteMissionListenerTest();
    auto ret = proxy->RegisterMissionListener(U16DEVICE_ID, listener);
    EXPECT_TRUE(ret != ERR_NONE);

    ret = proxy->RegisterMissionListener(u16localDeviceId_, listener);
    EXPECT_TRUE(ret != ERR_NONE);

    ret = proxy->RegisterMissionListener(u"", listener);
    EXPECT_TRUE(ret != ERR_NONE);

    ret = proxy->UnRegisterMissionListener(U16DEVICE_ID, listener);
    EXPECT_TRUE(ret != ERR_NONE);

    ret = proxy->UnRegisterMissionListener(u16localDeviceId_, listener);
    EXPECT_TRUE(ret != ERR_NONE);

    ret = proxy->UnRegisterMissionListener(u"", listener);
    EXPECT_TRUE(ret != ERR_NONE);
}

/**
 * @tc.name: testRegisterMissionListener003
 * @tc.desc: test register mission listener
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testRegisterMissionListener003, TestSize.Level3)
{
    DTEST_LOG << "testRegisterMissionListener003 begin" << std::endl;
    sptr<IRemoteObject> listener = new RemoteMissionListenerTest();
    auto ret = DistributedSchedMissionManager::GetInstance().RegisterMissionListener(u"", listener);
    EXPECT_TRUE(ret == ERR_NONE);
    DTEST_LOG << "testRegisterMissionListener003 end" << std::endl;
}

/**
 * @tc.name: testRegisterMissionListener004
 * @tc.desc: test register mission listener
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testRegisterMissionListener004, TestSize.Level3)
{
    DTEST_LOG << "testRegisterMissionListener004 begin" << std::endl;
    sptr<IRemoteObject> listener = new RemoteMissionListenerTest();
    auto ret = DistributedSchedMissionManager::GetInstance().RegisterMissionListener(U16DEVICE_ID, listener);
    EXPECT_TRUE(ret == ERR_NONE);
    DTEST_LOG << "testRegisterMissionListener004 end" << std::endl;
}

/**
 * @tc.name: testUnRegisterMissionListener001
 * @tc.desc: register mission listener
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testUnRegisterMissionListener001, TestSize.Level1)
{
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    auto ret = proxy->UnRegisterMissionListener(U16DEVICE_ID, nullptr);
    EXPECT_TRUE(ret != ERR_NONE);

    ret = proxy->UnRegisterMissionListener(u16localDeviceId_, nullptr);
    EXPECT_TRUE(ret != ERR_NONE);

    ret = proxy->UnRegisterMissionListener(u"", nullptr);
    EXPECT_TRUE(ret != ERR_NONE);
}

/**
 * @tc.name: testUnRegisterMissionListener002
 * @tc.desc: register mission listener
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testUnRegisterMissionListener002, TestSize.Level3)
{
    DTEST_LOG << "testUnRegisterMissionListener002 begin" << std::endl;
    sptr<IRemoteObject> listener = new RemoteMissionListenerTest();
    auto ret = DistributedSchedMissionManager::GetInstance().UnRegisterMissionListener(U16DEVICE_ID, listener);
    EXPECT_TRUE(ret == ERR_NONE);
    DTEST_LOG << "testUnRegisterMissionListener002 end" << std::endl;
}

/**
 * @tc.name: testUnRegisterMissionListener003
 * @tc.desc: register mission listener
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testUnRegisterMissionListener003, TestSize.Level3)
{
    DTEST_LOG << "testUnRegisterMissionListener003 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().UnRegisterMissionListener(U16DEVICE_ID, nullptr);
    EXPECT_TRUE(ret == INVALID_PARAMETERS_ERR);
    DTEST_LOG << "testUnRegisterMissionListener003 end" << std::endl;
}

/**
 * @tc.name: testIsDeviceIdValidated001
 * @tc.desc: judging whether the deviceId is valid
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testIsDeviceIdValidated001, TestSize.Level1)
{
    std::string emptyDeviceId = "";
    auto ret = DistributedSchedMissionManager::GetInstance().IsDeviceIdValidated(emptyDeviceId);
    EXPECT_FALSE(ret);

    ret = DistributedSchedMissionManager::GetInstance().IsDeviceIdValidated(DEVICE_ID);
    EXPECT_FALSE(ret);

    ret = DistributedSchedMissionManager::GetInstance().IsDeviceIdValidated(localDeviceId_);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: testGetMissionInfos001
 * @tc.desc: get missions
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testGetMissionInfos001, TestSize.Level1)
{
    std::string deviceid = DEVICE_ID;
    int32_t numMissions = NUM_MISSIONS;
    std::vector<AAFwk::MissionInfo> missionInfos;

    auto ret = DistributedSchedMissionManager::GetInstance().GetMissionInfos(deviceid, numMissions, missionInfos);
    EXPECT_TRUE(ret != ERR_NONE);
}

/**
 * @tc.name: testGetMissionInfos002
 * @tc.desc: get missions
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testGetMissionInfos002, TestSize.Level3)
{
    DTEST_LOG << "testGetMissionInfos002 begin" << std::endl;
    std::string deviceid = localDeviceId_;
    int32_t numMissions = NUM_MISSIONS;
    std::vector<AAFwk::MissionInfo> missionInfos;

    auto ret = DistributedSchedMissionManager::GetInstance().GetMissionInfos(deviceid, numMissions, missionInfos);
    EXPECT_TRUE(ret != ERR_NONE);
    DTEST_LOG << "testGetMissionInfos002 end" << std::endl;
}

/**
 * @tc.name: testGetMissionInfos003
 * @tc.desc: get missions
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testGetMissionInfos003, TestSize.Level3)
{
    DTEST_LOG << "testGetMissionInfos003 begin" << std::endl;
    std::string deviceid = "";
    int32_t numMissions = NUM_MISSIONS;
    std::vector<AAFwk::MissionInfo> missionInfos;

    auto ret = DistributedSchedMissionManager::GetInstance().GetMissionInfos(deviceid, numMissions, missionInfos);
    EXPECT_TRUE(ret != ERR_NONE);
    DTEST_LOG << "testGetMissionInfos003 end" << std::endl;
}

/**
 * @tc.name: testGetMissionInfos004
 * @tc.desc: get missions
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testGetMissionInfos004, TestSize.Level3)
{
    DTEST_LOG << "testGetMissionInfos004 begin" << std::endl;
    std::string deviceid = DEVICE_ID;
    int32_t numMissions = -1;
    std::vector<AAFwk::MissionInfo> missionInfos;
    auto ret = DistributedSchedMissionManager::GetInstance().GetMissionInfos(deviceid, numMissions, missionInfos);
    EXPECT_TRUE(ret == INVALID_PARAMETERS_ERR);
    DTEST_LOG << "testGetMissionInfos004 end" << std::endl;
}

/**
 * @tc.name: testGetMissionInfos005
 * @tc.desc: get missions
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testGetMissionInfos005, TestSize.Level3)
{
    DTEST_LOG << "testGetMissionInfos005 begin" << std::endl;
    std::string deviceid = "";
    int32_t numMissions = NUM_MISSIONS;
    std::vector<AAFwk::MissionInfo> missionInfos;
    auto ret = DistributedSchedMissionManager::GetInstance().GetMissionInfos(deviceid, numMissions, missionInfos);
    EXPECT_TRUE(ret == INVALID_PARAMETERS_ERR);
    DTEST_LOG << "testGetMissionInfos005 end" << std::endl;
}

/**
 * @tc.name: testGetMissionInfos006
 * @tc.desc: get missions
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testGetMissionInfos006, TestSize.Level3)
{
    DTEST_LOG << "testGetMissionInfos006 begin" << std::endl;
    std::string deviceid = "";
    int32_t numMissions = 0;
    std::vector<AAFwk::MissionInfo> missionInfos;
    auto ret = DistributedSchedMissionManager::GetInstance().GetMissionInfos(deviceid, numMissions, missionInfos);
    EXPECT_TRUE(ret == INVALID_PARAMETERS_ERR);
    DTEST_LOG << "testGetMissionInfos006 end" << std::endl;
}

/**
 * @tc.name: testFetchCachedRemoteMissions001
 * @tc.desc: test stop fake device, return error code
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testFetchCachedRemoteMissions001, TestSize.Level1)
{
    std::vector<DstbMissionInfo> infos;
    auto ret = DistributedSchedMissionManager::GetInstance().FetchCachedRemoteMissions(DEVICE_ID, 0, infos);
    EXPECT_TRUE(ret != ERR_NONE);
}

/**
 * @tc.name: testFetchCachedRemoteMissions002
 * @tc.desc: test empty deviceId, return error code
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testFetchCachedRemoteMissions002, TestSize.Level3)
{
    DTEST_LOG << "testFetchCachedRemoteMissions002 begin" << std::endl;
    std::vector<DstbMissionInfo> infos;
    auto ret = DistributedSchedMissionManager::GetInstance().FetchCachedRemoteMissions("", 0, infos);
    EXPECT_TRUE(ret == INVALID_PARAMETERS_ERR);
    DTEST_LOG << "testFetchCachedRemoteMissions002 end" << std::endl;
}

/**
 * @tc.name: testFetchCachedRemoteMissions003
 * @tc.desc: test local deviceId, return error code
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testFetchCachedRemoteMissions003, TestSize.Level3)
{
    DTEST_LOG << "testFetchCachedRemoteMissions003 begin" << std::endl;
    std::vector<DstbMissionInfo> infos;
    auto ret = DistributedSchedMissionManager::GetInstance().FetchCachedRemoteMissions(localDeviceId_, 0, infos);
    EXPECT_TRUE(ret == ERR_NULL_OBJECT);
    DTEST_LOG << "testFetchCachedRemoteMissions003 end" << std::endl;
}

/**
 * @tc.name: testFetchCachedRemoteMissions004
 * @tc.desc: test stop fake device, return error code
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testFetchCachedRemoteMissions004, TestSize.Level3)
{
    DTEST_LOG << "testFetchCachedRemoteMissions004 begin" << std::endl;
    std::vector<DstbMissionInfo> infos;
    auto ret = DistributedSchedMissionManager::GetInstance().FetchCachedRemoteMissions(DEVICE_ID, NUM_MISSIONS, infos);
    EXPECT_TRUE(ret != ERR_NONE);
    DTEST_LOG << "testFetchCachedRemoteMissions004 end" << std::endl;
}

/**
 * @tc.name: testFetchCachedRemoteMissions005
 * @tc.desc: test empty deviceId, return error code
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testFetchCachedRemoteMissions005, TestSize.Level3)
{
    DTEST_LOG << "testFetchCachedRemoteMissions005 begin" << std::endl;
    std::vector<DstbMissionInfo> infos;
    auto ret = DistributedSchedMissionManager::GetInstance().FetchCachedRemoteMissions("", NUM_MISSIONS, infos);
    EXPECT_TRUE(ret == INVALID_PARAMETERS_ERR);
    DTEST_LOG << "testFetchCachedRemoteMissions005 end" << std::endl;
}

/**
 * @tc.name: testFetchCachedRemoteMissions006
 * @tc.desc: test local deviceId, return error code
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testFetchCachedRemoteMissions006, TestSize.Level3)
{
    DTEST_LOG << "testFetchCachedRemoteMissions006 begin" << std::endl;
    std::vector<DstbMissionInfo> infos;
    auto ret = DistributedSchedMissionManager::GetInstance().FetchCachedRemoteMissions(localDeviceId_,
        NUM_MISSIONS, infos);
    EXPECT_TRUE(ret == ERR_NULL_OBJECT);
    DTEST_LOG << "testFetchCachedRemoteMissions006 end" << std::endl;
}

/**
 * @tc.name: testFetchCachedRemoteMissions007
 * @tc.desc: test stop fake device, return error code
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testFetchCachedRemoteMissions007, TestSize.Level3)
{
    DTEST_LOG << "testFetchCachedRemoteMissions007 begin" << std::endl;
    std::vector<DstbMissionInfo> infos;
    auto ret = DistributedSchedMissionManager::GetInstance().FetchCachedRemoteMissions(DEVICE_ID,
        NORMAL_NUM_MISSIONS, infos);
    EXPECT_TRUE(ret != ERR_NONE);
    DTEST_LOG << "testFetchCachedRemoteMissions007 end" << std::endl;
}

/**
 * @tc.name: testFetchCachedRemoteMissions008
 * @tc.desc: test empty deviceId, return error code
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testFetchCachedRemoteMissions008, TestSize.Level3)
{
    DTEST_LOG << "testFetchCachedRemoteMissions008 begin" << std::endl;
    std::vector<DstbMissionInfo> infos;
    auto ret = DistributedSchedMissionManager::GetInstance().FetchCachedRemoteMissions("", NORMAL_NUM_MISSIONS, infos);
    EXPECT_TRUE(ret == INVALID_PARAMETERS_ERR);
    DTEST_LOG << "testFetchCachedRemoteMissions008 end" << std::endl;
}

/**
 * @tc.name: testFetchCachedRemoteMissions009
 * @tc.desc: test local deviceId, return error code
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testFetchCachedRemoteMissions009, TestSize.Level3)
{
    DTEST_LOG << "testFetchCachedRemoteMissions009 begin" << std::endl;
    std::vector<DstbMissionInfo> infos;
    auto ret = DistributedSchedMissionManager::GetInstance().FetchCachedRemoteMissions(localDeviceId_,
        NORMAL_NUM_MISSIONS, infos);
    EXPECT_TRUE(ret == ERR_NULL_OBJECT);
    DTEST_LOG << "testFetchCachedRemoteMissions009 end" << std::endl;
}

/**
 * @tc.name: testNotifyRemoteDied001
 * @tc.desc: test notify remote died
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testNotifyRemoteDied001, TestSize.Level3)
{
    DTEST_LOG << "testNotifyRemoteDied001 begin" << std::endl;
    wptr<IRemoteObject> remote = nullptr;
    DistributedSchedMissionManager::GetInstance().NotifyRemoteDied(remote);
    DTEST_LOG << "testNotifyRemoteDied001 end" << std::endl;
}

/**
 * @tc.name: testInitDataStorage001
 * @tc.desc: test init data storage
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testInitDataStorage001, TestSize.Level3)
{
    DTEST_LOG << "testInitDataStorage001 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().InitDataStorage();
    EXPECT_TRUE(ret == ERR_NONE);
    DTEST_LOG << "testInitDataStorage001 end" << std::endl;
}

/**
 * @tc.name: testStopDataStorage002
 * @tc.desc: test stop data storage
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStopDataStorage002, TestSize.Level3)
{
    DTEST_LOG << "testStopDataStorage002 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().StopDataStorage();
    EXPECT_TRUE(ret == ERR_NONE);
    DTEST_LOG << "testStopDataStorage002 end" << std::endl;
}

/**
 * @tc.name: testStoreSnapshotInfo002
 * @tc.desc: test store snapshot info
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testStoreSnapshotInfo002, TestSize.Level3)
{
    DTEST_LOG << "testStoreSnapshotInfo002 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().StoreSnapshotInfo(DEVICE_ID, 0, 0, 0);
    EXPECT_TRUE(ret == INVALID_PARAMETERS_ERR);
    DTEST_LOG << "testStoreSnapshotInfo002 end" << std::endl;
}

/**
 * @tc.name: testRemoveSnapshotInfo002
 * @tc.desc: test remove snapshot info
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testRemoveSnapshotInfo002, TestSize.Level3)
{
    DTEST_LOG << "testRemoveSnapshotInfo002 begin" << std::endl;
    auto ret = DistributedSchedMissionManager::GetInstance().RemoveSnapshotInfo(DEVICE_ID, 0);
    EXPECT_TRUE(ret == INVALID_PARAMETERS_ERR);
    DTEST_LOG << "testRemoveSnapshotInfo002 end" << std::endl;
}

/**
 * @tc.name: testGetRemoteMissionSnapshotInfo001
 * @tc.desc: test get remote mission snapshot info
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testGetRemoteMissionSnapshotInfo001, TestSize.Level3)
{
    DTEST_LOG << "testGetRemoteMissionSnapshotInfo001 begin" << std::endl;
    unique_ptr<AAFwk::MissionSnapshot> missionSnapshot = nullptr;
    auto ret = DistributedSchedMissionManager::GetInstance().GetRemoteMissionSnapshotInfo("", 0, missionSnapshot);
    EXPECT_TRUE(ret == INVALID_PARAMETERS_ERR);
    DTEST_LOG << "testGetRemoteMissionSnapshotInfo001 end" << std::endl;
}

/**
 * @tc.name: testDeviceOnlineNotify001
 * @tc.desc: test device online notify
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testDeviceOnlineNotify001, TestSize.Level3)
{
    DTEST_LOG << "testDeviceOnlineNotify001 begin" << std::endl;
    DistributedSchedMissionManager::GetInstance().DeviceOnlineNotify("");
    DTEST_LOG << "testDeviceOnlineNotify001 end" << std::endl;
}

/**
 * @tc.name: testDeviceOnlineNotify002
 * @tc.desc: test device online notify
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testDeviceOnlineNotify002, TestSize.Level3)
{
    DTEST_LOG << "testDeviceOnlineNotify002 begin" << std::endl;
    DistributedSchedMissionManager::GetInstance().DeviceOnlineNotify(DEVICE_ID);
    DTEST_LOG << "testDeviceOnlineNotify002 end" << std::endl;
}

/**
 * @tc.name: testDeviceOfflineNotify001
 * @tc.desc: test device offline notify
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testDeviceOfflineNotify001, TestSize.Level3)
{
    DTEST_LOG << "testDeviceOfflineNotify001 begin" << std::endl;
    DistributedSchedMissionManager::GetInstance().DeviceOfflineNotify("");
    DTEST_LOG << "testDeviceOfflineNotify001 end" << std::endl;
}

/**
 * @tc.name: testDeleteDataStorage001
 * @tc.desc: test delete data storage
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testDeleteDataStorage001, TestSize.Level3)
{
    DTEST_LOG << "testDeleteDataStorage001 begin" << std::endl;
    DistributedSchedMissionManager::GetInstance().DeleteDataStorage("", false);
    DTEST_LOG << "testDeleteDataStorage001 end" << std::endl;
}

/**
 * @tc.name: testDeleteDataStorage002
 * @tc.desc: test delete data storage
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testDeleteDataStorage002, TestSize.Level3)
{
    DTEST_LOG << "testDeleteDataStorage002 begin" << std::endl;
    DistributedSchedMissionManager::GetInstance().DeleteDataStorage(DEVICE_ID, false);
    DTEST_LOG << "testDeleteDataStorage002 end" << std::endl;
}

/**
 * @tc.name: testNotifySnapshotChanged001
 * @tc.desc: test notify snapshot changed
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testNotifySnapshotChanged001, TestSize.Level3)
{
    DTEST_LOG << "testNotifySnapshotChanged001 begin" << std::endl;
    DistributedSchedMissionManager::GetInstance().NotifySnapshotChanged(DEVICE_ID, 0);
    DTEST_LOG << "testNotifySnapshotChanged001 end" << std::endl;
}

/**
 * @tc.name: testOnRemoteDied001
 * @tc.desc: test on remote died
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testOnRemoteDied001, TestSize.Level3)
{
    DTEST_LOG << "testOnRemoteDied001 begin" << std::endl;
    wptr<IRemoteObject> remote = nullptr;
    DistributedSchedMissionManager::GetInstance().OnRemoteDied(remote);
    DTEST_LOG << "testOnRemoteDied001 end" << std::endl;
}

/**
 * @tc.name: testOnRemoteDied002
 * @tc.desc: test on remote died
 * @tc.type: FUNC
 */
HWTEST_F(DMSMissionManagerTest, testOnRemoteDied002, TestSize.Level3)
{
    DTEST_LOG << "testOnRemoteDied002 begin" << std::endl;
    wptr<IRemoteObject> remote = new RemoteMissionListenerTest();
    DistributedSchedMissionManager::GetInstance().OnRemoteDied(remote);
    DTEST_LOG << "testOnRemoteDied002 end" << std::endl;
}
} // namespace DistributedSchedule
} // namespace OHOS
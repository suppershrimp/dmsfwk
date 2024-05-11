/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "dms_continue_manager_test.h"

#include "datetime_ex.h"
#include "distributed_sched_util.h"
#include "dtbschedmgr_log.h"
#define private public
#include "mission/dms_continue_send_manager.h"
#include "mission/dms_continue_recv_manager.h"
#undef private
#include "test_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedSchedule {
namespace {
const std::string TYPE = "type";
const std::string BUNDLENAME_01 = "bundleName01";
const std::string BUNDLENAME_02 = "bundleName02";
const std::string NETWORKID_01 = "networkId01";
const std::string NETWORKID_02 = "networkId02";
const std::string ABILITY_NAME_01 = "abilityName01";
const std::string CANCEL_FOCUSED_TASK = "cancel_mission_focused_task";
constexpr static int32_t DMS_SEND_LEN = 5;
constexpr static uint8_t DMS_0X0F = 0x0f;
constexpr int32_t MISSIONID_01 = 1;
constexpr int32_t MISSIONID_02 = 2;
constexpr int32_t ACTIVE = 0;
constexpr int32_t INACTIVE = 1;
constexpr int32_t CANCEL_FOCUSED_DELAYED = 10000;
constexpr int32_t DBMS_RETRY_MAX_TIME = 5;
}

void DMSContinueManagerTest::SetUpTestCase()
{
}

void DMSContinueManagerTest::TearDownTestCase()
{
}

void DMSContinueManagerTest::SetUp()
{
}

void DMSContinueManagerTest::TearDown()
{
}

void RemoteOnListenerStubTest::OnCallback(const uint32_t ContinueState, const std::string& srcDeviceId,
    const std::string &bundleName, const std::string &continueType, const std::string &srcBundleName)
{
}

/**
 * @tc.name: testUnInit001
 * @tc.desc: test UnInit
 * @tc.type: FUNC
 * @tc.require: I7F8KH
 */
HWTEST_F(DMSContinueManagerTest, testUnInit001, TestSize.Level3)
{
    DTEST_LOG << "DMSContinueManagerTest testUnInit001 begin" << std::endl;
    
    DistributedSchedUtil::MockManageMissions();
    DMSContinueSendMgr::GetInstance().Init();
    EXPECT_NE(DMSContinueSendMgr::GetInstance().eventHandler_, nullptr);

    /**
     * @tc.steps: step1. test UnInit when eventHandler is not nullptr;
     */
    DMSContinueSendMgr::GetInstance().UnInit();
    EXPECT_EQ(DMSContinueSendMgr::GetInstance().eventHandler_, nullptr);

    /**
     * @tc.steps: step2. test UnInit when eventHandler is nullptr;
     */
    DMSContinueSendMgr::GetInstance().UnInit();
    EXPECT_EQ(DMSContinueSendMgr::GetInstance().eventHandler_, nullptr);

    DTEST_LOG << "DMSContinueManagerTest testUnInit001 end" << std::endl;
}

/**
 * @tc.name: testPostUnfocusedTaskWithDelay001
 * @tc.desc: test PostUnfocusedTaskWithDelay
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testPostUnfocusedTaskWithDelay001, TestSize.Level3)
{
    DTEST_LOG << "DMSContinueManagerTest testPostUnfocusedTaskWithDelay001 begin" << std::endl;

    DistributedSchedUtil::MockManageMissions();
    DMSContinueSendMgr::GetInstance().Init();

    /**
     * @tc.steps: step1. test PostUnfocusedTaskWithDelay when eventHandler is not nullptr;
     */
    DMSContinueSendMgr::GetInstance().PostUnfocusedTaskWithDelay(0, UnfocusedReason::TIMEOUT);
    DMSContinueSendMgr::GetInstance().PostUnfocusedTaskWithDelay(0, UnfocusedReason::SCREENOFF);
    EXPECT_NE(DMSContinueSendMgr::GetInstance().eventHandler_, nullptr);

    /**
     * @tc.steps: step2. test PostUnfocusedTaskWithDelay when eventHandler is nullptr;
     */
    DMSContinueSendMgr::GetInstance().UnInit();
    DMSContinueSendMgr::GetInstance().PostUnfocusedTaskWithDelay(0, UnfocusedReason::TIMEOUT);
    EXPECT_EQ(DMSContinueSendMgr::GetInstance().eventHandler_, nullptr);
    DTEST_LOG << "DMSContinueManagerTest testPostUnfocusedTaskWithDelay001 end" << std::endl;
}

/**
 * @tc.name: testNotifyMissionFocused001
 * @tc.desc: test NotifyMissionFocused
 * @tc.type: FUNC
 * @tc.require: I7F8KH
 */
HWTEST_F(DMSContinueManagerTest, testNotifyMissionFocused001, TestSize.Level3)
{
    DTEST_LOG << "DMSContinueManagerTest testNotifyMissionFocused001 begin" << std::endl;

    DistributedSchedUtil::MockManageMissions();
    DMSContinueSendMgr::GetInstance().Init();

    /**
     * @tc.steps: step1. test NotifyMissionFocused when eventHandler is not nullptr;
     */
    DMSContinueSendMgr::GetInstance().NotifyMissionFocused(0, FocusedReason::NORMAL);
    DMSContinueSendMgr::GetInstance().NotifyMissionFocused(0, FocusedReason::SCREENOFF);
    EXPECT_NE(DMSContinueSendMgr::GetInstance().eventHandler_, nullptr);

    /**
     * @tc.steps: step2. test NotifyMissionFocused when eventHandler is nullptr;
     */
    DMSContinueSendMgr::GetInstance().UnInit();
    DMSContinueSendMgr::GetInstance().NotifyMissionFocused(0, FocusedReason::NORMAL);
    EXPECT_EQ(DMSContinueSendMgr::GetInstance().eventHandler_, nullptr);
    DTEST_LOG << "DMSContinueManagerTest testNotifyMissionFocused001 end" << std::endl;
}

/**
 * @tc.name: testNotifyMissionUnfocused001
 * @tc.desc: test NotifyMissionUnfocused
 * @tc.type: FUNC
 * @tc.require: I7F8KH
 */
HWTEST_F(DMSContinueManagerTest, testNotifyMissionUnfocused001, TestSize.Level3)
{
    DTEST_LOG << "DMSContinueManagerTest testNotifyMissionUnfocused001 begin" << std::endl;

    DistributedSchedUtil::MockManageMissions();
    DMSContinueSendMgr::GetInstance().Init();
    /**
     * @tc.steps: step1. test NotifyMissionUnfocused when eventHandler is not nullptr;
     */
    DMSContinueSendMgr::GetInstance().NotifyMissionUnfocused(0, UnfocusedReason::NORMAL);
    EXPECT_NE(DMSContinueSendMgr::GetInstance().eventHandler_, nullptr);

    /**
     * @tc.steps: step2. test NotifyMissionUnfocused when eventHandler is nullptr;
     */
    DMSContinueSendMgr::GetInstance().UnInit();
    DMSContinueSendMgr::GetInstance().NotifyMissionUnfocused(0, UnfocusedReason::NORMAL);
    EXPECT_EQ(DMSContinueSendMgr::GetInstance().eventHandler_, nullptr);
    DTEST_LOG << "DMSContinueManagerTest testNotifyMissionUnfocused001 end" << std::endl;
}

/**
 * @tc.name: testRegisterOnListener001
 * @tc.desc: test RegisterOnListener
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testRegisterOnListener001, TestSize.Level1)
{
    DTEST_LOG << "DMSContinueManagerTest testRegisterOnListener001 start" << std::endl;
    DMSContinueRecvMgr::GetInstance().Init();
    sptr<IRemoteObject> obj01 = new RemoteOnListenerStubTest();
    int32_t ret = DMSContinueRecvMgr::GetInstance().RegisterOnListener(TYPE, obj01);
    EXPECT_EQ(ret, ERR_OK);

    ret = DMSContinueRecvMgr::GetInstance().RegisterOnListener(TYPE, obj01);
    EXPECT_EQ(ret, NO_MISSION_INFO_FOR_MISSION_ID);

    sptr<IRemoteObject> obj02 = new RemoteOnListenerStubTest();
    ret = DMSContinueRecvMgr::GetInstance().RegisterOnListener(TYPE, obj02);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "DMSContinueManagerTest testRegisterOnListener001 end" << std::endl;
}

/**
 * @tc.name: testRegisterOffListener001
 * @tc.desc: test RegisterOffListener
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testRegisterOffListener001, TestSize.Level1)
{
    DTEST_LOG << "DMSContinueManagerTest testRegisterOffListener001 start" << std::endl;
    sptr<IRemoteObject> obj01 = new RemoteOnListenerStubTest();
    int32_t ret = DMSContinueRecvMgr::GetInstance().RegisterOnListener(TYPE, obj01);
    ret = DMSContinueRecvMgr::GetInstance().RegisterOffListener(TYPE, obj01);
    EXPECT_EQ(ret, ERR_OK);

    ret = DMSContinueRecvMgr::GetInstance().RegisterOffListener(TYPE, nullptr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "DMSContinueManagerTest testRegisterOffListener001 end" << std::endl;
}

/**
 * @tc.name: testRegisterOffListener002
 * @tc.desc: test RegisterOffListener when iterItem->second is empty.
 * @tc.type: FUNC
 * @tc.require: I7F8KH
 */
HWTEST_F(DMSContinueManagerTest, testRegisterOffListener002, TestSize.Level3)
{
    DTEST_LOG << "DMSContinueManagerTest testRegisterOffListener002 start" << std::endl;
    sptr<IRemoteObject> obj01 = new RemoteOnListenerStubTest();
    int32_t ret = DMSContinueRecvMgr::GetInstance().RegisterOnListener(TYPE, obj01);
    EXPECT_EQ(ret, ERR_OK);

    {
        std::lock_guard<std::mutex> registerOnListenerMapLock(
            DMSContinueRecvMgr::GetInstance().eventMutex_);
        DMSContinueRecvMgr::GetInstance().registerOnListener_[TYPE].clear();
    }
    ret = DMSContinueRecvMgr::GetInstance().RegisterOffListener(TYPE, obj01);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "DMSContinueManagerTest testRegisterOffListener002 end" << std::endl;
}

/**
 * @tc.name: testGetMissionId001
 * @tc.desc: test GetMissionId
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testGetMissionId001, TestSize.Level1)
{
    DTEST_LOG << "DMSContinueManagerTest testGetMissionId001 start" << std::endl;
    DMSContinueSendMgr::GetInstance().focusedMission_[BUNDLENAME_01] = MISSIONID_01;
    int32_t missionId;
    int32_t ret = DMSContinueSendMgr::GetInstance().GetMissionIdByBundleName(BUNDLENAME_01, missionId);
    EXPECT_EQ(missionId, MISSIONID_01);
    EXPECT_EQ(ret, ERR_OK);

    ret = DMSContinueSendMgr::GetInstance().GetMissionIdByBundleName(BUNDLENAME_02, missionId);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "DMSContinueManagerTest testGetMissionId001 end" << std::endl;
}

/**
 * @tc.name: testDealFocusedBusiness001
 * @tc.desc: test DealFocusedBusiness.
 * @tc.type: FUNC
 * @tc.require: I7F8KH
 */
HWTEST_F(DMSContinueManagerTest, testDealFocusedBusiness001, TestSize.Level3)
{
    DTEST_LOG << "DMSContinueManagerTest testDealFocusedBusiness001 start" << std::endl;

    /**
     * @tc.steps: step1. test DealFocusedBusiness when missionId is invalid;
     */
    int32_t ret = DMSContinueSendMgr::GetInstance().DealFocusedBusiness(-1);
    EXPECT_NE(ret, ERR_OK);
    
    DTEST_LOG << "DMSContinueManagerTest testDealFocusedBusiness001 end" << std::endl;
}

/**
 * @tc.name: testDealUnfocusedBusiness001
 * @tc.desc: test DealUnfocusedBusiness.
 * @tc.type: FUNC
 * @tc.require: I7F8KH
 */
HWTEST_F(DMSContinueManagerTest, testDealUnfocusedBusiness001, TestSize.Level3)
{
    DTEST_LOG << "DMSContinueManagerTest testDealUnfocusedBusiness001 start" << std::endl;

    /**
     * @tc.steps: step1. test DealUnfocusedBusiness when missionId is invalid;
     */
    int32_t ret = DMSContinueSendMgr::GetInstance().DealUnfocusedBusiness(-1, UnfocusedReason::NORMAL);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);

    /**
     * @tc.steps: step2. test DealUnfocusedBusiness when missionId is not invalid;
     */
    DMSContinueSendMgr::GetInstance().focusedMission_[BUNDLENAME_01] = MISSIONID_01;
    DMSContinueSendMgr::GetInstance().focusedMissionAbility_[MISSIONID_01] = ABILITY_NAME_01;
    ret = DMSContinueSendMgr::GetInstance().DealUnfocusedBusiness(MISSIONID_01, UnfocusedReason::NORMAL);
    EXPECT_EQ(ret, CAN_NOT_FOUND_ABILITY_ERR);

    /**
     * @tc.steps: step3. test NotifyDied when obj is nullptr;
     */
    sptr<IRemoteObject> obj01 = nullptr;
    DMSContinueRecvMgr::GetInstance().NotifyDied(obj01);

    /**
     * @tc.steps: step4. test NotifyDied when iterItem->second is empty;
     */
    std::vector<sptr<IRemoteObject>> objs;
    {
        std::lock_guard<std::mutex> registerOnListenerMapLock(
            DMSContinueRecvMgr::GetInstance().eventMutex_);
        DMSContinueRecvMgr::GetInstance().registerOnListener_[TYPE] = objs;
    }
    obj01 = new RemoteOnListenerStubTest();
    DMSContinueRecvMgr::GetInstance().NotifyDied(obj01);

    DTEST_LOG << "DMSContinueManagerTest testDealUnfocusedBusiness001 end" << std::endl;
}

/**
 * @tc.name: testVerifyBroadcastSource001
 * @tc.desc: test testVerifyBroadcastSource001.
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testVerifyBroadcastSource001, TestSize.Level3)
{
    DTEST_LOG << "DMSContinueManagerTest testVerifyBroadcastSource001 start" << std::endl;

    int32_t state = ACTIVE;
    std::string networkId = "test networkId";
    std::string bundleName = "test bundleName";
    std::string continueType = "test continueType";
    int32_t ret = DMSContinueRecvMgr::GetInstance().VerifyBroadcastSource(networkId,
        bundleName, continueType, state);
    EXPECT_EQ(ret, ERR_OK);

    state = INACTIVE;
    ret = DMSContinueRecvMgr::GetInstance().VerifyBroadcastSource(networkId, bundleName, continueType, state);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: testVerifyBroadcastSource002
 * @tc.desc: test testVerifyBroadcastSource002.
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testVerifyBroadcastSource002, TestSize.Level3)
{
    DTEST_LOG << "DMSContinueManagerTest testVerifyBroadcastSource002 start" << std::endl;

    int32_t state = ACTIVE;
    std::string networkId = "test networkId";
    std::string bundleName = "test bundleName";
    std::string continueType = "test continueType";
    int32_t ret = DMSContinueRecvMgr::GetInstance().VerifyBroadcastSource(networkId,
        bundleName, continueType, state);
    EXPECT_EQ(ret, ERR_OK);

    state = INACTIVE;
    networkId = "invalid networkId";
    ret = DMSContinueRecvMgr::GetInstance().VerifyBroadcastSource(networkId,
        bundleName, continueType, state);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
}

/**
 * @tc.name: testVerifyBroadcastSource003
 * @tc.desc: test testVerifyBroadcastSource003.
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testVerifyBroadcastSource003, TestSize.Level3)
{
    DTEST_LOG << "DMSContinueManagerTest testVerifyBroadcastSource003 start" << std::endl;

    int32_t state = ACTIVE;
    std::string networkId = "test networkId";
    std::string bundleName = "test bundleName";
    std::string continueType = "test continueType";
    int32_t ret = DMSContinueRecvMgr::GetInstance().VerifyBroadcastSource(networkId,
        bundleName, continueType, state);
    EXPECT_EQ(ret, ERR_OK);

    state = INACTIVE;
    bundleName = "invalid bundleName";
    ret = DMSContinueRecvMgr::GetInstance().VerifyBroadcastSource(networkId,
        bundleName, continueType, state);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
}

/**
 * @tc.name: testDealOnBroadcastBusiness001
 * @tc.desc: test DealOnBroadcastBusiness.
 * @tc.type: FUNC
 * @tc.require: I7F8KH
 */
HWTEST_F(DMSContinueManagerTest, testDealOnBroadcastBusiness001, TestSize.Level3)
{
    DTEST_LOG << "DMSContinueManagerTest testDealOnBroadcastBusiness001 start" << std::endl;

    /**
     * @tc.steps: step1. test DealOnBroadcastBusiness when senderNetworkId is invalid;
     */
    std::string senderNetworkId = "invalid senderNetworkId";
    uint16_t bundleNameId = 0;
    uint8_t continueTypeId = 0;
    int32_t state = 0;
    int32_t ret = DMSContinueRecvMgr::GetInstance().DealOnBroadcastBusiness(senderNetworkId, bundleNameId,
        continueTypeId, state, DBMS_RETRY_MAX_TIME);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);

    ret = DMSContinueRecvMgr::GetInstance().DealOnBroadcastBusiness(senderNetworkId, bundleNameId, continueTypeId,
        state, 0);
    EXPECT_EQ(ret, 0);

    /**
     * @tc.steps: step2. test NotifyRecvBroadcast when obj is nullptr;
     */
    sptr<IRemoteObject> obj = nullptr;
    std::string networkId = "invalid networkId";
    std::string bundleName = "invalid bundleName";
    std::string continueType = "invalid continueType";
    DMSContinueRecvMgr::GetInstance().NotifyRecvBroadcast(obj, networkId, bundleName, state, continueType);

    /**
     * @tc.steps: step3. test NotifyRecvBroadcast when missionId is invalid;
     */
    obj =  new RemoteOnListenerStubTest();
    DMSContinueRecvMgr::GetInstance().NotifyRecvBroadcast(obj, networkId, bundleName, state, continueType);

    DTEST_LOG << "DMSContinueManagerTest testDealOnBroadcastBusiness001 end" << std::endl;
}

/**
 * @tc.name: testPostOnBroadcastBusiness001
 * @tc.desc: test PostOnBroadcastBusiness
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testPostOnBroadcastBusiness001, TestSize.Level1)
{
    DTEST_LOG << "DMSContinueManagerTest testPostOnBroadcastBusiness001 start" << std::endl;
    std::string senderNetworkId = "invalid senderNetworkId";
    uint16_t bundleNameId = 0;
    uint8_t continueTypeId = 0;
    int32_t state = 0;

    DMSContinueRecvMgr::GetInstance().PostOnBroadcastBusiness(senderNetworkId, bundleNameId, continueTypeId, state);
    EXPECT_NE(DMSContinueRecvMgr::GetInstance().eventHandler_, nullptr);

    DTEST_LOG << "DMSContinueManagerTest testPostOnBroadcastBusiness001 end" << std::endl;
}

/**
 * @tc.name: testGetBundleName001
 * @tc.desc: test GetBundleName
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testGetBundleName001, TestSize.Level1)
{
    DTEST_LOG << "DMSContinueManagerTest testGetBundleName001 start" << std::endl;
    DMSContinueSendMgr::GetInstance().focusedMission_[BUNDLENAME_01] = MISSIONID_01;
    std::string bundleName;
    int32_t ret = DMSContinueSendMgr::GetInstance().GetBundleNameByMissionId(MISSIONID_01, bundleName);
    EXPECT_EQ(bundleName, BUNDLENAME_01);
    EXPECT_EQ(ret, ERR_OK);

    ret = DMSContinueSendMgr::GetInstance().GetBundleNameByMissionId(MISSIONID_02, bundleName);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "DMSContinueManagerTest testGetBundleName001 end" << std::endl;
}

/**
 * @tc.name: testIsContinue001
 * @tc.desc: test IsContinue
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testIsContinue001, TestSize.Level1)
{
    DTEST_LOG << "DMSContinueManagerTest testIsContinue001 start" << std::endl;
    DMSContinueSendMgr::GetInstance().focusedMission_[BUNDLENAME_02] = MISSIONID_02;
    DMSContinueSendMgr::GetInstance().info_.currentMissionId = MISSIONID_01;
    DMSContinueSendMgr::GetInstance().info_.currentIsContinuable = true;
    bool ret = DMSContinueSendMgr::GetInstance().IsContinue(MISSIONID_02, BUNDLENAME_02);
    EXPECT_EQ(ret, false);

    ret = DMSContinueSendMgr::GetInstance().IsContinue(MISSIONID_01, BUNDLENAME_01);
    EXPECT_EQ(ret, true);

    DMSContinueSendMgr::GetInstance().info_.currentIsContinuable = false;
    ret = DMSContinueSendMgr::GetInstance().IsContinue(MISSIONID_01, BUNDLENAME_01);
    EXPECT_EQ(ret, true);
    DTEST_LOG << "DMSContinueManagerTest testIsContinue001 end" << std::endl;
}

/**
 * @tc.name: testNotifyDied001
 * @tc.desc: test NotifyDied
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testNotifyDied001, TestSize.Level1)
{
    DTEST_LOG << "DMSContinueManagerTest testNotifyDied001 start" << std::endl;
    sptr<IRemoteObject> obj01 = new RemoteOnListenerStubTest();
    int32_t ret = DMSContinueRecvMgr::GetInstance().RegisterOnListener(TYPE, obj01);
    EXPECT_EQ(false, DMSContinueRecvMgr::GetInstance().registerOnListener_.empty());
    DMSContinueRecvMgr::GetInstance().NotifyDied(obj01);
    DTEST_LOG << "DMSContinueManagerTest testNotifyDied001 end" << std::endl;
}

/**
 * @tc.name: testSetMissionContinueState001
 * @tc.desc: test SetMissionContinueState
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testSetMissionContinueState001, TestSize.Level3)
{
    DTEST_LOG << "DMSContinueManagerTest testSetMissionContinueState001 start" << std::endl;
    DistributedSchedUtil::MockManageMissions();
    DMSContinueSendMgr::GetInstance().Init();
    OHOS::AAFwk::ContinueState state = OHOS::AAFwk::ContinueState::CONTINUESTATE_ACTIVE;

     /**
     * @tc.steps: step1. test SetMissionContinueState when eventHandler is not nullptr;
     */
    DMSContinueSendMgr::GetInstance().SetMissionContinueState(0, state);
    EXPECT_NE(DMSContinueSendMgr::GetInstance().eventHandler_, nullptr);

    /**
     * @tc.steps: step2. test SetMissionContinueState when eventHandler is nullptr;
     */
    DMSContinueSendMgr::GetInstance().UnInit();
    DMSContinueSendMgr::GetInstance().SetMissionContinueState(0, state);
    EXPECT_EQ(DMSContinueSendMgr::GetInstance().eventHandler_, nullptr);
    DTEST_LOG << "DMSContinueManagerTest testSetMissionContinueState001 end" << std::endl;
}

/**
 * @tc.name: testDealSetMissionContinueStateBusiness001
 * @tc.desc: test DealSetMissionContinueStateBusiness.
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testDealSetMissionContinueStateBusiness001, TestSize.Level3)
{
    DTEST_LOG << "DMSContinueManagerTest testDealSetMissionContinueStateBusiness001 start" << std::endl;
    OHOS::AAFwk::ContinueState state = OHOS::AAFwk::ContinueState::CONTINUESTATE_ACTIVE;
    DMSContinueSendMgr::GetInstance().info_.currentMissionId = MISSIONID_01;

    /**
     * @tc.steps: step1. test DealSetMissionContinueStateBusiness when missionId is invalid;
     */
    int32_t ret = DMSContinueSendMgr::GetInstance().DealSetMissionContinueStateBusiness(MISSIONID_02,
        state);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);

    /**
     * @tc.steps: step2. test DealUnfocusedBusiness when mission is not continueable;
     */
    DMSContinueSendMgr::GetInstance().info_.currentIsContinuable = false;
    ret = DMSContinueSendMgr::GetInstance().DealSetMissionContinueStateBusiness(MISSIONID_01, state);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);

    DTEST_LOG << "DMSContinueManagerTest testDealSetMissionContinueStateBusiness001 end" << std::endl;
}

#ifdef SUPPORT_COMMON_EVENT_SERVICE
/**
 * @tc.name: testOnDeviceScreenOff001
 * @tc.desc: test OnDeviceScreenOff normal
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testOnDeviceScreenOff001, TestSize.Level1)
{
    DTEST_LOG << "DMSContinueManagerTest testOnDeviceScreenOff001 start" << std::endl;

    DistributedSchedUtil::MockManageMissions();
     /**
     * @tc.steps: step1. test OnDeviceScreenOff when eventHandler is not nullptr;
     */
    DMSContinueSendMgr::GetInstance().Init();
    DMSContinueSendMgr::GetInstance().OnDeviceScreenOff();
    EXPECT_NE(DMSContinueSendMgr::GetInstance().eventHandler_, nullptr);

    /**
     * @tc.steps: step2. test OnDeviceScreenOff when eventHandler is nullptr;
     */
    DMSContinueSendMgr::GetInstance().UnInit();
    DMSContinueSendMgr::GetInstance().OnDeviceScreenOff();
    EXPECT_EQ(DMSContinueSendMgr::GetInstance().eventHandler_, nullptr);
    DTEST_LOG << "DMSContinueManagerTest testOnDeviceScreenOff001 end" << std::endl;
}
#endif

/**
 * @tc.name: testNotifyDeviceOffline001
 * @tc.desc: test NotifyDeviceOffline normal
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testNotifyDeviceOffline001, TestSize.Level1)
{
    DTEST_LOG << "DMSContinueManagerTest testNotifyDeviceOffline001 start" << std::endl;
    sptr<IRemoteObject> obj01 = new RemoteOnListenerStubTest();
    DMSContinueRecvMgr::GetInstance().RegisterOnListener(TYPE, obj01);
    EXPECT_NE(DMSContinueRecvMgr::GetInstance().registerOnListener_.size(), 0);

    DMSContinueRecvMgr::GetInstance().iconInfo_.senderNetworkId = NETWORKID_01;
    DMSContinueRecvMgr::GetInstance().NotifyDeviceOffline(NETWORKID_01);
    EXPECT_EQ(DMSContinueRecvMgr::GetInstance().iconInfo_.senderNetworkId, "");

    DTEST_LOG << "DMSContinueManagerTest testNotifyDeviceOffline001 end" << std::endl;
}

/**
 * @tc.name: testNotifyDeviceOffline002
 * @tc.desc: test NotifyDeviceOffline networkId empty
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testNotifyDeviceOffline002, TestSize.Level1)
{
    DTEST_LOG << "DMSContinueManagerTest testNotifyDeviceOffline002 start" << std::endl;
    DMSContinueRecvMgr::GetInstance().iconInfo_.senderNetworkId = NETWORKID_01;
    DMSContinueRecvMgr::GetInstance().NotifyDeviceOffline("");
    EXPECT_EQ(DMSContinueRecvMgr::GetInstance().iconInfo_.senderNetworkId, NETWORKID_01);

    DTEST_LOG << "DMSContinueManagerTest testNotifyDeviceOffline002 end" << std::endl;
}

/**
 * @tc.name: testNotifyDeviceOffline003
 * @tc.desc: test NotifyDeviceOffline networkId not match
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testNotifyDeviceOffline003, TestSize.Level1)
{
    DTEST_LOG << "DMSContinueManagerTest testNotifyDeviceOffline003 start" << std::endl;
    DMSContinueRecvMgr::GetInstance().iconInfo_.senderNetworkId = NETWORKID_01;
    DMSContinueRecvMgr::GetInstance().NotifyDeviceOffline(NETWORKID_02);
    EXPECT_EQ(DMSContinueRecvMgr::GetInstance().iconInfo_.senderNetworkId, NETWORKID_01);

    DTEST_LOG << "DMSContinueManagerTest testNotifyDeviceOffline003 end" << std::endl;
}

/**
 * @tc.name: testNotifyDataRecv001
 * @tc.desc: NotifyDataRecv
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testNotifyDataRecv001, TestSize.Level1)
{
    DTEST_LOG << "DMSContinueManagerTest testNotifyDataRecv001 start" << std::endl;
    std::string senderNetworkId = NETWORKID_01;
    uint8_t payload[] = {0xf0};
    uint32_t dataLen1 = DMS_SEND_LEN - 1;
    DMSContinueRecvMgr::GetInstance().NotifyDataRecv(senderNetworkId, payload, dataLen1);

    uint32_t dataLen2 = DMS_SEND_LEN;
    DMSContinueRecvMgr::GetInstance().NotifyDataRecv(senderNetworkId, payload, dataLen2);
    EXPECT_NE(payload[0] & DMS_0X0F, sizeof(uint32_t));
    DTEST_LOG << "DMSContinueManagerTest testNotifyDataRecv001 end" << std::endl;
}

/**
 * @tc.name: testSendSoftbusEvent001
 * @tc.desc: SendSoftbusEvent
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testSendSoftbusEvent001, TestSize.Level1)
{
    DTEST_LOG << "DMSContinueManagerTest testSendSoftbusEvent001 start" << std::endl;
    uint16_t bundleNameId = 0;
    uint8_t continueType = 1;
    uint8_t type = 0;
    bool ret = DMSContinueSendMgr::GetInstance().SendSoftbusEvent(bundleNameId, continueType, type);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "DMSContinueManagerTest testSendSoftbusEvent001 end" << std::endl;
}

/**
 * @tc.name: testNotifyDeviceOnline001
 * @tc.desc: NotifyDeviceOnline
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testNotifyDeviceOnline001, TestSize.Level1)
{
    DTEST_LOG << "DMSContinueManagerTest testNotifyDeviceOnline001 start" << std::endl;
    uint32_t ret = DMSContinueSendMgr::GetInstance().NotifyDeviceOnline();
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "DMSContinueManagerTest testNotifyDeviceOnline001 end" << std::endl;
}

/**
 * @tc.name: testGetAbilityNameByMissionId_001
 * @tc.desc: test GetAbilityNameByMissionId
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testGetAbilityNameByMissionId_001, TestSize.Level1)
{
    DTEST_LOG << "DMSContinueManagerTest testGetAbilityNameByMissionId_001 start" << std::endl;
    DMSContinueSendMgr::GetInstance().focusedMissionAbility_[MISSIONID_01] = ABILITY_NAME_01;
    std::string abilityName;
    int32_t ret = DMSContinueSendMgr::GetInstance().GetAbilityNameByMissionId(MISSIONID_01, abilityName);
    EXPECT_EQ(abilityName, ABILITY_NAME_01);
    EXPECT_EQ(ret, ERR_OK);

    ret = DMSContinueSendMgr::GetInstance().GetBundleNameByMissionId(MISSIONID_02, abilityName);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "DMSContinueManagerTest testGetAbilityNameByMissionId_001 end" << std::endl;
}

/**
 * @tc.name: testFocusedBusinessSendEvent_001
 * @tc.desc: test FocusedBusinessSendEvent
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testFocusedBusinessSendEvent_001, TestSize.Level1)
{
    DTEST_LOG << "DMSContinueManagerTest testFocusedBusinessSendEvent_001 start" << std::endl;
    int32_t ret = DMSContinueSendMgr::GetInstance().FocusedBusinessSendEvent(BUNDLENAME_01, ABILITY_NAME_01);
   
    EXPECT_EQ(ret, CAN_NOT_FOUND_ABILITY_ERR);
    DTEST_LOG << "DMSContinueManagerTest testFocusedBusinessSendEvent_001 end" << std::endl;
}

/**
 * @tc.name: testGetBundleNameIdAndContinueTypeId_001
 * @tc.desc: test GetBundleNameIdAndContinueTypeId
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testGetBundleNameIdAndContinueTypeId_001, TestSize.Level1)
{
    DTEST_LOG << "DMSContinueManagerTest testGetBundleNameIdAndContinueTypeId_001 start" << std::endl;
    DMSContinueSendMgr::GetInstance().focusedMission_[BUNDLENAME_01] = MISSIONID_01;
    DMSContinueSendMgr::GetInstance().focusedMissionAbility_[MISSIONID_01] = ABILITY_NAME_01;
    uint16_t bundleNameId = 0;
    uint8_t continueTypeId = 0;
    OHOS::AAFwk::ContinueState state = OHOS::AAFwk::ContinueState::CONTINUESTATE_ACTIVE;
    int32_t ret = DMSContinueSendMgr::GetInstance().GetBundleNameIdAndContinueTypeId(MISSIONID_01, state,
        bundleNameId, continueTypeId);
    
    EXPECT_EQ(ret, REMOTE_DEVICE_BIND_ABILITY_ERR);
    DTEST_LOG << "DMSContinueManagerTest testGetBundleNameIdAndContinueTypeId_001 end" << std::endl;
}

/**
 * @tc.name: testGetContinueType_001
 * @tc.desc: test GetContinueType
 * @tc.type: FUNC
 */
HWTEST_F(DMSContinueManagerTest, testGetContinueType_001, TestSize.Level1)
{
    DTEST_LOG << "DMSContinueManagerTest testGetContinueType_001 start" << std::endl;
    int32_t state = ACTIVE;
    std::string networkId = "test networkId";
    std::string bundleName = "test bundleName";
    std::string continueType = "test continueType";
    int32_t ret = DMSContinueRecvMgr::GetInstance().VerifyBroadcastSource(networkId,
        bundleName, continueType, state);
    EXPECT_EQ(ret, ERR_OK);
    
    EXPECT_FALSE(DMSContinueRecvMgr::GetInstance().GetContinueType(bundleName).empty());
    DTEST_LOG << "DMSContinueManagerTest testGetContinueType_001 end" << std::endl;
}
} // namespace DistributedSchedule
} // namespace OHOS
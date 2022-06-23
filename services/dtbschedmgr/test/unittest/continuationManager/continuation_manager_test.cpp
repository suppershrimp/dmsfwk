/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "continuation_manager_test.h"

#include "distributed_sched_service.h"
#include "device_selection_notifier_stub.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "test_log.h"

namespace OHOS {
namespace DistributedSchedule {
using namespace std;
using namespace testing;
using namespace testing::ext;
namespace {
constexpr int32_t UNREGISTER_TOKEN = 10000;
constexpr int32_t INVALID_STATUS = 10;
const std::string TEST_DEVICE_ID = "test deviceId";
const std::string EMPTY_DEVICE_ID = "";
const std::string CALLBACK_TYPE1 = "deviceConnect";
const std::string CALLBACK_TYPE2 = "deviceDisconnect";
const std::string INVALID_CALLBACK_TYPE = "deviceCancel";
}

class DeviceSelectionNotifierTest : public AAFwk::DeviceSelectionNotifierStub {
public:
    DeviceSelectionNotifierTest() = default;
    ~DeviceSelectionNotifierTest() = default;

    void OnDeviceConnect(const std::vector<AAFwk::ContinuationResult>& continuationResults) override;
    void OnDeviceDisconnect(const std::vector<std::string>& deviceIds) override;
};

void DeviceSelectionNotifierTest::OnDeviceConnect(const std::vector<AAFwk::ContinuationResult>& continuationResults)
{
}

void DeviceSelectionNotifierTest::OnDeviceDisconnect(const std::vector<std::string>& deviceIds)
{
}

void ContinationManagerTest::SetUpTestCase()
{
    DTEST_LOG << "ContinationManagerTest::SetUpTestCase" << std::endl;
}

void ContinationManagerTest::TearDownTestCase()
{
    DTEST_LOG << "ContinationManagerTest::TearDownTestCase" << std::endl;
}

void ContinationManagerTest::SetUp()
{
    DTEST_LOG << "ContinationManagerTest::SetUp" << std::endl;
}

void ContinationManagerTest::TearDown()
{
    DTEST_LOG << "ContinationManagerTest::TearDown" << std::endl;
}

sptr<IDistributedSched> ContinationManagerTest::GetDms()
{
    if (proxy_ != nullptr) {
        return proxy_;
    }
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_TRUE(samgr != nullptr);
    if (samgr == nullptr) {
        DTEST_LOG << "ContinationManagerTest samgr is nullptr" << std::endl;
        return nullptr;
    }
    DTEST_LOG << "ContinationManagerTest samgr is not nullptr" << std::endl;
    auto distributedObject = samgr->GetSystemAbility(DISTRIBUTED_SCHED_SA_ID);
    EXPECT_TRUE(distributedObject != nullptr);
    proxy_ = iface_cast<IDistributedSched>(distributedObject);
    if (proxy_ == nullptr) {
        DTEST_LOG << "ContinationManagerTest dmsProxy is nullptr" << std::endl;
    }
    DTEST_LOG << "ContinationManagerTest dmsProxy is not nullptr" << std::endl;
    return proxy_;
}

/**
 * @tc.name: RegisterTest_001
 * @tc.desc: test register token
 * @tc.type: FUNC
 * @tc.require: SR000H34KJ
 */
HWTEST_F(ContinationManagerTest, RegisterTest_001, TestSize.Level0)
{
    DTEST_LOG << "ContinationManagerTest RegisterTest_001 start" << std::endl;
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    int32_t token1 = -1;
    int32_t result1 = proxy->Register(token1);
    DTEST_LOG << "result1:" << result1 << std::endl;
    int32_t token2 = -1;
    int32_t result2 = proxy->Register(token2);
    DTEST_LOG << "result2:" << result2 << std::endl;
    EXPECT_EQ(ERR_OK, result1);
    EXPECT_EQ(ERR_OK, result2);
    EXPECT_EQ(1, token2 - token1);
    DTEST_LOG << "ContinationManagerTest RegisterTest_001 end" << std::endl;
}

/**
 * @tc.name: RegisterTest_002
 * @tc.desc: test register token with extra param
 * @tc.type: FUNC
 * @tc.require: AR000H34KK
 */
HWTEST_F(ContinationManagerTest, RegisterTest_002, TestSize.Level0)
{
    DTEST_LOG << "ContinationManagerTest RegisterTest_002 start" << std::endl;
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    std::shared_ptr<AAFwk::ContinuationExtraParams> continuationExtraParams =
        std::make_shared<AAFwk::ContinuationExtraParams>();
    int32_t token1 = -1;
    int32_t result1 = proxy->Register(continuationExtraParams, token1);
    DTEST_LOG << "result1:" << result1 << std::endl;
    int32_t token2 = -1;
    int32_t result2 = proxy->Register(continuationExtraParams, token2);
    DTEST_LOG << "result2:" << result2 << std::endl;
    EXPECT_EQ(ERR_OK, result1);
    EXPECT_EQ(ERR_OK, result2);
    EXPECT_EQ(1, token2 - token1);
    DTEST_LOG << "ContinationManagerTest RegisterTest_002 end" << std::endl;
}

/**
 * @tc.name: UnregisterTest_001
 * @tc.desc: test unregister token
 * @tc.type: FUNC
 * @tc.require: SR000H34KJ
 */
HWTEST_F(ContinationManagerTest, UnregisterTest_001, TestSize.Level0)
{
    DTEST_LOG << "ContinationManagerTest UnregisterTest_001 start" << std::endl;
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    int32_t token = -1;
    int32_t result1 = proxy->Register(token);
    DTEST_LOG << "result1:" << result1 << std::endl;
    int32_t result2 = proxy->Unregister(token);
    DTEST_LOG << "result2:" << result2 << std::endl;
    EXPECT_EQ(ERR_OK, result1);
    EXPECT_EQ(ERR_OK, result2);
    DTEST_LOG << "ContinationManagerTest UnregisterTest_001 end" << std::endl;
}

/**
 * @tc.name: UnregisterTest_002
 * @tc.desc: test unregister token with invalid token
 * @tc.type: FUNC
 * @tc.require: AR000H34KK
 */
HWTEST_F(ContinationManagerTest, UnregisterTest_002, TestSize.Level0)
{
    DTEST_LOG << "ContinationManagerTest UnregisterTest_002 start" << std::endl;
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    int32_t result = proxy->Unregister(UNREGISTER_TOKEN);
    DTEST_LOG << "result:" << result << std::endl;
    EXPECT_EQ(TOKEN_HAS_NOT_REGISTERED, result);
    DTEST_LOG << "ContinationManagerTest UnregisterTest_002 end" << std::endl;
}

/**
 * @tc.name: RegisterDeviceSelectionCallbackTest_001
 * @tc.desc: test register device selection callback
 * @tc.type: FUNC
 * @tc.require: SR000H34KJ
 */
HWTEST_F(ContinationManagerTest, RegisterDeviceSelectionCallbackTest_001, TestSize.Level0)
{
    DTEST_LOG << "ContinationManagerTest RegisterDeviceSelectionCallbackTest_001 start" << std::endl;
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    int32_t token = -1;
    int32_t result1 = proxy->Register(token);
    DTEST_LOG << "result1:" << result1 << std::endl;
    sptr<IRemoteObject> notifier = new DeviceSelectionNotifierTest();
    int32_t result2 = proxy->RegisterDeviceSelectionCallback(token, CALLBACK_TYPE1, notifier);
    DTEST_LOG << "result2:" << result2 << std::endl;
    int32_t result3 = proxy->RegisterDeviceSelectionCallback(token, CALLBACK_TYPE2, notifier);
    DTEST_LOG << "result3:" << result3 << std::endl;
    EXPECT_EQ(ERR_OK, result1);
    EXPECT_EQ(ERR_OK, result2);
    EXPECT_EQ(ERR_OK, result3);
    DTEST_LOG << "ContinationManagerTest RegisterDeviceSelectionCallbackTest_001 end" << std::endl;
}

/**
 * @tc.name: RegisterDeviceSelectionCallbackTest_002
 * @tc.desc: test register device selection callback with invalid token
 * @tc.type: FUNC
 * @tc.require: AR000H34KK
 */
HWTEST_F(ContinationManagerTest, RegisterDeviceSelectionCallbackTest_002, TestSize.Level0)
{
    DTEST_LOG << "ContinationManagerTest RegisterDeviceSelectionCallbackTest_002 start" << std::endl;
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    sptr<IRemoteObject> notifier = new DeviceSelectionNotifierTest();
    int32_t result1 = proxy->RegisterDeviceSelectionCallback(UNREGISTER_TOKEN, CALLBACK_TYPE1, notifier);
    DTEST_LOG << "result1:" << result1 << std::endl;
    int32_t result2 = proxy->RegisterDeviceSelectionCallback(UNREGISTER_TOKEN, CALLBACK_TYPE2, notifier);
    DTEST_LOG << "result2:" << result2 << std::endl;
    EXPECT_EQ(TOKEN_HAS_NOT_REGISTERED, result1);
    EXPECT_EQ(TOKEN_HAS_NOT_REGISTERED, result2);
    DTEST_LOG << "ContinationManagerTest RegisterDeviceSelectionCallbackTest_002 end" << std::endl;
}

/**
 * @tc.name: RegisterDeviceSelectionCallbackTest_003
 * @tc.desc: test register device selection callback with invalid callback type
 * @tc.type: FUNC
 * @tc.require: AR000H34KK
 */
HWTEST_F(ContinationManagerTest, RegisterDeviceSelectionCallbackTest_003, TestSize.Level0)
{
    DTEST_LOG << "ContinationManagerTest RegisterDeviceSelectionCallbackTest_003 start" << std::endl;
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    int32_t token = -1;
    int32_t result1 = proxy->Register(token);
    DTEST_LOG << "result1:" << result1 << std::endl;
    sptr<IRemoteObject> notifier = new DeviceSelectionNotifierTest();
    int32_t result2 = proxy->RegisterDeviceSelectionCallback(token, INVALID_CALLBACK_TYPE, notifier);
    DTEST_LOG << "result2:" << result2 << std::endl;
    EXPECT_EQ(ERR_OK, result1);
    EXPECT_EQ(UNKNOWN_CALLBACK_TYPE, result2);
    DTEST_LOG << "ContinationManagerTest RegisterDeviceSelectionCallbackTest_003 end" << std::endl;
}

/**
 * @tc.name: UnregisterDeviceSelectionCallbackTest_001
 * @tc.desc: test unregister device selection callback
 * @tc.type: FUNC
 * @tc.require: SR000H34KJ
 */
HWTEST_F(ContinationManagerTest, UnregisterDeviceSelectionCallbackTest_001, TestSize.Level0)
{
    DTEST_LOG << "ContinationManagerTest UnregisterDeviceSelectionCallbackTest_001 start" << std::endl;
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    int32_t token = -1;
    int32_t result1 = proxy->Register(token);
    DTEST_LOG << "result1:" << result1 << std::endl;
    sptr<IRemoteObject> notifier = new DeviceSelectionNotifierTest();
    int32_t result2 = proxy->RegisterDeviceSelectionCallback(token, CALLBACK_TYPE1, notifier);
    DTEST_LOG << "result2:" << result2 << std::endl;
    int32_t result3 = proxy->RegisterDeviceSelectionCallback(token, CALLBACK_TYPE2, notifier);
    DTEST_LOG << "result3:" << result3 << std::endl;
    int32_t result4 = proxy->UnregisterDeviceSelectionCallback(token, CALLBACK_TYPE1);
    DTEST_LOG << "result4:" << result4 << std::endl;
    int32_t result5 = proxy->UnregisterDeviceSelectionCallback(token, CALLBACK_TYPE2);
    DTEST_LOG << "result5:" << result5 << std::endl;
    EXPECT_EQ(ERR_OK, result1);
    EXPECT_EQ(ERR_OK, result2);
    EXPECT_EQ(ERR_OK, result3);
    EXPECT_EQ(ERR_OK, result4);
    EXPECT_EQ(ERR_OK, result5);
    DTEST_LOG << "ContinationManagerTest UnregisterDeviceSelectionCallbackTest_001 end" << std::endl;
}

/**
 * @tc.name: UnregisterDeviceSelectionCallbackTest_002
 * @tc.desc: test unregister device selection callback that has not been registered
 * @tc.type: FUNC
 * @tc.require: AR000H34KK
 */
HWTEST_F(ContinationManagerTest, UnregisterDeviceSelectionCallbackTest_002, TestSize.Level0)
{
    DTEST_LOG << "ContinationManagerTest UnregisterDeviceSelectionCallbackTest_002 start" << std::endl;
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    int32_t token = -1;
    int32_t result1 = proxy->Register(token);
    DTEST_LOG << "result1:" << result1 << std::endl;
    int32_t result2 = proxy->UnregisterDeviceSelectionCallback(token, CALLBACK_TYPE1);
    DTEST_LOG << "result2:" << result2 << std::endl;
    int32_t result3 = proxy->UnregisterDeviceSelectionCallback(token, CALLBACK_TYPE2);
    DTEST_LOG << "result3:" << result3 << std::endl;
    EXPECT_EQ(ERR_OK, result1);
    EXPECT_EQ(CALLBACK_HAS_NOT_REGISTERED, result2); 
    EXPECT_EQ(CALLBACK_HAS_NOT_REGISTERED, result3);
    DTEST_LOG << "ContinationManagerTest UnregisterDeviceSelectionCallbackTest_002 end" << std::endl;
}

/**
 * @tc.name: UnregisterDeviceSelectionCallbackTest_003
 * @tc.desc: test register device selection callback with invalid token
 * @tc.type: FUNC
 * @tc.require: AR000H34KK
 */
HWTEST_F(ContinationManagerTest, UnregisterDeviceSelectionCallbackTest_003, TestSize.Level0)
{
    DTEST_LOG << "ContinationManagerTest UnregisterDeviceSelectionCallbackTest_003 start" << std::endl;
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    int32_t result1 = proxy->UnregisterDeviceSelectionCallback(UNREGISTER_TOKEN, CALLBACK_TYPE1);
    DTEST_LOG << "result1:" << result1 << std::endl;
    int32_t result2 = proxy->UnregisterDeviceSelectionCallback(UNREGISTER_TOKEN, CALLBACK_TYPE2);
    DTEST_LOG << "result2:" << result2 << std::endl;
    EXPECT_EQ(TOKEN_HAS_NOT_REGISTERED, result1);
    EXPECT_EQ(TOKEN_HAS_NOT_REGISTERED, result2);
    DTEST_LOG << "ContinationManagerTest UnregisterDeviceSelectionCallbackTest_003 end" << std::endl;
}

/**
 * @tc.name: UnregisterDeviceSelectionCallbackTest_004
 * @tc.desc: test register device selection callback with invalid callback type
 * @tc.type: FUNC
 * @tc.require: AR000H34KK
 */
HWTEST_F(ContinationManagerTest, UnregisterDeviceSelectionCallbackTest_004, TestSize.Level0)
{
    DTEST_LOG << "ContinationManagerTest UnregisterDeviceSelectionCallbackTest_004 start" << std::endl;
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    int32_t token = -1;
    int32_t result1 = proxy->Register(token);
    DTEST_LOG << "result1:" << result1 << std::endl;
    int32_t result2 = proxy->UnregisterDeviceSelectionCallback(token, INVALID_CALLBACK_TYPE);
    DTEST_LOG << "result2:" << result2 << std::endl;
    EXPECT_EQ(ERR_OK, result1);
    EXPECT_EQ(UNKNOWN_CALLBACK_TYPE, result2);
    DTEST_LOG << "ContinationManagerTest UnregisterDeviceSelectionCallbackTest_004 end" << std::endl;
}

/**
 * @tc.name: StartDeviceManagerTest_001
 * @tc.desc: test start device manager
 * @tc.type: FUNC
 * @tc.require: SR000H34KJ
 */
HWTEST_F(ContinationManagerTest, StartDeviceManagerTest_001, TestSize.Level1)
{
    DTEST_LOG << "ContinationManagerTest StartDeviceManagerTest_001 start" << std::endl;
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    int32_t token = -1;
    int32_t result1 = proxy->Register(token);
    DTEST_LOG << "result1:" << result1 << std::endl;
    int32_t result2 = proxy->StartDeviceManager(token);
    DTEST_LOG << "result2:" << result2 << std::endl;
    EXPECT_EQ(ERR_OK, result1);
    DTEST_LOG << "ContinationManagerTest StartDeviceManagerTest_001 end" << std::endl;
}

/**
 * @tc.name: StartDeviceManagerTest_002
 * @tc.desc: test start device manager with extra param
 * @tc.type: FUNC
 * @tc.require: AR000H34KK
 */
HWTEST_F(ContinationManagerTest, StartDeviceManagerTest_002, TestSize.Level1)
{
    DTEST_LOG << "ContinationManagerTest StartDeviceManagerTest_002 start" << std::endl;
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    int32_t token = -1;
    int32_t result1 = proxy->Register(token);
    DTEST_LOG << "result1:" << result1 << std::endl;
    std::shared_ptr<AAFwk::ContinuationExtraParams> continuationExtraParams =
        std::make_shared<AAFwk::ContinuationExtraParams>();
    int32_t result2 = proxy->StartDeviceManager(token, continuationExtraParams);
    DTEST_LOG << "result2:" << result2 << std::endl;
    EXPECT_EQ(ERR_OK, result1);
    DTEST_LOG << "ContinationManagerTest StartDeviceManagerTest_002 end" << std::endl;
}

/**
 * @tc.name: StartDeviceManagerTest_003
 * @tc.desc: test start device manager with invalid token
 * @tc.type: FUNC
 * @tc.require: AR000H34KK
 */
HWTEST_F(ContinationManagerTest, StartDeviceManagerTest_003, TestSize.Level0)
{
    DTEST_LOG << "ContinationManagerTest StartDeviceManagerTest_003 start" << std::endl;
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    int32_t result1 = proxy->StartDeviceManager(UNREGISTER_TOKEN);
    DTEST_LOG << "result1:" << result1 << std::endl;
    std::shared_ptr<AAFwk::ContinuationExtraParams> continuationExtraParams =
        std::make_shared<AAFwk::ContinuationExtraParams>();
    int32_t result2 = proxy->StartDeviceManager(UNREGISTER_TOKEN, continuationExtraParams);
    DTEST_LOG << "result2:" << result2 << std::endl;
    EXPECT_EQ(TOKEN_HAS_NOT_REGISTERED, result1);
    EXPECT_EQ(TOKEN_HAS_NOT_REGISTERED, result2);
    DTEST_LOG << "ContinationManagerTest StartDeviceManagerTest_003 end" << std::endl;
}

/**
 * @tc.name: UpdateConnectStatusTest_001
 * @tc.desc: test update connect status
 * @tc.type: FUNC
 * @tc.require: SR000H34KJ
 */
HWTEST_F(ContinationManagerTest, UpdateConnectStatusTest_001, TestSize.Level0)
{
    DTEST_LOG << "ContinationManagerTest UpdateConnectStatusTest_001 start" << std::endl;
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    int32_t token = -1;
    int32_t result1 = proxy->Register(token);
    DTEST_LOG << "result1:" << result1 << std::endl;
    std::string deviceId = TEST_DEVICE_ID;
    AAFwk::DeviceConnectStatus deviceConnectStatus = AAFwk::DeviceConnectStatus::CONNECTING;
    int32_t result2 = proxy->UpdateConnectStatus(token, deviceId, deviceConnectStatus);
    DTEST_LOG << "result2:" << result2 << std::endl;
    EXPECT_EQ(ERR_OK, result1);
    EXPECT_EQ(ERR_OK, result2);
    DTEST_LOG << "ContinationManagerTest UpdateConnectStatusTest_001 end" << std::endl;
}

/**
 * @tc.name: UpdateConnectStatusTest_002
 * @tc.desc: test update connect status with invalid token
 * @tc.type: FUNC
 * @tc.require: AR000H34KK
 */
HWTEST_F(ContinationManagerTest, UpdateConnectStatusTest_002, TestSize.Level0)
{
    DTEST_LOG << "ContinationManagerTest UpdateConnectStatusTest_002 start" << std::endl;
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    std::string deviceId = TEST_DEVICE_ID;
    AAFwk::DeviceConnectStatus deviceConnectStatus = AAFwk::DeviceConnectStatus::CONNECTING;
    int32_t result = proxy->UpdateConnectStatus(UNREGISTER_TOKEN, deviceId, deviceConnectStatus);
    DTEST_LOG << "result:" << result << std::endl;
    EXPECT_EQ(TOKEN_HAS_NOT_REGISTERED, result);
    DTEST_LOG << "ContinationManagerTest UpdateConnectStatusTest_002 end" << std::endl;
}

/**
 * @tc.name: UpdateConnectStatusTest_003
 * @tc.desc: test update connect status with empty deviceId
 * @tc.type: FUNC
 * @tc.require: AR000H34KK
 */
HWTEST_F(ContinationManagerTest, UpdateConnectStatusTest_003, TestSize.Level0)
{
    DTEST_LOG << "ContinationManagerTest UpdateConnectStatusTest_003 start" << std::endl;
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    int32_t token = -1;
    int32_t result1 = proxy->Register(token);
    DTEST_LOG << "result1:" << result1 << std::endl;
    std::string deviceId = EMPTY_DEVICE_ID;
    AAFwk::DeviceConnectStatus deviceConnectStatus = AAFwk::DeviceConnectStatus::CONNECTING;
    int32_t result2 = proxy->UpdateConnectStatus(token, deviceId, deviceConnectStatus);
    DTEST_LOG << "result2:" << result2 << std::endl;
    EXPECT_EQ(ERR_OK, result1);
    EXPECT_EQ(ERR_NULL_OBJECT, result2);
    DTEST_LOG << "ContinationManagerTest UpdateConnectStatusTest_003 end" << std::endl;
}

/**
 * @tc.name: UpdateConnectStatusTest_004
 * @tc.desc: test update connect status with invalid status
 * @tc.type: FUNC
 * @tc.require: AR000H34KK
 */
HWTEST_F(ContinationManagerTest, UpdateConnectStatusTest_004, TestSize.Level0)
{
    DTEST_LOG << "ContinationManagerTest UpdateConnectStatusTest_004 start" << std::endl;
    sptr<IDistributedSched> proxy = GetDms();
    if (proxy == nullptr) {
        return;
    }
    int32_t token = -1;
    int32_t result1 = proxy->Register(token);
    DTEST_LOG << "result1:" << result1 << std::endl;
    std::string deviceId = TEST_DEVICE_ID;
    int32_t result2 = proxy->UpdateConnectStatus(token, deviceId, static_cast<AAFwk::DeviceConnectStatus>(INVALID_STATUS));
    DTEST_LOG << "result2:" << result2 << std::endl;
    EXPECT_EQ(ERR_OK, result1);
    EXPECT_EQ(INVALID_CONNECT_STATUS, result2);
    DTEST_LOG << "ContinationManagerTest UpdateConnectStatusTest_004 end" << std::endl;
}
} // namespace DistributedSchedule
} // namespace OHOS
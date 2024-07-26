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

#include "dsched_continue_event_test.h"
#include "test_log.h"
#include "cJSON.h"
#include "dtbschedmgr_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedSchedule {

namespace {
    const std::string BASEDIR = "/data/service/el1/public/database/DistributedSchedule";
    const int32_t WAITTIME = 2000;
}
void DSchedContinueEventTest::SetUpTestCase()
{
    mkdir(BASEDIR.c_str(), (S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));
    DTEST_LOG << "DSchedContinueEventTest::SetUpTestCase" << std::endl;
}

void DSchedContinueEventTest::TearDownTestCase()
{
    (void)remove(BASEDIR.c_str());
    DTEST_LOG << "DSchedContinueEventTest::TearDownTestCase" << std::endl;
}

void DSchedContinueEventTest::TearDown()
{
    usleep(WAITTIME);
    DTEST_LOG << "DSchedContinueEventTest::TearDown" << std::endl;
}

void DSchedContinueEventTest::SetUp()
{
    usleep(WAITTIME);
    DTEST_LOG << "DSchedContinueEventTest::SetUp" << std::endl;
}

/**
 * @tc.name: DSchedContinueStartCmdTest_001
 * @tc.desc: test DSchedContinueStartCmd class
 * @tc.type: FUNC
 */
HWTEST_F(DSchedContinueEventTest, DSchedContinueStartCmdTest_001, TestSize.Level0)
{
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueStartCmdTest_001 begin" << std::endl;
    std::shared_ptr<DSchedContinueStartCmd> continueStartCmd = std::make_shared<DSchedContinueStartCmd>();
    std::string jsonStr = "";
    int32_t ret = continueStartCmd->Marshal(jsonStr);
    EXPECT_EQ(ret, ERR_OK);
    ret = continueStartCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, ERR_OK);
    jsonStr = "";
    ret = continueStartCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueStartCmdTest_001 end" << std::endl;
    usleep(WAITTIME);
}

/**
 * @tc.name: DSchedContinueStartCmdTest_002
 * @tc.desc: test DSchedContinueStartCmd class
 * @tc.type: FUNC
 */
HWTEST_F(DSchedContinueEventTest, DSchedContinueStartCmdTest_002, TestSize.Level0)
{
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueStartCmdTest_002 begin" << std::endl;
    std::shared_ptr<DSchedContinueStartCmd> continueStartCmd = std::make_shared<DSchedContinueStartCmd>();
    std::string jsonStr = "";
    int32_t ret = continueStartCmd->Marshal(jsonStr);
    cJSON *rootValue = cJSON_Parse(jsonStr.c_str());
    ASSERT_NE(nullptr, rootValue);
    cJSON_DeleteItemFromObject(rootValue, "BaseCmd");
    char *data = cJSON_Print(rootValue);
    ASSERT_NE(nullptr, data);
    jsonStr = std::string(data);
    cJSON_Delete(rootValue);
    ret = continueStartCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueStartCmdTest_002 end" << std::endl;
    usleep(WAITTIME);
}

/**
 * @tc.name: DSchedContinueStartCmdTest_003
 * @tc.desc: test DSchedContinueStartCmd class
 * @tc.type: FUNC
 */
HWTEST_F(DSchedContinueEventTest, DSchedContinueStartCmdTest_003, TestSize.Level0)
{
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueStartCmdTest_003 begin" << std::endl;
    std::shared_ptr<DSchedContinueStartCmd> continueStartCmd = std::make_shared<DSchedContinueStartCmd>();
    std::string jsonStr = "";
    int32_t ret = continueStartCmd->Marshal(jsonStr);
    cJSON *rootValue = cJSON_Parse(jsonStr.c_str());
    ASSERT_NE(nullptr, rootValue);
    cJSON_DeleteItemFromObject(rootValue, "Direction");
    char *data = cJSON_Print(rootValue);
    ASSERT_NE(nullptr, data);
    jsonStr = std::string(data);
    cJSON_Delete(rootValue);
    ret = continueStartCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueStartCmdTest_003 end" << std::endl;
    usleep(WAITTIME);
}

/**
 * @tc.name: DSchedContinueStartCmdTest_004
 * @tc.desc: test DSchedContinueStartCmd class
 * @tc.type: FUNC
 */
HWTEST_F(DSchedContinueEventTest, DSchedContinueStartCmdTest_004, TestSize.Level0)
{
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueStartCmdTest_004 begin" << std::endl;
    std::shared_ptr<DSchedContinueStartCmd> continueStartCmd = std::make_shared<DSchedContinueStartCmd>();
    std::string jsonStr = "";
    int32_t ret = continueStartCmd->Marshal(jsonStr);
    cJSON *rootValue = cJSON_Parse(jsonStr.c_str());
    ASSERT_NE(nullptr, rootValue);
    cJSON_DeleteItemFromObject(rootValue, "WantParams");
    char *data = cJSON_Print(rootValue);
    ASSERT_NE(nullptr, data);
    jsonStr = std::string(data);
    cJSON_Delete(rootValue);
    ret = continueStartCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueStartCmdTest_004 end" << std::endl;
    usleep(WAITTIME);
}

/**
 * @tc.name: DSchedContinueDataCmdTest_001
 * @tc.desc: test DSchedContinueDataCmd class
 * @tc.type: FUNC
 */
HWTEST_F(DSchedContinueEventTest, DSchedContinueDataCmdTest_001, TestSize.Level0)
{
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueDataCmdTest_001 begin" << std::endl;
    std::shared_ptr<DSchedContinueDataCmd> continueDataCmd = std::make_shared<DSchedContinueDataCmd>();
    std::string jsonStr = "";
    int32_t ret = continueDataCmd->Marshal(jsonStr);
    EXPECT_EQ(ret, ERR_OK);
    ret = continueDataCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, ERR_OK);
    jsonStr = "";
    ret = continueDataCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueDataCmdTest_001 end" << std::endl;
    usleep(WAITTIME);
}

/**
 * @tc.name: DSchedContinueDataCmdTest_002
 * @tc.desc: test DSchedContinueDataCmd class
 * @tc.type: FUNC
 */
HWTEST_F(DSchedContinueEventTest, DSchedContinueDataCmdTest_002, TestSize.Level0)
{
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueDataCmdTest_002 begin" << std::endl;
    std::shared_ptr<DSchedContinueDataCmd> continueDataCmd = std::make_shared<DSchedContinueDataCmd>();
    std::string jsonStr = "";
    int32_t ret = continueDataCmd->MarshalCallerInfo(jsonStr);
    EXPECT_EQ(ret, ERR_OK);
    ret = continueDataCmd->UnmarshalCallerInfo(jsonStr);
    EXPECT_EQ(ret, ERR_OK);
    jsonStr = "";
    ret = continueDataCmd->MarshalAccountInfo(jsonStr);
    EXPECT_EQ(ret, ERR_OK);
    ret = continueDataCmd->UnmarshalAccountInfo(jsonStr);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueDataCmdTest_002 end" << std::endl;
    usleep(WAITTIME);
}

/**
 * @tc.name: DSchedContinueDataCmdTest_003
 * @tc.desc: test DSchedContinueDataCmd class
 * @tc.type: FUNC
 */
HWTEST_F(DSchedContinueEventTest, DSchedContinueDataCmdTest_003, TestSize.Level0)
{
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueDataCmdTest_003 begin" << std::endl;
    std::shared_ptr<DSchedContinueDataCmd> continueDataCmd = std::make_shared<DSchedContinueDataCmd>();
    std::string jsonStr = "";
    int32_t ret = continueDataCmd->Marshal(jsonStr);
    cJSON *rootValue = cJSON_Parse(jsonStr.c_str());
    ASSERT_NE(nullptr, rootValue);
    cJSON_DeleteItemFromObject(rootValue, "BaseCmd");
    char *data = cJSON_Print(rootValue);
    ASSERT_NE(nullptr, data);
    jsonStr = std::string(data);
    ret = continueDataCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    cJSON_AddStringToObject(rootValue, "BaseCmd", "errorInfo");
    data = cJSON_Print(rootValue);
    ASSERT_NE(nullptr, data);
    jsonStr = std::string(data);
    ret = continueDataCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    cJSON_Delete(rootValue);
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueDataCmdTest_003 end" << std::endl;
    usleep(WAITTIME);
}

/**
 * @tc.name: DSchedContinueDataCmdTest_004
 * @tc.desc: test DSchedContinueDataCmd class
 * @tc.type: FUNC
 */
HWTEST_F(DSchedContinueEventTest, DSchedContinueDataCmdTest_004, TestSize.Level0)
{
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueDataCmdTest_004 begin" << std::endl;
    std::shared_ptr<DSchedContinueDataCmd> continueDataCmd = std::make_shared<DSchedContinueDataCmd>();
    std::string jsonStr = "";
    int32_t ret = continueDataCmd->Marshal(jsonStr);
    cJSON *rootValue = cJSON_Parse(jsonStr.c_str());
    ASSERT_NE(nullptr, rootValue);
    cJSON_DeleteItemFromObject(rootValue, "RequestCode");
    char *data = cJSON_Print(rootValue);
    ASSERT_NE(nullptr, data);
    jsonStr = std::string(data);
    cJSON_Delete(rootValue);
    ret = continueDataCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueDataCmdTest_004 end" << std::endl;
    usleep(WAITTIME);
}

/**
 * @tc.name: DSchedContinueDataCmdTest_005
 * @tc.desc: test DSchedContinueDataCmd class
 * @tc.type: FUNC
 */
HWTEST_F(DSchedContinueEventTest, DSchedContinueDataCmdTest_005, TestSize.Level0)
{
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueDataCmdTest_005 begin" << std::endl;
    std::shared_ptr<DSchedContinueDataCmd> continueDataCmd = std::make_shared<DSchedContinueDataCmd>();
    std::string jsonStr = "";
    int32_t ret = continueDataCmd->Marshal(jsonStr);
    cJSON *rootValue = cJSON_Parse(jsonStr.c_str());
    ASSERT_NE(nullptr, rootValue);
    cJSON_DeleteItemFromObject(rootValue, "CallerInfo");
    char *data = cJSON_Print(rootValue);
    ASSERT_NE(nullptr, data);
    jsonStr = std::string(data);
    ret = continueDataCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    cJSON_AddStringToObject(rootValue, "CallerInfo", "errorInfo");
    data = cJSON_Print(rootValue);
    ASSERT_NE(nullptr, data);
    jsonStr = std::string(data);
    ret = continueDataCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    cJSON_Delete(rootValue);
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueDataCmdTest_005 end" << std::endl;
    usleep(WAITTIME);
}

/**
 * @tc.name: DSchedContinueDataCmdTest_006
 * @tc.desc: test DSchedContinueDataCmd class
 * @tc.type: FUNC
 */
HWTEST_F(DSchedContinueEventTest, DSchedContinueDataCmdTest_006, TestSize.Level0)
{
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueDataCmdTest_006 begin" << std::endl;
    std::shared_ptr<DSchedContinueDataCmd> continueDataCmd = std::make_shared<DSchedContinueDataCmd>();
    std::string jsonStr = "";
    int32_t ret = continueDataCmd->Marshal(jsonStr);
    cJSON *rootValue = cJSON_Parse(jsonStr.c_str());
    ASSERT_NE(nullptr, rootValue);
    cJSON_DeleteItemFromObject(rootValue, "AccountInfo");
    char *data = cJSON_Print(rootValue);
    ASSERT_NE(nullptr, data);
    jsonStr = std::string(data);
    ret = continueDataCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    cJSON_AddStringToObject(rootValue, "AccountInfo", "errorInfo");
    data = cJSON_Print(rootValue);
    ASSERT_NE(nullptr, data);
    jsonStr = std::string(data);
    ret = continueDataCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    cJSON_Delete(rootValue);
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueDataCmdTest_006 end" << std::endl;
    usleep(WAITTIME);
}

/**
 * @tc.name: DSchedContinueReplyCmdTest_001
 * @tc.desc: test DSchedContinueReplyCmd class
 * @tc.type: FUNC
 */
HWTEST_F(DSchedContinueEventTest, DSchedContinueReplyCmdTest_001, TestSize.Level0)
{
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueReplyCmdTest_001 begin" << std::endl;
    std::shared_ptr<DSchedContinueReplyCmd> continueReplyCmd = std::make_shared<DSchedContinueReplyCmd>();
    std::string jsonStr = "";
    int32_t ret = continueReplyCmd->Marshal(jsonStr);
    EXPECT_EQ(ret, ERR_OK);
    ret = continueReplyCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, ERR_OK);
    jsonStr = "";
    ret = continueReplyCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueReplyCmdTest_001 end" << std::endl;
    usleep(WAITTIME);
}

/**
 * @tc.name: DSchedContinueReplyCmdTest_002
 * @tc.desc: test DSchedContinueReplyCmd class
 * @tc.type: FUNC
 */
HWTEST_F(DSchedContinueEventTest, DSchedContinueReplyCmdTest_002, TestSize.Level0)
{
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueReplyCmdTest_002 begin" << std::endl;
    std::shared_ptr<DSchedContinueReplyCmd> continueReplyCmd = std::make_shared<DSchedContinueReplyCmd>();
    std::string jsonStr = "";
    int32_t ret = continueReplyCmd->Marshal(jsonStr);
    cJSON *rootValue = cJSON_Parse(jsonStr.c_str());
    ASSERT_NE(nullptr, rootValue);
    cJSON_DeleteItemFromObject(rootValue, "BaseCmd");
    char *data = cJSON_Print(rootValue);
    ASSERT_NE(nullptr, data);
    jsonStr = std::string(data);
    ret = continueReplyCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    cJSON_AddStringToObject(rootValue, "BaseCmd", "errorInfo");
    data = cJSON_Print(rootValue);
    ASSERT_NE(nullptr, data);
    jsonStr = std::string(data);
    ret = continueReplyCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    cJSON_Delete(rootValue);
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueReplyCmdTest_002 end" << std::endl;
    usleep(WAITTIME);
}

/**
 * @tc.name: DSchedContinueReplyCmdTest_003
 * @tc.desc: test DSchedContinueReplyCmd class
 * @tc.type: FUNC
 */
HWTEST_F(DSchedContinueEventTest, DSchedContinueReplyCmdTest_003, TestSize.Level0)
{
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueReplyCmdTest_003 begin" << std::endl;
    std::shared_ptr<DSchedContinueReplyCmd> continueReplyCmd = std::make_shared<DSchedContinueReplyCmd>();
    std::string jsonStr = "";
    int32_t ret = continueReplyCmd->Marshal(jsonStr);
    cJSON *rootValue = cJSON_Parse(jsonStr.c_str());
    ASSERT_NE(nullptr, rootValue);
    cJSON_DeleteItemFromObject(rootValue, "AppVersion");
    char *data = cJSON_Print(rootValue);
    ASSERT_NE(nullptr, data);
    jsonStr = std::string(data);
    cJSON_Delete(rootValue);
    ret = continueReplyCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueReplyCmdTest_003 end" << std::endl;
    usleep(WAITTIME);
}

/**
 * @tc.name: DSchedContinueReplyCmdTest_004
 * @tc.desc: test DSchedContinueReplyCmd class
 * @tc.type: FUNC
 */
HWTEST_F(DSchedContinueEventTest, DSchedContinueReplyCmdTest_004, TestSize.Level0)
{
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueReplyCmdTest_004 begin" << std::endl;
    std::shared_ptr<DSchedContinueReplyCmd> continueReplyCmd = std::make_shared<DSchedContinueReplyCmd>();
    std::string jsonStr = "";
    int32_t ret = continueReplyCmd->Marshal(jsonStr);
    cJSON *rootValue = cJSON_Parse(jsonStr.c_str());
    ASSERT_NE(nullptr, rootValue);
    cJSON_DeleteItemFromObject(rootValue, "Reason");
    char *data = cJSON_Print(rootValue);
    ASSERT_NE(nullptr, data);
    jsonStr = std::string(data);
    cJSON_Delete(rootValue);
    ret = continueReplyCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueReplyCmdTest_004 end" << std::endl;
    usleep(WAITTIME);
}

/**
 * @tc.name: DSchedContinueEndCmdTest_001
 * @tc.desc: test DSchedContinueEndCmd class
 * @tc.type: FUNC
 */
HWTEST_F(DSchedContinueEventTest, DSchedContinueEndCmdTest_001, TestSize.Level0)
{
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueEndCmdTest_001 begin" << std::endl;
    std::shared_ptr<DSchedContinueReplyCmd> continueReplyCmd = std::make_shared<DSchedContinueReplyCmd>();
    std::string jsonStr = "";
    int32_t ret = continueReplyCmd->Marshal(jsonStr);
    EXPECT_EQ(ret, ERR_OK);
    ret = continueReplyCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, ERR_OK);
    jsonStr = "";
    ret = continueReplyCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueEndCmdTest_001 end" << std::endl;
    usleep(WAITTIME);
}

/**
 * @tc.name: DSchedContinueEndCmdTest_002
 * @tc.desc: test DSchedContinueEndCmd class
 * @tc.type: FUNC
 */
HWTEST_F(DSchedContinueEventTest, DSchedContinueEndCmdTest_002, TestSize.Level0)
{
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueEndCmdTest_002 begin" << std::endl;
    std::shared_ptr<DSchedContinueEndCmd> continueEndCmd = std::make_shared<DSchedContinueEndCmd>();
    std::string jsonStr = "";
    int32_t ret = continueEndCmd->Marshal(jsonStr);
    cJSON *rootValue = cJSON_Parse(jsonStr.c_str());
    ASSERT_NE(nullptr, rootValue);
    cJSON_DeleteItemFromObject(rootValue, "BaseCmd");
    char *data = cJSON_Print(rootValue);
    ASSERT_NE(nullptr, data);
    jsonStr = std::string(data);
    ret = continueEndCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    cJSON_AddStringToObject(rootValue, "BaseCmd", "errorInfo");
    data = cJSON_Print(rootValue);
    ASSERT_NE(nullptr, data);
    jsonStr = std::string(data);
    ret = continueEndCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    cJSON_Delete(rootValue);
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueEndCmdTest_002 end" << std::endl;
    usleep(WAITTIME);
}

/**
 * @tc.name: DSchedContinueEndCmdTest_003
 * @tc.desc: test DSchedContinueEndCmd class
 * @tc.type: FUNC
 */
HWTEST_F(DSchedContinueEventTest, DSchedContinueEndCmdTest_003, TestSize.Level0)
{
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueEndCmdTest_003 begin" << std::endl;
    std::shared_ptr<DSchedContinueEndCmd> continueEndCmd = std::make_shared<DSchedContinueEndCmd>();
    std::string jsonStr = "";
    int32_t ret = continueEndCmd->Marshal(jsonStr);
    cJSON *rootValue = cJSON_Parse(jsonStr.c_str());
    ASSERT_NE(nullptr, rootValue);
    cJSON_DeleteItemFromObject(rootValue, "Result");
    char *data = cJSON_Print(rootValue);
    ASSERT_NE(nullptr, data);
    jsonStr = std::string(data);
    cJSON_Delete(rootValue);
    ret = continueEndCmd->Unmarshal(jsonStr);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "DSchedContinueEventTest DSchedContinueEndCmdTest_003 end" << std::endl;
    usleep(WAITTIME);
}
}
}
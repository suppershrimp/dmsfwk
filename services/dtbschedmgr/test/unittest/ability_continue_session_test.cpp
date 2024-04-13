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

#include "ability_continue_session_test.h"

#include "test_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedSchedule {

void AbilityContinueSessionTest::SetUpTestCase()
{
    DTEST_LOG << "AbilityContinueSessionTest::SetUpTestCase" << std::endl;
}

void AbilityContinueSessionTest::TearDownTestCase()
{
    DTEST_LOG << "AbilityContinueSessionTest::TearDownTestCase" << std::endl;
}

void AbilityContinueSessionTest::TearDown()
{
    DTEST_LOG << "AbilityContinueSessionTest::TearDown" << std::endl;
}

void AbilityContinueSessionTest::SetUp()
{
    DTEST_LOG << "AbilityContinueSessionTest::SetUp" << std::endl;
}

/**
 * @tc.name: UpdateContinueSessionId_001
 * @tc.desc: generated continueSessionId by bundleName and abilityName
 * @tc.type: FUNC
 * @tc.require: SR20240116512594
 */
HWTEST_F(AbilityContinueSessionTest, UpdateContinueSessionId_001, TestSize.Level3)
{
    DTEST_LOG << "AbilityContinueSessionTest UpdateContinueSessionId_001 begin" << std::endl;
    std::string bundleName = "ohos.dms.bundleName_001";
    std::string abilityName = "ohos.dms.abilityName_001";
    AbilityContinueSession::GetInstance().UpdateContinueSessionId("bundleName", "abilityName");
    std::string continueSessionId = AbilityContinueSession::GetInstance().GetContinueSessionId();
    EXPECT_NE(continueSessionId.length(), 0);
    DTEST_LOG << "AbilityContinueSessionTest UpdateContinueSessionId_001 end" << std::endl;
}

/**
 * @tc.name: GetPersistentId_001
 * @tc.desc: generated continueSessionId by bundleName and abilityName
 * @tc.type: FUNC
 * @tc.require: SR20240116512594
 */
HWTEST_F(AbilityContinueSessionTest, GetPersistentId_001, TestSize.Level3)
{
    DTEST_LOG << "AbilityContinueSessionTest GetPersistentId_001 begin" << std::endl;
    int32_t persistentId;
    int32_t err = AbilityContinueSession::GetInstance().GetPersistentId(persistentId);
    EXPECT_NE(INVALID_PARAMETERS_ERR, err);
    DTEST_LOG << "AbilityContinueSessionTest UpdateContinueSessionId_001 end" << std::endl;
}
}
}
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

#include "distributed_ability_manager_service_test.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "test_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedSchedule {
namespace {
}

void DistributedAbilityManagerServiceTest::SetUpTestCase()
{
    DTEST_LOG << "DistributedAbilityManagerServiceTest::SetUpTestCase" << std::endl;
}

void DistributedAbilityManagerServiceTest::TearDownTestCase()
{
    DTEST_LOG << "DistributedAbilityManagerServiceTest::TearDownTestCase" << std::endl;
}

void DistributedAbilityManagerServiceTest::TearDown()
{
    DTEST_LOG << "DistributedAbilityManagerServiceTest::TearDown" << std::endl;
}

void DistributedAbilityManagerServiceTest::SetUp()
{
    DTEST_LOG << "DistributedAbilityManagerServiceTest::SetUp" << std::endl;
}

/**
 * @tc.name: DistributedAbilityManagerServiceTest_001
 * @tc.desc: check whether the dms systemability exists
 * @tc.type: FUNC
 */
HWTEST_F(DistributedAbilityManagerServiceTest, DistributedAbilityManagerServiceTest_001, TestSize.Level2)
{
    DTEST_LOG << "DistributedAbilityManagerServiceTest_001 begin" << std::endl;
    auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_TRUE(sm != nullptr);
    if (sm == nullptr) {
        DTEST_LOG << "DistributedAbilityManagerServiceTest sm is nullptr" << std::endl;
        return;
    }
    DTEST_LOG << "DistributedAbilityManagerServiceTest sm is not nullptr" << std::endl;
    auto distributedObject = sm->GetSystemAbility(DISTRIBUTED_SCHED_SA_ID);
    EXPECT_EQ(distributedObject != nullptr, true);
}
}
}

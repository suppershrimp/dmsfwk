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

#include "distributed_sched_dumper_test.h"
#include "test_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedSchedule {
void DistributedSchedDumperTest::SetUpTestCase()
{
    DTEST_LOG << "DistributedSchedDumperTest::SetUpTestCase" << std::endl;
}

void DistributedSchedDumperTest::TearDownTestCase()
{
    DTEST_LOG << "DistributedSchedDumperTest::TearDownTestCase" << std::endl;
}

void DistributedSchedDumperTest::TearDown()
{
    DTEST_LOG << "DistributedSchedDumperTest::TearDown" << std::endl;
}

void DistributedSchedDumperTest::SetUp()
{
    DTEST_LOG << "DistributedSchedDumperTest::SetUp" << std::endl;
}

/**
 * @tc.name: DumpDefault_001
 * @tc.desc: dump default info
 * @tc.type: FUNC
 * @tc.require: I60TOK
 */
HWTEST_F(DistributedSchedDumperTest, DumpDefault_001, TestSize.Level3)
{
    DTEST_LOG << "DistributedSchedDumperTest DumpDefault_001 begin" << std::endl;
    std::string result = "";
    bool res = DistributedSchedDumper::DumpDefault(result);
    EXPECT_EQ(res, true);
    DTEST_LOG << "DistributedSchedDumperTest DumpDefault_001 end" << std::endl;
}

/**
 * @tc.name: Dump_001
 * @tc.desc: dump info with null arg
 * @tc.type: FUNC
 * @tc.require: I60TOK
 */
HWTEST_F(DistributedSchedDumperTest, Dump_001, TestSize.Level3)
{
    DTEST_LOG << "DistributedSchedDumperTest Dump_001 begin" << std::endl;
    const std::vector<std::string> args = {};
    std::string result = "";
    bool res = DistributedSchedDumper::Dump(args, result);
    EXPECT_EQ(res, true);
    DTEST_LOG << "DistributedSchedDumperTest Dump_001 end" << std::endl;
}

/**
 * @tc.name: Dump_002
 * @tc.desc: dump info with a correct arg
 * @tc.type: FUNC
 * @tc.require: I60TOK
 */
HWTEST_F(DistributedSchedDumperTest, Dump_002, TestSize.Level3)
{
    DTEST_LOG << "DistributedSchedDumperTest Dump_002 begin" << std::endl;
    const std::vector<std::string> args = {"-connect"};
    std::string result = "";
    bool res = DistributedSchedDumper::Dump(args, result);
    EXPECT_EQ(res, true);
    DTEST_LOG << "DistributedSchedDumperTest Dump_002 end" << std::endl;
}

/**
 * @tc.name: Dump_003
 * @tc.desc: dump info with a invalid arg
 * @tc.type: FUNC
 * @tc.require: I60TOK
 */
HWTEST_F(DistributedSchedDumperTest, Dump_003, TestSize.Level3)
{
    DTEST_LOG << "DistributedSchedDumperTest Dump_003 begin" << std::endl;
    const std::vector<std::string> args = {"-invalid"};
    std::string result = "";
    bool res = DistributedSchedDumper::Dump(args, result);
    EXPECT_EQ(res, false);
    DTEST_LOG << "DistributedSchedDumperTest Dump_003 end" << std::endl;
}

/**
 * @tc.name: Dump_004
 * @tc.desc: dump info with two args
 * @tc.type: FUNC
 * @tc.require: I60TOK
 */
HWTEST_F(DistributedSchedDumperTest, Dump_004, TestSize.Level3)
{
    DTEST_LOG << "DistributedSchedDumperTest Dump_004 begin" << std::endl;
    const std::vector<std::string> args = {"-invalid1", "-invalid2"};
    std::string result = "";
    bool res = DistributedSchedDumper::Dump(args, result);
    EXPECT_EQ(res, false);
    DTEST_LOG << "DistributedSchedDumperTest Dump_004 end" << std::endl;
}
}
}
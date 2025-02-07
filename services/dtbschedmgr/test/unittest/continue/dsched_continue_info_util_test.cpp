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

#include <vector>
#include "dsched_continue_info_util_test.h"
#include "dtbschedmgr_log.h"
#include "test_log.h"
#include "mission/distributed_bm_storage.h"
#include "continue/util/dsched_continue_info_util.h"
#include "bundle/bundle_manager_internal.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedSchedule {
const std::string TEST_SOURCE_NETWORK_ID = "networkId1";
const std::string TEST_INVALID_SOURCE_NETWORK_ID = "networkIdXXXXX";
const std::string TEST_BUNDLE_NAME = "bundleName";
const std::string TEST_EMPTY_CONTINUE_BUNDLE_BUNDLE_NAME = "bundleNameNoContinueBundle";
const std::string TEST_SINK_NOT_EXIST_BUNDLE_NAME_NAME = "bundleNameSinkNotExist";
const uint16_t TEST_BUNDLE_NAME_ID = 0;
const uint16_t TEST_EMPTY_CONTINUE_BUNDLE_BUNDLE_ID = 1;
const uint16_t TEST_SINK_NOT_EXIST_BUNDLE_NAME_ID = 2;

const uint8_t TEST_CONTINUE_TYPE_ID = 0;
const uint8_t TEST_INVALID_CONTINUE_TYPE_ID = 1;
const int32_t DBMS_RETRY_MAX_TIME = 5;
namespace {
    const std::string TAG = "DSchedContinueInfoUtilTest";
}
void DSchedContinueInfoUtilTest::SetUpTestCase()
{
}

void DSchedContinueInfoUtilTest::TearDownTestCase()
{
}

void DSchedContinueInfoUtilTest::TearDown()
{
}

void DSchedContinueInfoUtilTest::SetUp()
{
}

bool DmsBmStorage::GetDistributedBundleInfo(const std::string &networkId,
    const uint16_t &bundleNameId, DmsBundleInfo &distributeBundleInfo)
{
    if(networkId == TEST_INVALID_SOURCE_NETWORK_ID){
        return false;
    }
    if(bundleNameId == TEST_EMPTY_CONTINUE_BUNDLE_BUNDLE_ID){
        distributeBundleInfo.bundleName = TEST_EMPTY_CONTINUE_BUNDLE_BUNDLE_NAME;
    }else if(bundleNameId == TEST_SINK_NOT_EXIST_BUNDLE_NAME_ID){
        distributeBundleInfo.bundleName = TEST_SINK_NOT_EXIST_BUNDLE_NAME_NAME;
    }else{
        distributeBundleInfo.bundleName = TEST_BUNDLE_NAME;
    }
    distributeBundleInfo.developerId = "developerId";
    distributeBundleInfo.bundleNameId = bundleNameId;
    std::vector<DmsAbilityInfo> dmsAbilityInfos;
    DmsAbilityInfo dmsAbilityInfo;
    dmsAbilityInfo.abilityName = "abilityName";
    dmsAbilityInfo.continueType.push_back("continueType");
    dmsAbilityInfo.continueTypeId.push_back(0);
    dmsAbilityInfo.continueBundleName.push_back("bundleName3");
    dmsAbilityInfo.continueBundleName.push_back("bundleName2");
    dmsAbilityInfo.continueBundleName.push_back("bundleName1");
    dmsAbilityInfo.continueBundleName.push_back(TEST_BUNDLE_NAME);
    dmsAbilityInfos.push_back(dmsAbilityInfo);
    distributeBundleInfo.dmsAbilityInfos = dmsAbilityInfos;
    return true;
}

bool GetDistributedBundleInfo(const std::string &networkId, const std::string &bundleName,
    DmsBundleInfo &distributeBundleInfo){
    if(networkId == TEST_INVALID_SOURCE_NETWORK_ID){
        return false;
    }
    distributeBundleInfo.bundleName = bundleName;
    distributeBundleInfo.developerId = "developerId";
    distributeBundleInfo.bundleNameId = 0;
    std::vector<DmsAbilityInfo> dmsAbilityInfos;
    DmsAbilityInfo dmsAbilityInfo;
    dmsAbilityInfo.abilityName = "abilityName";
    dmsAbilityInfo.continueType.push_back("continueType");
    dmsAbilityInfo.continueTypeId.push_back(0);
    dmsAbilityInfo.moduleName = "moduleName";
    dmsAbilityInfo.continueBundleName.push_back("bundleName3");
    dmsAbilityInfo.continueBundleName.push_back("bundleName2");
    dmsAbilityInfo.continueBundleName.push_back("bundleName1");
    dmsAbilityInfo.continueBundleName.push_back(bundleName);
    dmsAbilityInfos.push_back(dmsAbilityInfo);
    distributeBundleInfo.dmsAbilityInfos = dmsAbilityInfos;
    return true;
}

bool BundleManagerInternal::GetContinueBundle4Src(const std::string &srcBundleName,
                                                  std::vector<std::string> &bundleNameList)
{
    if(srcBundleName == TEST_BUNDLE_NAME){
        bundleNameList.push_back("bundleName6");
        bundleNameList.push_back("bundleName5");
        bundleNameList.push_back("bundleName1");
        bundleNameList.push_back(TEST_BUNDLE_NAME);
    }
    return true;
}

int32_t BundleManagerInternal::GetLocalBundleInfo(const std::string& bundleName,
    AppExecFwk::BundleInfo &localBundleInfo)
{
    if(bundleName == TEST_SINK_NOT_EXIST_BUNDLE_NAME_NAME){
        return INVALID_PARAMETERS_ERR;
    }
    AppExecFwk::AbilityInfo abilityInfo3;
    abilityInfo3.moduleName = "moduleName3";
    abilityInfo3.name = "abilityName3";
    abilityInfo3.continueType.push_back("continueType3");
    localBundleInfo.abilityInfos.push_back(abilityInfo3);

    AppExecFwk::AbilityInfo abilityInfo2;
    abilityInfo2.moduleName = "moduleName2";
    abilityInfo2.name = "abilityName2";
    abilityInfo2.continueType.push_back("continueType2");
    localBundleInfo.abilityInfos.push_back(abilityInfo2);

    AppExecFwk::AbilityInfo abilityInfo;
    abilityInfo.moduleName = "moduleName";
    abilityInfo.name = "abilityName";
    abilityInfo.continueType.push_back("continueType");
    localBundleInfo.abilityInfos.push_back(abilityInfo);

    return ERR_OK;
}

bool BundleManagerInternal::GetAppProvisionInfo4CurrentUser(const std::string &bundleName,
    AppExecFwk::AppProvisionInfo &appProvisionInfo)
{
    appProvisionInfo.developerId = "developerId";
    return true;
}


    /**
 * @tc.name: CompleteContinueInfoTest_001
 * @tc.desc: DSchedContinueInfoUtil
 * @tc.type: FUNC
 */
HWTEST_F(DSchedContinueInfoUtilTest, CompleteContinueInfoTest_001, TestSize.Level0)
{
    DTEST_LOG << "DSchedContinueTest CompleteContinueInfoTest_001 begin" << std::endl;
    DSchedContinueInfo continueInfo;

    // max retry time = false
    bool ret = DSchedContinueInfoUtil::GetInstance().CompleteContinueInfo(
            TEST_SOURCE_NETWORK_ID, TEST_BUNDLE_NAME_ID, TEST_CONTINUE_TYPE_ID,continueInfo, DBMS_RETRY_MAX_TIME);
    EXPECT_FALSE(ret);

    // invalid networkId = false
    ret = DSchedContinueInfoUtil::GetInstance().CompleteContinueInfo(
            TEST_INVALID_SOURCE_NETWORK_ID, TEST_BUNDLE_NAME_ID, TEST_CONTINUE_TYPE_ID, continueInfo);
    EXPECT_FALSE(ret);

    // no matched continueTypeId complete src = false
    ret = DSchedContinueInfoUtil::GetInstance().CompleteContinueInfo(
            TEST_SOURCE_NETWORK_ID, TEST_BUNDLE_NAME_ID, TEST_INVALID_CONTINUE_TYPE_ID,continueInfo);
    EXPECT_FALSE(ret);

    // no continue bundleNames = false
    ret = DSchedContinueInfoUtil::GetInstance().CompleteContinueInfo(
            TEST_SOURCE_NETWORK_ID, TEST_EMPTY_CONTINUE_BUNDLE_BUNDLE_ID, TEST_CONTINUE_TYPE_ID,continueInfo);
    EXPECT_TRUE(ret);

    // sink not exist src bundleName = false
    ret = DSchedContinueInfoUtil::GetInstance().CompleteContinueInfo(
            TEST_SOURCE_NETWORK_ID, TEST_SINK_NOT_EXIST_BUNDLE_NAME_ID, TEST_CONTINUE_TYPE_ID,continueInfo);
    EXPECT_FALSE(ret);

    //
    ret = DSchedContinueInfoUtil::GetInstance().CompleteContinueInfo(
            TEST_SOURCE_NETWORK_ID, TEST_BUNDLE_NAME_ID, TEST_CONTINUE_TYPE_ID,continueInfo);
    EXPECT_TRUE(ret);
    DTEST_LOG << "DSchedContinueTest CompleteContinueInfoTest_001 end ret:" << ret << std::endl;
}
}
}

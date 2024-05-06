/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#define private public
#define protected public
#include "array_wrapper.h"
#include "base_object.h"
#include "bool_wrapper.h"
#include "byte_wrapper.h"
#include "distributed_want_params.h"
#include "distributed_want_params_wrapper.h"
#include "double_wrapper.h"
#include "float_wrapper.h"
#include "int_wrapper.h"
#include "long_wrapper.h"
#include "short_wrapper.h"
#include "string_wrapper.h"
#include "test_log.h"
#include "zchar_wrapper.h"
#undef private
#undef protected

using namespace testing::ext;
using namespace OHOS;
using namespace AAFwk;
using namespace DistributedSchedule;
using OHOS::Parcel;
class DistributedWantParamsBaseTest : public testing::Test {
public:
    DistributedWantParamsBaseTest()
    {}
    ~DistributedWantParamsBaseTest()
    {}
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<DistributedWantParams> wantParamsIn_ = nullptr;
    std::shared_ptr<DistributedWantParams> wantParamsOut_ = nullptr;
    static sptr<DistributedWantParams> distributedWantParams_;
};

sptr<DistributedWantParams> DistributedWantParamsBaseTest::distributedWantParams_;

void DistributedWantParamsBaseTest::SetUpTestCase(void)
{
    distributedWantParams_ = new DistributedWantParams();
}

void DistributedWantParamsBaseTest::TearDownTestCase(void)
{}

void DistributedWantParamsBaseTest::SetUp(void)
{
    wantParamsIn_ = std::make_shared<DistributedWantParams>();
    wantParamsOut_ = std::make_shared<DistributedWantParams>();
}

void DistributedWantParamsBaseTest::TearDown(void)
{
}

/**
 * @tc.number: DistributedWantParams_Parcelable_0100
 * @tc.name: Marshalling/Unmarshalling
 * @tc.desc: marshalling WantParams, and then check result.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_Parcelable_0100, Function | MediumTest | Level3)
{
    std::string keyStr = "12345667";
    std::string valueStr = "sdasdfdsffdgfdg";
    wantParamsIn_->SetParam(keyStr, String::Box(valueStr));
    Parcel in;
    if (wantParamsOut_ != nullptr) {
        wantParamsIn_->Marshalling(in);
        std::shared_ptr<DistributedWantParams> wantParamsOut_(DistributedWantParams::Unmarshalling(in));
        EXPECT_EQ(valueStr, String::Unbox(IString::Query(wantParamsOut_->GetParam(keyStr))));
    }
}

/**
 * @tc.number: DistributedWantParams_Parcelable_0200
 * @tc.name: Marshalling/Unmarshalling
 * @tc.desc: marshalling WantParams, and then check result.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_Parcelable_0200, Function | MediumTest | Level3)
{
    std::string keyStr = "12345667";
    bool valueBool = true;
    wantParamsIn_->SetParam(keyStr, Boolean::Box(valueBool));

    Parcel in;
    if (wantParamsOut_ != nullptr) {
        wantParamsIn_->Marshalling(in);
        std::shared_ptr<DistributedWantParams> wantParamsOut_(DistributedWantParams::Unmarshalling(in));
        EXPECT_EQ(valueBool, Boolean::Unbox(IBoolean::Query(wantParamsOut_->GetParam(keyStr))));
    }
}

/**
 * @tc.number: DistributedWantParams_Parcelable_0300
 * @tc.name: Marshalling/Unmarshalling
 * @tc.desc: marshalling WantParams, and then check result.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_Parcelable_0300, Function | MediumTest | Level3)
{
    std::string keyStr = "12345667";
    int valueInteger = 12345;
    wantParamsIn_->SetParam(keyStr, Integer::Box(valueInteger));
    Integer::Unbox(IInteger::Query(wantParamsIn_->GetParam(keyStr)));

    Parcel in;
    wantParamsIn_->Marshalling(in);
    std::shared_ptr<DistributedWantParams> wantParamsOut_(DistributedWantParams::Unmarshalling(in));
    if (wantParamsOut_ != nullptr) {
        int right = Integer::Unbox(IInteger::Query(wantParamsOut_->GetParam(keyStr)));
        EXPECT_EQ(valueInteger, right);
        wantParamsOut_ = nullptr;
    }
}

/**
 * @tc.number: DistributedWantParams_Parcelable_0400
 * @tc.name: Marshalling/Unmarshalling
 * @tc.desc: marshalling WantParams, and then check result.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_Parcelable_0400, Function | MediumTest | Level3)
{
    std::string keyStr = "12345667";
    long valueLong = 1234567;
    wantParamsIn_->SetParam(keyStr, Long::Box(valueLong));

    Parcel in;
    wantParamsIn_->Marshalling(in);
    std::shared_ptr<DistributedWantParams> wantParamsOut_(DistributedWantParams::Unmarshalling(in));
    std::string outString(String::Unbox(IString::Query(wantParamsOut_->GetParam(keyStr))));
    EXPECT_STREQ(std::to_string(valueLong).c_str(), outString.c_str());
}

/**
 * @tc.number: DistributedWantParams_Parcelable_0700
 * @tc.name: Marshalling/Unmarshalling
 * @tc.desc: marshalling array, and then check result.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_Parcelable_0700, Function | MediumTest | Level3)
{
    sptr<AAFwk::IArray> ao = new (std::nothrow) AAFwk::Array(2, AAFwk::g_IID_IString);
    std::string valueStr0 = "TestValue0";
    ao->Set(0, String::Box(valueStr0));
    std::string valueStr1 = "TestValue1";
    ao->Set(1, String::Box(valueStr1));

    DistributedWantParams l1;
    l1.SetParam("l1", ao);
    wantParamsIn_->SetParam("l2",  DistributedWantParamWrapper::Box(l1));

    Parcel in;
    wantParamsIn_->Marshalling(in);
    std::shared_ptr<DistributedWantParams> wantParamsOut_(DistributedWantParams::Unmarshalling(in));
    DistributedWantParams l1Out =
        DistributedWantParamWrapper::Unbox(IDistributedWantParams::Query(wantParamsOut_->GetParam("l2")));
    IArray *aoOut = IArray::Query(l1Out.GetParam("l1"));

    long size;
    aoOut->GetLength(size);
    EXPECT_EQ(size, 2);
    sptr<IInterface> str0;
    sptr<IInterface> str1;
    aoOut->Get(0, str0);
    aoOut->Get(1, str1);

    EXPECT_EQ(valueStr0, String::Unbox(IString::Query(str0)));
    EXPECT_EQ(valueStr1, String::Unbox(IString::Query(str1)));
}

/**
 * @tc.number: DistributedWantParams_GetStringByType_0200
 * @tc.name: GetStringByType
 * @tc.desc: Test get bool with invalid type in the WantParam.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetStringByType_0200, Function | MediumTest | Level3)
{
    const std::string value = "true";
    sptr<IInterface> boolObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_BOOLEAN, value);
    std::string stringVal =
        DistributedWantParams::GetStringByType(boolObj, DistributedWantParams::VALUE_TYPE_BOOLEAN);
    EXPECT_EQ(stringVal, "true");
}

/**
 * @tc.number: DistributedWantParams_GetStringByType_0300
 * @tc.name: GetStringByType
 * @tc.desc: Test get string with invalid type in the WantParam.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetStringByType_0300, Function | MediumTest | Level3)
{
    const std::string value = "string";
    sptr<IInterface> stringObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_STRING, value);
    std::string stringVal =
        DistributedWantParams::GetStringByType(stringObj, DistributedWantParams::VALUE_TYPE_STRING);
    EXPECT_EQ(stringVal, "string");
}

/**
 * @tc.number: DistributedWantParams_GetStringByType_0400
 * @tc.name: GetStringByType
 * @tc.desc: Test get byte with invalid type in the WantParam.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetStringByType_0400, Function | MediumTest | Level3)
{
    const std::string value = "129";
    sptr<IInterface> byteObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_BYTE, value);
    std::string byteVal = DistributedWantParams::GetStringByType(byteObj, DistributedWantParams::VALUE_TYPE_BYTE);
    EXPECT_EQ(byteVal, "129");
}

/**
 * @tc.number: DistributedWantParams_GetStringByType_0500
 * @tc.name: GetStringByType
 * @tc.desc: Test get char with invalid type in the WantParam.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetStringByType_0500, Function | MediumTest | Level3)
{
    const std::string value = "I";
    sptr<IInterface> charObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_CHAR, value);
    std::string charVal = DistributedWantParams::GetStringByType(charObj, DistributedWantParams::VALUE_TYPE_CHAR);
    EXPECT_EQ(charVal, "I");
}

/**
 * @tc.number: DistributedWantParams_GetStringByType_0600
 * @tc.name: GetStringByType
 * @tc.desc: Test get short with invalid type in the WantParam.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetStringByType_0600, Function | MediumTest | Level3)
{
    const std::string value = "123";
    sptr<IInterface> shortObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_SHORT, value);
    std::string shortVal = DistributedWantParams::GetStringByType(shortObj, DistributedWantParams::VALUE_TYPE_SHORT);
    EXPECT_EQ(shortVal, "123");
}

/**
 * @tc.number: DistributedWantParams_GetStringByType_0700
 * @tc.name: GetStringByType
 * @tc.desc: Test get int with invalid type in the WantParam.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetStringByType_0700, Function | MediumTest | Level3)
{
    const std::string value = "-1";
    sptr<IInterface> intObj = DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_INT, value);
    std::string intVal = DistributedWantParams::GetStringByType(intObj, DistributedWantParams::VALUE_TYPE_INT);
    EXPECT_EQ(intVal, "-1");
}

/**
 * @tc.number: DistributedWantParams_GetStringByType_0800
 * @tc.name: GetStringByType
 * @tc.desc: Test get long with invalid type in the WantParam.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetStringByType_0800, Function | MediumTest | Level3)
{
    const std::string value = "-1";
    sptr<IInterface> longObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_LONG, value);
    std::string longVal = DistributedWantParams::GetStringByType(longObj, DistributedWantParams::VALUE_TYPE_LONG);
    EXPECT_EQ(longVal, "-1");
}

/**
 * @tc.number: DistributedWantParams_GetStringByType_0900
 * @tc.name: GetStringByType
 * @tc.desc: Test get float with invalid type in the WantParam.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetStringByType_0900, Function | MediumTest | Level3)
{
    const std::string value = "-1.0004";
    sptr<IInterface> floatObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_FLOAT, value);
    std::string floatVal = DistributedWantParams::GetStringByType(floatObj, DistributedWantParams::VALUE_TYPE_FLOAT);
    EXPECT_EQ(floatVal, "-1.000400");
}

/**
 * @tc.number: DistributedWantParams_GetStringByType_1000
 * @tc.name: GetStringByType
 * @tc.desc: Test get float with invalid type in the WantParam.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetStringByType_1000, Function | MediumTest | Level3)
{
    const std::string value = "-1.00000004";
    sptr<IInterface> doubleObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_DOUBLE, value);
    std::string doubleVal =
        DistributedWantParams::GetStringByType(doubleObj, DistributedWantParams::VALUE_TYPE_DOUBLE);
    EXPECT_EQ(doubleVal, "-1.000000");
}

/**
 * @tc.number: DistributedWantParams_GetStringByType_1100
 * @tc.name: GetStringByType
 * @tc.desc: Test get float with invalid type in the WantParam.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetStringByType_1100, Function | MediumTest | Level3)
{
    const std::string value = "I5{2,3,5,7,11}";
    sptr<IInterface> arrayObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_ARRAY, value);
    std::string arrayVal = DistributedWantParams::GetStringByType(arrayObj, DistributedWantParams::VALUE_TYPE_ARRAY);
    EXPECT_EQ(arrayVal, "I5{2,3,5,7,11}");
}

/**
 * @tc.number: DistributedWantParams_NewArrayData_0100
 * @tc.name: NewArrayData
 * @tc.desc: Test NewArrayData with invalid sptr<IArray> parameter.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_NewArrayData_0100, Function | MediumTest | Level3)
{
    sptr<AAFwk::IArray> array = new (std::nothrow) AAFwk::Array(0, g_IID_IDistributedWantParams);
    DistributedWantParams wp;
    wp.SetParam("param1", array);
    IArray *iarray = IArray::Query(array);
    sptr<IArray> destAO = nullptr;
    bool result = wp.NewArrayData(iarray, destAO);
    EXPECT_FALSE(result);
}

/**
 * @tc.number: DistributedWantParams_GetDataType_0100
 * @tc.name: GetDataType
 * @tc.desc: Test GetDataType with invalid parameter.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetDataType_0100, Function | MediumTest | Level3)
{
    int type = DistributedWantParams::GetDataType(nullptr);
    EXPECT_EQ(type, DistributedWantParams::VALUE_TYPE_NULL);
}

/**
 * @tc.number: DistributedWantParams_GetDataType_0200
 * @tc.name: GetDataType
 * @tc.desc: Test GetDataType with bool parameter.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetDataType_0200, Function | MediumTest | Level3)
{
    const std::string value = "true";
    sptr<IInterface> boolObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_BOOLEAN, value);
    int type = DistributedWantParams::GetDataType(boolObj);
    EXPECT_EQ(type, DistributedWantParams::VALUE_TYPE_BOOLEAN);
}

/**
 * @tc.number: DistributedWantParams_GetDataType_0300
 * @tc.name: GetDataType
 * @tc.desc: Test GetDataType with byte parameter.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetDataType_0300, Function | MediumTest | Level3)
{
    const std::string value = "129";
    sptr<IInterface> byteObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_BYTE, value);
    int type = DistributedWantParams::GetDataType(byteObj);
    EXPECT_EQ(type, DistributedWantParams::VALUE_TYPE_BYTE);
}

/**
 * @tc.number: DistributedWantParams_GetDataType_0400
 * @tc.name: GetDataType
 * @tc.desc: Test GetDataType with char parameter.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetDataType_0400, Function | MediumTest | Level3)
{
    const std::string value = "I";
    sptr<IInterface> charObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_CHAR, value);
    int type = DistributedWantParams::GetDataType(charObj);
    EXPECT_EQ(type, DistributedWantParams::VALUE_TYPE_CHAR);
}

/**
 * @tc.number: DistributedWantParams_GetDataType_0500
 * @tc.name: GetDataType
 * @tc.desc: Test GetDataType with short parameter.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetDataType_0500, Function | MediumTest | Level3)
{
    const std::string value = "123";
    sptr<IInterface> shortObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_SHORT, value);
    int type = DistributedWantParams::GetDataType(shortObj);
    EXPECT_EQ(type, DistributedWantParams::VALUE_TYPE_SHORT);
}

/**
 * @tc.number: DistributedWantParams_GetDataType_0600
 * @tc.name: GetDataType
 * @tc.desc: Test GetDataType with int parameter.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetDataType_0600, Function | MediumTest | Level3)
{
    const std::string value = "-1";
    sptr<IInterface> intObj = DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_INT, value);
    int type = DistributedWantParams::GetDataType(intObj);
    EXPECT_EQ(type, DistributedWantParams::VALUE_TYPE_INT);
}

/**
 * @tc.number: DistributedWantParams_GetDataType_0700
 * @tc.name: GetDataType
 * @tc.desc: Test GetDataType with long parameter.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetDataType_0700, Function | MediumTest | Level3)
{
    const std::string value = "-1";
    sptr<IInterface> longObj = DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_LONG, value);
    int type = DistributedWantParams::GetDataType(longObj);
    EXPECT_EQ(type, DistributedWantParams::VALUE_TYPE_LONG);
}

/**
 * @tc.number: DistributedWantParams_GetDataType_0800
 * @tc.name: GetDataType
 * @tc.desc: Test GetDataType with float parameter.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetDataType_0800, Function | MediumTest | Level3)
{
    const std::string value = "-1.0004";
    sptr<IInterface> floatObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_FLOAT, value);
    int type = DistributedWantParams::GetDataType(floatObj);
    EXPECT_EQ(type, DistributedWantParams::VALUE_TYPE_FLOAT);
}

/**
 * @tc.number: DistributedWantParams_GetDataType_0900
 * @tc.name: GetDataType
 * @tc.desc: Test GetDataType with double parameter.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetDataType_0900, Function | MediumTest | Level3)
{
    const std::string value = "-1.00000004";
    sptr<IInterface> doubleObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_DOUBLE, value);
    int type = DistributedWantParams::GetDataType(doubleObj);
    EXPECT_EQ(type, DistributedWantParams::VALUE_TYPE_DOUBLE);
}

/**
 * @tc.number: DistributedWantParams_GetDataType_1000
 * @tc.name: GetDataType
 * @tc.desc: Test GetDataType with string parameter.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetDataType_1000, Function | MediumTest | Level3)
{
    const std::string value = "hello";
    sptr<IInterface> stringObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_STRING, value);
    int type = DistributedWantParams::GetDataType(stringObj);
    EXPECT_EQ(type, DistributedWantParams::VALUE_TYPE_STRING);
}

/**
 * @tc.number: DistributedWantParams_GetDataType_1100
 * @tc.name: GetDataType
 * @tc.desc: Test GetDataType with array parameter.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetDataType_1100, Function | MediumTest | Level3)
{
    const std::string value = "I5{2,3,5,7,11}";
    sptr<IInterface> interfaceObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_ARRAY, value);
    int type = DistributedWantParams::GetDataType(interfaceObj);
    EXPECT_EQ(type, DistributedWantParams::VALUE_TYPE_ARRAY);
}

/**
 * @tc.number: DistributedWantParams_GetInterfaceByType_0100
 * @tc.name: GetInterfaceByType
 * @tc.desc: Test GetInterfaceByType with boolean type.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetInterfaceByType_0100, Function | MediumTest | Level3)
{
    const std::string value = "true";
    sptr<IInterface> boolObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_BOOLEAN, value);
    EXPECT_TRUE(Boolean::Unbox(IBoolean::Query(boolObj)));
}

/**
 * @tc.number: DistributedWantParams_GetInterfaceByType_0200
 * @tc.name: GetInterfaceByType
 * @tc.desc: Test GetInterfaceByType with byte type.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetInterfaceByType_0200, Function | MediumTest | Level3)
{
    const std::string value = "129";
    sptr<IInterface> byteObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_BYTE, value);
    EXPECT_EQ(Byte::Unbox(IByte::Query(byteObj)), 129);
}

/**
 * @tc.number: DistributedWantParams_GetInterfaceByType_0300
 * @tc.name: GetInterfaceByType
 * @tc.desc: Test GetInterfaceByType with char type.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetInterfaceByType_0300, Function | MediumTest | Level3)
{
    const std::string value = "I";
    sptr<IInterface> charObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_CHAR, value);
    EXPECT_EQ(Char::Unbox(IChar::Query(charObj)), 'I');
}

/**
 * @tc.number: DistributedWantParams_GetInterfaceByType_0400
 * @tc.name: GetInterfaceByType
 * @tc.desc: Test GetInterfaceByType with short type.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetInterfaceByType_0400, Function | MediumTest | Level3)
{
    const std::string value = "123";
    sptr<IInterface> shortObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_SHORT, value);
    EXPECT_EQ(Short::Unbox(IShort::Query(shortObj)), 123);
}

/**
 * @tc.number: DistributedWantParams_GetInterfaceByType_0500
 * @tc.name: GetInterfaceByType
 * @tc.desc: Test GetInterfaceByType with integer type.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetInterfaceByType_0500, Function | MediumTest | Level3)
{
    const std::string value = "-1";
    sptr<IInterface> intObj = DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_INT, value);
    EXPECT_EQ(Integer::Unbox(IInteger::Query(intObj)), -1);
}

/**
 * @tc.number: DistributedWantParams_GetInterfaceByType_0600
 * @tc.name: GetInterfaceByType
 * @tc.desc: Test GetInterfaceByType with long type.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetInterfaceByType_0600, Function | MediumTest | Level3)
{
    const std::string value = "-1";
    sptr<IInterface> longObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_LONG, value);
    EXPECT_EQ(Long::Unbox(ILong::Query(longObj)), -1);
}

/**
 * @tc.number: DistributedWantParams_GetInterfaceByType_0700
 * @tc.name: GetInterfaceByType
 * @tc.desc: Test GetInterfaceByType with float type.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetInterfaceByType_0700, Function | MediumTest | Level3)
{
    const std::string value = "-1.0004";
    sptr<IInterface> floatObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_FLOAT, value);
    EXPECT_FLOAT_EQ(Float::Unbox(IFloat::Query(floatObj)), -1.0004);
}

/**
 * @tc.number: DistributedWantParams_GetInterfaceByType_0800
 * @tc.name: GetInterfaceByType
 * @tc.desc: Test GetInterfaceByType with double type.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetInterfaceByType_0800, Function | MediumTest | Level3)
{
    const std::string value = "-1.00000004";
    sptr<IInterface> doubleObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_DOUBLE, value);
    EXPECT_DOUBLE_EQ(Double::Unbox(IDouble::Query(doubleObj)), -1.00000004);
}

/**
 * @tc.number: DistributedWantParams_GetInterfaceByType_0900
 * @tc.name: GetInterfaceByType
 * @tc.desc: Test GetInterfaceByType with string type.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetInterfaceByType_0900, Function | MediumTest | Level3)
{
    const std::string value = "hello";
    sptr<IInterface> stringObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_STRING, value);
    EXPECT_EQ(String::Unbox(IString::Query(stringObj)), std::string("hello"));
}

/**
 * @tc.number: DistributedWantParams_GetInterfaceByType_1000
 * @tc.name: GetInterfaceByType
 * @tc.desc: Test GetInterfaceByType with array type.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetInterfaceByType_1000, Function | MediumTest | Level3)
{
    const std::string value = "I5{2,3,5,7,11}";
    sptr<IInterface> interfaceObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_ARRAY, value);
    Array* arrayObj = static_cast<Array *>(IArray::Query(interfaceObj));
    sptr<IInterface> valueObj;
    arrayObj->Get(0, valueObj);
    EXPECT_EQ(Integer::Unbox(IInteger::Query(valueObj)), 2);
    arrayObj->Get(1, valueObj);
    EXPECT_EQ(Integer::Unbox(IInteger::Query(valueObj)), 3);
    arrayObj->Get(2, valueObj);
    EXPECT_EQ(Integer::Unbox(IInteger::Query(valueObj)), 5);
    arrayObj->Get(3, valueObj);
    EXPECT_EQ(Integer::Unbox(IInteger::Query(valueObj)), 7);
    arrayObj->Get(4, valueObj);
    EXPECT_EQ(Integer::Unbox(IInteger::Query(valueObj)), 11);
}

/**
 * @tc.number: DistributedWantParams_GetInterfaceByType_1100
 * @tc.name: GetInterfaceByType
 * @tc.desc: Test GetInterfaceByType with invalid type.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_GetInterfaceByType_1100, Function | MediumTest | Level3)
{
    const std::string value = "123";
    sptr<IInterface> arrayObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_NULL, value);
    EXPECT_EQ(arrayObj, nullptr);
}

/**
 * @tc.number: DistributedWantParams_CompareInterface_0100
 * @tc.name: CompareInterface
 * @tc.desc: Test CompareInterface with bool type.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_CompareInterface_0100, Function | MediumTest | Level3)
{
    const std::string value = "true";
    sptr<IInterface> interfaceObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_BOOLEAN, value);
    bool result =
        DistributedWantParams::CompareInterface(interfaceObj, interfaceObj, DistributedWantParams::VALUE_TYPE_BOOLEAN);
    EXPECT_TRUE(result);
}

/**
 * @tc.number: DistributedWantParams_CompareInterface_0200
 * @tc.name: CompareInterface
 * @tc.desc: Test CompareInterface with byte type.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_CompareInterface_0200, Function | MediumTest | Level3)
{
    const std::string value = "129";
    sptr<IInterface> interfaceObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_BYTE, value);
    bool result =
        DistributedWantParams::CompareInterface(interfaceObj, interfaceObj, DistributedWantParams::VALUE_TYPE_BYTE);
    EXPECT_TRUE(result);
}

/**
 * @tc.number: DistributedWantParams_CompareInterface_0300
 * @tc.name: CompareInterface
 * @tc.desc: Test CompareInterface with char type.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_CompareInterface_0300, Function | MediumTest | Level3)
{
    const std::string value = "I";
    sptr<IInterface> interfaceObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_CHAR, value);
    bool result =
        DistributedWantParams::CompareInterface(interfaceObj, interfaceObj, DistributedWantParams::VALUE_TYPE_CHAR);
    EXPECT_TRUE(result);
}

/**
 * @tc.number: DistributedWantParams_CompareInterface_0400
 * @tc.name: CompareInterface
 * @tc.desc: Test CompareInterface with short type.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_CompareInterface_0400, Function | MediumTest | Level3)
{
    const std::string value = "123";
    sptr<IInterface> interfaceObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_SHORT, value);
    bool result =
        DistributedWantParams::CompareInterface(interfaceObj, interfaceObj, DistributedWantParams::VALUE_TYPE_SHORT);
    EXPECT_TRUE(result);
}

/**
 * @tc.number: DistributedWantParams_CompareInterface_0500
 * @tc.name: CompareInterface
 * @tc.desc: Test CompareInterface with integer type.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_CompareInterface_0500, Function | MediumTest | Level3)
{
    const std::string value = "-1";
    sptr<IInterface> interfaceObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_INT, value);
    bool result =
        DistributedWantParams::CompareInterface(interfaceObj, interfaceObj, DistributedWantParams::VALUE_TYPE_INT);
    EXPECT_TRUE(result);
}

/**
 * @tc.number: DistributedWantParams_CompareInterface_0600
 * @tc.name: CompareInterface
 * @tc.desc: Test CompareInterface with long type.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_CompareInterface_0600, Function | MediumTest | Level3)
{
    const std::string value = "-1";
    sptr<IInterface> interfaceObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_LONG, value);
    bool result =
        DistributedWantParams::CompareInterface(interfaceObj, interfaceObj, DistributedWantParams::VALUE_TYPE_LONG);
    EXPECT_TRUE(result);
}

/**
 * @tc.number: DistributedWantParams_CompareInterface_0700
 * @tc.name: CompareInterface
 * @tc.desc: Test CompareInterface with float type.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_CompareInterface_0700, Function | MediumTest | Level3)
{
    const std::string value = "-1.0004";
    sptr<IInterface> interfaceObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_FLOAT, value);
    bool result =
        DistributedWantParams::CompareInterface(interfaceObj, interfaceObj, DistributedWantParams::VALUE_TYPE_FLOAT);
    EXPECT_TRUE(result);
}

/**
 * @tc.number: DistributedWantParams_CompareInterface_0800
 * @tc.name: CompareInterface
 * @tc.desc: Test CompareInterface with double type.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_CompareInterface_0800, Function | MediumTest | Level3)
{
    const std::string value = "-1.00000004";
    sptr<IInterface> interfaceObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_DOUBLE, value);
    bool result =
        DistributedWantParams::CompareInterface(interfaceObj, interfaceObj, DistributedWantParams::VALUE_TYPE_DOUBLE);
    EXPECT_TRUE(result);
}

/**
 * @tc.number: DistributedWantParams_CompareInterface_0900
 * @tc.name: CompareInterface
 * @tc.desc: Test CompareInterface with string type.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_CompareInterface_0900, Function | MediumTest | Level3)
{
    const std::string value = "hello";
    sptr<IInterface> interfaceObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_STRING, value);
    bool result =
        DistributedWantParams::CompareInterface(interfaceObj, interfaceObj, DistributedWantParams::VALUE_TYPE_STRING);
    EXPECT_TRUE(result);
}

/**
 * @tc.number: DistributedWantParams_CompareInterface_1000
 * @tc.name: CompareInterface
 * @tc.desc: Test CompareInterface with array type.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_CompareInterface_1000, Function | MediumTest | Level3)
{
    const std::string value = "I5{2,3,5,7,11}";
    sptr<IInterface> interfaceObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_ARRAY, value);
    bool result =
        DistributedWantParams::CompareInterface(interfaceObj, interfaceObj, DistributedWantParams::VALUE_TYPE_ARRAY);
    EXPECT_TRUE(result);
}

/**
 * @tc.number: DistributedWantParams_WriteArrayToParStr_0100
 * @tc.name: WriteArrayToParcelString
 * @tc.desc: Test WriteArrayToParcelString string content.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_WriteArrayToParStr_0100, Function | MediumTest | Level3)
{
    Parcel parcel;
    DistributedWantParams wp;
    bool result = wp.WriteArrayToParcelString(parcel, nullptr);
    EXPECT_FALSE(result);
}

/**
 * @tc.number: DistributedWantParams_WriteArrayToParBo_0100
 * @tc.name: WriteArrayToParcelBool
 * @tc.desc: Test WriteArrayToParcelBool bool content.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_WriteArrayToParBo_0100, Function | MediumTest | Level3)
{
    Parcel parcel;
    DistributedWantParams wp;
    bool result = wp.WriteArrayToParcelBool(parcel, nullptr);
    EXPECT_FALSE(result);
}

/**
 * @tc.number: DistributedWantParams_WriteArrayToParBy_0100
 * @tc.name: WriteArrayToParcelByte
 * @tc.desc: Test WriteArrayToParcelByte byte content.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_WriteArrayToParBy_0100, Function | MediumTest | Level3)
{
    Parcel parcel;
    DistributedWantParams wp;
    bool result = wp.WriteArrayToParcelByte(parcel, nullptr);
    EXPECT_FALSE(result);
}

/**
 * @tc.number: DistributedWantParams_WriteArrayToParCh_0100
 * @tc.name: WriteArrayToParcelChar
 * @tc.desc: Test WriteArrayToParcelChar char content.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_WriteArrayToParCh_0100, Function | MediumTest | Level3)
{
    Parcel parcel;
    DistributedWantParams wp;
    bool result = wp.WriteArrayToParcelChar(parcel, nullptr);
    EXPECT_FALSE(result);
}

/**
 * @tc.number: DistributedWantParams_WriteArrayToParSho_0100
 * @tc.name: WriteArrayToParcelShort
 * @tc.desc: Test WriteArrayToParcelShort short content.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_WriteArrayToParSho_0100, Function | MediumTest | Level3)
{
    Parcel parcel;
    DistributedWantParams wp;
    bool result = wp.WriteArrayToParcelShort(parcel, nullptr);
    EXPECT_FALSE(result);
}

/**
 * @tc.number: DistributedWantParams_WriteArrayToParInt_0100
 * @tc.name: WriteArrayToParcelInt
 * @tc.desc: Test WriteArrayToParcelInt int content.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_WriteArrayToParInt_0100, Function | MediumTest | Level3)
{
    Parcel parcel;
    DistributedWantParams wp;
    bool result = wp.WriteArrayToParcelInt(parcel, nullptr);
    EXPECT_FALSE(result);
}

/**
 * @tc.number: DistributedWantParams_WriteArrayToParLo_0100
 * @tc.name: WriteArrayToParcelLong
 * @tc.desc: Test WriteArrayToParcelLong long content.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_WriteArrayToParLo_0100, Function | MediumTest | Level3)
{
    Parcel parcel;
    DistributedWantParams wp;
    bool result = wp.WriteArrayToParcelLong(parcel, nullptr);
    EXPECT_FALSE(result);
}

/**
 * @tc.number: DistributedWantParams_WriteArrayToParFl_0100
 * @tc.name: WriteArrayToParcelFloat
 * @tc.desc: Test WriteArrayToParcelFloat float content.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_WriteArrayToParFl_0100, Function | MediumTest | Level3)
{
    Parcel parcel;
    DistributedWantParams wp;
    bool result = wp.WriteArrayToParcelFloat(parcel, nullptr);
    EXPECT_FALSE(result);
}

/**
 * @tc.number: DistributedWantParams_WriteArrayToParDou_0100
 * @tc.name: WriteArrayToParcelDouble
 * @tc.desc: Test WriteArrayToParcelDouble double content.
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_WriteArrayToParDou_0100, Function | MediumTest | Level3)
{
    Parcel parcel;
    DistributedWantParams wp;
    bool result = wp.WriteArrayToParcelDouble(parcel, nullptr);
    EXPECT_FALSE(result);
}

class DistributedUnsupportedDataTest : public testing::Test {
public:
    DistributedUnsupportedDataTest()
    {}
    ~DistributedUnsupportedDataTest()
    {
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<DistributedUnsupportedData> unsupportedData_ = nullptr;
};

void DistributedUnsupportedDataTest::SetUpTestCase(void)
{}

void DistributedUnsupportedDataTest::TearDownTestCase(void)
{}

void DistributedUnsupportedDataTest::SetUp(void)
{
    unsupportedData_ = std::make_shared<DistributedUnsupportedData>();
}

void DistributedUnsupportedDataTest::TearDown(void)
{
}

/**
 * @tc.number: DistributedWantParams_ReadArrayToParcel_1000
 * @tc.name: ReadArrayToParcel
 * @tc.desc: Test ReadArrayToParcel.
 * @tc.require: issueI648W6
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_ReadArrayToParcel_1000, Function | MediumTest | Level3)
{
    DistributedWantParams wantParams;
    Parcel parcel;
    int type = 1;
    sptr<IArray> destAO = nullptr;
    bool result = wantParams.ReadArrayToParcel(parcel, type, destAO);
    EXPECT_EQ(result, true);
}

/**
 * @tc.number: DistributedWantParams_ReadUnsupportedData_1000
 * @tc.name: ReadUnsupportedData
 * @tc.desc: Test ReadUnsupportedData.
 * @tc.require: issueI648W6
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_ReadUnsupportedData_1000, Function | MediumTest | Level3)
{
    DistributedWantParams wantParams;
    Parcel parcel;
    std::string key = "this is key";
    int type = 1;
    bool result = wantParams.ReadUnsupportedData(parcel, key, type);
    EXPECT_EQ(result, false);

    parcel.WriteInt32(-1);
    bool result1 = wantParams.ReadUnsupportedData(parcel, key, type);
    EXPECT_EQ(result1, false);

    int type1 = 50;
    bool result2 = wantParams.ReadFromParcelParam(parcel, key, type1);
    EXPECT_EQ(result2, false);
}

/**
 * @tc.number: DistributedWantParams_Operator_1000
 * @tc.name: Operator
 * @tc.desc: Test Operator.
 * @tc.require: I77HFZ
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_Operator_1000, Function | MediumTest | Level3)
{
    DTEST_LOG << "DistributedWantParamsBaseTest DistributedWantParams_Operator_1000 begin" << std::endl;
    DistributedWantParams wantParams1;
    DistributedWantParams wantParams2;
    int32_t typeId = DistributedWantParams::VALUE_TYPE_WANTPARAMS;
    DistributedWantParams ret = (wantParams1 = wantParams2);
    EXPECT_EQ(typeId, DistributedWantParams::VALUE_TYPE_WANTPARAMS);
    DTEST_LOG << "DistributedWantParamsBaseTest DistributedWantParams_Operator_1000 end" << std::endl;
}

/**
 * @tc.number: DistributedWantParams_Operator_2000
 * @tc.name: Operator
 * @tc.desc: Test Operator.
 * @tc.require: I77HFZ
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_Operator_2000, Function | MediumTest | Level3)
{
    DTEST_LOG << "DistributedWantParamsBaseTest DistributedWantParams_Operator_2000 begin" << std::endl;
    DistributedWantParams wantParams1;
    const DistributedWantParams wantParams2;
    int32_t typeId = DistributedWantParams::VALUE_TYPE_WANTPARAMS;
    DistributedWantParams ret = (wantParams1 = wantParams2);
    EXPECT_EQ(typeId, DistributedWantParams::VALUE_TYPE_WANTPARAMS);
    DTEST_LOG << "DistributedWantParamsBaseTest DistributedWantParams_Operator_2000 end" << std::endl;
}

/**
 * @tc.number: DistributedWantParams_Operator_3000
 * @tc.name: Operator
 * @tc.desc: Test Operator.
 * @tc.require: I77HFZ
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_Operator_3000, Function | MediumTest | Level3)
{
    DTEST_LOG << "DistributedWantParamsBaseTest DistributedWantParams_Operator_3000 begin" << std::endl;
    DistributedUnsupportedData data1;
    const DistributedUnsupportedData data2;
    int32_t typeId = DistributedWantParams::VALUE_TYPE_WANTPARAMS;
    DistributedUnsupportedData ret = (data1 = data2);
    EXPECT_EQ(typeId, DistributedWantParams::VALUE_TYPE_WANTPARAMS);
    DTEST_LOG << "DistributedWantParamsBaseTest DistributedWantParams_Operator_3000 end" << std::endl;
}

/**
 * @tc.number: DistributedWantParams_CompareInterface_1200
 * @tc.name: CompareInterface
 * @tc.desc: Test CompareInterface.
 * @tc.require: I77HFZ
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_CompareInterface_1200, Function | MediumTest | Level3)
{
    DTEST_LOG << "DistributedWantParamsBaseTest DistributedWantParams_CompareInterface_1200 begin" << std::endl;
    const std::string value = "wantParam";
    sptr<IInterface> interfaceObj =
        DistributedWantParams::GetInterfaceByType(DistributedWantParams::VALUE_TYPE_NULL, value);
    bool result =
        DistributedWantParams::CompareInterface(interfaceObj, interfaceObj, DistributedWantParams::VALUE_TYPE_NULL);
    EXPECT_FALSE(result);
    DTEST_LOG << "DistributedWantParamsBaseTest DistributedWantParams_CompareInterface_1200 end" << std::endl;
}

/**
 * @tc.number: DistributedWantParams_remove_0100
 * @tc.name: remove
 * @tc.desc: Test remove.
 * @tc.require: I77HFZ
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_remove_0100, Function | MediumTest | Level3)
{
    DTEST_LOG << "DistributedWantParamsBaseTest DistributedWantParams_remove_0100 begin" << std::endl;
    distributedWantParams_->params_["hello"] = String::Box("World");
    distributedWantParams_->Remove("hello");
    distributedWantParams_->params_.clear();
    EXPECT_TRUE(distributedWantParams_->params_.empty());
    DTEST_LOG << "DistributedWantParamsBaseTest DistributedWantParams_remove_0100 end" << std::endl;
}

/**
 * @tc.number: DistributedWantParams_IsEmpty_0100
 * @tc.name: IsEmpty
 * @tc.desc: Test IsEmpty.
 * @tc.require: I77HFZ
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_IsEmpty_0100, Function | MediumTest | Level3)
{
    DTEST_LOG << "DistributedWantParamsBaseTest DistributedWantParams_IsEmpty_0100 begin" << std::endl;
    distributedWantParams_->params_.clear();
    EXPECT_TRUE(distributedWantParams_->IsEmpty());
    DTEST_LOG << "DistributedWantParamsBaseTest DistributedWantParams_IsEmpty_0100 end" << std::endl;
}

/**
 * @tc.number: DistributedWantParams_DoMarshalling_0100
 * @tc.name: DoMarshalling
 * @tc.desc: Test DoMarshalling.
 * @tc.require: I77HFZ
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_DoMarshalling_0100, Function | MediumTest | Level3)
{
    DTEST_LOG << "DistributedWantParamsBaseTest DistributedWantParams_DoMarshalling_0100 begin" << std::endl;
    DistributedUnsupportedData data;
    distributedWantParams_->cachedUnsupportedData_.emplace_back(std::move(data));
    Parcel tempParcel;
    bool result = wantParamsIn_->DoMarshalling(tempParcel);
    distributedWantParams_->cachedUnsupportedData_.clear();
    EXPECT_TRUE(result);
    DTEST_LOG << "DistributedWantParamsBaseTest DistributedWantParams_DoMarshalling_0100 end" << std::endl;
}


/**
 * @tc.number: DistributedWantParams_ReadUnsupportedData_1100
 * @tc.name: ReadUnsupportedData
 * @tc.desc: Test ReadUnsupportedData.
 * @tc.require: I77HFZ
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_ReadUnsupportedData_1100, Function | MediumTest | Level3)
{
    DTEST_LOG << "DistributedWantParamsBaseTest DistributedWantParams_ReadUnsupportedData_1100 begin" << std::endl;
    DistributedWantParams wantParams;
    Parcel parcel;
    std::string key = "this is key";
    int type = 1;
    parcel.WriteInt32(0);
    bool result = wantParams.ReadUnsupportedData(parcel, key, type);
    EXPECT_FALSE(result);

    int bufferSize = 1;
    parcel.WriteInt32(bufferSize);
    parcel.WriteInt32(0);
    bool result2 = wantParams.ReadUnsupportedData(parcel, key, type);
    EXPECT_FALSE(result2);
    DTEST_LOG << "DistributedWantParamsBaseTest DistributedWantParams_ReadUnsupportedData_1100 end" << std::endl;
}

/**
 * @tc.number: DistributedWantParams_ReadFromParcelParam_1000
 * @tc.name: ReadFromParcelParam
 * @tc.desc: Test ReadFromParcelParam.
 * @tc.require: I77HFZ
 */
HWTEST_F(DistributedWantParamsBaseTest, DistributedWantParams_ReadFromParcelParam_1000, Function | MediumTest | Level3)
{
    DTEST_LOG << "DistributedWantParamsBaseTest DistributedWantParams_ReadFromParcelParam_1000 begin" << std::endl;
    DistributedWantParams wantParams;
    Parcel parcel;
    std::string key = "this is key";
    int type = 51;
    int bufferSize = 1;
    parcel.WriteInt32(bufferSize);
    parcel.WriteInt32(0);
    bool result = wantParams.ReadFromParcelParam(parcel, key, type);
    EXPECT_TRUE(result);
    DTEST_LOG << "DistributedWantParamsBaseTest DistributedWantParams_ReadFromParcelParam_1000 end" << std::endl;
}
/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#ifndef OHOS_DISTRIBUTED_MESSAGE_PARCEL_MOCK_FOR_STUB_H
#define OHOS_DISTRIBUTED_MESSAGE_PARCEL_MOCK_FOR_STUB_H

#include <memory>
#include <string>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "message_parcel.h"
#include "iremote_broker.h"

namespace OHOS::DistributedSchedule {
class DistributedIfaceCast {
public:
    virtual ~DistributedIfaceCast() = default;
    virtual sptr<IRemoteBroker> iface_cast(const sptr<IRemoteObject> &) = 0;
public:
    static inline std::shared_ptr<DistributedIfaceCast> cast = nullptr;
};

class DIfaceCastMock : public DistributedIfaceCast {
public:
    MOCK_METHOD1(iface_cast, sptr<IRemoteBroker>(const sptr<IRemoteObject> &));
};

class DistributedMessageParcelForStub {
public:
    virtual ~DistributedMessageParcelForStub() = default;
public:
    virtual std::u16string ReadInterfaceToken() = 0;
public:
    static inline std::shared_ptr<DistributedMessageParcelForStub> messageParcel = nullptr;
};

class MessageParcelMockForStub : public DistributedMessageParcelForStub {
public:
    MOCK_METHOD0(ReadInterfaceToken, std::u16string());
};
} // namespace OHOS::DistributedSchedule
#endif // OHOS_DISTRIBUTED_MESSAGE_PARCEL_MOCK_FOR_STUB_H
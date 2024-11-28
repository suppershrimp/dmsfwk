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

#include "js_continuation_state_client.h"
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"
#include "iservice_registry.h"
#include "ipc_skeleton.h"
#include "distributedsched_ipc_interface_code.h"

namespace OHOS {
    namespace DistributedSchedule {
        namespace {
            const std::string TAG = "ContinuationStateClient";
            const std::u16string DMS_PROXY_INTERFACE_TOKEN = u"ohos.distributedschedule.accessToken";
        }

        sptr <IRemoteObject> ContinuationStateClient::GetDmsProxy() {
            auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
            if (samgrProxy == nullptr) {
                HILOGE("fail to get samgr.");
                return nullptr;
            }
            return samgrProxy->CheckSystemAbility(DISTRIBUTED_SCHED_SA_ID);
        }

        int32_t
        ContinuationStateClient::RegisterContinueStateCallback(const sptr <JsContinuationStateManagerStub> stub) {
            JsContinuationStateManagerStub::StateCallbackData callbackData = stub.callbackData_;
            sptr <IRemoteObject> remote = GetDmsProxy();
            if (remote == nullptr) {
                return 0;
            }

            MessageParcel data;
            if (!data.WriteInterfaceToken(DMS_PROXY_INTERFACE_TOKEN)) {
                return 0;
            }

            if (!data.WriteString(callbackData.bundleName)) {
                return 0;
            }

            if (!data.WriteString(callbackData.abilityName)) {
                return 0;
            }
            if (!data.WriteRemoteObject(stub)) {
                return 0;
            }

            MessageParcel reply;
            MessageOption option;

            int32_t error = remote->SendRequest(
                    static_cast<uint32_t>(IDSchedInterfaceCode::CONTINUE_STATE_CALLBACK_REGISTER),
                    data, reply, option);
            if (error != ERR_NONE) {
                return error;
            }
            int32_t result = reply.ReadInt32();
            return result;
        }


        int32_t
        ContinuationStateClient::UnRegisterContinueStateCallback(const sptr <JsContinuationStateManagerStub> stub) {
            JsContinuationStateManagerStub::StateCallbackData callbackData = stub.callbackData_;
            sptr <IRemoteObject> remote = GetDmsProxy();
            if (remote == nullptr) {
                return 0;
            }

            MessageParcel data;
            if (!data.WriteInterfaceToken(DMS_PROXY_INTERFACE_TOKEN)) {
                return 0;
            }

            if (!data.WriteString(callbackData.bundleName)) {
                return 0;
            }

            if (!data.WriteString(callbackData.abilityName)) {
                return 0;
            }

            MessageParcel reply;
            MessageOption option;

            int32_t error = remote->SendRequest(
                    static_cast<uint32_t>(IDSchedInterfaceCode::CONTINUE_STATE_CALLBACK_UNREGISTER),
                    data, reply, option);
            if (error != ERR_NONE) {
                return error;
            }
            int32_t result = reply.ReadInt32();
            return result;
        }


    } // namespace DistributedSchedule
} // namespace OHOS
/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "svc_distributed_connection.h"

#include <chrono>
#include <iomanip>
#include <thread>

#include "ability_manager_client.h"
#include "dtbschedmgr_log.h"
#include "hisysevent.h"
#include "distributed_extension_proxy.h"

namespace OHOS {
namespace DistributedSchedule {
const std::string TAG = "SvcDistributedConnection";
constexpr int32_t WAIT_TIME = 3;
constexpr int32_t TIME_OUT_CLOSE = 10 * 1000 * 1000;
const std::string FILE_BACKUP_EVENTS = "FILE_BACKUP_EVENTS";
using namespace std;

void SvcDistributedConnection::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    HILOGI("called begin");
    if (remoteObject == nullptr) {
        HILOGE("Failed to ability connect done, remote is nullptr");
        return;
    }
    auto func = [this]() {
        usleep(TIME_OUT_CLOSE);
        this->DisconnectDistributedExtAbility();
    };
    std::thread task(func);
    task.detach();
    distributedProxy_ = iface_cast<DExtensionProxy>(remoteObject);
    if (distributedProxy_ == nullptr) {
        HILOGE("Failed to ability connect done, distributedProxy_ is nullptr");
        return;
    }
    isConnected_.store(true);
    string bundleName = element.GetBundleName();
    HILOGI("bundleName:%{public}s, OnAbilityConnectDone, bundleNameIndexInfo:%{public}s", bundleName.c_str(),
        bundleNameIndexInfo_.c_str());
    
    if (bundleNameIndexInfo_.find(bundleName) == string::npos) {
        HILOGE("Current bundle name is wrong, bundleNameIndexInfo:%{public}s, bundleName:%{public}s",
            bundleNameIndexInfo_.c_str(), bundleName.c_str());
        return;
    }
    bundleName = bundleNameIndexInfo_;
    HILOGI("called end");
}

void SvcDistributedConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    HILOGI("called begin");
    isConnected_.store(false);
    string bundleName = element.GetBundleName();
    HILOGI("bundleName:%{public}s, OnAbilityDisconnectDone, bundleNameIndexInfo:%{public}s", bundleName.c_str(),
        bundleNameIndexInfo_.c_str());
    if (bundleNameIndexInfo_.find(bundleName) == string::npos) {
        HILOGE("Current bundle name is wrong, bundleNameIndexInfo:%{public}s, bundleName:%{public}s",
            bundleNameIndexInfo_.c_str(), bundleName.c_str());
        return;
    }
    bundleName = bundleNameIndexInfo_;
    HILOGI("called end, name: %{public}s", bundleNameIndexInfo_.c_str());
}

ErrCode SvcDistributedConnection::ConnectDExtAbility(AAFwk::Want &want, int32_t userId, bool isCleanCalled)
{
    HILOGI("SvcDistributedConnection::ConnectDExtAbility Called begin");
    isCleanCalled_.store(isCleanCalled);
    isConnectCalled_.store(true);
    std::unique_lock<std::mutex> lock(mutex_);
    ErrCode ret = AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want, this, userId);
    HILOGI("Called end, ret=%{public}d, userId=%{public}d.", ret, userId);
    return ret;
}

ErrCode SvcDistributedConnection::DisconnectDistributedExtAbility()
{
    HILOGI("called begin");
    isConnectCalled_.store(false);
    std::unique_lock<std::mutex> lock(mutex_);
    int32_t res = distributedProxy_->TriggerOnDestroy();
    if (res != ERR_OK) {
        HILOGE("Destroy connect failed.");
    }
    ErrCode ret = AppExecFwk::AbilityManagerClient::GetInstance()->DisconnectAbility(this);
    auto callback = [extConn {wptr(this)}] {
        auto extPtr = extConn.promote();
        if (!extPtr) {
            HILOGE("Disconnect failed.");
            return false;
        }
        return extPtr->isConnected_.load() == false;
    };
    if (condition_.wait_for(lock, std::chrono::seconds(WAIT_TIME), callback)) {
        HILOGI("Wait until the connection ends");
    }
    HILOGI("called end, ret=%{public}d", ret);
    return ret;
}

bool SvcDistributedConnection::IsExtAbilityConnected()
{
    return isConnected_.load();
}

sptr<IDExtension> SvcDistributedConnection::GetDistributedExtProxy()
{
    return distributedProxy_;
}
}
}

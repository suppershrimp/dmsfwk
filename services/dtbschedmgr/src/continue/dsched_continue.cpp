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

#include "dsched_continue.h"

#include <chrono>
#include <sys/prctl.h>
#include <thread>

#include "ability_manager_client.h"
#include "bool_wrapper.h"
#include "bundle/bundle_manager_internal.h"
#include "continue_scene_session_handler.h"
#include "datetime_ex.h"
#include "dfx/dms_continue_time_dumper.h"
#include "distributed_radar.h"
#include "distributed_sched_permission.h"
#include "distributed_sched_service.h"
#include "distributed_sched_utils.h"
#include "dsched_continue_event.h"
#include "dsched_continue_manager.h"
#include "dsched_data_buffer.h"
#include "dtbschedmgr_device_info_storage.h"
#include "dtbschedmgr_log.h"
#include "ipc_skeleton.h"
#include "parcel_helper.h"
#ifdef SUPPORT_DISTRIBUTED_MISSION_MANAGER
#include "mission/dms_continue_send_manager.h"
#endif
#include "scene_board_judgement.h"
#include "softbus_adapter/transport/dsched_transport_softbus_adapter.h"

namespace OHOS {
namespace DistributedSchedule {
using namespace AAFwk;
namespace {
const std::string TAG = "DSchedContinue";
const std::string VERSION_CODE_KEY = "version";
const std::string DMS_SRC_NETWORK_ID = "dmsSrcNetworkId";
const std::string DMS_VERSION_ID = "dmsVersion";
const std::string SUPPORT_CONTINUE_PAGE_STACK_KEY = "ohos.extra.param.key.supportContinuePageStack";
const std::string SUPPORT_CONTINUE_SOURCE_EXIT_KEY = "ohos.extra.param.key.supportContinueSourceExit";
const std::string SUPPORT_CONTINUE_MODULE_NAME_UPDATE_KEY = "ohos.extra.param.key.supportContinueModuleNameUpdate";
const std::string DMSDURATION_SAVETIME = "ohos.dschedule.SaveDataTime";
const std::string DMS_PERSISTENT_ID = "ohos.dms.persistentId";
const std::string DMS_CONTINUE_SESSION_ID = "ohos.dms.continueSessionId";
const std::string DSCHED_EVENT_KEY = "IDSchedEventListener";
const std::u16string NAPI_MISSION_CALLBACK_INTERFACE_TOKEN = u"ohos.DistributedSchedule.IMissionCallback";

constexpr int32_t DSCHED_CONTINUE_PROTOCOL_VERSION = 1;
constexpr uint32_t MAX_MODULENAME_LEN = 2048;
constexpr int32_t DEFAULT_REQUEST_CODE = -1;
constexpr int32_t NOTIFY_MISSION_CALLBACK_RESULT = 4;
constexpr int32_t DMS_VERSION = 5;
constexpr int32_t START_PERMISSION = 0;

constexpr int32_t CONTINUE_BEGIN_TIME = 0;
constexpr int32_t CONTINUE_END_TIME = 1;
constexpr int32_t CONTINUE_TOTAL_TIME = 2;
constexpr int32_t CONTINUE_FIRST_TRANS_TIME = 3;
constexpr int32_t CONTINUE_DATA_TRANS_TIME = 5;
constexpr int32_t CONTINUE_START_ABILITY_TIME = 6;
constexpr int32_t GET_ABILITY_STATE_RETRY_TIMES = 40;
constexpr int32_t GET_ABILITY_STATE_SLEEP_TIME = 50;
}

DSchedContinue::DSchedContinue(int32_t subServiceType, int32_t direction,  const sptr<IRemoteObject>& callback,
    const DSchedContinueInfo& continueInfo) : subServiceType_(subServiceType), direction_(direction),
    continueInfo_(continueInfo), callback_(callback)
{
    HILOGI("DSchedContinue create");
    version_ = DSCHED_CONTINUE_PROTOCOL_VERSION;
    continueByType_ = !continueInfo.continueType_.empty();
}

DSchedContinue::DSchedContinue(std::shared_ptr<DSchedContinueStartCmd> startCmd, int32_t sessionId)
{
    HILOGI("DSchedContinue create by start command");
    version_ = startCmd->version_;
    subServiceType_ = startCmd->subServiceType_;
    continueByType_ = startCmd->continueByType_;
    direction_ = (startCmd->direction_ == CONTINUE_SOURCE) ? CONTINUE_SINK : CONTINUE_SOURCE;
    continueInfo_.sourceDeviceId_ = startCmd->srcDeviceId_;
    continueInfo_.sourceBundleName_ = startCmd->srcBundleName_;
    continueInfo_.sinkDeviceId_ = startCmd->dstDeviceId_;
    continueInfo_.sinkBundleName_ = startCmd->dstBundleName_;
    continueInfo_.continueType_ = startCmd->continueType_;
    continueInfo_.missionId_ = startCmd->sourceMissionId_;
    softbusSessionId_ = sessionId;

    if (continueInfo_.sourceBundleName_.empty() && continueInfo_.sinkBundleName_.empty()
        && continueInfo_.missionId_ != 0) {
        MissionInfo missionInfo;
        if (AbilityManagerClient::GetInstance()->GetMissionInfo("", continueInfo_.missionId_, missionInfo) != ERR_OK) {
            HILOGE("get missionInfo failed");
            return;
        }
        continueInfo_.sourceBundleName_ = missionInfo.want.GetBundle();
        continueInfo_.sinkBundleName_ = missionInfo.want.GetBundle();
    }
}

DSchedContinue::~DSchedContinue()
{
    HILOGI("DSchedContinue delete enter");
    if ((eventHandler_ != nullptr) && (eventHandler_->GetEventRunner() != nullptr)) {
        eventHandler_->GetEventRunner()->Stop();
    }
    eventThread_.join();
    eventHandler_ = nullptr;
    HILOGI("DSchedContinue delete end");
}

int32_t DSchedContinue::Init()
{
    HILOGI("DSchedContinue init start");
    auto dContinue = std::shared_ptr<DSchedContinue>(shared_from_this());
    stateMachine_ = std::make_shared<DSchedContinueStateMachine>(dContinue);
    if (direction_ == CONTINUE_SOURCE) {
        stateMachine_->UpdateState(DSCHED_CONTINUE_SOURCE_START_STATE);
    } else {
        stateMachine_->UpdateState(DSCHED_CONTINUE_SINK_START_STATE);
    }

    eventThread_ = std::thread(&DSchedContinue::StartEventHandler, this);
    std::unique_lock<std::mutex> lock(eventMutex_);
    eventCon_.wait(lock, [this] {
        return eventHandler_ != nullptr;
    });
    HILOGI("DSchedContinue init end");
    return ERR_OK;
}

void DSchedContinue::StartEventHandler()
{
    HILOGI("called");
    std::string eventThreadName = "DSchedContinue_EventHandler";
    prctl(PR_SET_NAME, eventThreadName.c_str());
    auto runner = AppExecFwk::EventRunner::Create(false);
    if (runner == nullptr) {
        HILOGE("continue start eventHandler failed.");
        return;
    }
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        eventHandler_ = std::make_shared<DSchedContinueEventHandler>(runner, shared_from_this());
    }
    eventCon_.notify_one();
    runner->Run();
}

int32_t DSchedContinue::OnContinueMission(const OHOS::AAFwk::WantParams& wantParams)
{
    HILOGI("called");
    return PostStartTask(wantParams);
}

int32_t DSchedContinue::PostStartTask(const OHOS::AAFwk::WantParams& wantParams)
{
    DSchedContinueEventType eventType = (subServiceType_ == CONTINUE_PULL) ?
        DSCHED_CONTINUE_REQ_PULL_EVENT : DSHCED_CONTINUE_REQ_PUSH_EVENT;
    HILOGI("PostStartTask %d, continueInfo: %s", eventType, continueInfo_.toString().c_str());
    if (eventHandler_ == nullptr) {
        HILOGE("PostStartTask eventHandler is nullptr");
        return INVALID_PARAMETERS_ERR;
    }

    auto wantParamsPtr = std::make_shared<OHOS::AAFwk::WantParams>(wantParams);
    auto msgEvent = AppExecFwk::InnerEvent::Get(eventType, wantParamsPtr, 0);
    if (!eventHandler_->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE)) {
        HILOGE("PostStartTask eventHandler send event type %d fail", eventType);
        return CONTINUE_SEND_EVENT_FAILED;
    }
    return ERR_OK;
}

int32_t DSchedContinue::OnStartCmd(int32_t appVersion)
{
    HILOGI("called");
    return PostCotinueAbilityTask(appVersion);
}

int32_t DSchedContinue::PostCotinueAbilityTask(int32_t appVersion)
{
    DSchedContinueEventType eventType = DSHCED_CONTINUE_ABILITY_EVENT;
    HILOGI("PostCotinueAbilityTask %d, continueInfo %s", eventType, continueInfo_.toString().c_str());
    if (eventHandler_ == nullptr) {
        HILOGE("PostCotinueAbilityTask eventHandler is nullptr");
        return INVALID_PARAMETERS_ERR;
    }
    auto data = std::make_shared<int32_t>(appVersion);
    auto msgEvent = AppExecFwk::InnerEvent::Get(eventType, data, 0);
    if (!eventHandler_->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE)) {
        HILOGE("PostCotinueAbilityTask eventHandler send event type %d fail", eventType);
        return CONTINUE_SEND_EVENT_FAILED;
    }
    return ERR_OK;
}

int32_t DSchedContinue::OnReplyCmd(std::shared_ptr<DSchedContinueReplyCmd> cmd)
{
    HILOGI("called");
    return PostReplyTask(cmd);
}

int32_t DSchedContinue::PostReplyTask(std::shared_ptr<DSchedContinueReplyCmd> cmd)
{
    HILOGI("PostReplyTask called, replyCmd: %d, result: %d, reason: %s", cmd->replyCmd_, cmd->result_,
        cmd->reason_.c_str());

    DSchedContinueEventType eventType = DSCHED_CONTINUE_INVALID_EVENT;
    switch (cmd->replyCmd_) {
        case DSCHED_CONTINUE_CMD_START: {
            eventType = DSHCED_CONTINUE_ABILITY_EVENT;
            break;
        }
        case DSCHED_CONTINUE_CMD_END: {
            eventType = DSCHED_CONTINUE_END_EVENT;
            break;
        }
        default:
            HILOGW("PostReplyTask %d, receive irrelevant reply to cmd %d", eventType, cmd->replyCmd_);
            return ERR_OK;
    }

    HILOGI("PostReplyTask %d, continueInfo %s", eventType, continueInfo_.toString().c_str());
    if (eventHandler_ == nullptr) {
        HILOGE("PostReplyTask eventHandler is nullptr");
        return INVALID_PARAMETERS_ERR;
    }

    auto result = std::make_shared<int32_t>(cmd->result_);
    auto msgEvent = AppExecFwk::InnerEvent::Get(eventType, result, 0);
    if (!eventHandler_->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE)) {
        HILOGE("PostReplyTask eventHandler send event type %d fail", eventType);
        return CONTINUE_SEND_EVENT_FAILED;
    }
    return ERR_OK;
}

int32_t DSchedContinue::OnStartContinuation(const OHOS::AAFwk::Want& want, int32_t callerUid,
    int32_t status, uint32_t accessToken)
{
    HILOGI("called");
    return PostContinueSendTask(want, callerUid, status, accessToken);
}

int32_t DSchedContinue::PostContinueSendTask(const OHOS::AAFwk::Want& want, int32_t callerUid, int32_t status,
    uint32_t accessToken)
{
    HILOGI("PostContinueSendTask called");
    if (eventHandler_ == nullptr) {
        HILOGE("eventHandler_ is nullptr");
        return INVALID_PARAMETERS_ERR;
    }
    DSchedContinueEventType eventType = DSHCED_CONTINUE_SEND_DATA_EVENT;
    if (status != ERR_OK) {
        HILOGE("continuation has been rejected, status: %d", status);
        eventType = DSCHED_CONTINUE_END_EVENT;
        auto result = std::make_shared<int32_t>(status);
        auto msgEvent = AppExecFwk::InnerEvent::Get(eventType, result, 0);
        if (!eventHandler_->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE)) {
            HILOGE("PostContinueSendTask eventHandler send event type %d fail", eventType);
            return CONTINUE_SEND_EVENT_FAILED;
        }
    }

    HILOGI("PostContinueSendTask %d, continueInfo %s", eventType, continueInfo_.toString().c_str());
    if (eventHandler_ == nullptr) {
        HILOGE("PostContinueSendTask eventHandler is nullptr");
        return INVALID_PARAMETERS_ERR;
    }
    auto data = std::make_shared<ContinueAbilityData>();
    data->want = want;
    data->callerUid  = callerUid;
    data->accessToken = accessToken;

    auto msgEvent = AppExecFwk::InnerEvent::Get(eventType, data, 0);
    if (!eventHandler_->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE)) {
        HILOGE("PostContinueSendTask eventHandler send event type %d fail", eventType);
        return CONTINUE_SEND_EVENT_FAILED;
    }
    return ERR_OK;
}

int32_t DSchedContinue::OnContinueDataCmd(std::shared_ptr<DSchedContinueDataCmd> cmd)
{
    HILOGI("called");
    return PostContinueDataTask(cmd);
}

int32_t DSchedContinue::PostContinueDataTask(std::shared_ptr<DSchedContinueDataCmd> cmd)
{
    DSchedContinueEventType eventType = DSCHED_CONTINUE_DATA_EVENT;
    HILOGI("PostContinueDataTask %d, continueInfo %s", eventType, continueInfo_.toString().c_str());
    if (eventHandler_ == nullptr) {
        HILOGE("PostContinueDataTask eventHandler is nullptr");
        return INVALID_PARAMETERS_ERR;
    }

    auto msgEvent = AppExecFwk::InnerEvent::Get(eventType, cmd, 0);
    if (!eventHandler_->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE)) {
        HILOGE("PostContinueDataTask eventHandler send event type %d fail", eventType);
        return CONTINUE_SEND_EVENT_FAILED;
    }
    return ERR_OK;
}

int32_t DSchedContinue::OnNotifyComplete(int32_t missionId, bool isSuccess)
{
    HILOGI("called");
    if (!isSuccess) {
        HILOGE("start ability not success");
        PostNotifyCompleteTask(CONTINUE_CALL_START_ABILITY_FAILED);
        return ERR_OK;
    }
    if (missionId <= 0) {
        HILOGE("start ability returns invalid missionId");
        PostNotifyCompleteTask(INVALID_PARAMETERS_ERR);
        return ERR_OK;
    }
    PostNotifyCompleteTask(ERR_OK);
    return ERR_OK;
}

int32_t DSchedContinue::OnContinueEndCmd(std::shared_ptr<DSchedContinueEndCmd> cmd)
{
    HILOGI("called");
    ContinueSceneSessionHandler::GetInstance().ClearContinueSessionId();
    return PostNotifyCompleteTask(cmd->result_);
}

int32_t DSchedContinue::PostNotifyCompleteTask(int32_t result)
{
    DSchedContinueEventType eventType = DSCHED_CONTINUE_COMPLETE_EVENT;
    HILOGI("PostNotifyCompleteTask %d, continueInfo %s", eventType, continueInfo_.toString().c_str());

    if (eventHandler_ == nullptr) {
        HILOGE("PostNotifyCompleteTask eventHandler is nullptr");
        return INVALID_PARAMETERS_ERR;
    }

    auto data = std::make_shared<int32_t>(result);
    auto msgEvent = AppExecFwk::InnerEvent::Get(eventType, data, 0);
    if (!eventHandler_->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE)) {
        HILOGE("PostNotifyCompleteTask eventHandler send event type %d fail", eventType);
        return CONTINUE_SEND_EVENT_FAILED;
    }
    return ERR_OK;
}

int32_t DSchedContinue::OnContinueEnd(int32_t result)
{
    HILOGI("called");
    return PostContinueEndTask(result);
}

int32_t DSchedContinue::PostContinueEndTask(int32_t result)
{
    DSchedContinueEventType eventType = DSCHED_CONTINUE_END_EVENT;
    HILOGI("PostContinueEndTask %d, continueInfo %s", eventType, continueInfo_.toString().c_str());
    if (eventHandler_ == nullptr) {
        HILOGE("PostContinueEndTask eventHandler is nullptr");
        return INVALID_PARAMETERS_ERR;
    }

    auto data = std::make_shared<int32_t>(result);
    auto msgEvent = AppExecFwk::InnerEvent::Get(eventType, data, 0);
    if (!eventHandler_->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE)) {
        HILOGE("PostContinueEndTask eventHandler send event type %d fail", eventType);
        return CONTINUE_SEND_EVENT_FAILED;
    }
    return ERR_OK;
}

void DSchedContinue::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr || stateMachine_ == nullptr) {
        HILOGE("event or state machine is null");
        return;
    }
    auto eventId = event->GetInnerEventId();
    HILOGI("process event %d with state %d", eventId, stateMachine_->GetStateType());
    int32_t ret = stateMachine_->Execute(event);
    if (ret != ERR_OK) {
        HILOGE("event %d excute failed, ret %d", eventId, ret);
        OnContinueEnd(ret);
    }
    return;
}

int32_t DSchedContinue::ExecuteContinueReq(std::shared_ptr<DistributedWantParams> wantParams)
{
    HILOGI("ExecuteContinueReq start, continueInfo: %s", continueInfo_.toString().c_str());
    DurationDumperStart();

    if (subServiceType_ == CONTINUE_PULL) {
        DmsRadar::GetInstance().ClickIconDmsContinue("ContinueMission", ERR_OK);
        QuickStartAbility();
    }

    std::string peerDeviceId = (direction_ == CONTINUE_SOURCE) ?
        continueInfo_.sinkDeviceId_ : continueInfo_.sourceDeviceId_;
    softbusSessionId_ = DSchedTransportSoftbusAdapter::GetInstance().ConnectDevice(peerDeviceId);
    if (softbusSessionId_ <= 0) {
        HILOGE("ExecuteContinueReq connect peer device %s failed, ret %d",
            DnetworkAdapter::AnonymizeNetworkId(peerDeviceId).c_str(), softbusSessionId_);
        return softbusSessionId_;
    }
    HILOGI("ExecuteContinueReq peer %s connected, sessionId %d",
        DnetworkAdapter::AnonymizeNetworkId(peerDeviceId).c_str(), softbusSessionId_);

    auto startCmd = std::make_shared<DSchedContinueStartCmd>();
    int32_t ret = PackStartCmd(startCmd, wantParams);
    if (ret != ERR_OK) {
        HILOGE("ExecuteContinueReq pack start cmd failed, ret %d", ret);
        return ret;
    }
    ret = SendCommand(startCmd);
    if (ret != ERR_OK) {
        HILOGE("ExecuteContinueReq send start cmd failed, ret %d", ret);
        return ret;
    }
    if (direction_ == CONTINUE_SINK) {
        stateMachine_->UpdateState(DSCHED_CONTINUE_DATA_STATE);
        DmsContinueTime::GetInstance().SetDurationEnd(CONTINUE_FIRST_TRANS_TIME, GetTickCount());
    }
    HILOGI("ExecuteContinueReq end");
    return ERR_OK;
}

int32_t DSchedContinue::QuickStartAbility()
{
    HILOGI("called");
    if (!Rosen::SceneBoardJudgement::IsSceneBoardEnabled()) {
        HILOGE("sceneBoard not available.");
        return INVALID_PARAMETERS_ERR;
    }

    std::string abilityName = QuerySinkAbilityName();
    if (abilityName.empty()) {
        HILOGE("QuickStartAbility failed.");
        return INVALID_PARAMETERS_ERR;
    }
    ContinueSceneSessionHandler::GetInstance().UpdateContinueSessionId(continueInfo_.sinkBundleName_, abilityName);
    std::string continueSessionId = ContinueSceneSessionHandler::GetInstance().GetContinueSessionId();
    HILOGI("continueSessionId is %{public}s", continueSessionId.c_str());

    AAFwk::Want want;
    want.SetElementName(continueInfo_.sinkBundleName_, abilityName);
    want.SetParam(DMS_CONTINUE_SESSION_ID, continueSessionId);

    return StartAbility(want, DEFAULT_REQUEST_CODE);
}

std::string DSchedContinue::QuerySinkAbilityName()
{
    std::string abilityName = BundleManagerInternal::GetAbilityName(continueInfo_.sinkDeviceId_,
        continueInfo_.sinkBundleName_, continueInfo_.continueType_);
    if (!abilityName.empty()) {
        return abilityName;
    }

    AppExecFwk::BundleInfo localBundleInfo;
    if (BundleManagerInternal::GetLocalBundleInfo(continueInfo_.sinkBundleName_, localBundleInfo) != ERR_OK) {
        HILOGE("get local bundle info failed.");
        return abilityName;
    }
    if (localBundleInfo.abilityInfos.empty() || localBundleInfo.abilityInfos.size() > 1) {
        HILOGE("quick start is not supported, abilityInfos size: %{public}d",
               static_cast<int32_t>(localBundleInfo.abilityInfos.size()));
        return abilityName;
    }

    return localBundleInfo.abilityInfos.front().name;
}

void DSchedContinue::DurationDumperStart()
{
    DmsContinueTime::GetInstance().Init();
    DmsContinueTime::GetInstance().SetNetWorkId(continueInfo_.sourceDeviceId_, continueInfo_.sinkDeviceId_);
    DmsContinueTime::GetInstance().SetPull(subServiceType_ == CONTINUE_PULL);

    std::string strBeginTime = DmsContinueTime::GetInstance().GetCurrentTime();
    DmsContinueTime::GetInstance().SetDurationStrTime(CONTINUE_BEGIN_TIME, strBeginTime);
    int64_t tick = GetTickCount();
    DmsContinueTime::GetInstance().SetDurationBegin(CONTINUE_TOTAL_TIME, tick);
    DmsContinueTime::GetInstance().SetDurationBegin(CONTINUE_FIRST_TRANS_TIME, tick);
}

int32_t DSchedContinue::PackStartCmd(std::shared_ptr<DSchedContinueStartCmd>& cmd,
    std::shared_ptr<DistributedWantParams> wantParams)
{
    cmd->version_ = version_;
    cmd->serviceType_ = SERVICE_TYPE_CONTINUE;
    cmd->subServiceType_ = subServiceType_;
    cmd->command_ = DSCHED_CONTINUE_CMD_START;
    cmd->srcDeviceId_ = continueInfo_.sourceDeviceId_;
    cmd->srcBundleName_ = continueInfo_.sourceBundleName_;
    cmd->dstDeviceId_ = continueInfo_.sinkDeviceId_;
    cmd->dstBundleName_ = continueInfo_.sinkBundleName_;
    cmd->continueType_ = continueInfo_.continueType_;
    cmd->sourceMissionId_ = continueInfo_.missionId_;
    cmd->continueByType_ = continueByType_;
    cmd->dmsVersion_ = DMS_VERSION;

    cmd->direction_ = direction_;
    if (subServiceType_ == CONTINUE_PULL && continueInfo_.missionId_ == 0) {
        AppExecFwk::BundleInfo localBundleInfo;
        int32_t ret = BundleManagerInternal::GetLocalBundleInfoV9(continueInfo_.sinkBundleName_, localBundleInfo);
        if (ret != ERR_OK) {
            HILOGE("pack start cmd failed, the bundle is not installed on local device.");
            return ret;
        }
        cmd->appVersion_ = static_cast<int32_t>(localBundleInfo.versionCode);
    }
    cmd->wantParams_ = *wantParams;
    return ERR_OK;
}

int32_t DSchedContinue::ExecuteContinueAbility(int32_t appVersion)
{
    HILOGI("ExecuteContinueAbility start");
    DmsRadar::GetInstance().SaveDataDmsContinue("ContinueAbility", ERR_OK);

    int32_t result = GetMissionIdByBundleName();
    if (result != ERR_OK) {
        HILOGE("ExecuteContinueAbility GetMissionIdByBundleName failed");
        return result;
    }

    result = CheckContinueAbilityPermission();
    if (result != ERR_OK) {
        HILOGE("ExecuteContinueAbility CheckContinueAbilityPermission failed");
        return result;
    }

    auto tick = GetTickCount();
    DmsContinueTime::GetInstance().SetDurationEnd(CONTINUE_FIRST_TRANS_TIME, tick);
    DmsContinueTime::GetInstance().SetSaveDataDurationBegin(tick);

    HILOGI("ExecuteContinueAbility call continueAbility begin, continueInfo: %s", continueInfo_.toString().c_str());
    result = AbilityManagerClient::GetInstance()->ContinueAbility(continueInfo_.sinkDeviceId_,
        continueInfo_.missionId_, appVersion);
    HILOGI("ExecuteContinueAbility call continueAbility end, result: %d.", result);

    if (result != ERR_OK) {
        return CONTINUE_CALL_CONTINUE_ABILITY_FAILED;
    }

    stateMachine_->UpdateState(DSCHED_CONTINUE_ABILITY_STATE);
    HILOGI("ExecuteContinueAbility end");
    return result;
}

int32_t DSchedContinue::GetMissionIdByBundleName()
{
#ifdef SUPPORT_DISTRIBUTED_MISSION_MANAGER
    if (continueInfo_.missionId_ == 0) {
        return DMSContinueSendMgr::GetInstance().GetMissionIdByBundleName(continueInfo_.sourceBundleName_,
            continueInfo_.missionId_);
    }
#endif
    return ERR_OK;
}

int32_t DSchedContinue::CheckContinueAbilityPermission()
{
    if (!CheckBundleContinueConfig(continueInfo_.sourceBundleName_)) {
        HILOGI("App does not allow continue in config file, bundle name %s", continueInfo_.sourceBundleName_.c_str());
        return REMOTE_DEVICE_BIND_ABILITY_ERR;
    }

    MissionInfo missionInfo;
    int32_t result = AbilityManagerClient::GetInstance()->GetMissionInfo("", continueInfo_.missionId_, missionInfo);
    if (result != ERR_OK) {
        HILOGE("get missionInfo failed");
        return NO_MISSION_INFO_FOR_MISSION_ID;
    }

    if (missionInfo.continueState != AAFwk::ContinueState::CONTINUESTATE_ACTIVE) {
        HILOGE("Mission continue state set to INACTIVE. Can't continue. Mission id: %d", continueInfo_.missionId_);
        return INVALID_PARAMETERS_ERR;
    }
    return ERR_OK;
}

int32_t DSchedContinue::ExecuteContinueReply()
{
    HILOGI("ExecuteContinueReply start, continueInfo: %s", continueInfo_.toString().c_str());

    AppExecFwk::BundleInfo bundleInfo;
    if (BundleManagerInternal::GetLocalBundleInfoV9(continueInfo_.sourceBundleName_, bundleInfo) != ERR_OK) {
        HILOGE("ExecuteContinueReply get local bundleInfo failed, the bundle is not installed on local device.");
        return INVALID_PARAMETERS_ERR;
    }
    auto cmd = std::make_shared<DSchedContinueReplyCmd>();
    int32_t ret = PackReplyCmd(cmd, DSCHED_CONTINUE_CMD_START, bundleInfo.versionCode, ERR_OK, "ExecuteContinueReply");
    if (ret != ERR_OK) {
        HILOGE("ExecuteContinueReply pack reply cmd failed, ret %d", ret);
        return ret;
    }
    ret = SendCommand(cmd);
    if (ret != ERR_OK) {
        HILOGE("ExecuteContinueReply send reply cmd failed, ret %d", ret);
        return ret;
    }
    stateMachine_->UpdateState(DSCHED_CONTINUE_DATA_STATE);
    HILOGI("ExecuteContinueReply end");
    return ERR_OK;
}

int32_t DSchedContinue::ExecuteContinueSend(std::shared_ptr<ContinueAbilityData> data)
{
    HILOGI("ExecuteContinueSend start, continueInfo: %s", continueInfo_.toString().c_str());
    DurationDumperBeforeStartRemoteAbility();

    SetCleanMissionFlag(data->want);

    AAFwk::Want newWant = data->want;
    auto flags = newWant.GetFlags();
    if ((flags & AAFwk::Want::FLAG_ABILITY_CONTINUATION) == 0) {
        HILOGE("StartContinuation want continuation flags invalid!");
        return INVALID_REMOTE_PARAMETERS_ERR;
    }

    int result = SetWantForContinuation(newWant);
    if (result != ERR_OK) {
        HILOGE("set new want failed");
        return result;
    }

    AppExecFwk::AbilityInfo abilityInfo;
    CallerInfo callerInfo;
    callerInfo.sourceDeviceId = continueInfo_.sourceDeviceId_;
    callerInfo.uid = data->callerUid;
    callerInfo.accessToken = data->accessToken;
    if (!BundleManagerInternal::GetCallerAppIdFromBms(callerInfo.uid, callerInfo.callerAppId)) {
        HILOGE("GetCallerAppIdFromBms failed");
        return INVALID_PARAMETERS_ERR;
    }
    if (!BundleManagerInternal::GetBundleNameListFromBms(callerInfo.uid, callerInfo.bundleNames)) {
        HILOGE("GetBundleNameListFromBms failed");
        return INVALID_PARAMETERS_ERR;
    }
    callerInfo.extraInfoJson[DMS_VERSION_ID] = DMS_VERSION;
    AccountInfo accountInfo;
    int32_t ret = DistributedSchedPermission::GetInstance().GetAccountInfo(continueInfo_.sinkDeviceId_, callerInfo,
        accountInfo);
    if (ret != ERR_OK) {
        HILOGE("GetAccountInfo failed");
        return ret;
    }

    auto cmd = std::make_shared<DSchedContinueDataCmd>();
    PackDataCmd(cmd, newWant, abilityInfo, callerInfo, accountInfo);
    ret = SendCommand(cmd);
    DmsRadar::GetInstance().SaveDataDmsRemoteWant("SendContinueData", ret);
    if (ret != ERR_OK) {
        HILOGE("ExecuteContinueSend send data cmd failed, ret %d", ret);
        return ret;
    }

    DmsContinueTime::GetInstance().SetDurationEnd(CONTINUE_DATA_TRANS_TIME, GetTickCount());

    stateMachine_->UpdateState(DSCHED_CONTINUE_SOURCE_WAIT_END_STATE);
    HILOGI("ExecuteContinueSend end");
    return ERR_OK;
}

void DSchedContinue::DurationDumperBeforeStartRemoteAbility()
{
    auto tick = GetTickCount();
    DmsContinueTime::GetInstance().SetSaveDataDurationEnd(tick);
    DmsContinueTime::GetInstance().SetDurationBegin(CONTINUE_DATA_TRANS_TIME, tick);
    DmsContinueTime::GetInstance().SetDurationBegin(CONTINUE_START_ABILITY_TIME, tick);
}

void DSchedContinue::SetCleanMissionFlag(const OHOS::AAFwk::Want& want)
{
    auto value =  want.GetParams().GetParam(SUPPORT_CONTINUE_SOURCE_EXIT_KEY);
    IBoolean *ao = IBoolean::Query(value);
    if (ao != nullptr) {
        isSourceExit_ = AAFwk::Boolean::Unbox(ao);
    }
}

int32_t DSchedContinue::SetWantForContinuation(AAFwk::Want& newWant)
{
    newWant.SetParam("sessionId", continueInfo_.missionId_);
    newWant.SetParam("deviceId", continueInfo_.sourceDeviceId_);

    AppExecFwk::BundleInfo localBundleInfo;
    if (BundleManagerInternal::GetLocalBundleInfo(newWant.GetBundle(), localBundleInfo) != ERR_OK) {
        HILOGE("get local bundle info failed");
        return INVALID_PARAMETERS_ERR;
    }
    newWant.SetParam(VERSION_CODE_KEY, static_cast<int32_t>(localBundleInfo.versionCode));
    HILOGD("local version = %u!", localBundleInfo.versionCode);

    bool isPageStackContinue = newWant.GetBoolParam(SUPPORT_CONTINUE_PAGE_STACK_KEY, true);
    std::string moduleName = newWant.GetStringParam(SUPPORT_CONTINUE_MODULE_NAME_UPDATE_KEY);
    if (!isPageStackContinue && !moduleName.empty() && moduleName.length() <= MAX_MODULENAME_LEN) {
        HILOGD("set application moduleName = %s!", moduleName.c_str());
        auto element = newWant.GetElement();
        newWant.SetElementName(element.GetDeviceID(), element.GetBundleName(), element.GetAbilityName(), moduleName);
    }

    std::string saveDataTime =
        DmsContinueTime::GetInstance().WriteDurationInfo(DmsContinueTime::GetInstance().GetSaveDataDuration());
    newWant.SetParam(DMSDURATION_SAVETIME, saveDataTime);
    if (subServiceType_ == CONTINUE_PUSH) {
        DmsContinueTime::GetInstance().SetSrcBundleName(newWant.GetElement().GetBundleName());
        DmsContinueTime::GetInstance().SetSrcAbilityName(newWant.GetElement().GetAbilityName());
        DmsContinueTime::GetInstance().SetDstBundleName(newWant.GetElement().GetBundleName());
        DmsContinueTime::GetInstance().SetDstAbilityName(newWant.GetElement().GetAbilityName());
    }
    return ERR_OK;
}

int32_t DSchedContinue::PackDataCmd(std::shared_ptr<DSchedContinueDataCmd>& cmd,
    const OHOS::AAFwk::Want& want, const AppExecFwk::AbilityInfo& abilityInfo, const CallerInfo& callerInfo,
    const AccountInfo& accountInfo)
{
    cmd->version_ = version_;
    cmd->serviceType_ = SERVICE_TYPE_CONTINUE;
    cmd->subServiceType_ = subServiceType_;
    cmd->command_ = DSCHED_CONTINUE_CMD_DATA;
    cmd->srcDeviceId_ = continueInfo_.sourceDeviceId_;
    cmd->srcBundleName_ = continueInfo_.sourceBundleName_;
    cmd->dstDeviceId_ = continueInfo_.sinkDeviceId_;
    cmd->dstBundleName_ = continueInfo_.sinkBundleName_;
    cmd->continueType_ = continueInfo_.continueType_;
    cmd->sourceMissionId_ = continueInfo_.missionId_;
    cmd->continueByType_ = continueByType_;
    cmd->dmsVersion_ = DMS_VERSION;

    cmd->want_ = want;
    AppExecFwk::CompatibleAbilityInfo compatibleAbilityInfo;
    abilityInfo.ConvertToCompatiableAbilityInfo(compatibleAbilityInfo);
    cmd->abilityInfo_ = compatibleAbilityInfo;
    cmd->callerInfo_ = callerInfo;
    cmd->accountInfo_ = accountInfo;
    cmd->requestCode_ = DEFAULT_REQUEST_CODE;
    return ERR_OK;
}

int32_t DSchedContinue::ExecuteContinueData(std::shared_ptr<DSchedContinueDataCmd> cmd)
{
    HILOGI("ExecuteContinueData start, continueInfo: %s", continueInfo_.toString().c_str());
    DurationDumperBeforeStartAbility(cmd);

    std::string localDeviceId;
    std::string deviceId = cmd->want_.GetElement().GetDeviceID();
    if (!GetLocalDeviceId(localDeviceId) ||
        !CheckDeviceIdFromRemote(localDeviceId, deviceId, cmd->callerInfo_.sourceDeviceId)) {
        HILOGE("check deviceId failed");
        return INVALID_REMOTE_PARAMETERS_ERR;
    }

    int32_t ret = DistributedSchedService::GetInstance().CheckTargetPermission(cmd->want_, cmd->callerInfo_,
        cmd->accountInfo_, START_PERMISSION, true);
    if (ret != ERR_OK) {
        HILOGE("ExecuteContinueData CheckTargetPermission failed!");
        return ret;
    }

    OHOS::AAFwk::Want want = cmd->want_;
    if (subServiceType_ == CONTINUE_PULL) {
        UpdateWantForContinueType(want);
        int32_t persistentId;
        if (ContinueSceneSessionHandler::GetInstance().GetPersistentId(persistentId) != ERR_OK) {
            HILOGE("get persistentId failed, stop start ability");
            return OnContinueEnd(ERR_OK);
        }
        HILOGI("get persistentId success, persistentId: %d", persistentId);
        WaitAbilityStateInitial(persistentId);
        want.SetParam(DMS_PERSISTENT_ID, persistentId);

        if (ContinueSceneSessionHandler::GetInstance().GetPersistentId(persistentId) != ERR_OK) {
            HILOGE("get persistentId failed, stop start ability");
            return OnContinueEnd(ERR_OK);
        }
    }
    
    ret = StartAbility(want, cmd->requestCode_);
    if (ret == ERR_OK) {
        stateMachine_->UpdateState(DSCHED_CONTINUE_SINK_WAIT_END_STATE);
        HILOGI("ExecuteContinueData end");
    }
    return ret;
}

void DSchedContinue::DurationDumperBeforeStartAbility(std::shared_ptr<DSchedContinueDataCmd> cmd)
{
    if (subServiceType_ == CONTINUE_PULL) {
        std::string timeInfo = cmd->want_.GetStringParam(DMSDURATION_SAVETIME);
        DmsContinueTime::GetInstance().ReadDurationInfo(timeInfo.c_str());
        DmsContinueTime::GetInstance().SetSrcBundleName(cmd->want_.GetElement().GetBundleName());
        DmsContinueTime::GetInstance().SetSrcAbilityName(cmd->want_.GetElement().GetAbilityName());
        DmsContinueTime::GetInstance().SetDstBundleName(cmd->want_.GetElement().GetBundleName());
        DmsContinueTime::GetInstance().SetDstAbilityName(cmd->want_.GetElement().GetAbilityName());
    }
    DmsContinueTime::GetInstance().SetDurationBegin(CONTINUE_START_ABILITY_TIME, GetTickCount());

    eventData_.moduleName = cmd->want_.GetElement().GetModuleName();
    eventData_.abilityName = cmd->want_.GetElement().GetAbilityName();
}

int32_t DSchedContinue::UpdateWantForContinueType(OHOS::AAFwk::Want& want)
{
    std::string srcAbilityName = want.GetElement().GetAbilityName();
    std::string sinkAbilityName = BundleManagerInternal::GetAbilityName(continueInfo_.sinkDeviceId_,
        continueInfo_.sinkBundleName_, continueInfo_.continueType_);
    if (!sinkAbilityName.empty() && sinkAbilityName != srcAbilityName) {
        OHOS::AppExecFwk::ElementName element = want.GetElement();
        want.SetElementName(element.GetDeviceID(), element.GetBundleName(), sinkAbilityName);
        want.RemoveParam(SUPPORT_CONTINUE_PAGE_STACK_KEY);
        want.SetParam(SUPPORT_CONTINUE_PAGE_STACK_KEY, false);

        DmsContinueTime::GetInstance().SetDstAbilityName(sinkAbilityName);
    }
    return ERR_OK;
}

bool DSchedContinue::WaitAbilityStateInitial(int32_t persistentId)
{
    int32_t retryTimeout = GET_ABILITY_STATE_RETRY_TIMES;
    int32_t err;
    do {
        bool state = false;
        err = AAFwk::AbilityManagerClient::GetInstance()->GetAbilityStateByPersistentId(persistentId, state);
        if (err == ERR_OK && state) {
            HILOGI("ability state initial.");
            return state;
        }
        HILOGI("waiting ability state initial...");
        std::this_thread::sleep_for(std::chrono::milliseconds(GET_ABILITY_STATE_SLEEP_TIME));
    } while (--retryTimeout > 0);

    HILOGE("wait timeout, persistentId: %{public}d, errorCode: %{public}d",
           persistentId, err);
    return false;
}

int32_t DSchedContinue::StartAbility(const OHOS::AAFwk::Want& want, int32_t requestCode)
{
    int32_t ret = AAFwk::AbilityManagerClient::GetInstance()->Connect();
    if (ret != ERR_OK) {
        HILOGE("ExecuteContinueData connect ability server failed %d", ret);
        return ret;
    }

    int32_t activeAccountId = 0;
    ret = DistributedSchedService::GetInstance().QueryOsAccount(activeAccountId);
    if (ret != ERR_OK) {
        HILOGE("ExecuteContinueData QueryOsAccount failed %d", ret);
        return ret;
    }

    HILOGI("ExecuteContinueData StartAbility start");
    DmsRadar::GetInstance().ClickIconDmsStartAbility("StartAbility", ret);
    ret = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want, DEFAULT_REQUEST_CODE, activeAccountId);
    if (ret != ERR_OK) {
        HILOGE("StartAbility failed %d", ret);
        return ret;
    }
    return ret;
}

int32_t DSchedContinue::ExecuteNotifyComplete(int32_t result)
{
    HILOGI("ExecuteNotifyComplete start, continueInfo: %s", continueInfo_.toString().c_str());
    DmsContinueTime::GetInstance().SetDurationEnd(CONTINUE_START_ABILITY_TIME, GetTickCount());

    int32_t ret = 0;
    if (direction_ == CONTINUE_SINK) {
        auto cmd = std::make_shared<DSchedContinueEndCmd>();
        PackEndCmd(cmd, result);

        ret = SendCommand(cmd);
        if (ret != ERR_OK) {
            HILOGE("ExecuteNotifyComplete send end cmd failed, ret %d", ret);
            return ret;
        }

        stateMachine_->UpdateState(DSCHED_CONTINUE_SINK_END_STATE);
        HILOGI("ExecuteNotifyComplete end");
        return ERR_OK;
    }

    auto cmd = std::make_shared<DSchedContinueReplyCmd>();
    PackReplyCmd(cmd, DSCHED_CONTINUE_CMD_END, 0, result, "ExecuteNotifyComplete");
    ret = SendCommand(cmd);
    if (ret != ERR_OK) {
        HILOGE("ExecuteNotifyComplete send reply cmd failed, ret %d", ret);
        return ret;
    }

    stateMachine_->UpdateState(DSCHED_CONTINUE_SOURCE_END_STATE);
    PostContinueEndTask(result);

    HILOGI("ExecuteNotifyComplete end");
    return ERR_OK;
}

int32_t DSchedContinue::PackReplyCmd(std::shared_ptr<DSchedContinueReplyCmd> cmd, int32_t replyCmd, int32_t appVersion,
    int32_t result, const std::string reason)
{
    cmd->version_ = version_;
    cmd->serviceType_ = SERVICE_TYPE_CONTINUE;
    cmd->subServiceType_ = subServiceType_;
    cmd->command_ = DSCHED_CONTINUE_CMD_REPLY;
    cmd->srcDeviceId_ = continueInfo_.sourceDeviceId_;
    cmd->srcBundleName_ = continueInfo_.sourceBundleName_;
    cmd->dstDeviceId_ = continueInfo_.sinkDeviceId_;
    cmd->dstBundleName_ = continueInfo_.sinkBundleName_;
    cmd->continueType_ = continueInfo_.continueType_;
    cmd->sourceMissionId_ = continueInfo_.missionId_;
    cmd->continueByType_ = continueByType_;
    cmd->dmsVersion_ = DMS_VERSION;

    cmd->replyCmd_ = replyCmd;
    cmd->appVersion_ = appVersion;
    cmd->result_ = result;
    cmd->reason_ = reason;
    return ERR_OK;
}

int32_t DSchedContinue::ExecuteContinueEnd(int32_t result)
{
    HILOGI("ExecuteContinueEnd start, continueInfo: %s", continueInfo_.toString().c_str());

    std::string peerDeviceId = (direction_ == CONTINUE_SOURCE) ?
        continueInfo_.sinkDeviceId_ : continueInfo_.sourceDeviceId_;
    if (result != ERR_OK ||
        (subServiceType_ == CONTINUE_PULL && direction_ == CONTINUE_SINK) ||
        (subServiceType_ == CONTINUE_PUSH && direction_ == CONTINUE_SOURCE)) {
        HILOGI("ExecuteContinueEnd disconnect peer device %s",
            DnetworkAdapter::AnonymizeNetworkId(peerDeviceId).c_str());
        DSchedTransportSoftbusAdapter::GetInstance().DisconnectDevice(peerDeviceId);
    }

    if (result == ERR_OK && direction_ == CONTINUE_SOURCE && isSourceExit_) {
        int32_t ret = AbilityManagerClient::GetInstance()->CleanMission(continueInfo_.missionId_);
        HILOGD("ExecuteContinueEnd clean mission result: %d", ret);
    }

    if (direction_ == CONTINUE_SINK) {
        DmsRadar::GetInstance().ClickIconDmsRecvOver("NotifyContinuationResultFromRemote", result);
    }

    NotifyContinuationCallbackResult(result);
    NotifyDSchedEventResult(result);
    DurationDumperComplete(result);

    DSchedContinueManager::GetInstance().OnContinueEnd(continueInfo_);
    HILOGI("ExecuteContinueEnd end");
    return ERR_OK;
}

void DSchedContinue::NotifyContinuationCallbackResult(int32_t result)
{
    HILOGD("continuation result is: %d", result);
    if (callback_ == nullptr) {
        HILOGW("callback object null.");
        return;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(NAPI_MISSION_CALLBACK_INTERFACE_TOKEN)) {
        HILOGE("write token failed");
        return;
    }
    PARCEL_WRITE_HELPER_NORET(data, Int32, (result == ERR_OK) ? 0 : NOTIFYCOMPLETECONTINUATION_FAILED);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = callback_->SendRequest(NOTIFY_MISSION_CALLBACK_RESULT, data, reply, option);
    if (ret != ERR_OK) {
        HILOGE("send request failed, ret: %d", ret);
    }
    return;
}

void DSchedContinue::NotifyDSchedEventResult(int32_t result)
{
    if (direction_ == CONTINUE_SINK) {
        ContinueEvent event;
        event.srcNetworkId = continueInfo_.sourceDeviceId_;
        event.dstNetworkId = continueInfo_.sinkDeviceId_;
        event.bundleName = continueInfo_.sourceBundleName_;
        event.moduleName = eventData_.moduleName;
        event.abilityName = eventData_.abilityName;
        DistributedSchedService::GetInstance().NotifyDSchedEventCallbackResult(DSCHED_EVENT_KEY, result, event);
    }
}

void DSchedContinue::DurationDumperComplete(int32_t result)
{
    if (result != ERR_OK) {
        return;
    }
    if ((subServiceType_ == CONTINUE_PULL && direction_ == CONTINUE_SINK) ||
        (subServiceType_ == CONTINUE_PUSH && direction_ == CONTINUE_SOURCE)) {
        std::string strEndTime = DmsContinueTime::GetInstance().GetCurrentTime();
        DmsContinueTime::GetInstance().SetDurationEnd(CONTINUE_TOTAL_TIME, GetTickCount());
        DmsContinueTime::GetInstance().SetDurationStrTime(CONTINUE_END_TIME, strEndTime);
        DmsContinueTime::GetInstance().AppendInfo();
        DmsContinueTime::GetInstance().SetPull(false);
    }
    return;
}

int32_t DSchedContinue::ExecuteContinueError(int32_t result)
{
    auto cmd = std::make_shared<DSchedContinueEndCmd>();
    PackEndCmd(cmd, result);
    SendCommand(cmd);
    if (direction_ == CONTINUE_SOURCE) {
        stateMachine_->UpdateState(DSCHED_CONTINUE_SINK_END_STATE);
    } else {
        stateMachine_->UpdateState(DSCHED_CONTINUE_SOURCE_END_STATE);
    }
    OnContinueEnd(result);
    HILOGI("ExecuteNotifyComplete end");
    return ERR_OK;
}

int32_t DSchedContinue::PackEndCmd(std::shared_ptr<DSchedContinueEndCmd> cmd, bool result)
{
    cmd->version_ = version_;
    cmd->serviceType_ = SERVICE_TYPE_CONTINUE;
    cmd->subServiceType_ = subServiceType_;
    cmd->command_ = DSCHED_CONTINUE_CMD_END;
    cmd->srcDeviceId_ = continueInfo_.sourceDeviceId_;
    cmd->srcBundleName_ = continueInfo_.sourceBundleName_;
    cmd->dstDeviceId_ = continueInfo_.sinkDeviceId_;
    cmd->dstBundleName_ = continueInfo_.sinkBundleName_;
    cmd->continueType_ = continueInfo_.continueType_;
    cmd->sourceMissionId_ = continueInfo_.missionId_;
    cmd->continueByType_ = continueByType_;
    cmd->dmsVersion_ = DMS_VERSION;

    cmd->result_ = result;
    return ERR_OK;
}

int32_t DSchedContinue::SendCommand(std::shared_ptr<DSchedContinueCmdBase> cmd)
{
    HILOGI("SendCommand start, cmd %d", cmd->command_);
    std::string jsonStr;
    int32_t ret = cmd->Marshal(jsonStr);
    if (ret != ERR_OK) {
        HILOGE("SendCommand marshal cmd %d failed, ret %d", cmd->command_, ret);
        return ret;
    }
    auto buffer = std::make_shared<DSchedDataBuffer>(jsonStr.length() + 1);
    ret = memcpy_s(buffer->Data(), buffer->Capacity(),
        reinterpret_cast<uint8_t *>(const_cast<char *>(jsonStr.c_str())), jsonStr.length());
    if (ret != ERR_OK) {
        HILOGE("SendCommand memcpy_s failed, cmd %d, ret %d", cmd->command_, ret);
        return ret;
    }
    ret = DSchedTransportSoftbusAdapter::GetInstance().SendData(softbusSessionId_, SERVICE_TYPE_CONTINUE, buffer);
    if (ret != ERR_OK) {
        HILOGE("SendCommand send data failed, cmd %d, ret %d", cmd->command_, ret);
        return ret;
    }
    HILOGI("SendCommand end, cmd %d", cmd->command_);
    return ERR_OK;
}

int32_t DSchedContinue::GetSessionId()
{
    return softbusSessionId_;
}

DSchedContinueInfo DSchedContinue::GetContinueInfo()
{
    return continueInfo_;
}

bool DSchedContinue::GetLocalDeviceId(std::string& localDeviceId)
{
    if (!DtbschedmgrDeviceInfoStorage::GetInstance().GetLocalDeviceId(localDeviceId)) {
        HILOGE("GetLocalDeviceId failed");
        return false;
    }
    return true;
}

bool DSchedContinue::CheckDeviceIdFromRemote(const std::string& localDevId, const std::string& destDevId,
    const std::string& srcDevId)
{
    if (localDevId.empty() || destDevId.empty() || srcDevId.empty()) {
        HILOGE("CheckDeviceIdFromRemote failed");
        return false;
    }
    // destDevId set by remote must be same with localDevId
    if (localDevId != destDevId) {
        HILOGE("destDevId is not same with localDevId");
        return false;
    }
    HILOGD("CheckDeviceIdFromRemote srcDevId %s", srcDevId.c_str());
    HILOGD("CheckDeviceIdFromRemote localDevId %s", localDevId.c_str());
    HILOGD("CheckDeviceIdFromRemote destDevId %s", destDevId.c_str());

    if (srcDevId == destDevId || srcDevId == localDevId) {
        HILOGE("destDevId is different with localDevId and destDevId");
        return false;
    }

    if (srcDevId != continueInfo_.sourceDeviceId_) {
        HILOGE("srcDevId is not correct");
        return false;
    }
    return true;
}

void DSchedContinue::OnDataRecv(int32_t command, std::shared_ptr<DSchedDataBuffer> dataBuffer)
{
    HILOGI("called, command %d", command);
    int32_t ret = 0;
    uint8_t *data = dataBuffer->Data();
    std::string jsonStr(reinterpret_cast<const char *>(data), dataBuffer->Capacity());

    switch (command) {
        case DSCHED_CONTINUE_CMD_START: {
            HILOGW("continue already started, cmd abort.");
            break;
        }
        case DSCHED_CONTINUE_CMD_DATA: {
            auto dataCmd = std::make_shared<DSchedContinueDataCmd>();
            ret = dataCmd->Unmarshal(jsonStr);
            if (ret != ERR_OK) {
                HILOGE("Unmarshal data cmd failed, ret: %d", ret);
                return;
            }
            OnContinueDataCmd(dataCmd);
            break;
        }
        case DSCHED_CONTINUE_CMD_REPLY: {
            auto replyCmd = std::make_shared<DSchedContinueReplyCmd>();
            ret = replyCmd->Unmarshal(jsonStr);
            if (ret != ERR_OK) {
                HILOGE("Unmarshal reply cmd failed, ret: %d", ret);
                return;
            }
            OnReplyCmd(replyCmd);
            break;
        }
        case DSCHED_CONTINUE_CMD_END: {
            auto endCmd = std::make_shared<DSchedContinueEndCmd>();
            ret = endCmd->Unmarshal(jsonStr);
            if (ret != ERR_OK) {
                HILOGE("Unmarshal end cmd failed, ret: %d", ret);
                return;
            }
            OnContinueEndCmd(endCmd);
            break;
        }
        default:
            HILOGW("Invalid command.");
            break;
    }
    return;
}

void DSchedContinue::OnShutDown()
{
}

void DSchedContinue::OnBind()
{
}
}  // namespace DistributedSchedule
}  // namespace OHOS

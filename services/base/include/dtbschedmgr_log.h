/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef SERVICES_DTBSCHEDMGR_INCLUDE_DTBSCHEDMGR_LOG_H_
#define SERVICES_DTBSCHEDMGR_INCLUDE_DTBSCHEDMGR_LOG_H_

#include "errors.h"
#include "hilog/log_c.h"
#include "hilog/log_cpp.h"
#include "inttypes.h"

namespace OHOS {
namespace DistributedSchedule {
static constexpr OHOS::HiviewDFX::HiLogLabel DSCHED_SERVICE_LABEL = {LOG_CORE, 0xD001800, "DSched_Service"};

#ifdef HILOGD
#undef HILOGD
#endif

#ifdef HILOGF
#undef HILOGF
#endif

#ifdef HILOGE
#undef HILOGE
#endif

#ifdef HILOGW
#undef HILOGW
#endif

#ifdef HILOGI
#undef HILOGI
#endif

#define DMS_LOG(level, fmt, ...) HiviewDFX::HiLog::level(DSCHED_SERVICE_LABEL, \
    "%{public}s::%{public}s " fmt, TAG.c_str(), __FUNCTION__, ##__VA_ARGS__)

#define HILOGF(fmt, ...) DMS_LOG(Fatal, fmt, ##__VA_ARGS__)
#define HILOGE(fmt, ...) DMS_LOG(Error, fmt, ##__VA_ARGS__)
#define HILOGW(fmt, ...) DMS_LOG(Warn,  fmt, ##__VA_ARGS__)
#define HILOGI(fmt, ...) DMS_LOG(Info,  fmt, ##__VA_ARGS__)
#define HILOGD(fmt, ...) DMS_LOG(Debug, fmt, ##__VA_ARGS__)

enum {
    /**
     * Module type: Distributed schedule Service side
     */
    DMS_MODULE_TYPE_SERVICE = 0,
};

// offset of dms error, only be used in this file.
constexpr ErrCode DMS_SERVICE_ERR_OFFSET = ErrCodeOffset(SUBSYS_DISTRIBUTEDSCHEDULE, DMS_MODULE_TYPE_SERVICE);

enum {
    /**
     * Result(16300502) for failed to get the missionInfo of the specified missionId.
     */
    NO_MISSION_INFO_FOR_MISSION_ID = 16300502,
    /**
     * Result(16300505) for the operation device must be the device where the application to be continued
     * is located or the target device to be continued.
     */
    OPERATION_DEVICE_NOT_INITIATOR_OR_TARGET = 16300505,
    /**
     * Result(16300506) for the local continuation task is already in progress.
     */
    CONTINUE_ALREADY_IN_PROGRESS = 16300506,
    /**
     * Result(29360220) for the mission for continuing is not alive, try again after restart mission.
     */
    MISSION_FOR_CONTINUING_IS_NOT_ALIVE = 16300507,
    /**
     * Result(29360128) for invalid parameters.
     */
    INVALID_PARAMETERS_ERR = DMS_SERVICE_ERR_OFFSET,
     /**
     * Result(29360129) for no response when RPC call.
     */
    RPC_CALL_NORESPONSE_ERR = 29360129,
    /**
     * Result(29360130) for not find connect callback.
     */
    NO_CONNECT_CALLBACK_ERR = 29360130,
    /**
     * Result(29360131) for remote invalid parameters.
     */
    INVALID_REMOTE_PARAMETERS_ERR = 29360131,
    /**
     * Result(29360132) for remote device fail to bind ability.
     */
    REMOTE_DEVICE_BIND_ABILITY_ERR = 29360132,
    /**
     * Result(29360133) for remote device fail to unbind ability.
     */
    REMOTE_DEVICE_UNBIND_ABILITY_ERR = 29360133,
    /**
     * Result(29360134) for remote invalid parameters.
     */
    REMOTE_FAIL_GET_SHELLINFO_ERR = 29360134,
    /**
     * Result(29360135) for remote device fail to start ability.
     */
    REMOTE_DEVICE_START_ABILITY_ERR = 29360135,
    /**
     * Result(29360136) for remote device fail to stop ability.
     */
    REMOTE_DEVICE_STOP_ABILITY_ERR = 29360136,
    /**
     * Result(29360137) for reg repeat ability token.
     */
    REG_REPEAT_ABILITY_TOKEN_ERR = 29360137,
    /**
     * Result(29360138) for no ability token.
     */
    NO_ABILITY_TOKEN_ERR = 29360138,
    /**
     * Result(29360139) for no appthread.
     */
    NO_APP_THREAD_ERR = 29360139,
    /**
     * Result(29360140) for select device continue error.
     */
    SELECT_DEVICE_CONTINUE_ERR = 29360140,
    /**
     * Result(29360141) for dump write file error.
     */
    DMS_WRITE_FILE_FAILED_ERR = 29360141,
    /**
     * Result(29360142) for notify app the bind remote service's device is offline.
     */
    DEVICE_OFFLINE_ERR = 29360142,
    /**
     * Result(29360143) for not find hitrace id.
     */
    NO_HITRACE_RECORD_ERR = 29360143,
    /**
     * Result(29360144) for get local deviceid fail.
     */
    GET_LOCAL_DEVICE_ERR = 29360144,
    /**
     * Result(29360145) for get local deviceid fail.
     */
    GET_DATAABILITY_ERR = 29360145,
    /**
     * Result(29360146) for DistributedSched Service Publish Exception.
     */
    DMS_PUBLISH_EXCEPTION = 29360146,
    /**
     * Result(29360147) for DistributedSched Storage Exception.
     */
    DMS_GET_STORAGE_EXCEPTION = 29360147,
    /**
     * Result(29360148) for DistributedSched Service Get System Ability Manager Exception.
     */
    DMS_GET_SAMGR_EXCEPTION = 29360148,
    /**
     * Result(29360149) for DistributedSched Service IPC call noresponse error.
     */
    IPC_CALL_NORESPONSE_ERR = 29360149,
    /**
     * Result(29360150) for DistributedSched Service call bundle manager service error.
     */
    BUNDLE_MANAGER_SERVICE_ERR = 29360150,
    /**
     * Result(29360151) for DistributedSched Service call distributed manager error.
     */
    DISTRIBUTED_MANAGER_ERR = 29360151,
    /**
     * Result(29360152) for DistributedSched Service NotifyCompleteContinuation failed.
     */
    NOTIFYCOMPLETECONTINUATION_FAILED = 29360152,
    /**
     * Result(29360153) for DistributedSched Service Continue Ability Timeout Error.
     */
    CONTINUE_ABILITY_TIMEOUT_ERR = 29360153,
    /**
     * Result(29360154) for DistributedSched Service Notify Complete Continuation abilityToken null.
     */
    ABILITYTOKEN_NULL = 29360154,
    /**
     * Result(29360155) for get device list fail.
     */
    GET_DEVICE_LIST_ERR = 29360155,
    /**
     * Result(29360156) for notify ability install error.
     */
    NOTIFY_ABILITY_INSTALL_ERR = 29360156,
    /**
     * Result(29360157) for permission denied.
     */
    DMS_PERMISSION_DENIED = 29360157,
    /**
     * Result(29360158) for notify ability free install.
     */
    NOTIFY_ABILITY_FREE_INSTALL = 29360158,
    /**
     * Result(29360159) for notify app invalid uid.
     */
    BIND_ABILITY_UID_INVALID_ERR = 29360159,
    /**
     * Result(29360160) for notify app happen connect leak.
     */
    BIND_ABILITY_LEAK_ERR = 29360160,
    /**
     * Result(29360161) for can not service registry.
     */
    NOT_FIND_SERVICE_REGISTRY = 29360161,
    /**
     * Result(29360162) for can not get service.
     */
    NOT_FIND_SERVICE_PROXY = 29360162,
    /**
     * Result(29360163) for IPC Send Request to DEF fail.
     */
    SEND_REQUEST_DEF_FAIL = 29360163,
    /**
     * Result(29360164) for BMS can not find the specified ability.
     */
    CAN_NOT_FOUND_ABILITY_ERR = 29360164,
    /**
     * Result(29360165) for DMS can not register device state callback.
     */
    REGISTER_DEVICE_STATE_CALLBACK_ERR = 29360165,
    /**
     * Result(29360166) for DMS can not unregister device state callback.
     */
    UNREGISTER_DEVICE_STATE_CALLBACK_ERR = 29360166,
    /**
     * Result(29360167) for DMS can not start auxiliary device FA.
     */
    START_AUXILIARY_FA_ERR = 29360167,
    /**
     * Result(29360168) for DMS can not get dmsproxy handle.
     */
    GET_DMSPROXY_HANDLE_FAIL = 29360168,
    /**
     * Result(29360169) for dmsproxy service exception.
     */
    DMSPROXY_SERVICE_ERR = 29360169,
    /**
     * Result(29360170) for softbus service unavailable.
     */
    SOFTBUS_SERVICE_ERR = 29360170,
    /**
     * Result(29360171) for softbus register callback fail.
     */
    SOFTBUS_REGISTER_CALLBACK_ERR = 29360171,
    /**
     * Result(29360172) for get DPermission sa fail.
     */
    GET_DISTRIBUTED_PERMISSION_SA_ERR = 29360172,
    /**
     * Result(29360173) for get BMS sa fail.
     */
    GET_BUNDLE_MANAGER_ERR = 29360173,
    /**
     * Result(29360174) for get remote dms fail.
     */
    GET_REMOTE_DMS_FAIL = 29360174,
    /**
     * Result(29360175) for account access permission check failed.
     */
    DMS_ACCOUNT_ACCESS_PERMISSION_DENIED = 29360175,
    /**
     * Result(29360176) for component access permission check failed.
     */
    DMS_COMPONENT_ACCESS_PERMISSION_DENIED = 29360176,
    /**
     * Result(29360177) for distributed communication permission check failed.
     */
    DMS_DISTRIBUTED_COMMUNICATION_PERMISSION_DENIED = 29360177,
    /**
     * Result(29360178) for osd function not support.
     */
    MISSION_OSD_NOT_SUPPORTED = 29360178,
    /**
     * Result(29360179) for osd switch is off.
     */
    MISSION_OSD_NOT_ENABLED = 29360179,
    /**
     * Result(29360180) for osd switch is on.
     */
    MISSION_OSD_ENABLED = 29360180,
    /**
     * Result(29360181) for front app check failed.
     */
    CHECK_FRONT_APP_FAIL = 29360181,
    /*
     * Result(29360184) for notify mission change listener failed.
     */
    MISSION_NOTIFY_CHANGE_LISTENER_ERR = 29360184,
    /**
     * Result(29360185) for subscribe data storage change failed.
     */
    MISSION_SUBSCRIBE_DATA_STORAGE_ERR = 29360185,
    /**
     * Result(29360186) for sync snapshot failed.
     */
    MISSION_SYNC_SNAPSHOT_ERR = 29360186,
    /**
     * Result(29360187) for sync mission info list failed.
     */
    MISSION_SYNC_MISSION_INFO_LIST_ERR = 29360187,
    /**
     * Result(29360188) for distributed  communication permission check failed.
     */
    REMOTE_DISTRIBUTED_COMMUNICATION_PERMISSION_DENIED = 29360188,
    /*
     * Result(29360191) for blur snapshot failed.
     */
    MISSION_BLUR_SNAPSHOT_ERR = 29360191,
    /*
     * Result(29360192) for take foreground snapshot failed.
     */
    MISSION_TAKE_FOREGROUND_SNAPSHOT_ERR = 29360192,
    /*
     * Result(29360193) for ccm switch not support.
     */
    MISSION_OSD_CCM_NOT_SUPPORTED = 29360193,
    /*
     * Result(29360194) for wifi switch is off.
     */
    MISSION_OSD_WIFI_OFF = 29360194,
    /*
     * Result(29360195) for child mode is on.
     */
    MISSION_OSD_CHILDMODE_ON = 29360195,
    /*
     * Result(29360196) for cloud switch is off.
     */
    MISSION_OSD_CLOUD_SWITCH_OFF = 29360196,
    /*
     * Result(29360197) for continue ability rejected.
     */
    CONTINUE_ABILITY_REJECTED = 29360197,
    /*
     * Result(29360198) for continue save data failed.
     */
    CONTINUE_SAVE_DATA_FAILED = 29360198,
    /*
     * Result(29360199) for onContinue failed.
     */
    CONTINUE_ON_CONTINUE_FAILED = 29360199,
    /*
     * Result(29360200) for continue get content failed.
     */
    CONTINUE_GET_CONTENT_FAILED = 29360200,
    /*
     * Result(29360201) for call permission check failed.
     */
    CALL_PERMISSION_DENIED = 29360201,
    /*
     * Result(29360202) for continue remote not install and support free install.
     */
    CONTINUE_REMOTE_UNINSTALLED_SUPPORT_FREEINSTALL = 29360202,
    /*
     * Result(29360203) for continue remote not install and not support free install.
     */
    CONTINUE_REMOTE_UNINSTALLED_UNSUPPORT_FREEINSTALL = 29360203,
    /*
     * Result(29360204) for continue remote version not match.
     */
    CONTINUE_REMOTE_VERSION_MISMATCH = 29360204,
    /*
     * Result(29360205) for continue freeinstall failed.
     */
    CONTINUE_FREE_INSTALL_FAILED = 29360205,
    /*
     * Result(29360206) for continue remote cannot install.
     */
    CONTINUE_REMOTE_UNINSTALLED_CANNOT_FREEINSTALL = 29360206,
    /*
     * Result(29360207) for app registered exceed max times.
     */
    REGISTER_EXCEED_MAX_TIMES = 29360207,
    /*
     * Result(29360208) for token has not registered.
     */
    TOKEN_HAS_NOT_REGISTERED = 29360208,
    /*
     * Result(29360209) for callback has registered.
     */
    CALLBACK_HAS_REGISTERED = 29360209,
    /*
     * Result(29360210) for callback has not registered.
     */
    CALLBACK_HAS_NOT_REGISTERED = 29360210,
    /*
     * Result(29360211) for connect ability failed.
     */
    CONNECT_ABILITY_FAILED = 29360211,
    /*
     * Result(29360212) for disconnect ability failed.
     */
    DISCONNECT_ABILITY_FAILED = 29360212,
    /*
     * Result(29360213) for panel name not configured.
     */
    PANEL_NAME_NOT_CONFIGURED = 29360213,
    /*
     * Result(29360214) for unknown callback type.
     */
    UNKNOWN_CALLBACK_TYPE = 29360214,
    /*
     * Result(29360215) for invalid connect status.
     */
    INVALID_CONNECT_STATUS = 29360215,
    /*
     * Result(29360216) for invalid continuation mode.
     */
    INVALID_CONTINUATION_MODE = 29360216,
    /**
     * Result(29360217) for dms version is empty.
     */
    DMS_VERSION_EMPTY = 29360217,
    /**
     * Result(29360218) for DistributedSched Service parse dms version error.
     */
    DMS_VERSION_PARSE_EXCEPTION = 29360218,
    /*
     * Result(29360219) for background permission check failed.
     */
    DMS_BACKGROUND_PERMISSION_DENIED = 29360219,
    /*
     * Result(29360220) for start control permission check failed.
     */
    DMS_START_CONTROL_PERMISSION_DENIED = 29360220,
};
} // namespace DistributedSchedule
} // namespace OHOS
#endif // SERVICES_DTBSCHEDMGR_INCLUDE_DTBSCHEDMGR_LOG_H_

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

#ifndef OHOS_DISTRIBUTED_DISTRIBUTEDSCHED_IPC_INTERFACE_CODE_H
#define OHOS_DISTRIBUTED_DISTRIBUTEDSCHED_IPC_INTERFACE_CODE_H

namespace OHOS {
namespace DistributedSchedule {
/* SAID: 1401 */
enum class IDSchedInterfaceCode : uint32_t {
    START_REMOTE_ABILITY = 1,
    STOP_REMOTE_ABILITY = 3,
    START_ABILITY_FROM_REMOTE = 4,
    STOP_ABILITY_FROM_REMOTE = 5,
    CONNECT_REMOTE_ABILITY = 6,
    DISCONNECT_REMOTE_ABILITY = 7,
    CONNECT_ABILITY_FROM_REMOTE = 8,
    DISCONNECT_ABILITY_FROM_REMOTE = 9,
    START_CONTINUATION = 11,
    NOTIFY_COMPLETE_CONTINUATION = 12,
    NOTIFY_CONTINUATION_RESULT_FROM_REMOTE = 13,
    REGISTER_ABILITY_TOKEN = 14,
    UNREGISTER_ABILITY_TOKEN = 15,
    CONTINUE_ABILITY = 16,
    NOTIFY_PROCESS_DIED_FROM_REMOTE = 17,
    NOTIFY_DSCHED_EVENT_RESULT_FROM_REMOTE = 18,
    SEND_RESULT_FROM_REMOTE = 20,
    GET_REMOTE_APPTHREAD = 35,
    CONTINUE_MISSION = 36,
    CONTINUE_MISSION_OF_BUNDLENAME = 37,

    // requeset code for free install
    START_FREE_INSTALL_FROM_REMOTE = 51,
    NOTIFY_COMPLETE_FREE_INSTALL_FROM_REMOTE = 52,

    // request code for mission
    GET_MISSION_INFOS = 80,
    REGISTER_MISSION_LISTENER = 84,
    UNREGISTER_MISSION_LISTENER = 85,
    START_SYNC_MISSIONS_FROM_REMOTE = 86,
    STOP_SYNC_MISSIONS_FROM_REMOTE = 87,
    CHECK_SUPPORTED_OSD = 88,
    CHECK_SUPPORT_OSD_FROM_REMOTE = 89,
    MISSION_CHANGED = 90,
    NOTIFY_MISSIONS_CHANGED_FROM_REMOTE = 91,
    START_SYNC_MISSIONS = 92,
    TRY_OPENP2PSESSION_FROM_REMOTE = 93,
    SWITCH_CHANGED = 94,
    NOTIFY_SWITCH_CHANGED_FROM_REMOTE = 95,
    GET_CACHED_SUPPORTED_OSD = 96,
    ALL_CONNECT_TO_DMS = 97,
    STOP_SYNC_MISSIONS = 98,
    GET_REMOTE_MISSION_SNAPSHOT_INFO = 99,

    // request code for call ability
    START_REMOTE_ABILITY_BY_CALL = 150,
    RELEASE_REMOTE_ABILITY = 151,
    START_ABILITY_BY_CALL_FROM_REMOTE = 152,
    RELEASE_ABILITY_FROM_REMOTE = 153,
    NOTIFY_STATE_CHANGED_FROM_REMOTE = 154,

    // request code for upload distributed component info
    GET_DISTRIBUTED_COMPONENT_LIST = 161,
    
    START_REMOTE_FREE_INSTALL = 200,
    // form share
    START_REMOTE_SHARE_FORM = 220,
    START_SHARE_FORM_FROM_REMOTE = 221,

    // stop externsion ability
    STOP_REMOTE_EXTERNSION_ABILITY = 225,
    STOP_EXTERNSION_ABILITY_FROM_REMOTE = 226,
    REGISTER_ON_LISTENER = 260,
    REGISTER_OFF_LISTENER = 261,
    REGISTER_DSCHED_EVENT_LISTENER = 262,
    UNREGISTER_DSCHED_EVENT_LISTENER = 263,
    GET_CONTINUE_INFO = 264,

    // request code for set continue state
    SET_MISSION_CONTINUE_STATE = 300,
};

enum class IDRreeInstallCallbackInterfaceCode : uint32_t {
    ON_FREE_INSTALL_DONE = 0,
    CMD_MAX,
};

enum class IDBundleManagerCallbackInterfaceCod {
    ON_QUERY_INSTALLATION_DONE = 1,
};
} // namespace DistributedSchedule
} // namespace OHOS
#endif // OHOS_DISTRIBUTED_DISTRIBUTEDSCHED_IPC_INTERFACE_CODE_H

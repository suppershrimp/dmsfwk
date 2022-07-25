/**
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

import ServiceExtensionAbility from '@ohos.application.ServiceExtensionAbility';
import wantConstant from '@ohos.ability.wantConstant'

export default class ServiceAbilitySecond extends ServiceExtensionAbility {
    onCreate() {
        console.info('ServiceAbilitySecond onCreate');
    }

    onRequest(want, startId) {
        console.info('ServiceAbilitySecond onRequest, want: ' + want.abilityName);
        console.info('ServiceAbilitySecond onRequest, startId: ' + startId);
        console.info('ServiceAbilitySecond onRequest');
        let str = {
            'deviceId': '',
            'bundleName': 'com.example.qianyiyingyong.hmservice',
            'abilityName': 'com.example.qianyiyingyong.ServiceAbility',
            'flags': wantConstant.Flags.FLAG_INSTALL_ON_DEMAND,
        }
        console.info('ServiceAbilitySecond onRequest START, str is : ' + JSON.stringify(str));

        let options ={
            windowMode: 0,
            displayId:2
        }
        console.info('ServiceAbilitySecond onRequest START, options is : ' + JSON.stringify(options));

        this.context.startAbility(str, options).then((data) => {
            console.log("ServiceAbilitySecond onRequest data: " + JSON.stringify(data))
        }).catch((error) => {
            console.log("ServiceAbilitySecond onRequest error: " + JSON.stringify(error))
        })
    }

    onDestroy() {
        console.info('ServiceAbilitySecond onDestroy');
    }
};
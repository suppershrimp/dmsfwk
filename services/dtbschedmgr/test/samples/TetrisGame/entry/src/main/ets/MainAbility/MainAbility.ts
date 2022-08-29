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

import Ability from '@ohos.application.Ability'
import { TetrisViewModel, State, GameObject } from '../Model/TetrisViewModel'
import AbilityConstant from '@ohos.application.AbilityConstant'
import Logger from './Logger'
import bundle from '@ohos.bundle'
import accessControl from '@ohos.abilityAccessCtrl'

const BUNDLE_NAME = 'com.example.tetrisgame'
const PERMISSION_REJECT = -1

class ContentStorage {
}

var versionCode


export default class MainAbility extends Ability {
    contentStorage: ContentStorage
    //数据传输
    onContinue(wantParam: { [key: string]: any }) {
        if (versionCode != wantParam.version) {
            Logger.info('version is not same')
            return 1
        }
        Logger.info(wantParam.pageStack)
        var state: State = AppStorage.Get<State>('state')
        wantParam['state'] = state
        var game: TetrisViewModel = AppStorage.Get<TetrisViewModel>('game')
        game.pause()
        wantParam['gameObject'] = game.gameObject
        return 0
    }

    handleParam(want, launchParam) {
        if (launchParam.launchReason == AbilityConstant.LaunchReason.CONTINUATION) {
            Logger.info(want.parameters.pageStack)
            AppStorage.SetOrCreate<State>('state', want.parameters.state)
            AppStorage.SetOrCreate<TetrisViewModel>('game', new TetrisViewModel(want.parameters.gameObject))
            Logger.info('ready to restore')
            this.contentStorage = new ContentStorage()
            this.context.restoreWindowStage(this.contentStorage)
        }
    }

    onCreate(want, launchParam) {
        let bundleName = 'com.example.tetrisgame'
        let bundleFlags = 1
        let options = {
            'userId': 100
        }
        bundle.getBundleInfo(bundleName, bundleFlags, options)
            .then((data) => {
                versionCode = JSON.stringify(data.versionCode)
                Logger.info('Operation successful.  Data: ' + JSON.stringify(data.versionCode))
            }).catch((error) => {
            Logger.info('Operation failed. Cause: ' + JSON.stringify(error));
        })
        Logger.info('[Demo] MainAbility onCreate')
        globalThis.abilityWant = want;
        this.handleParam(want, launchParam)
    }

    onNewWant(want, launchParam) {
        Logger.info('[Demo] MainAbility onNewWant')
        this.handleParam(want, launchParam)
    }

    onDestroy() {
        console.log("[Demo] MainAbility onDestroy")
    }

    onWindowStageCreate(windowStage) {
        // Main window is created, set main page for this ability
        console.log("[Demo] MainAbility onWindowStageCreate")
        this.requestPermissions()
        windowStage.loadContent("pages/index", (err, data) => {
            if (err.code) {
                console.error('Failed to load the content. Cause:' + JSON.stringify(err));
                return;
            }
            console.info('Succeeded in loading the content. Data: ' + JSON.stringify(data))
        });
    }

    onWindowStageDestroy() {
        // Main window is destroyed, release UI related resources
        console.log("[Demo] MainAbility onWindowStageDestroy")
    }

    onForeground() {
        // Ability has brought to foreground
        console.log("[Demo] MainAbility onForeground")
    }

    onBackground() {
        // Ability has back to background
        let state: State = AppStorage.Get<State>('state')
        let game: TetrisViewModel = AppStorage.Get<TetrisViewModel>('game')
        if (state.isStarted) {
            state.isPause = true
            state.isMenu = true
            clearInterval(game.gameObject.timeOutId)
            AppStorage.Set<State>('state', state)
            AppStorage.Set<TetrisViewModel>('game', game)
        }

    }

    requestPermissions = async () => {
        let permissions: Array<string> = [
            'ohos.permission.DISTRIBUTED_DATASYNC'
        ];
        let needGrantPermission = false
        let accessManger = accessControl.createAtManager()
        Logger.info('app permission get bundle info')
        let bundleInfo = await bundle.getApplicationInfo(BUNDLE_NAME, 0, 100)
        Logger.info('app permission query permission ${bundle.accessTokenId.toString()}')
        for (const permission of permissions) {
            Logger.info('app permission query grant status ${permission}')
            try {
                let grantStatus = await accessManger.verifyAccessToken(bundleInfo.accessTokenId, permission)
                if (grantStatus == PERMISSION_REJECT) {
                    needGrantPermission = true
                    break
                }
            } catch (err) {
                Logger.error('app permission query grant status error ${permission} ${JSON.stringify(err)}')
                needGrantPermission = true
                break
            }
        }
        if (needGrantPermission) {
            Logger.info('app permission needGrantPermission')
            try {
                await this.context.requestPermissionsFromUser(permissions)
            } catch (err) {
                Logger.error('app permission ${JSON.stringify(err)}')
            }
        }
        else {
            Logger.info('app permission already granted')
        }
    }
};

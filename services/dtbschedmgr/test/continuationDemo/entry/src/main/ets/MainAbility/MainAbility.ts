import Ability from '@ohos.application.Ability';
import Want from '@ohos.application.Want';
import {LaunchParam} from '@ohos.application.Ability';

import {LaunchReason} from '@ohos.application.Ability';
import {OnContinueResult} from '@ohos.application.Ability';
import distributedObject from '@ohos.data.distributedDataObject';

class ContenStorage {
}
enum TestEnum {
    ONE = 1,
    TWO = 2,
}
var g_object = distributedObject.createDistributedObject({name:undefined, age:18, isVis:false});

export default class MainAbility extends Ability {
    contentStorage : ContenStorage
    sessionId : string;

    onCreate(want , launchParam) {
        // Ability is creating, initialize resources for this ability
        console.log("MainAbility onCreate-wdd", launchParam.launchReason);

        if (launchParam.launchReason == 3) {

            this.sessionId = want.parameters["session"]
            console.log("onCreate LaunchReason = CONTINUATION",want.parameters["session"]);

//            var dsobject2 = distributedObject.createDistributedObject({age:19});
            function statusCallback(sessionId, networkid, status) {
                console.info("continue object status changed sessionId: " + sessionId + " networkId: " + networkid);
                console.info("continue status: " + status + " g_object.name = " + g_object.name);
            }
            g_object.setSessionId(this.sessionId);

            g_object.on("status", statusCallback);
            let name = g_object.name;
//            console.log("continue sessionId: " + this.sessionId + ", name: " + dsobject2.name);

            this.contentStorage = new ContenStorage();
//            this.contentStorage.setOrCreate('objtest', {id:1231, name:"afvarvearv"})
            console.log("onCreate LaunchReason = CONTINUATION",want.parameters["myData"]);
            this.context.restoreWindowStage(this.contentStorage);
        }
    }

    onContinue(wantParams : {[key: string]: any}) {
//        var dsobject1 = distributedObject.createDistributedObject({name:"Amy", age:18, isVis:false});
        console.log("MainAbility onContinue ");
        this.sessionId = "369792";
        wantParams["myData"] = "my12345678";
        wantParams["session"] = this.sessionId;

        g_object.setSessionId(this.sessionId);
        g_object.name = "wdd";
        return 0;
    }

    onWindowStageRestore(windowStage) {
        // Main window is created, set main page for this ability
        console.log("MainAbility onWindowStageRestore")
    }

    onDestroy() {
        // Ability is destroying, release resources for this ability
        console.log("MainAbility onDestroy")
    }

    onWindowStageCreate(windowStage) {
        // Main window is created, set main page for this ability
        console.log("MainAbility onWindowStageCreate")

        let context = this.context
        globalThis.onButtonClick = (() => {
            context.startAbility({bundleName: "com.ohos.launcher", abilityName: "com.ohos.launcher.MainAbility"}).then(() => {
                console.info("Succeeded to start ability")
            }, (err) => {
                console.error("Failed to start ability", err.code)
            })
        })

        windowStage.setUIContent(this.context, "pages/index", null)
    }

    onWindowStageDestroy() {
        // Main window is destroyed, release UI related resources
        console.log("MainAbility onWindowStageDestroy")
    }

    onForeground() {
        // Ability has brought to foreground
        console.log("MainAbility onForeground")
    }

    onBackground() {
        // Ability has back to background
        console.log("MainAbility onBackground")
    }
};

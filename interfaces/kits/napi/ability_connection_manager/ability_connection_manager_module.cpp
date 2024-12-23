#include "native_engine/native_engine.h"
#include "js_ability_connection_manager.h"

static napi_module _module = {
    .nm_filename = "distributedsched/libabilityconnectionmanager_napi.z.so/abilityconnectionmanager.js",
    .nm_register_func = OHOS::DistributedCollab::JsAbilityConnectionManagerInit,
    .nm_modname = "distributedsched.abilityConnectionManager",
};

__attribute__((constructor)) static void NAPI_application_abilityconnectionmanager_AutoRegister()
{
    napi_module_register(&_module);
}
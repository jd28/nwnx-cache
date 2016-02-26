#include "NWNXCache.h"
#include "Manager.h"
#include "core/pluginlink.h"
#include "core/ipc/ipc.h"

void Manager::EventObjectCreated(const CNWSObject *obj, uint32_t reqid) {
    ObjectMask mask = static_cast<ObjectMask>(1 << obj->ObjectType);
    if(object_type_mask & mask) {
        manager->Add(obj, reqid);
    }
}

void Manager::EventObjectDestroyed(const CNWSObject *obj) {
    ObjectMask mask = static_cast<ObjectMask>(1 << obj->ObjectType);
    if(object_type_mask & mask) {
        manager->Erase(obj);
    }
}

void Manager::HookEvents() {

    bool h = SignalConnect(CoreCNWSObjectCreated, "CACHE", [this] (const void *object, nwobjid requestedId) -> bool {
        EventObjectCreated((const CNWSObject*)object, requestedId);
        return false;
    });
    h = SignalConnect(CoreCNWSObjectDestroyed, "CACHE", [this] (const void* object) -> bool {
        EventObjectDestroyed((const CNWSObject*)object);
        return false;
    });
}


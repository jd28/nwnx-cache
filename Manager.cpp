#include "NWNXCache.h"
#include "Manager.h"
#include "core/pluginlink.h"
#include "core/ipc/ipc.h"

void Manager::EventObjectCreated(const CNWSObject *obj, uint32_t reqid) {
    ObjectMask mask = static_cast<ObjectMask>(1 << obj->ObjectType);
    if(object_type_mask & mask) {
        manager->Add(obj, reqid);
    } else {
        cache.Log(0, "Not an object for us.\n");
    }
}

void Manager::EventObjectDestroyed(const CNWSObject *obj) {
    ObjectMask mask = static_cast<ObjectMask>(1 << obj->ObjectType);
    if(object_type_mask & mask) {
        manager->Erase(obj);
    } else {
        cache.Log(0, "Not an object for us.\n");
    }
}

void Manager::HookEvents() {

    bool h = SignalConnect(CoreCNWSObjectCreated, "CACHE", [this] (const void *object, nwobjid requestedId) -> bool {
        EventObjectCreated((const CNWSObject*)object, requestedId);
        return false;
    });
    if(!h) {
        cache.Log(0, "Unable to hook event EVENT_CORE_OBJECT_CREATED!\n");
    }

    h = SignalConnect(CoreCNWSObjectDestroyed, "CACHE", [this] (const void* object) -> bool {
        EventObjectDestroyed((const CNWSObject*)object);
        return false;
    });

    if(!h) {
        cache.Log(0, "Unable to hook event EVENT_CORE_OBJECT_CREATED!\n");
    }
}


#pragma once

#include "api/CNWSObject.h"
#include "include/nwn_const.h"
#include "core/ipc/ipc.h"
#include "core/pluginlink.h"

#include <functional>
#include <google/dense_hash_map>

namespace detail {

template <typename T>
static T DefaultAddCallback(const CNWSObject *, uint32_t) {
    return T();
}

template <typename T>
static void DefaultEraseCallback(const CNWSObject*, T&) {

}

}

enum class ObjectMask : size_t {
    INVALID = 0,
    GUI = 1 << OBJECT_TYPE_GUI,
    TILE = 1 << OBJECT_TYPE_TILE,
    MODULE = 1 << OBJECT_TYPE_MODULE,
    AREA = 1 << OBJECT_TYPE_AREA,
    CREATURE = 1 << OBJECT_TYPE_CREATURE,
    ITEM = 1 << OBJECT_TYPE_ITEM,
    TRIGGER = 1 << OBJECT_TYPE_TRIGGER,
    PROJECTILE = 1 << OBJECT_TYPE_PROJECTILE,
    PLACEABLE = 1 << OBJECT_TYPE_PLACEABLE,
    DOOR = 1 << OBJECT_TYPE_DOOR,
    AREA_OF_EFFECT = 1 << OBJECT_TYPE_AREA_OF_EFFECT,
    WAYPOINT = 1 << OBJECT_TYPE_WAYPOINT,
    ENCOUNTER = 1 << OBJECT_TYPE_ENCOUNTER,
    STORE = 1 << OBJECT_TYPE_STORE,
    PORTAL = 1 << OBJECT_TYPE_PORTAL,
    SOUND = 1 << OBJECT_TYPE_SOUND,
    ALL = 0xFFFFFFFF
};

inline ObjectMask operator&(ObjectMask lhs, ObjectMask rhs) {
    size_t l = static_cast<size_t>(lhs);
    size_t r = static_cast<size_t>(rhs);
    return static_cast<ObjectMask>(l & r);
}

inline ObjectMask operator|(ObjectMask lhs, ObjectMask rhs) {
    size_t l = static_cast<size_t>(lhs);
    size_t r = static_cast<size_t>(rhs);
    return static_cast<ObjectMask>(l | r);
}

class ObjectCache {
public:
    // Callback when an object is inserted into the hash.
    template <typename T>
    using AddCallback = std::function<T(const CNWSObject*, uint32_t)>;

    // Callback when an object is removed from the hash.
    template <typename T>
    using EraseCallback = std::function<void(const CNWSObject *, T&)>;

private:
    // Using type erasure... maybe not a great idea, but there is no
    // way I can think of to have this work cross plugin.

    struct concept {
        explicit concept(size_t size) : element_size(size) {}
        size_t element_size = 0;
        size_t GetElementSize() { return element_size; }
        virtual void Add(const CNWSObject* obj, uint32_t reqid) = 0;
        virtual void* Get(uint32_t obj) = 0;
        virtual void Erase(const CNWSObject *obj) = 0;
        virtual ~concept() = default;
    };

    template <typename T>
    struct model : public concept {
        google::dense_hash_map<uint32_t, T> holder;
        AddCallback<T> add;
        EraseCallback<T> erase;

        model(AddCallback<T> add, EraseCallback<T> erase)
            : concept(sizeof(T))
            , add{std::move(add)}
            , erase{std::move(erase)}
        {
            holder.set_empty_key(OBJECT_INVALID);
        }

        virtual void Erase(const CNWSObject *obj) override {
            auto it = holder.find(obj->ObjectID);
            if(it != holder.end()) {
                erase(obj, it->second);
                holder.erase(it);
            }
        }

        virtual void Add(const CNWSObject* obj, uint32_t reqid) override {
            holder.insert({obj->ObjectID, add(obj, reqid)});
        }

        void* Get(uint32_t obj) {
            auto it = holder.find(obj);
            if(it != holder.end()) {
                return &it->second;
            }
            return nullptr;
        }
    };

    void EventObjectCreated(const CNWSObject* obj, uint32_t reqid) {
        ObjectMask mask = static_cast<ObjectMask>(1 << obj->ObjectType);
        if((object_type_mask & mask) != ObjectMask::INVALID) {
            manager->Add(obj, reqid);
        }
    }

    void EventObjectDestroyed(const CNWSObject* obj) {
        ObjectMask mask = static_cast<ObjectMask>(1 << obj->ObjectType);
        if((object_type_mask & mask) != ObjectMask::INVALID) {
            manager->Erase(obj);
        }
    }

    void HookEvents() {

        bool h = SignalConnect(CoreCNWSObjectCreated, "CACHE", [this] (const void *object, nwobjid requestedId) -> bool {
            EventObjectCreated((const CNWSObject*)object, requestedId);
            return false;
        });
        h = SignalConnect(CoreCNWSObjectDestroyed, "CACHE", [this] (const void* object) -> bool {
            EventObjectDestroyed((const CNWSObject*)object);
            return false;
        });
    }

    std::unique_ptr<concept> manager;
    std::string name_;
    ObjectMask object_type_mask = ObjectMask::ALL;

    template <typename T>
    explicit ObjectCache(T* dummy, std::string name, ObjectMask object_type_mask = ObjectMask::ALL,
                     AddCallback<T> add_callback = detail::DefaultAddCallback<T>,
                     EraseCallback<T> erase_callback = detail::DefaultEraseCallback<T>)
        : manager{new model<T>(add_callback, erase_callback)}
        , name_{std::move(name)}
        , object_type_mask{object_type_mask}
    {
        HookEvents();
    }

public:

    template<typename T>
    static ObjectCache create(std::string name, ObjectMask object_type_mask = ObjectMask::ALL,
                   AddCallback<T> add_callback = detail::DefaultAddCallback<T>,
                   EraseCallback<T> erase_callback = detail::DefaultEraseCallback<T>) {
        T t;
        return ObjectCache(&t, name, object_type_mask, add_callback, erase_callback);
    }

    // Gets a cached element for an object.
    template<typename T>
    T* Get(uint32_t obj) {
        // The only sanity check. I guess some RTTI mechanism might be better.
        if(manager->GetElementSize() != sizeof(T)) {
            //objects.Log(0, "WARNING: sizeof(T) and the manager's type are not equal!\n");
            return nullptr;
        }
        return reinterpret_cast<T*>(manager->Get(obj));
    }

    const std::string name() const { return name_; }
};

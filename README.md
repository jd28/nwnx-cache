### nwnx_cache

#### What is this?

A plugin that can cache arbitrary data, filtered by object types.  Callbacks
are also available to the registering plugin that can initialize/destruct cached
data when an object is added/erased.  All managers registered with nwnx_cache are
owned by it.

ObjectCaches automatically hooks object created events.  Note these are currently only for
CNWSObjects.

The ObjectCache class could be used internally by plugins however there is some overhead by doing
so.

#### Usage

Any plugin can use the CacheRegisterObjectCache service to add a cache.

```c++
#include "plugins/cache/pluginlink.h"
#include "plugins/cache/ObjectCache.h"

// Somewhere in the CorePluginsLoaded event.

ObjectCache MyIntObjectCache = ObjectCache::create<int>(
  "AnIntObjectCache",
  // Only creatures and placeables allowed
  ObjectMask::CREATURE | ObjectMask::PLACEABLE,
  [](const CNWSObject*, nwobjid) -> int {
    return 42; // Intialize every initial entry.
  });

// Let the world know.
ServiceCall(CacheRegisterObjectCache, std::move(MyIntObjectCache));

```

Any plugin can request a manager to read cached data using the CacheGetObjectCache.  Likewise edit as well.  Note that references can be invalidated so do not cache values returned by Get.

```c++
#include "plugins/cache/pluginlink.h"
#include "plugins/cache/ObjectCache.h"

ObjectCache *TheirIntObjectCache = nullptr;
ServiceCall(CacheGetObjectCache, "AnIntObjectCache", &TheirIntObjectCache);

if(TheirIntObjectCache) {
  int* SomeObjectsInt = TheirIntObjectCache->Get<int>(SomeObjectID);
  if(SomeObjectsInt) {
    // Go crazy using this int.
  }
}
```


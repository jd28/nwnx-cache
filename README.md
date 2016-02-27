### nwnx_cache

#### What is this?

A plugin that can cache arbitrary data, filtered by object types.  Callbacks
are also available to the registering plugin that can initialize/destruct cached
data when an object is added/erased.  All managers registered with nwnx_cache are
owned by it.

Managers automatically hooks object created events.  Note these are currently only for
CNWSObjects.

The Manager class could be used internally by plugins however there is some overhead by doing
so.

#### Usage

Any plugin can use the ServiceCacheRegisterManager service to add a cache.

```c++
#include "plugins/cache/pluginlink.h"
#include "plugins/cache/Manager.h"

// Somewhere in the CorePluginsLoaded event.

static Manager MyIntManager = Manager::create<int>(
  "AnIntManger",
  // Only creatures and placeables allowed
  ObjectMask::CREATURE | ObjectMask::PLACEABLE,
  [](const CNWSObject*, nwobjid) -> int {
    return 42; // Intialize every initial entry.
  });

// Let the world know.
ServiceCall(ServiceCacheRegisterManager, &MyIntManager);

```

Any plugin can request a manager to read cached data using the ServiceGetRegisterManager.  Likewise edit as well.  Managers
are currently owned by their creating plugins.  Note that references
can be invalidated so do not cache values returned by Get.

```c++
#include "plugins/cache/pluginlink.h"
#include "plugins/cache/Manger.h"

Manager *TheirIntManager = nullptr;
ServiceCall(ServiceCacheGetManager, "AnIntManger", &TheirIntManager);

if(TheirIntManager) {
  int* SomeObjectsInt = manager->Get<int>(SomeObjectID);
  if(SomeObjectsInt) {
    // Go crazy using this int.
  }
}
```


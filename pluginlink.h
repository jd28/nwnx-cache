#include "core/ipc/Service.h"
#include "Manager.h"

using ServiceCacheRegisterManager = Service<
    // Cache takes ownership.  You will need to pass an rvalue reference.
    Manager
>;

using ServiceCacheGetManager = Service<
    // Manager name.
    const char*,
    // Manager return value.  Managers can never be removed so it's safe to
    // cache this value.  Note that due to the order of plugin destruction...
    Manager**
>;

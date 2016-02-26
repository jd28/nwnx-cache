#include "core/ipc/Service.h"

class Manager;

using ServiceCacheRegisterManager = Service<
    Manager*
>;

using ServiceCacheGetManager = Service<
    // Manager name.
    const char*,
    // Manager return value.  Managers can never be removed so it's safe to
    // cache this value.  Note that due to the order of plugin destruction...
    Manager**
>;

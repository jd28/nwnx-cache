#include "NWNXCache.h"

CNWNXCache cache;

PLUGININFO pluginInfo = {
    sizeof(PLUGININFO),
    "NWNXObjects",
    PLUGIN_MAKE_VERSION(0, 0, 0, 4),
    "",
    "jmd",
    "jmd2028@gmail.com",
    "(c) 2011-current jmd",
    "",
    0 //not transient
};


extern "C" PLUGININFO* GetPluginInfo(DWORD nwnxVersion)
{
    return &pluginInfo;
}

extern "C" CNWNXBase *GetClassObject()
{
    return &cache;
}

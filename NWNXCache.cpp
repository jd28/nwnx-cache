#include "NWNXCache.h"
#include "pluginlink.h"
#include "core/ipc/ipc.h"
#include "Manager.h"

extern CNWNXCache cache;

void RegisterManagerService(Manager* mgr) {
    if(mgr) {
        cache.RegisterManager(mgr);
    }
}

void GetManagerService(const char* name, Manager** mgr) {
    *mgr = cache.GetManager(name);
}

bool CNWNXCache::OnCreate(gline *config, const char *LogDir)
{
    confKey = "CACHE";

    char log[128];

    sprintf(log, "%s/nwnx_cache.txt", LogDir);

    // call the base class function
    if (!CNWNXBase::OnCreate(config, log))
        return false;

    ServiceRegister(ServiceCacheRegisterManager, [this](Manager* mgr) {
            if(mgr) { RegisterManager(mgr); }
    });

    ServiceRegister(ServiceCacheGetManager, [this](const char* name, Manager** mgr) {
        *mgr = cache.GetManager(name);
    });

    SignalConnect(CorePluginsLoaded, "CACHE", [this]() -> bool {
                      static Manager manager = Manager::create<int>("TEST_MANAGER");
                      RegisterManager(&manager);
                      return false;
                  });

    return true;
}

char *CNWNXCache::OnRequest(char *gameObject, char *Request, char *Parameters)
{
    return nullptr;
}

unsigned long CNWNXCache::OnRequestObject(char *gameObject, char *Request)
{
    return OBJECT_INVALID;
}

bool CNWNXCache::OnRelease()
{
    return true;
}

void CNWNXCache::RegisterManager(Manager *mgr)
{
   auto it = std::find_if(managers.cbegin(), managers.cend(), [mgr](const Manager* m) {
       return mgr->name() == m->name();
   });

   if(it == managers.end()) {
       Log(0, "Registering Manager: %s\n", mgr->name().c_str());
       managers.push_back(mgr);
   }
}

Manager *CNWNXCache::GetManager(const std::string& mgr)
{
    auto it = std::find_if(managers.begin(), managers.end(), [&mgr](const Manager* m) {
        return mgr == m->name();
    });
    if(it != managers.end()) {
        return *it;
    }
    return nullptr;
}

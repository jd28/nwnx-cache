#pragma once

#include "NWNXApi.h"
#include "NWNXBase.h"
#include "core/pluginlink.h"
#include "pluginlink.h"
#include <vector>

class Manager;

class CNWNXCache : public CNWNXBase {
    std::vector<Manager*> managers;

public:
    virtual bool OnCreate(gline *nwnxConfig, const char *LogFile) override;
    virtual char *OnRequest(char *gameObject, char *Request, char *Parameters) override;
    virtual unsigned long OnRequestObject(char *gameObject, char *Request) override;
    virtual bool OnRelease() override;

    void RegisterManager(Manager* mgr);
    Manager* GetManager(const std::string& mgr);
};

#pragma once

#include "ShotScript.h"

#include <memory>
#include <string>
#include <unordered_map>

class ShotScriptManager {
private:
    std::unordered_map<std::string, std::shared_ptr<ShotScript>> scripts;

public:
    static ShotScriptManager& GetInstance() {
        static ShotScriptManager instance;
        return instance;
    }

    std::shared_ptr<ShotScript> CloneShotScript(const std::string& id) {
        auto it = scripts.find(id);
        if (it == scripts.end()) return nullptr;
        return it->second->Clone();
    }


    void RegisterShotScript(const std::string& id, std::shared_ptr<ShotScript> script) {
        
        if (id.empty() || !script) {
            return; // ID‚ª‹ó‚Ü‚½‚ÍƒXƒNƒŠƒvƒg‚ª–³Œø‚Èê‡‚Í‰½‚à‚µ‚È‚¢
        }

        // ‚·‚Å‚É“o˜^‚³‚ê‚Ä‚¢‚È‚¯‚ê‚Î‘}“ü
        auto it = scripts.find(id);

        if (it != scripts.end()) {
            return;
        }
        scripts[id] = script;
    }
};

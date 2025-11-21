#pragma once

#include "EnemyScript.h"

#include <memory>
#include <string>
#include <unordered_map>

class EnemyScriptManager {
private:
    std::unordered_map<std::string, std::shared_ptr<EnemyScript>> scripts;

public:
    static EnemyScriptManager& GetInstance() {
        static EnemyScriptManager instance;
        return instance;
    }

    std::shared_ptr<EnemyScript> CloneEnemyScript(const std::string& id) {
        auto it = scripts.find(id);
        if (it == scripts.end()) return nullptr;
        return it->second->Clone();
    }

    void RegisterEnemyScript(const std::string& id, std::shared_ptr<EnemyScript> script) {
        
        if(id.empty() || !script) {
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

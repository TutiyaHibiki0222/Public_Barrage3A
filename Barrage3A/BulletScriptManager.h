#pragma once

#include "EnemyBullet.h"


#include <memory>
#include <string>
#include <unordered_map>

class BulletScriptManager {
private:
    std::unordered_map<std::string, std::shared_ptr<BulletScript>> scripts;
public:
    static BulletScriptManager& GetInstance() {
        static BulletScriptManager instance;
        return instance;
    }

    std::shared_ptr<BulletScript> CloneBulletScript(const std::string& id) {
        auto it = scripts.find(id);
        if (it == scripts.end()) return nullptr;
        return it->second->Clone();
    }

    void RegisterBulletScript(const std::string& id, std::shared_ptr<BulletScript> script) {
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


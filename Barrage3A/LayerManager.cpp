#include "LayerManager.h"

namespace GameEngine {

    bool LayerManager::LoadLayers(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;

        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string idStr, name;
            std::getline(ss, idStr, ',');
            std::getline(ss, name, ',');

            int id = std::stoi(idStr);
            Layer layer = ToLayer(id);

            nameToLayer[name] = layer;
            layerToName[layer] = name;
        }

        return true;
    }

    bool LayerManager::LoadCollisionMatrix(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;

        std::string line;

        // 1行目：ヘッダ読み飛ばし
        std::getline(file, line);
        std::vector<std::string> headers;
        std::stringstream ssHeader(line);
        std::string name;
        std::getline(ssHeader, name, ','); // 空のセル（左上）
        while (std::getline(ssHeader, name, ',')) {
            headers.push_back(name);
        }

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string rowName;
            std::getline(ss, rowName, ',');

            Layer rowLayer = GetLayerByName(rowName);
            uint32_t mask = 0;

            for (size_t i = 0; i < headers.size(); ++i) {
                std::string value;
                std::getline(ss, value, ',');

                if (value == "1") {
                    Layer colLayer = GetLayerByName(headers[i]);
                    mask |= ToMask(colLayer);
                }
            }

            collisionMatrix[static_cast<int>(rowLayer)] = mask;
        }

        return true;
    }

    Layer LayerManager::GetLayerByName(const std::string& name) const {
        auto it = nameToLayer.find(name);
        if (it != nameToLayer.end()) return it->second;
        return Layer::Default; // fallback
    }

    const std::string& LayerManager::GetLayerName(Layer layer) const {
        return layerToName.at(layer);
    }

}
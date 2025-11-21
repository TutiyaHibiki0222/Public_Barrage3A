#pragma once
#include <unordered_map>
#include <memory>
#include "File.hpp"
#include "JsonUtility.h"
#include "Path2D.h"


struct PathData {
    std::vector<Vector2D> points;
    std::vector<int> segmentInterpolations;
    JSON(points, segmentInterpolations)
};

class PathManager {
private:
    std::unordered_map<int, std::shared_ptr<Path2D>> paths;

    PathManager() {}

public:
    PathManager(const PathManager&) = delete;
    PathManager& operator=(const PathManager&) = delete;

    static PathManager& GetInstance() {
        static PathManager instance;
        return instance;
    }

    // パスを登録
    void AddPath(int id, std::shared_ptr<Path2D> path) {
        paths[id] = path;
    }

    void AddJsonPath(int id, const std::string& filepath) {
        try {
            auto jsonText = System::IO::File::ReadAllText(filepath);
            auto data = JsonUtility::FromJson<PathData>(jsonText);

            auto path = std::make_shared<Path2D>();
            path->SetPath(data.points, data.segmentInterpolations);

            AddPath(id, path);
        }
        catch (...) {

        }
    }

    // パスを取得。存在しなければ nullptr
    std::shared_ptr<Path2D> GetPath(int id) const {
        auto it = paths.find(id);
        if (it != paths.end()) {
            return it->second;
        }
        return nullptr;
    }
};

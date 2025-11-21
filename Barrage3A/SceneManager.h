#pragma once
#include "Scene.h"      // sceneóﬁ.
#include "Transitor.h"

enum class SceneType {
    Title,
    Game,
    Settings,
    GameOver
};

class SceneManager {
private:
    std::unique_ptr<Scene> currentScene;
    std::unique_ptr<Transitor> transitor;           // é¿çsíÜÇÃââèo

    SceneType scene     = SceneType::Title;
    SceneType newScene  = SceneType::Title;
    bool forceReload    = false;

    bool isTransitor    = false;
    SceneManager() {}

    std::unique_ptr<Scene> CreateScene(SceneType type);

public:
    static SceneManager& GetInstance();

    void Init(SceneType type);
    void LoadScene(SceneType type, bool reload = false);
	void ReLoadScene();
    void Update();
    void UpdateTransitor();
    void DrawTransitor();
    void Draw();
    void Release();

    template<
        typename T, typename... Args,
        typename = std::enable_if_t<
        std::is_base_of<Transitor, T>::value &&
        std::is_constructible<T, int, Args...>::value
        >
    >
    void SetTransitor(int frame = 60, Args&&... args) {
        if (transitor && transitor->IsStart()) return; // Ç∑Ç≈Ç…ëJà⁄íÜÇ»ÇÁâΩÇ‡ÇµÇ»Ç¢

        try {
            transitor = std::make_unique<T>(frame, std::forward<Args>(args)...);
        }
        catch (const std::exception& e) {
            std::cerr << "Transitor creation failed: " << e.what() << std::endl;
            transitor.reset();
        }
        catch (...) {
            std::cerr << "Unknown error in SetTransitor." << std::endl;
            transitor.reset();
        }
    }

    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
};

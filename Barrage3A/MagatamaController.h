/*
    ◆ MagatamaController.h

    クラス名        : MagatamaController クラス
    作成日          : 2025/07/10 (木) 0:50:18
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#pragma once
#include "headers.h"
#include "AppBase.h"
#include "Texture2D.h"
#include "Magatama.h"
#include <deque>

// MagatamaController class and AppBase class
class MagatamaController : public AppBase {
private:    // 非公開.
    int maxTrailLength = 60;
    int magatamaMax    = 4;
    std::vector<std::shared_ptr<Magatama>> magatamas;
    std::vector<std::vector<Vector2D>> normalOffsets = {
        /* count=1 */
        { Vector2D(0, +45) },

        /* count=2 */
        { Vector2D(-40, 0), Vector2D(40, 0) },

        /* count=3 */
        { Vector2D(-40, 30), Vector2D(40, 30), Vector2D(0, 45) },

        /* count=4 */
        { Vector2D(-40, +15), Vector2D(40, +15), Vector2D(-20, +35), Vector2D(20, +35) }
    };
    std::vector<std::vector<Vector2D>> focusedOffsets = {
        /* count=1 */
        { Vector2D(0, +25) },

        /* count=2 */
        { Vector2D(-15, 25), Vector2D(15, 25) },

        /* count=3 */
        { Vector2D(-30, 20), Vector2D(30, 20), Vector2D(0, 35) },

        /* count=4 */
        { Vector2D(-30, +25), Vector2D(30, +25), Vector2D(-10, +35), Vector2D(10, +35) }
    };

public:     // 公開.

    // コンストラクタ.
    MagatamaController();
    MagatamaController(std::shared_ptr<GameObject>);

    // AppBase 関数.
    void Init(int count, std::shared_ptr<GameEngine::Sprite> spriteTemplate, Vector2D pos = {0,0});
    void SetMagatamaCount(int level);
    std::vector<Vector2D> GetMagatamaPositions();
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<MagatamaController>(*this);
    }
};
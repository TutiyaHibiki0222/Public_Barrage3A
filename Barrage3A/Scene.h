#pragma once
// シーンクラス.
class Scene {
public:
    virtual ~Scene() {}
    virtual void Init()     = 0;        // 初期化.
    virtual void Update()   = 0;        // 更新処理.
    virtual void Draw()     = 0;        // 描画処理.
    virtual void Release()  = 0;        // リソース解放.
};

// TitleScene .
//class TitleScene : public Scene {
//private:
//
//
//public:
//    void Init()     override;
//    void Update()   override;
//    void Draw()     override;
//    void Release()  override;
//};

//// GameScene .
//class GameScene : public Scene {
//private:
//	bool isChangeScene = false; // シーン変更フラグ.
//
//
//public:
//    void Init() override;
//    void Update() override;
//    void Draw() override;
//    void Release() override;
//};


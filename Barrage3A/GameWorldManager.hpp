/*
    GameWorldManager.hpp

    class-
    - GameWorldManager

    作成日         : 2025/04/10
    最終変更日     : 2025/04/10
*/
#pragma once
//
// AppBase Camera と 固定World座標を管理する class;
//

#include "Vector.h"
#include "Transform2D.h"

namespace GameEngine {

    class GameWorldManager {
        friend class Camera;
    private:
        Vector2D worldPosition;    // ワールド位置 (基本 position 0,0が中心になるように);
        std::shared_ptr<Transform2D> cameraTransform2D;  // カメラ位置を指すポインタ(中心 + cameraの位置 用)
        
        // 振動関連
        float shakeDuration = 0.0f;         // 残り振動時間
        float shakeMagnitude = 0.0f;        // 振動の強さ（振れ幅）
        Vector2D shakeOffset = { 0, 0 };    // 実際に加算するオフセット

        // コンストラクタでカメラ位置をポインタで受け取る
        GameWorldManager() : worldPosition(0, 0), cameraTransform2D(nullptr) {}
    public:

        static GameWorldManager& GetInstance() {
            static GameWorldManager instance;
            return instance;
        }

        // ワールドオフセットを取得するメソッド
        Vector2D WorldOffSet() const {
            if (cameraTransform2D) {
                // カメラ位置のオフセット
                Vector2D offset = cameraTransform2D->GetLocalPosition();
                //offset.y = -offset.y;
                offset.x = -offset.x;
                // 画面の中心に合わせて調整したワールド座標
                return worldPosition + offset + shakeOffset;
            }
            return worldPosition + shakeOffset;
        }

        // ワールド位置の設定
        void SetWorldPosition(const Vector2D& newPosition) {
            worldPosition = newPosition;
        }

        // ワールド位置の取得
        Vector2D GetWorldPosition() const {
            return worldPosition;
        }

        Vector2D ScreenToWorld(const Vector2D screenPos) const {
            Vector2D relativeToCenter = screenPos - worldPosition;
            if (cameraTransform2D) {
                // カメラ位置のオフセット
                Vector2D offset = cameraTransform2D->GetLocalPosition();
                relativeToCenter.y = -relativeToCenter.y;
                // 画面の中心に合わせて調整したワールド座標
                return relativeToCenter - offset;
            }
            // スクリーン中央からの相対位置に変換（画面左上 → 中央原点）
            relativeToCenter.y = -relativeToCenter.y;
            return relativeToCenter;
        }
        // カメラの位置を設定
        void SetCameraPosition(std::shared_ptr<Transform2D> newPosition) {
            cameraTransform2D = newPosition;
        }

        Vector2D ScreenToViewport(const Vector2D& screenPos) const {
            Vector2D screenSize = Window::GetInstance().GetMaxVector2D();
            return Vector2D(screenPos.x / screenSize.x, screenPos.y / screenSize.y);
        }

        Vector2D ViewportToWorld(const Vector2D& viewportPos) const {
            Vector2D screenSize = Window::GetInstance().GetMaxVector2D();
            return  Vector2D(viewportPos.x * screenSize.x, viewportPos.y * screenSize.y);
        }

        // カメラを振動させる（秒数と強さ）
        void StartShake(float duration, float magnitude) {
            shakeDuration = duration;
            shakeMagnitude = magnitude;
        }

        void Update() {
            if (shakeDuration > 0.0f) {
                shakeDuration -= Time.deltaTime;
                float randX = (Random.Value()) * 2.0f - 1.0f;
                float randY = (Random.Value()) * 2.0f - 1.0f;
                shakeOffset.x = randX * shakeMagnitude;
                shakeOffset.y = randY * shakeMagnitude;
                if (shakeDuration <= 0.0f) {
                    shakeOffset = Vector2D(0, 0);  // 終了
                }
            }
        }

    };
}


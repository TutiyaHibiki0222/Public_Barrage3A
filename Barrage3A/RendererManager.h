/*
    RendererManager.hpp

    :class
        - RendererManager

    作成日         : 2025/05/04
    最終変更日     : 2025/05/04
*/
#pragma once
#define DEBUG_RENDERER (_DEBUG && false)


#include <vector>
#include <memory>
#include <thread>
#include <mutex>

#include "Linq.hpp"
#include "IDraw.h"
#include "GameEngine.h"
#include "Application.hpp"
#include "Canvas.h" 

/// <summary>
/// 描画処理を管理するManager.
/// </summary>
class RendererManager {
private:

    std::vector<std::weak_ptr<IRendererDraw>> renderers;

    RendererManager() = default;
    ~RendererManager() = default;
public:
    static RendererManager& GetInstance() {
        static RendererManager instance;
        return instance;
    }

    void AddRenderer(std::shared_ptr<IRendererDraw> renderer) {
        renderers.push_back(renderer);
    }

    bool IsAABBInsideScreen(const RectF& aabb, int screenW, int screenH) {
        return !(aabb.x + aabb.w < 0 || aabb.y + aabb.h < 0 || aabb.x > screenW || aabb.y > screenH);
    }

    void Remove(std::shared_ptr<IRendererDraw> renderer) {
        // std::find_if を使って renderer が一致する weak_ptr を見つける
        auto it = std::find_if(renderers.begin(), renderers.end(),
            [&renderer](const std::weak_ptr<IRendererDraw>& w) {
                if (auto spt = w.lock()) {
                    return spt == renderer; // shared_ptr と比較
                }
                return false; // 無効な weak_ptr は無視
            });

        // 見つかった場合は削除
        if (it != renderers.end()) {
            renderers.erase(it);
        }
    }

    void Render() {
        std::vector<std::shared_ptr<IRendererDraw>> overlayRenderers;
        std::vector<std::shared_ptr<IRendererDraw>> cameraRenderers;
        std::vector<std::shared_ptr<IRendererDraw>> worldRenderers;

        Vector2D winSize = Window::GetInstance().GetMaxVector2D();
#if DEBUG_RENDERER
        auto start = std::chrono::high_resolution_clock::now();
#endif
        if (renderers.size() >= GameEngine::Application::GetInstanse().THRESHOLD) {
            CheckVisibleRenderers(renderers, overlayRenderers, cameraRenderers, worldRenderers, winSize);
        }
        else {
            for (const auto& w : renderers) {
                if (auto spt = w.lock()) {
                    if (!spt->IsDraw()) continue;

                    RectF aabb = spt->GetAABB();
                    if (!IsAABBInsideScreen(aabb, (int)winSize.x, (int)winSize.y)) continue;

                    switch (spt->GetRenderMode()) {
                    case RenderMode::ScreenSpaceOverlay:
                        overlayRenderers.push_back(spt);
                        break;
                    case RenderMode::ScreenSpaceCamera:
                        cameraRenderers.push_back(spt);
                        break;
                    case RenderMode::WorldSpace:
                    default:
                        worldRenderers.push_back(spt);
                        break;
                    }
                }
            }

        }
#if DEBUG_RENDERER
        auto aabbEnd = std::chrono::high_resolution_clock::now();   // 計測終了
        auto cDuration = std::chrono::duration_cast<std::chrono::microseconds>(aabbEnd - start);

        std::cout << "AABB処理    : " << cDuration.count() << "μs" << std::endl;
        auto drawStart = std::chrono::high_resolution_clock::now();
#endif

        // 深度ソートなど必要に応じてソートして描画
        std::sort(worldRenderers.begin(), worldRenderers.end(), [](auto& a, auto& b) {
            return a->GetSortingOrder() < b->GetSortingOrder();
            });
        for (auto& r : worldRenderers) r->Draw();

        // CameraSpace UI
        std::sort(cameraRenderers.begin(), cameraRenderers.end(), [](auto& a, auto& b) {
            return a->GetSortingOrder() < b->GetSortingOrder();
            });
        for (auto& r : cameraRenderers) r->Draw();

        // Overlay UI は最後に描画
        std::sort(overlayRenderers.begin(), overlayRenderers.end(), [](auto& a, auto& b) {
            return a->GetSortingOrder() < b->GetSortingOrder();
            });
        for (auto& r : overlayRenderers) r->Draw();

#if DEBUG_RENDERER
        auto end = std::chrono::high_resolution_clock::now();   // 計測終了
        auto drawDuration = std::chrono::duration_cast<std::chrono::microseconds>(end - drawStart);
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "描画処理     : " << drawDuration.count() << "μs" << std::endl;
        std::cout << "描画処理合計 : " << duration.count() << "μs" << std::endl;

        // デバッグ情報を画面に表示
        std::string debugInfo = "Draw count: " + std::to_string(visibleRenderers.size()) + ", No draw count: " + std::to_string(renderers.size() - visibleRenderers.size());

        // 画面の上部にデバッグ情報を表示
        DrawString(10, 10, debugInfo.c_str(), GetColor(255, 255, 255));
#endif
    }
    void CheckVisibleRenderers(
        const std::vector<std::weak_ptr<IRendererDraw>>& renderers,
        std::vector<std::shared_ptr<IRendererDraw>>& overlayRenderers,
        std::vector<std::shared_ptr<IRendererDraw>>& cameraRenderers,
        std::vector<std::shared_ptr<IRendererDraw>>& worldRenderers,
        const Vector2D& winSize,
        int numThreads = std::thread::hardware_concurrency()
    ) {
        std::mutex visMutex;
        std::vector<std::thread> threads;

        size_t total = renderers.size();
        size_t chunk = (total + numThreads - 1) / numThreads;

        for (int t = 0; t < numThreads; ++t) {
            size_t start = t * chunk;
            size_t end = std::min(start + chunk, total);

            threads.emplace_back([&, start, end]() {
                // スレッドローカルの振り分け配列を用意
                std::vector<std::shared_ptr<IRendererDraw>> localOverlay;
                std::vector<std::shared_ptr<IRendererDraw>> localCamera;
                std::vector<std::shared_ptr<IRendererDraw>> localWorld;

                for (size_t i = start; i < end; ++i) {
                    if (auto spt = renderers[i].lock()) {
                        if (!spt->IsDraw()) continue;

                        RectF aabb = spt->GetAABB();
                        if (!IsAABBInsideScreen(aabb, (int)winSize.x, (int)winSize.y)) continue;

                        switch (spt->GetRenderMode()) {
                        case RenderMode::ScreenSpaceOverlay:
                            localOverlay.push_back(spt);
                            break;
                        case RenderMode::ScreenSpaceCamera:
                            localCamera.push_back(spt);
                            break;
                        case RenderMode::WorldSpace:
                        default:
                            localWorld.push_back(spt);
                            break;
                        }
                    }
                }

                // 排他制御でまとめて共有配列に追加
                std::lock_guard<std::mutex> lock(visMutex);
                overlayRenderers.insert(overlayRenderers.end(), localOverlay.begin(), localOverlay.end());
                cameraRenderers.insert(cameraRenderers.end(), localCamera.begin(), localCamera.end());
                worldRenderers.insert(worldRenderers.end(), localWorld.begin(), localWorld.end());
                });
        }

        for (auto& t : threads) t.join();
    }
};
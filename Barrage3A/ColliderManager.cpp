#include "ColliderManager.h"
#include "GameObject.h"
#include "QuadTree.h"
#include "GameObjectMgr.h"
#include "LayerManager.h"
#include "Stopwatch.hpp"

#include "CollisionDispatcher.h"

#if (false)
//void CollisionManager::CheckCollisions() {
//    // 現在の接触状態を保持するためのセット
//    std::unordered_set<std::pair<GameObject*, GameObject*>, pair_hash> currentCollisions;
//
//    int checkCount = 0;     // 衝突チェック回数
//    int collisionCount = 0; // 衝突しているオブジェクトのカウント
//
//    for (size_t i = 0; i < colliders.size(); ++i) {
//        auto colliderA = colliders[i];
//
//        for (size_t j = i + 1; j < colliders.size(); ++j) {
//            auto colliderB = colliders[j];
//            checkCount++; // 衝突チェックのカウントを増加
//
//            // 衝突が発生したかチェック
//            if (colliderA->CheckCollision(colliderB)) {
//                collisionCount++; // 衝突が検出された場合、カウントを増加
//
//                // 現在の衝突状態を記録
//                currentCollisions.insert({ colliderA->GetGameObject(), colliderB->GetGameObject() });
//
//                // 以前の衝突リストに存在しない場合、OnCollisionEnterを呼び出す
//                if (!previousCollisions.count({ colliderA->GetGameObject(), colliderB->GetGameObject() })) {
//                    colliderA->GetGameObject()->OnCollisionEnter(colliderB->GetGameObject());
//                    colliderB->GetGameObject()->OnCollisionEnter(colliderA->GetGameObject());
//                }
//                else {
//                    // 以前の衝突状態がある場合はOnCollisionStayを呼び出す
//                    colliderA->GetGameObject()->OnCollisionStay(colliderB->GetGameObject());
//                    colliderB->GetGameObject()->OnCollisionStay(colliderA->GetGameObject());
//                }
//            }
//        }
//    }
//
//    // 前回の衝突状態から現在の衝突状態を引き、OnCollisionExitを呼び出す
//    for (const auto& pair : previousCollisions) {
//        if (!currentCollisions.count(pair)) {
//            pair.first->OnCollisionExit(pair.second);
//            pair.second->OnCollisionExit(pair.first);
//        }
//    }
//
//    // 前回の衝突状態を更新
//    previousCollisions = currentCollisions;
//
//    // 結果の表示
//    printfDx("衝突チェック回数 : %d\n", checkCount);
//    printfDx("当たっているオブジェクトの数 : %d\n", collisionCount);
//}
#else

void CollisionManager::CheckCollisions() {
    if (colliders.size() < 2) return;

    // DebugStopwatch::Start("CollisionManager");

    // イベントを遅延処理するためのリスト
    std::vector<std::pair<CollisionPair, CollisionEventType>> collisionEvents;

    switch (mode)
    {
    case CollisionCheckMode::QuadTree:
        CheckCollisionsQuadTreeMode(collisionEvents);
        break;
    case CollisionCheckMode::Layer_Vs_Layer:
        CheckCollisionsLayerVsLayerMode(collisionEvents);
        break;
    case CollisionCheckMode::Beta_LinearQuadTree:
        break;
    default:
        break;
    }

    
    // すべての衝突チェックが終了した後にイベントを発生させる
    for (const auto& event : collisionEvents) {
        auto obj1 = event.first.first;
        auto obj2 = event.first.second;
        
        auto CallEvent = [&](auto eventFunc) {
            if (obj1) (obj1.get()->*eventFunc)(obj2.get());
            if (obj2) (obj2.get()->*eventFunc)(obj1.get());
        };

        switch (event.second) {
        case CollisionEventType::Enter: CallEvent(&GameObject::OnCollisionEnter); break;
        case CollisionEventType::Stay:  CallEvent(&GameObject::OnCollisionStay); break;
        case CollisionEventType::Exit:  CallEvent(&GameObject::OnCollisionExit); break;
        }
    }


    // DebugStopwatch::PrintStop("CollisionManager","CheckCollisionsメソッド 掛かった時間");
}
#endif

MyRectangle CollisionManager::CalculateWorldBounds() {
    float minX =  std::numeric_limits<float>::infinity();
    float minY =  std::numeric_limits<float>::infinity();
    float maxX = -std::numeric_limits<float>::infinity();
    float maxY = -std::numeric_limits<float>::infinity();

    for (const auto& collider : colliders) {
        Vector2D position = collider->GetGameObject()->transform->GetWorldPosition();
        MyRectangle bounds = collider->GetBounds();

        minX = Mathf::Min(minX, position.x - bounds.width / 2);
        minY = Mathf::Min(minY, position.y - bounds.height / 2);
        maxX = Mathf::Max(maxX, position.x + bounds.width / 2);
        maxY = Mathf::Max(maxY, position.y + bounds.height / 2);
    }

    // コライダーが1つもない場合のエラーハンドリング
    if (minX ==  std::numeric_limits<float>::infinity() ||
        minY ==  std::numeric_limits<float>::infinity() ||
        maxX == -std::numeric_limits<float>::infinity() ||
        maxY == -std::numeric_limits<float>::infinity()) {
        std::cerr << "Warning: No colliders found for bounds calculation." << std::endl;
        return MyRectangle(0, 0, 0, 0); // デフォルト値を返す
    }

    return MyRectangle(minX, minY, maxX - minX, maxY - minY);
}

void CollisionManager::RemoveCollisionObject(std::shared_ptr<GameObject> gameObject) {
    for (size_t i = 0; i < colliders.size(); ) {
        auto obj = colliders[i]->GetGameObject();
        if (obj == gameObject) {
            std::swap(colliders[i], colliders.back());
            colliders.pop_back();
        }
        else {
            ++i;
        }
    }
}


void CollisionManager::CheckCollisionsQuadTreeMode(std::vector<std::pair<CollisionPair, CollisionEventType>>& collisionEvents) {
    MyRectangle bounds = isQuadTrueSizeAuto ? CalculateWorldBounds() : myRectangleSize;
    QuadTree quadTree(bounds, maxObjects, maxLevels);

    std::vector<std::shared_ptr<Collider2D>> activeColliders;
    activeColliders.reserve(colliders.size());

    for (const auto& collider : colliders) {
        if (!collider->IsEnabled()) continue;
        auto go = collider->GetGameObject();
        if (!go || !go->IsActive()) continue;
        activeColliders.push_back(collider);
        quadTree.Insert(collider);
    }

    std::set<CollisionPair> currentCollisions;
    std::vector<std::shared_ptr<Collider2D>> foundColliders;

    for (const auto& collider : activeColliders) {
        foundColliders.clear();
        quadTree.Retrieve(collider->GetBounds(), foundColliders);

        for (const auto& otherCollider : foundColliders) {
            if (collider == otherCollider) continue;

            auto goA = collider->GetGameObject();
            auto goB = otherCollider->GetGameObject();
            if (!goA || !goB) continue;

            Layer layer1 = goA->GetLayer();
            Layer layer2 = goB->GetLayer();
            if (!LayerManager::GetInstance().CanCollide(layer1, layer2)) continue;

            CollisionPair pair = MakeOrderedPair(goA, goB);
            if (currentCollisions.count(pair)) continue;

#if DEBUG_COLLIDER
            auto pos1 = goA->transform->GetWorldPosition();
            auto pos2 = goB->transform->GetWorldPosition();
            DrawLine((int)pos1.x, (int)pos1.y, (int)pos2.x, (int)pos2.y, GetColor(0, 255, 0));
#endif
            if (CollisionDispatcher::CheckCollision(collider, otherCollider)) {
                currentCollisions.insert(pair);

                if (!previousCollisions.count(pair)) {
                    collisionEvents.push_back({ pair, CollisionEventType::Enter });
                }
                else {
                    collisionEvents.push_back({ pair, CollisionEventType::Stay });
                }
            }
        }
    }

    for (const auto& pair : previousCollisions) {
        if (!currentCollisions.count(pair)) {
            collisionEvents.push_back({ pair, CollisionEventType::Exit });
        }
    }

#if DEBUG_COLLIDER
    quadTree.Draw();
#endif

    std::swap(previousCollisions, currentCollisions);
}

#if(TRUE)
void CollisionManager::CheckCollisionsLayerVsLayerMode(std::vector<std::pair<CollisionPair, CollisionEventType>>& collisionEvents) {
    std::vector<std::vector<std::shared_ptr<Collider2D>>> collidersByLayer((size_t)Layer::Count);
	// レイヤーごとにコライダーを分類.
    for (auto& collider : colliders) {
        if (!collider->IsEnabled()) continue;
        auto go = collider->GetGameObject();
        if (!go || !go->IsActive()) continue;
        auto layerIdx = static_cast<size_t>(go->GetLayer());
        if (layerIdx >= collidersByLayer.size()) continue;
        collidersByLayer[layerIdx].push_back(collider);
    }

    std::set<CollisionPair> currentCollisions;

    auto tryCollision = [&](const std::shared_ptr<Collider2D>& a, const std::shared_ptr<Collider2D>& b) {
        auto goA = a->GetGameObject();
        auto goB = b->GetGameObject();
        if (!goA || !goB) return;

        auto pair = MakeOrderedPair(goA, goB);
        if (currentCollisions.count(pair)) return;

        if (CollisionDispatcher::CheckCollision(a, b)) {
            currentCollisions.insert(pair);

            if (!previousCollisions.count(pair)) {
                collisionEvents.push_back({ pair, CollisionEventType::Enter });
            }
            else {
                collisionEvents.push_back({ pair, CollisionEventType::Stay });
            }
        }
        };

#pragma omp parallel for
    auto& mask = LayerManager::GetInstance();
    for (int i = 0; i < (int)Layer::Count; ++i) {
        for (int j = i; j < (int)Layer::Count; ++j) {
            if (!mask.CanCollide((Layer)i, (Layer)j)) continue;

            auto& listA = collidersByLayer[i];
            auto& listB = collidersByLayer[j];

            if (listA.empty() || listB.empty()) continue;

            if (i == j) {
                for (size_t a = 0; a < listA.size(); ++a) {
                    for (size_t b = a + 1; b < listB.size(); ++b) {
                        tryCollision(listA[a], listB[b]);
                    }
                }
            }
            else {
                for (auto& a : listA) {
                    for (auto& b : listB) {
                        tryCollision(a, b);
                    }
                }
            }
        }
    }

    // Exit イベント発行
    for (const auto& pair : previousCollisions) {
        if (!currentCollisions.count(pair)) {
            collisionEvents.push_back({ pair, CollisionEventType::Exit });
        }
    }

    std::swap(previousCollisions, currentCollisions);
}
#else
#include <future>
void CollisionManager::CheckCollisionsLayerVsLayerMode(std::vector<std::pair<CollisionPair, CollisionEventType>>& collisionEvents) {
    std::vector<std::vector<std::shared_ptr<Collider2D>>> collidersByLayer((size_t)Layer::Count);

    for (auto& collider : colliders) {
        if (!collider->IsEnabled()) continue;
        auto go = collider->GetGameObject();
        if (!go || !go->IsActive()) continue;
        collidersByLayer[(size_t)go->GetLayer()].push_back(collider);
    }

    std::unordered_set<std::pair<uintptr_t, uintptr_t>, pair_hash> currentCollisions;
    std::mutex mutex;  // currentCollisions 用

    std::vector<std::future<std::vector<std::pair<CollisionPair, CollisionEventType>>>> futures;

    for (int i = 0; i < (int)Layer::Count; ++i) {
        for (int j = i; j < (int)Layer::Count; ++j) {
            if (!LayerManager::GetInstance().CanCollide((Layer)i, (Layer)j)) continue;

            const auto& listA = collidersByLayer[i];
            const auto& listB = collidersByLayer[j];

            if (listA.empty() || listB.empty()) continue;

            futures.push_back(std::async(std::launch::async, [&, listA, listB, i, j]() {
                std::vector<std::pair<CollisionPair, CollisionEventType>> localEvents;

                for (size_t a = 0; a < listA.size(); ++a) {
                    size_t bStart = (i == j) ? a + 1 : 0;
                    for (size_t b = bStart; b < listB.size(); ++b) {
                        auto aCol = listA[a];
                        auto bCol = listB[b];
                        if (!aCol || !bCol) continue;
                        auto goA = aCol->GetGameObject();
                        auto goB = bCol->GetGameObject();
                        if (!goA || !goB) continue;

                        uintptr_t addrA = reinterpret_cast<uintptr_t>(goA.get());
                        uintptr_t addrB = reinterpret_cast<uintptr_t>(goB.get());
                        auto pairKey = addrA < addrB ? std::make_pair(addrA, addrB) : std::make_pair(addrB, addrA);

                        if (aCol->CheckCollision(bCol)) {
                            std::lock_guard<std::mutex> lock(mutex);
                            if (currentCollisions.insert(pairKey).second) {
                                if (previousCollisions.count(pairKey)) {
                                    localEvents.push_back({ {goA, goB}, CollisionEventType::Stay });
                                }
                                else {
                                    localEvents.push_back({ {goA, goB}, CollisionEventType::Enter });
                                }
                            }
                        }
                    }
                }
                return localEvents;
                }));
        }
    }

    for (auto& f : futures) {
        auto result = f.get(); // join
        collisionEvents.insert(collisionEvents.end(), result.begin(), result.end());
    }

    // 衝突終了イベントの判定（Exit）
    for (const auto& pair : previousCollisions) {
        if (!currentCollisions.count(pair)) {
            collisionEvents.push_back({
                { GameObjectMgr::GetInstance().FindByRawPtr(pair.first),
                  GameObjectMgr::GetInstance().FindByRawPtr(pair.second) },
                CollisionEventType::Exit
                });
        }
    }

    std::swap(currentCollisions, previousCollisions);
}
#endif
#pragma once
/*
    Coroutine.hpp

    class-
    - YieldInstruction
    - WaitForSeconds
    - WaitForSecondsRealtime
	- WaitForFrames
    - Coroutine
    - WaitUntil
    - WaitWhile
    - WaitForFixedUpdate
    - CoroutineManager

    作成日         : 2025/01/27
    最終変更日     : 2025/04/13
*/

// c++ 20以上.
#if (_MSVC_LANG >= 202002L)

#include <coroutine>
#include <memory>
#include <vector>
#include <thread>
#include <functional>
// プログラムを C# 風に変更するマクロ.
#define null     nullptr
#define _yield   co_yield

namespace System {

    enum class YieldInstructionType {
        None,               // Default.
        WaitForFixedUpdate, // 最適化用に.
    };

    class YieldInstruction {
    public:
        virtual bool IsReady() const = 0;
        virtual ~YieldInstruction() = default;
        virtual YieldInstructionType GetType() const { return YieldInstructionType::None; }
    };

    // 秒間待機.
    class WaitForSeconds : public YieldInstruction {
    private:
        float duration;
        float startTime;
    public:
        explicit WaitForSeconds(float seconds)
            : duration(seconds), startTime(Time.time) {}

        bool IsReady() const override {
            return (Time.time - startTime) >= duration;
        }
    };

    // TimeScale影響しない.
    class WaitForSecondsRealtime : public YieldInstruction {
    private:
        float duration;
        float startTime;
    public:
        explicit WaitForSecondsRealtime(float seconds)
            : duration(seconds), startTime(Time.unscaledTime) {}

        bool IsReady() const override {
            return (Time.unscaledTime - startTime) >= duration;
        }
    };

    // フレーム数分待機.
    class WaitForFrames : public YieldInstruction {
    private:
        int framesToWait;
        int startFrame;
    public:
        explicit WaitForFrames(int frames)
            : framesToWait(frames), startFrame(Time.frameCount) {}

        bool IsReady() const override {
            return (Time.frameCount - startFrame) >= framesToWait;
        }
    };

    // コルーチン オブジェ.
    class Coroutine : public std::enable_shared_from_this<Coroutine>
    {
        friend class CoroutineManager;
    public:
        // プロミスタイプ.
        struct promise_type {
            using OwnerType = void*;
            OwnerType owner = nullptr; // 実際のオーナー.

            // Coroutine型のオブジェクトを返すためのメソッド.
            Coroutine get_return_object() {
                return Coroutine{ std::coroutine_handle<promise_type>::from_promise(*this) };
            }
            // コルーチンの初期サスペンドポイントを定義.
            std::suspend_always initial_suspend()   noexcept { return {}; }
            // コルーチンの終了サスペンドポイントを定義.
            std::suspend_always final_suspend()     noexcept { return {}; }
            // コルーチンの戻り値がvoidの場合に呼び出.
            void return_void() {}
            // コルーチン内で例外がスローされた場合に呼び出.
            void unhandled_exception() { std::terminate(); }

            // 他のコルーチンを待機する処理.
            std::shared_ptr<Coroutine> nestedCoroutine = nullptr;  // ネストされたコルーチンのポインタ.
            std::suspend_always yield_value(std::shared_ptr<Coroutine> coroutine) noexcept {
                nestedCoroutine = coroutine; // ネストコルーチンの shared_ptr をコピー.
                return {};
            }

            // 現在のYieldInstructionを保持するポインタ.
            // コルーチンの実行中に渡される値を一時的に保持.
            YieldInstruction* currentYield;
            // 値を「yield」するときに呼び出される.
            // 渡されたYieldInstructionポインタをcurrentYieldに保存し、コルーチンの実行を一時停止.
            std::suspend_always yield_value(YieldInstruction* __yield) noexcept {
                currentYield = __yield;
                return {};
            }

            // キャンセル用.
            bool isCanceled = false;
            void cancel() noexcept          { isCanceled = true; }
            bool canceled() const noexcept  { return isCanceled; }
        };

        // コンストラクタ.
        explicit Coroutine(std::coroutine_handle<promise_type> h) : handle(h) {}
        
        // ムーブコンストラクタ.
        Coroutine(Coroutine&& other) noexcept : handle(std::move(other.handle)) {
            other.handle = nullptr;
        }

        // 禁止.
        Coroutine(const Coroutine&) = delete;
        Coroutine& operator=(const Coroutine&) = delete;
        Coroutine& operator=(Coroutine&& other) noexcept {
            if (this != &other) {
                Destroy();
                handle = other.handle;
                other.handle = nullptr;
            }
            return *this;
        }
        // デストラクタ.
        ~Coroutine() {
            Destroy();
        }
        
        /// <summary>
        /// Coroutine 中身の全てのタスクが終えているか？.
        /// </summary>
        /// <returns></returns>
        bool IsDone() const {
            return !handle || handle.done();
        }
        
        /// <summary>
        /// コルーチンを進行させる（更新する）.
        /// </summary>
        /// <returns>実行可能であれば resume</returns>
        bool Update() {
            if (IsDone()) return false;

            auto& promise = handle.promise();

            if (promise.canceled()) {
                return false;
            }

            // ネストされたコルーチンの処理.
            if (promise.nestedCoroutine) {
                if (promise.nestedCoroutine->IsDone()) {
                    promise.nestedCoroutine.reset();
                }
                else {
                    return true; // ネストされたコルーチンがまだ実行中.
                }

            }

            // `YieldInstruction` の処理.
            if (!promise.currentYield || promise.currentYield->IsReady()) {
                promise.currentYield = nullptr;
                handle.resume();
            }

            return !IsDone();
        }

        /// <summary>
        /// 自身と 相手が 同じかどうか?.
        /// </summary>
        /// <param name="other"> 相手 </param>
        /// <returns> 等しかったら true </returns>
        bool operator==(const System::Coroutine& other) const {
            return this->handle == other.handle;
        }
        
        /// <summary>
        /// オーナーclassを取得.
        /// </summary>
        /// <returns>オーナー</returns>
        auto GetOwner() const { return handle.promise().owner; }
        
        /// <summary>
        /// オーナーclassを static_cast 型 で取得.
        /// </summary>
        /// <typeparam name="T"> 型 </typeparam>
        /// <returns>型を取得.</returns>
        template <typename T>
        T* GetOwnerAs() const {
            return static_cast<T*>(handle.promise().owner);
        }
        
        /// <summary>
        /// キャンセルされているか.
        /// </summary>
        /// <returns> キャンセル されているか ,(してる:true / false)</returns>
        bool IsCanceled() const { return handle.promise().canceled(); }
        /// <summary>
        /// キャンセルするか.
        /// </summary>
        void Cancel()           { handle.promise().cancel(); }
        /// <summary>
        /// 自身の shared_ptr を返す.
        /// </summary>
        /// <returns> shared_ptr<Coroutine> </returns>
        std::shared_ptr<Coroutine> GetMySharedQtr() { return shared_from_this(); }

    protected:
        void Destroy() {
            if (handle) handle.destroy();
            handle = nullptr;
        }
        // c++ 20 標準の コルーチンライブラリ.
        std::coroutine_handle<promise_type> handle;
    };

    // 条件が終了するまで待機.
    class WaitUntil : public YieldInstruction {
    public:
        // コルーチンが終了まで待機.
        explicit WaitUntil(const std::shared_ptr<Coroutine>& coroutine)
            : coroutineToWait(coroutine) {}
        // とある条件まで待機.
        explicit WaitUntil(std::function<bool()> condition)
            : conditionFunc(std::move(condition)) {}

        bool IsReady() const override {
            // 条件が指定されていれば、それをチェック.
            if (conditionFunc) {
                return conditionFunc();
            }
            // コルーチンが指定されていれば、その終了状態をチェック.
            if (auto coroutine = coroutineToWait.lock()) {
                return coroutine->IsDone();
            }
            return true;  // コルーチンが無効（終了または解放された場合）として扱う.
        }

    private:
        std::weak_ptr<Coroutine> coroutineToWait;
        std::function<bool()> conditionFunc;
    };

    class WaitWhile : public YieldInstruction {
        std::function<bool()> condition;

    public:
        WaitWhile(std::function<bool()> cond)
            : condition(std::move(cond)) {}

        bool IsReady() const override {
            return !condition();  // 条件が false になったら再開
        }
    };
    // FixUpdateまで待機
    class WaitForFixedUpdate : public YieldInstruction {
        bool triggered = false;

    public:
        void Trigger() { triggered = true; }

        bool IsReady() const override {
            return triggered;
        }

        YieldInstructionType GetType() const override { return YieldInstructionType::WaitForFixedUpdate; }
    };

    class WaitForCancel : public YieldInstruction {
        std::weak_ptr<System::Coroutine> coroutine;

    public:
        explicit WaitForCancel(std::shared_ptr<System::Coroutine> coro)
            : coroutine(coro) {
        }

        bool IsReady() const override {
            auto c = coroutine.lock();
            return c ? c->IsCanceled() : true;  // コルーチンがなくなったら即再開
        }
    };

    class CoroutineManager {
    private:
        CoroutineManager() = default;
        ~CoroutineManager() = default;

        // 禁止されたコピー・代入
        CoroutineManager(const CoroutineManager&) = delete;
        CoroutineManager& operator=(const CoroutineManager&) = delete;

        std::vector<std::shared_ptr<Coroutine>> coroutines;
        std::vector<std::shared_ptr<Coroutine>> newCoroutines;
        std::vector<std::shared_ptr<Coroutine>> coroutinesToRemove;
    public:
        static CoroutineManager& GetInstance() {
            static CoroutineManager instance;
            return instance;
        }

        // コルーチンを開始する.
        std::shared_ptr<Coroutine> StartCoroutine(Coroutine coroutine) {
            auto coro_ptr = std::make_shared<Coroutine>(std::move(coroutine));
            newCoroutines.push_back(coro_ptr);  // 新しいコルーチンを一時的に保存.
            return coro_ptr;
        }

        // コルーチンを開始する.
        std::shared_ptr<Coroutine> StartCoroutine(Coroutine coroutine, void* owner) {
            auto coro_ptr = std::make_shared<Coroutine>(std::move(coroutine));
            coro_ptr->handle.promise().owner = owner;
            newCoroutines.push_back(coro_ptr);  // 新しいコルーチンを一時的に保存.
            return coro_ptr;
        }

        // コルーチンを停止.
        void StopCoroutine(std::shared_ptr<Coroutine> coroutine) {
            coroutinesToRemove.push_back(coroutine);
        }

        // 全てのコルーチンを終了.
        void StopAllCoroutines() {
            coroutines.clear();
            newCoroutines.clear();
            coroutinesToRemove.clear();
        }

        void StopCoroutinesByOwner(void* owner) {
            for (auto& c : coroutines) {
                if (c->GetOwner() == owner) {
                    coroutinesToRemove.push_back(c);
                }
            }
            newCoroutines.erase(
                std::remove_if(newCoroutines.begin(), newCoroutines.end(),
                    [owner](const std::shared_ptr<Coroutine>& c) {
                        return c->GetOwner() == owner;
                    }),
                newCoroutines.end()
            );
        }

        void FixedUpdate() {
            for (auto& c : coroutines) {
                auto* yield = c->handle.promise().currentYield;
                if (yield && yield->GetType() == YieldInstructionType::WaitForFixedUpdate) {
                    static_cast<WaitForFixedUpdate*>(yield)->Trigger();
                }
            }
        }

        void Update() {
            // 新規追加.
            coroutines.insert(coroutines.end(), newCoroutines.begin(), newCoroutines.end());
            newCoroutines.clear();
            
            // 停止指定されたコルーチンはまとめて削除
            if (!coroutinesToRemove.empty()) {
                coroutines.erase(
                    std::remove_if(coroutines.begin(), coroutines.end(),
                        [this](const std::shared_ptr<Coroutine>& c) {
                            return std::find(coroutinesToRemove.begin(), coroutinesToRemove.end(), c) != coroutinesToRemove.end();
                        }),
                    coroutines.end()
                );
                coroutinesToRemove.clear();
            }
            // コルーチンを進行・終了済みの削除.
            coroutines.erase(
                std::remove_if(coroutines.begin(), coroutines.end(),
                    [](auto& c) { return !c->Update(); }),
                coroutines.end()
            );
        }

        bool IsAllCoroutinesFinished() const {
            return coroutines.empty();
        }
    };

    inline std::shared_ptr<Coroutine> StartCoroutine(Coroutine coroutine) {
        return CoroutineManager::GetInstance().StartCoroutine(std::move(coroutine));
    }

    inline std::shared_ptr<Coroutine> StartCoroutine(Coroutine coroutine,void* owner) {
        return CoroutineManager::GetInstance().StartCoroutine(std::move(coroutine), owner);
    }

    inline void StopCoroutine(std::shared_ptr<Coroutine> coroutine) {
        CoroutineManager::GetInstance().StopCoroutine(coroutine);
    }

    inline void StopAllCoroutines() {
        CoroutineManager::GetInstance().StopAllCoroutines();
    }

    inline void StopAllCoroutines(void* owner) {
        CoroutineManager::GetInstance().StopCoroutinesByOwner(owner);
    }
} // namespace System

#define CoManager System::CoroutineManager::GetInstance()
#endif
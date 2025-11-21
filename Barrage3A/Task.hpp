/*
	
	// 非同期処理の結果を表すTask
*/
#pragma once
#include <coroutine>
#include <exception>
#include <memory>
#include <utility>
#include <iostream>

template<typename T = void>
class UniTask {
public:
    // void用Promise
    struct PromiseVoid {
        std::exception_ptr exception;
        std::coroutine_handle<> continuation = nullptr;

        UniTask get_return_object() {
            return UniTask{ std::coroutine_handle<PromiseVoid>::from_promise(*this) };
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        struct final_awaiter {
            bool await_ready() noexcept { return false; }
            template<typename PromiseType>
            void await_suspend(std::coroutine_handle<PromiseType> h) noexcept {
                if (h.promise().continuation) h.promise().continuation.resume();
            }
            void await_resume() noexcept {}
        };
        auto final_suspend() noexcept { return final_awaiter{}; }

        void return_void() noexcept {}
        void unhandled_exception() noexcept { exception = std::current_exception(); }
    };

    // 非void用Promise
    struct PromiseValue {
        T result{};
        std::exception_ptr exception;
        std::coroutine_handle<> continuation = nullptr;

        UniTask get_return_object() {
            return UniTask{ std::coroutine_handle<PromiseValue>::from_promise(*this) };
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        struct final_awaiter {
            bool await_ready() noexcept { return false; }
            template<typename PromiseType>
            void await_suspend(std::coroutine_handle<PromiseType> h) noexcept {
                if (h.promise().continuation) h.promise().continuation.resume();
            }
            void await_resume() noexcept {}
        };
        auto final_suspend() noexcept { return final_awaiter{}; }

        void return_value(T value) noexcept { result = std::move(value); }
        void unhandled_exception() noexcept { exception = std::current_exception(); }
    };

    using promise_type = std::conditional_t<std::is_void_v<T>, PromiseVoid, PromiseValue>;

    using handle_type = std::coroutine_handle<promise_type>;

    UniTask(handle_type h) : coro(h) {}
    ~UniTask() { if (coro) coro.destroy(); }

    UniTask(const UniTask&) = delete;
    UniTask& operator=(const UniTask&) = delete;

    UniTask(UniTask&& other) noexcept : coro(other.coro) {
        other.coro = nullptr;
    }
    UniTask& operator=(UniTask&& other) noexcept {
        if (this != &other) {
            if (coro) coro.destroy();
            coro = other.coro;
            other.coro = nullptr;
        }
        return *this;
    }

    bool await_ready() const noexcept {
        return !coro || coro.done();
    }

    bool await_suspend(std::coroutine_handle<> awaiting) noexcept {
        coro.promise().continuation = awaiting;
        return true; // 中断して再開は外部に任せる
    }

    auto await_resume() {
        if (coro.promise().exception) {
            std::rethrow_exception(coro.promise().exception);
        }
        if constexpr (std::is_void_v<T>) {
            return;
        }
        else {
            return coro.promise().result;
        }
    }

    void resume() {
        if (coro && !coro.done()) coro.resume();
    }

    bool done() const {
        return !coro || coro.done();
    }

private:
    handle_type coro;
};
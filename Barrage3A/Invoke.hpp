/*
    Invoke.hpp

    class-
    - TimerTask
    - InvokeManager 

    çÏê¨ì˙         : 2025/04/13
    ç≈èIïœçXì˙     : 2025/04/13
*/
#pragma once

#include "GameEngine.h"
#include <functional>

namespace System {

    class TimerTask {
    public:
        float delay;
        float elapsed;
        bool repeat;
        std::function<void()> callback;

    public:
        TimerTask(float delay, std::function<void()> callback, bool repeat = false)
            : delay(delay), elapsed(0), callback(callback), repeat(repeat) {}

        void Execute() {
            callback();
            if (repeat) {
                elapsed = 0;
            }
        }
    };

    class InvokeManager {
    private:
        std::vector<TimerTask> newTasks;
        std::vector<TimerTask> tasks;
        InvokeManager() {};

    public:
        static InvokeManager& GetInstance() {
            static InvokeManager instance;
            return instance;
        }

        void Update() {
            for (auto it = tasks.begin(); it != tasks.end();) {
                it->elapsed += Time.unscaledDeltaTime;
                if (it->elapsed >= it->delay) {
                    it->Execute();
                    if (!it->repeat) {
                        it = tasks.erase(it);
                    }
                    else {
                        ++it;
                    }
                }
                else {
                    ++it;
                }
            }

            tasks.insert(tasks.end(), newTasks.begin(), newTasks.end());
            newTasks.clear();
        }

        void Invoke(std::function<void()> callback, float delay, bool repeat = false) {
            newTasks.emplace_back(delay, callback, repeat);
        }

        void Clear() {
            tasks.clear();
            newTasks.clear();
        }

        void CancelTask(std::function<void()> callback) {
            tasks.erase(std::remove_if(tasks.begin(), tasks.end(),
                [&callback](const TimerTask& task) {
                    return task.callback.target<void()>() == callback.target<void()>();
                }), tasks.end());
        }
    };

    inline void Invoke(std::function<void()> callback, float delay, bool repeat = false) {
        InvokeManager::GetInstance().Invoke(callback, delay, repeat);
    }

}
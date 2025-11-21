#pragma once

#include <iostream>
#include <chrono>
#include <unordered_map>
#include <string>

#ifdef _DEBUG

class DebugStopwatch {
public:
    static void Start(const std::string& key) {
        _startTimes[key] = std::chrono::high_resolution_clock::now();
    }

    static long long Stop(const std::string& key) {
        auto end = std::chrono::high_resolution_clock::now();
        auto it = _startTimes.find(key);
        if (it != _startTimes.end()) {
            auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - it->second).count();
            _startTimes.erase(it);
            return elapsed;
        }
        return -1;
    }

    static long long Elapsed(const std::string& key) {
        auto now = std::chrono::high_resolution_clock::now();
        auto it = _startTimes.find(key);
        if (it != _startTimes.end()) {
            return std::chrono::duration_cast<std::chrono::microseconds>(now - it->second).count();
        }
        return -1;
    }
    // 追加: key と任意のメッセージ付きで時間を表示
    static void Print(const std::string& key, const std::string& message) {
        long long elapsed = Elapsed(key);
        if (elapsed >= 0) {
            std::cout << key << " : " << message << " : " << elapsed << " us" << std::endl;
        }
        else {
            std::cout << key << " : " << message << " : " << "No timing info." << std::endl;
        }
    }

    static void PrintStop(const std::string& key, const std::string& message) {
        long long elapsed = Stop(key);
        if (elapsed >= 0) {
            std::cout << key << " : " << message << " : " << elapsed << " us" << std::endl;
        }
        else {
            std::cout << key << " : " << message << " : " << "No timing info." << std::endl;
        }
    }

private:
    static std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> _startTimes;
};
#else

// _DEBUG でない場合は空実装
class DebugStopwatch {
public:
    static void Start(const std::string&) {}
    static long long Stop(const std::string&) { return 0; }
    static long long Elapsed(const std::string&) { return 0; }
    static void Print(const std::string& key, const std::string& message) {}
    static void PrintStop(const std::string & key, const std::string & message) {}
};

#endif

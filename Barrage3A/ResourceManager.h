#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include <typeindex>
#include <stdexcept>
#include <iostream>

class ResourceManager {
private:
    struct ResourceEntry {
        std::shared_ptr<void> data;
        std::type_index type;

        ResourceEntry()
            : data(nullptr), type(typeid(void)) {
        }

        ResourceEntry(std::shared_ptr<void> d, std::type_index t)
            : data(std::move(d)), type(t) {
        }
    };

    inline static std::unordered_map<std::string, ResourceEntry> resourceDataMap;

public:
    // ------------------------------------------------------------
    // 登録
    // ------------------------------------------------------------
    template <typename T>
    static void Set(const std::string& key, const T& value) {
        resourceDataMap.insert_or_assign(
            key,
            ResourceEntry(std::make_shared<T>(value), std::type_index(typeid(T)))
        );
    }

    template <typename T>
    static void Set(const std::string& key, std::shared_ptr<T> ptr) {
        resourceDataMap.insert_or_assign(
            key,
            ResourceEntry(ptr, std::type_index(typeid(T)))
        );
    }

    // ------------------------------------------------------------
    // 通常取得（参照共有）
    // ------------------------------------------------------------
    template <typename T>
    static T& Get(const std::string& key) {
        auto it = resourceDataMap.find(key);
        if (it == resourceDataMap.end())
            throw std::runtime_error("Resource not found: " + key);

        const auto& entry = it->second;
        if (entry.type != std::type_index(typeid(T)))
            throw std::runtime_error("Type mismatch for resource: " + key);

        return *std::static_pointer_cast<T>(entry.data);
    }

    template <typename T>
    static std::shared_ptr<T> GetPtr(const std::string& key) {
        auto it = resourceDataMap.find(key);
        if (it == resourceDataMap.end())
            throw std::runtime_error("Resource not found: " + key);

        const auto& entry = it->second;
        if (entry.type != std::type_index(typeid(T)))
            throw std::runtime_error("Type mismatch for resource: " + key);

        return std::static_pointer_cast<T>(entry.data);
    }

    // ------------------------------------------------------------
    // Clone（安全複製版）: データをコピーして返す
    // ------------------------------------------------------------
    template <typename T>
    static T GetClone(const std::string& key) {
        auto it = resourceDataMap.find(key);
        if (it == resourceDataMap.end())
            throw std::runtime_error("Resource not found: " + key);

        const auto& entry = it->second;
        if (entry.type != std::type_index(typeid(T)))
            throw std::runtime_error("Type mismatch for resource: " + key);

        // コピーコンストラクタで複製
        return *std::static_pointer_cast<T>(entry.data);
    }

    // ------------------------------------------------------------
    // Clone（安全複製版）: shared_ptrを返す
    // ------------------------------------------------------------
    template <typename T>
    static std::shared_ptr<T> GetPtrClone(const std::string& key) {
        auto it = resourceDataMap.find(key);
        if (it == resourceDataMap.end())
            throw std::runtime_error("Resource not found: " + key);

        const auto& entry = it->second;
        if (entry.type != std::type_index(typeid(T)))
            throw std::runtime_error("Type mismatch for resource: " + key);

        // 新しいshared_ptrとしてクローンを生成
        return std::make_shared<T>(*std::static_pointer_cast<T>(entry.data));
    }

    // ------------------------------------------------------------
    // 存在チェック
    // ------------------------------------------------------------
    static bool Has(const std::string& key) {
        return resourceDataMap.find(key) != resourceDataMap.end();
    }

    // ------------------------------------------------------------
    // 削除（個別 or 全体）
    // ------------------------------------------------------------
    static void Remove(const std::string& key) {
        resourceDataMap.erase(key);
    }

    static void Clear() {
        resourceDataMap.clear();
    }
};

using Resource = ResourceManager;

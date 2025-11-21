#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <array>
#include <sstream>
#include <type_traits>
#include <stdexcept>
#include <unordered_map> // これに変更
#include "JsonValue.hpp"

// Forward declaration
template<typename T>
struct Reflector;

// toString関数群
// 追加する toString テンプレート（Reflector 経由）
template <typename T>
std::string toString(const T& obj) {
    std::unordered_map<std::string, std::string> fieldMap;
    Reflector<T>::Reflect(obj, fieldMap);

    std::string result = "{";
    for (auto it = fieldMap.begin(); it != fieldMap.end(); ++it) {
        result += "\"" + it->first + "\":" + it->second;
        auto next = it;
        ++next;
        if (next != fieldMap.end()) result += ",";
    }
    result += "}";
    return result;
}
inline std::string toString(int value) { return std::to_string(value); }
inline std::string toString(float value) { return std::to_string(value); }
inline std::string toString(double value) { return std::to_string(value); }
inline std::string toString(bool value) { return value ? "true" : "false"; }
inline std::string toString(const std::string& value) {
    std::string escaped = "\"";
    for (char c : value) {
        if (c == '\"') escaped += "\\\"";
        else if (c == '\\') escaped += "\\\\";
        else escaped += c;
    }
    escaped += "\"";
    return escaped;
}

template <typename T>
std::string toString(const std::vector<T>& values) {
    std::string result = "[";
    for (const auto& val : values) {
        result += toString(val) + ",";
    }
    if (!values.empty()) result.pop_back();
    result += "]";
    return result;
}

template <typename T, size_t N>
std::string toString(const std::array<T, N>& a) {
    std::string result = "[";
    for (size_t i = 0; i < N; ++i) {
        result += toString(a[i]);
        if (i < N - 1) result += ",";
    }
    result += "]";
    return result;
}
template <typename K, typename V>
std::string toString(const std::unordered_map<K, V>& m) {
    std::string result = "{";
    bool first = true;
    for (const auto& [key, val] : m) {
        if (!first) result += ",";
        result += toString(key) + ":" + toString(val);
        first = false;
    }
    result += "}";
    return result;
}

template <typename K, typename V>
std::string toString(const std::map<K, V>& m) {
    std::string result = "{";
    bool first = true;
    for (const auto& [key, val] : m) {
        if (!first) result += ",";
        result += toString(key) + ":" + toString(val);
        first = false;
    }
    result += "}";
    return result;
}

// split関数（文字列をカンマ区切りで分割し、空白除去）
inline std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);
        tokens.push_back(token);
    }
    return tokens;
}

// reflectImpl ヘルパー関数
template <typename Tuple, size_t... Is>
void reflectImpl(
    const std::vector<std::string>& fields,
    const Tuple& values,
    std::unordered_map<std::string, std::string>& fieldMap,
    std::index_sequence<Is...>
) 
{
    ((fieldMap[fields[Is]] = toString(std::get<Is>(values))), ...);
}

template <typename Tuple>
void reflectImpl(const std::vector<std::string>& fields, const Tuple& values, std::unordered_map<std::string, std::string>& fieldMap) {
    reflectImpl(fields, values, fieldMap, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
}

// 独自トレイト：std::vector判定
template<typename T> struct is_std_vector : std::false_type {};
template<typename T, typename A> struct is_std_vector<std::vector<T, A>> : std::true_type {};

// FromJsonValue実装
template<typename T>
T FromJsonValue(const JsonValue& v) {
    if constexpr (std::is_same_v<T, int>) {
        if (!v.IsPrimitive()) throw std::runtime_error("Expected int");
        return static_cast<int>(std::get<double>(v.AsPrimitive()));
    }
    else if constexpr (std::is_same_v<T, float>) {
        if (!v.IsPrimitive()) throw std::runtime_error("Expected float");
        return static_cast<float>(std::get<double>(v.AsPrimitive()));
    }
    else if constexpr (std::is_same_v<T, double>) {
        if (!v.IsPrimitive()) throw std::runtime_error("Expected double");
        return std::get<double>(v.AsPrimitive());
    }
    else if constexpr (std::is_same_v<T, bool>) {
        if (!v.IsPrimitive()) throw std::runtime_error("Expected bool");
        return std::get<bool>(v.AsPrimitive());
    }
    else if constexpr (std::is_same_v<T, std::string>) {
        if (!v.IsPrimitive()) throw std::runtime_error("Expected string");
        return std::get<std::string>(v.AsPrimitive());
    }
    else if constexpr (is_std_vector<T>::value) {
        if (!v.IsArray()) throw std::runtime_error("Expected array");
        std::vector<typename T::value_type> result;
        for (const auto& elem : v.AsArray()) {
            result.push_back(FromJsonValue<typename T::value_type>(elem));
        }
        return result;
    }
    else {
        // 構造体やユーザー定義型の処理
        if (!v.IsObject()) throw std::runtime_error("Expected object");
        T obj;
        Reflector<T>::From(obj, v.AsObject());
        return obj;
    }
}

template<typename K, typename V>
std::unordered_map<K, V> FromJsonValue(const JsonValue& v) {
    if (!v.IsObject()) throw std::runtime_error("Expected object for unordered_map");
    std::unordered_map<K, V> result;
    for (const auto& [keyStr, valJson] : v.AsObject()) {
        // キー変換：キーは文字列なので、必要に応じてKに変換
        if constexpr (std::is_same_v<K, std::string>) {
            // そのまま文字列を使う
            result.emplace(keyStr, FromJsonValue<V>(valJson));
        }
        else if constexpr (std::is_enum_v<K>) {
            // Enumなら文字列→enum変換ロジックを用意する（後述）
            K keyEnum = Reflector<K>::FromString(keyStr);
            result.emplace(keyEnum, FromJsonValue<V>(valJson));
        }
        else {
            // 数値型などは string→数値変換が必要ならここで実装
            std::istringstream iss(keyStr);
            K key;
            iss >> key;
            if (iss.fail()) throw std::runtime_error("Key conversion failed");
            result.emplace(key, FromJsonValue<V>(valJson));
        }
    }
    return result;
}

template<typename K, typename V>
std::map<K, V> FromJsonValue(const JsonValue& v) {
    if (!v.IsObject()) throw std::runtime_error("Expected object for map");
    std::map<K, V> result;
    for (const auto& [keyStr, valJson] : v.AsObject()) {
        if constexpr (std::is_same_v<K, std::string>) {
            result.emplace(keyStr, FromJsonValue<V>(valJson));
        }
        else if constexpr (std::is_enum_v<K>) {
            K keyEnum = Reflector<K>::FromString(keyStr);
            result.emplace(keyEnum, FromJsonValue<V>(valJson));
        }
        else {
            std::istringstream iss(keyStr);
            K key;
            iss >> key;
            if (iss.fail()) throw std::runtime_error("Key conversion failed");
            result.emplace(key, FromJsonValue<V>(valJson));
        }
    }
    return result;
}

// setFromJson用ヘルパー関数
template <typename T>
void setFromJson(const std::string& fieldName, T& value, const std::unordered_map<std::string, JsonValue>& data) {
    auto it = data.find(fieldName);
    if (it != data.end()) {
        value = FromJsonValue<T>(it->second);
    }
}

// setClassヘルパー
template <typename Tuple, size_t... Is>
void setClass(
    const std::vector<std::string>& fields,
    Tuple& values,
    const std::unordered_map<std::string, JsonValue>& data,
    std::index_sequence<Is...>
) {
    (setFromJson(fields[Is], std::get<Is>(values), data), ...);
}

// JSONマクロ
#define JSON(...) \
    void reflect(std::unordered_map<std::string, std::string>& fieldMap) const { \
        std::vector<std::string> fields = split(#__VA_ARGS__, ','); \
        auto values = std::tie(__VA_ARGS__); \
        reflectImpl(fields, values, fieldMap); \
    } \
    void from(const std::unordered_map<std::string, JsonValue>& jsonObject) { \
        std::vector<std::string> fields = split(#__VA_ARGS__, ','); \
        auto values = std::tie(__VA_ARGS__); \
        setClass(fields, values, jsonObject, std::make_index_sequence<std::tuple_size<decltype(values)>::value>{}); \
    }

// Reflector の定義
template<typename T>
struct Reflector {
    static void Reflect(const T& obj, std::unordered_map<std::string, std::string>& fieldMap) {
        obj.reflect(fieldMap); // ユーザー定義 reflect メソッドが std::unordered_map を受け取れるようにする
    }
    static void From(T& obj, const std::unordered_map<std::string, JsonValue>& jsonObject) {
        obj.from(jsonObject); // FromJson の方はそのまま std::map か unordered_map に合わせる
    }
};
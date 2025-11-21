#pragma once

#if (_MSVC_LANG >= 202002L)

#ifndef PROPERTY_H
#define PROPERTY_H

#include <memory>
#include <optional>
#include <functional>
#include <type_traits>
#include <concepts>

namespace System {

    // アクセス権限を定義
    struct All {};          // 全て
    struct Get {};          // Getのみ
    struct Set {};          // Setのみ
    struct PrivateGet {};   // 自分のみGet
    struct PrivateSet {};   // 自分のみSet

    // タプルを展開するヘルパー（std::tuple の型を展開する）
    template <typename Target, typename Tuple>
    struct has_access_impl;

    // 通常のパターン（タプルでない場合）
    template <typename Target, typename... List>
    struct has_access_impl<Target, std::tuple<List...>> {
        static constexpr bool value = (std::is_same_v<Target, List> || ...);
    };

    // エイリアステンプレートで簡略化
    template <typename Target, typename Tuple>
    constexpr bool has_access = has_access_impl<Target, Tuple>::value;

    template <typename T>
    concept IsClassType = std::is_class_v<T>;

    template <typename T, IsClassType FriendClass, typename... AccessTypes>
    class Property {
    private:
        Property(const Property&) = delete;
        Property& operator=(const Property&) = delete;

        using Access = std::conditional_t<sizeof...(AccessTypes) == 0, std::tuple<All>, std::tuple<AccessTypes...>>;

        T value;  // 通常型もポインタ型もTとしてそのまま持つ
        T* pData = nullptr;

        std::optional<std::function<T()>> getter;
        std::optional<std::function<void(const T&)>> setter;

        // Tがスマートポインタ型かどうかを判定
        static constexpr bool is_smart_ptr =
            std::is_same_v<T, std::shared_ptr<typename T::element_type>> ||
            std::is_same_v<T, std::unique_ptr<typename T::element_type>> ||
            std::is_same_v<T, std::weak_ptr<typename T::element_type>>;

        T get() const {
            if (getter) return (*getter)();
            if (pData)  return *pData;
            return value;
        }

        void set(const T& _value) {
            if (setter) {
                (*setter)(_value);
            }
            else if (pData) {
                *pData = _value;
            }
            else {
                value = _value;
            }
        }

        friend FriendClass;

    public:
        explicit Property(T initValue = T()) : value(std::move(initValue)) {}

        explicit Property(T* dataPtr,
            std::optional<std::function<T()>> getFunc = std::nullopt,
            std::optional<std::function<void(const T&)>> setFunc = std::nullopt)
            : pData(dataPtr), getter(getFunc), setter(setFunc) {}

        // Getter
        template <typename U = Access>
            requires has_access<Get, U> || has_access<All, U>
        operator T() const {
            return get();
        }

        // Setter
        template <typename U = Access>
            requires has_access<Set, U> || has_access<All, U>
        void operator=(const T& _value) {
            set(_value);
        }

    protected:
        // プライベート getter
        template <typename U = Access>
            requires has_access<PrivateGet, U>
        operator T() const {
            return get();
        }

        // プライベート setter
        template <typename U = Access>
            requires has_access<PrivateSet, U>
        void operator=(const T& _value) {
            set(_value);
        }
    };

}
// GetterとSetter用の簡略化されたマクロ
#define GET [&]() 
#define SET [&](const auto& value)

#endif  // PROPERTY_H

#endif  // _MSVC_LANG
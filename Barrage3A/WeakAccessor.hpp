/*
    WeakAccessor.hpp

    class 
        - WeakAccessor<T>

    çÏê¨ì˙         : 2025/04/18
    ç≈èIïœçXì˙     : 2025/04/18

    ****************************
    * c++  weak_ptr<T>Çé©ìÆâª *
    ****************************
*/
#pragma once

#include <memory>

namespace System {

    template<typename T>
    class WeakAccessor {
    private:
        std::weak_ptr<T> ptr;
    public:
        WeakAccessor() = default;
        WeakAccessor(std::weak_ptr<T> p) : ptr(std::move(p)) {}
        WeakAccessor(std::shared_ptr<T> p) : ptr(p) {}

        std::shared_ptr<T> lock() const {
            return ptr.lock();
        }

        T* get() const {
            auto sharedPtr = ptr.lock();
            return sharedPtr ? sharedPtr.get() : nullptr;
        }

        T* operator->() const {
            auto sharedPtr = ptr.lock();
            return sharedPtr ? sharedPtr.get() : nullptr;
        }

        operator std::shared_ptr<T>() const {
            return lock();
        }

        operator bool() const {
            return !ptr.expired();
        }

        WeakAccessor& operator=(const std::shared_ptr<T>& sptr) {
            ptr = sptr;
            return *this;
        }

        WeakAccessor& operator=(const std::weak_ptr<T>& wptr) {
            ptr = wptr;
            return *this;
        }

        bool expired() const {
            return ptr.expired();
        }

        T& operator*() const {
            auto sharedPtr = ptr.lock();
            if (!sharedPtr) throw std::runtime_error("Dereferencing null WeakAccessor");
            return *sharedPtr;
        }

        bool operator==(const WeakAccessor<T>& other) const {
            return ptr.lock() == other.ptr.lock();
        }

        bool operator!=(const WeakAccessor<T>& other) const {
            return !(*this == other);
        }

        bool operator==(const std::shared_ptr<T>& other) const {
            auto sharedPtr = ptr.lock();
            return sharedPtr == other;
        }

        bool operator!=(const std::shared_ptr<T>& other) const {
            return !(*this == other);
        }

        bool operator==(const std::weak_ptr<T>& other) const {
            auto sharedPtr = ptr.lock();
            auto otherSharedPtr = other.lock();
            return sharedPtr == otherSharedPtr;
        }

        bool operator!=(const std::weak_ptr<T>& other) const {
            return !(*this == other);
        }

        uintptr_t GetRawPtrAsUintPtr() const {
            return reinterpret_cast<uintptr_t>(get());
        }

    };

};


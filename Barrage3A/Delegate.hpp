/*
    Delegate.hpp

    class-
        Delegate<T,Args...>

    çÏê¨ì˙         : 2025/04/03
    ç≈èIïœçXì˙     : 2025/04/03
*/
#pragma once

#include <iostream>
#include <vector>
#include <functional>

namespace System {

    template<typename T = void, typename... Args>
    class Delegate {
    private:
        using FunctionType = std::function<T(Args...)>;
        static constexpr std::size_t ArgsSize = sizeof...(Args);

        std::vector<FunctionType> functions;

    public:
        void Add(FunctionType func) {
            functions.push_back(std::move(func));
        }

        void Remove(FunctionType func) {
            functions.erase(
                std::remove_if(functions.begin(), functions.end(),
                    [&](const FunctionType& f) { return f.target<void>() == func.target<void>(); }),
                functions.end()
            );
        }

        void Clear() {
            functions.clear();
        }

        template<typename U = T, typename = std::enable_if_t<sizeof...(Args) == ArgsSize>>
        U Invoke(Args... args) {
            if constexpr (!std::is_void_v<U>) {
                U result{};
                for (auto& func : functions) {
                    result = func(args...);
                }
                return result;
            }
            else {
                for (auto& func : functions) {
                    func(args...);
                }
            }
        }

        // ä÷êîÇî‘çÜÇ≈åƒÇ—èoÇ∑
        T InvokeByIndex(size_t index, Args... args) {
           
            if (index >= functions.size()) {
                throw std::out_of_range("Function index out of range");
            }

            if constexpr (!std::is_void_v<T>) {
                return functions[index](args...);
            }
            else {
                functions[index](args...);
            }
        }
        
        template <size_t N>
        T InvokeByIndex(Args... args) {

            if (N >= functions.size()) {
                throw std::out_of_range("Function index out of range");
            }

            if constexpr (!std::is_void_v<T>) {
                return functions[N](args...);
            }
            else {
                functions[N](args...);
            }
        }
        
        // ä÷êîÇî‘çÜÇ≈çÌèú
        void RemoveByIndex(size_t index) {
            if (index >= functions.size()) {
                throw std::out_of_range("Function index out of range");
            }
            functions.erase(functions.begin() + index);
        }

        template<typename = std::enable_if_t<sizeof...(Args) == ArgsSize>>
        T operator()(Args... args) {
            if (!std::is_void<T>::value)
                return Invoke(args...);
            else
                Invoke(args...);
        }

        FunctionType operator[](std::size_t N) {
            if (N >= functions.size()) {
                throw std::out_of_range("Function index out of range");
            }
            return functions[N];
        }


        void operator+=(FunctionType func) {
            Add(std::move(func));
        }

        void operator-=(FunctionType func) {
            Remove(std::move(func));
        }

        void operator=(FunctionType func)  {
            Clear();
            Add(std::move(func));
        }
    };
}
#pragma once

namespace System {
    namespace Collections {
        namespace Generic {
            #include <stdexcept>
            #include <memory>
            #include <vector>
            #include <list>
            #include <stdexcept>
            #include <functional>

            // 可変配列.
            template<typename T>
            class List {
            private:
                T* data;
                size_t capacity;
                size_t count;

                void Resize(size_t newCapacity) {
                    T* newData = new T[newCapacity];
                    for (size_t i = 0; i < count; ++i) {
                        newData[i] = data[i];
                    }
                    delete[] data;
                    data = newData;
                    capacity = newCapacity;
                }

            public:
                List(size_t initialCapacity = 4)
                    : data(new T[initialCapacity]), capacity(initialCapacity), count(0) {}

                ~List() {
                    delete[] data;
                }

                void Add(const T& item) {
                    if (count == capacity) {
                        Resize(capacity == 0 ? 4 : capacity * 2);
                    }
                    data[count++] = item;
                }

                void RemoveAt(size_t index) {
                    if (index >= count) {
                        throw std::out_of_range("Index out of range");
                    }
                    for (size_t i = index; i < count - 1; ++i) {
                        data[i] = data[i + 1];
                    }
                    --count;
                }

                void Clear() {
                    delete[] data;
                    data = new T[capacity];
                    count = 0;
                }

                const T& operator[](size_t index) const {
                    if (index >= count) {
                        throw std::out_of_range("Index out of range");
                    }
                    return data[index];
                }

                T& operator[](size_t index) {
                    if (index >= count) {
                        throw std::out_of_range("Index out of range");
                    }
                    return data[index];
                }

                size_t Count() const {
                    return count;
                }

                T* begin() { return data; }
                T* end() { return data + count; }
                const T* begin() const { return data; }
                const T* end() const { return data + count; }
            };
            // 連想配列.
            template<typename TKey, typename TValue>
            class Dictionary {
            private:
                struct KeyValuePair
                {
                    TKey    key;
                    TValue  value;

                    KeyValuePair(const TKey& k, const TValue& v) : key(k), value(v) {}
                };
                std::vector<std::list<KeyValuePair>> table;
                size_t count;
                size_t capacity;

                size_t Hash(const TKey& key) const {
                    return std::hash<TKey>()(key) % capacity;
                }

                void Resize(size_t newCapacity) {
                    std::vector<std::list<KeyValuePair>> newTable(newCapacity);
                    for (const auto& bucket : table) {
                        for (const auto& pair : bucket) {
                            size_t index = std::hash<TKey>()(pair.key) % newCapacity;
                            newTable[index].emplace_back(pair.key, pair.value);
                        }
                    }
                    table = std::move(newTable);
                    capacity = newCapacity;
                }
            public:
                Dictionary(size_t initialCapacity = 16)
                    : table(initialCapacity), count(0), capacity(initialCapacity) {}

                void Add(const TKey& key, const TValue& value) {
                    if (count >= capacity * 0.75) {
                        Resize(capacity * 2);
                    }
                    size_t index = Hash(key);
                    for (const auto& pair : table[index]) {
                        if (pair.key == key) {
                            throw std::invalid_argument("Key already exists");
                        }
                    }
                    table[index].emplace_back(key, value);
                    ++count;
                }

                TValue& operator[](const TKey& key) {
                    size_t index = Hash(key);
                    for (auto& pair : table[index]) {
                        if (pair.key == key) {
                            return pair.value;
                        }
                    }
                    table[index].emplace_back(key, TValue());
                    ++count;
                    return table[index].back().value;
                }

                bool TryGetValue(const TKey& key, TValue& value) const {
                    size_t index = Hash(key);
                    for (const auto& pair : table[index]) {
                        if (pair.key == key) {
                            value = pair.value;
                            return true;
                        }
                    }
                    return false;
                }

                size_t Count() const {
                    return count;
                }
            };
            // スタック型配列.
            template<typename T>
            class Stack {
            private:
                std::vector<T> data;
            public:
                void Push(const T& value) { data.push_back(value); }
                T Pop() {
                    if (data.empty()) {
                        throw std::out_of_range("Stack is empty");
                    }
                    T value = data.back();
                    data.pop_back();
                    return value;
                }

                // スタックの先頭要素を取得.
                T& Peek() {
                    if (data.empty()) {
                        throw std::out_of_range("Stack is empty");
                    }
                    return data.back();
                }

                // 要素数を取得
                size_t Count() const {
                    return data.size();
                }

                // スタックが空かどうかを確認
                bool IsEmpty() const {
                    return data.empty();
                }

                // 全要素をクリア
                void Clear() {
                    data.clear();
                }
            };
            // キュー型配列.
            template<typename T>
            class Queue {
            private:
                std::vector<T> data;
                size_t frontIndex;

            public:
                // コンストラクタ
                Queue() : frontIndex(0) {}

                // 要素をキューに追加
                void Enqueue(const T& value) {
                    data.push_back(value);
                }

                // 要素を取り出して削除
                T Dequeue() {
                    if (IsEmpty()) {
                        throw std::out_of_range("Queue is empty");
                    }
                    T value = data[frontIndex];
                    ++frontIndex;

                    // 範囲外の要素を削除してメモリを節約
                    if (frontIndex > data.size() / 2) {
                        // 先頭の半分を削除
                        data.erase(data.begin(), data.begin() + frontIndex);
                        frontIndex = 0;
                    }

                    return value;
                }

                // キューの先頭要素を取得
                T& Peek() {
                    if (IsEmpty()) {
                        throw std::out_of_range("Queue is empty");
                    }
                    return data[frontIndex];
                }

                // 要素数を取得
                size_t Count() const {
                    return data.size() - frontIndex;
                }

                // キューが空かどうかを確認
                bool IsEmpty() const {
                    return frontIndex >= data.size();
                }

                // 全要素をクリア
                void Clear() {
                    data.clear();
                    frontIndex = 0;
                }

                // = 演算子のオーバーロード.
                Queue& operator+=(const T& value) {
                    Enqueue(value);
                    return *this;
                }

                // () 演算子のオーバーロード.
                T operator()() {
                    return Dequeue();
                }
            };
            // 循環バッファ.
            template<typename T>
            class CircularBuffer {
            private:
                std::vector<T> buffer;
                size_t head;
                size_t tail;
                size_t maxSize;
                bool full;

            public:
                CircularBuffer(size_t size) : maxSize(size), head(0), tail(0), full(false) {
                    buffer.resize(size);
                }

                // 要素を追加する
                void Enqueue(const T& value) {
                    if (full) {
                        // バッファが満杯の場合、先頭を上書き
                        head = (head + 1) % maxSize;
                    }

                    buffer[tail] = value;
                    tail = (tail + 1) % maxSize;
                    full = tail == head;
                }

                // 要素を取り出して削除する
                T Dequeue() {
                    if (IsEmpty()) {
                        throw std::out_of_range("Buffer is empty");
                    }

                    T value = buffer[head];
                    head = (head + 1) % maxSize;
                    full = false;
                    return value;
                }

                // バッファの先頭要素を取得
                T& Peek() {
                    if (IsEmpty()) {
                        throw std::out_of_range("Buffer is empty");
                    }
                    return buffer[head];
                }

                // バッファが空かどうかを確認
                bool IsEmpty() const {
                    return (!full && (head == tail));
                }

                // バッファが満杯かどうかを確認
                bool IsFull() const {
                    return full;
                }

                // バッファのサイズ（最大容量）
                size_t Size() const {
                    return maxSize;
                }

                // バッファ内の要素数
                size_t Count() const {
                    if (full) {
                        return maxSize;
                    }
                    if (tail >= head) {
                        return tail - head;
                    }
                    return maxSize + tail - head;
                }
            };

        } // namespace Generic
    } // namespace Collections
} // namespace System
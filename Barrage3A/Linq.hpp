#pragma once
/*
    Linq.hpp

    class-
    - Linq

    作成日         : 2025/03/15
    最終変更日     : 2025/03/17
*/

#include <list>
#include <vector>
#include <numeric>
#include <algorithm>
#include <deque>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <utility>

#if defined(_MSVC_LANG) && _MSVC_LANG >= 202002L
#include <ranges>
#include <numeric>
#else
namespace System {
    namespace Linq {
        struct identity {
            template <class Type>
            constexpr Type&& operator()(Type&& _Left) const noexcept {
                return std::forward<Type>(_Left);
            }

            using is_transparent = int;
        };
    }
}
#endif

namespace System {
    namespace Linq {

        // is_pair が std::pair 型に対して true_type を返す
        template <typename T>
        struct is_pair : std::false_type {}; // デフォルトは false

        template <typename First, typename Second>
        struct is_pair<std::pair<First, Second>> : std::true_type {};
        
        template <typename T>
        constexpr bool is_pair_v = is_pair<T>::value;

        // 型を取得する構造体
        template <typename T>
        struct GetFirstType {
            using type = T; // std::pair でない場合、そのまま T を返す
        };

        template <typename First, typename Second>
        struct GetFirstType<std::pair<First, Second>> {
            using type = First; // std::pair の場合は First 型を返す
        };
              
        template<typename Container>
        class Linq {
        private:
            using Type = typename Container::value_type;
            using Key  = typename GetFirstType<Type>::type;
            std::vector<Type> data;
        public:

            // begin() と end() メソッドを定義
            auto begin() const { return data.begin(); }
            auto end()   const { return data.end(); }

            // コンストラクタ vectorに自動変換.
            explicit Linq(const Container& container) {
                data.assign(container.begin(), container.end());
            }

            // std::array 用
            template<typename U, std::size_t N>
            explicit Linq(const std::array<U, N>& arr) {
                data.assign(arr.begin(), arr.end());
            }
            /// <summary>
            /// 現在のデータをVector型で取得 - 保持しているデータをそのまま返します.
            /// </summary>
            /// <returns> 現在のデータコンテナ </returns>
            auto ToVector() {
                return data;
            }
            /// <summary>
            ///  現在のデータをList型で取得 - 保持しているデータをそのまま返します.
            /// </summary>
            /// <returns> 現在のデータコンテナ </returns>
            auto ToList() const {
                std::list<Type> list(data.begin(), data.end());
                return list;
            }

            // 要素を追加（単一要素）
            Linq& operator+=(const Type& value) {
                data.push_back(value);
                return *this;
            }

            // 要素を追加（複数要素）
            Linq& operator+=(const std::vector<Type>& values) {
                data.insert(data.end(), values.begin(), values.end());
                return *this;
            }

            // 要素を削除（単一要素）
            Linq& operator-=(const Type& value) {
                data.erase(std::remove(data.begin(), data.end(), value), data.end());
                return *this;
            }

            // 要素を削除（複数要素）
            Linq& operator-=(const std::vector<Type>& values) {
                for (const auto& value : values) {
                    data.erase(std::remove(data.begin(), data.end(), value), data.end());
                }
                return *this;
            }


            // 要素を追加
            void Add(const Type& value) {
                data.push_back(value);
            }

            // 複数要素を追加
            void Add(const std::vector<Type>& values) {
                data.insert(data.end(), values.begin(), values.end());
            }

            // 要素を削除
            void Remove(const Type& value) {
                data.erase(std::remove(data.begin(), data.end(), value), data.end());
            }

            // 複数要素を削除
            void Remove(const std::vector<Type>& values) {
                for (const auto& value : values) {
                    data.erase(std::remove(data.begin(), data.end(), value), data.end());
                }
            }

#if defined(_MSVC_LANG) && _MSVC_LANG >= 202002L
            /// <summary>
            /// Print - 要素を指定したフォーマットで出力
            /// </summary>
            /// <typeparam name="Func"></typeparam>
            /// <param name="predicate"></param>
            template <typename Func = std::identity>
            void Print(Func predicate = {}) {
                for (const auto& item : data) {
                    std::cout << predicate(item) << " ";
                }
                std::cout << std::endl;
            }

            /// <summary>
            /// ToDictionary - キーと値の選択関数を適用して辞書を作成.
            /// </summary>
            /// <typeparam name="FuncKey">キーを選択する関数の型</typeparam>
            /// <typeparam name="FuncValue">値を選択する関数の型</typeparam>
            /// <param name="keySelector">各要素からキーを取得する関数</param>
            /// <param name="valueSelector">各要素から値を取得する関数</param>
            /// <returns>キーと値のペアを持つ std::unordered_map</returns>
            template<typename FuncKey, typename FuncValue>
            auto ToDictionary(FuncKey keySelector, FuncValue valueSelector) const {
                std::unordered_map<typename std::invoke_result_t<FuncKey, Type>, typename std::invoke_result_t<FuncValue, Type>> dictionary;
                for (const auto& item : data) {
                    dictionary[keySelector(item)] = valueSelector(item);
                }
                return dictionary;
            }

            /// <summary>
            /// Where - 条件に合った要素をフィルタリング.
            /// </summary>
            /// <typeparam name="Func">フィルタ条件を判定する関数の型</typeparam>
            /// <param name="predicate">要素をフィルタリングする述語関数</param>
            /// <returns>フィルタ後の要素を持つ Linq オブジェクト</returns>
            template<typename Func>
            auto Where(Func predicate) {
                auto view = data | std::views::filter(predicate);
                std::vector<Type> result(view.begin(), view.end());
                return Linq<Type>(std::move(result));
            }

            /// <summary>
            /// Select - 各要素に変換関数を適用し、新しいコレクションを生成.
            /// </summary>
            /// <typeparam name="Func">変換を行う関数の型</typeparam>
            /// <param name="transformer">要素を変換する関数</param>
            /// <returns>変換後の要素を持つ Linq オブジェクト</returns>
            template<typename Func>
            auto Select(Func transformer) const {
                using ResultType = std::invoke_result_t<Func, Type>;
                std::vector<ResultType> transformed;
                std::ranges::transform(data, std::back_inserter(transformed), transformer);
                return Linq<std::vector<ResultType>>(std::move(transformed));
            }

            /// <summary>
            /// SelectMany - 各要素を変換し、フラット化されたシーケンスを生成.
            /// </summary>
            /// <typeparam name="Func">変換関数の型</typeparam>
            /// <param name="transformer">各要素をシーケンスに変換する関数</param>
            /// <returns>フラット化された要素を持つ Linq オブジェクト</returns>
            template<typename Func>
            auto SelectMany(Func transformer) const {
                using ResultType = std::invoke_result_t<Func, Type>;
                using ValueType = typename ResultType::value_type;
                std::vector<ValueType> flattened;
                for (const auto& item : data) {
                    auto transformed = transformer(item);
                    flattened.insert(flattened.end(), transformed.begin(), transformed.end());
                }

                return Linq<std::vector<ValueType>>(std::move(flattened));
            }

            /// <summary>
            /// OrderBy - 指定した比較関数に基づいて要素を昇順に並び替え.
            /// </summary>
            /// <typeparam name="Func">比較関数の型</typeparam>
            /// <param name="comparator">要素を比較する関数</param>
            /// <returns>並び替え後の Linq オブジェクト</returns>
            template<typename Func>
            auto OrderBy(Func comparator) {
                auto result = data;
                std::ranges::sort(result,comparator);
                return Linq<std::vector<typename Container::value_type>>(std::move(result));
            }

            /// <summary>
            /// OrderByDescending - 指定したキーに基づいて要素を降順に並び替え.
            /// </summary>
            /// <typeparam name="Func">キー選択関数の型</typeparam>
            /// <param name="comparator">キーを取得する関数</param>
            /// <returns>降順ソート後の Linq オブジェクト</returns>
            template <typename Func = std::identity>
            auto OrderByDescending(Func comparator = {}) const {
                auto result = data;
                std::ranges::sort(result, std::greater<>{}, comparator);
                return Linq<std::vector<Type>>(std::move(result));
            }

            /// <summary>
            /// OrderByAscending - 指定したキーに基づいて要素を昇順に並び替え.
            /// </summary>
            /// <typeparam name="Func">キー選択関数の型</typeparam>
            /// <param name="comparator">キーを取得する関数</param>
            /// <returns>昇順ソート後の Linq オブジェクト</returns>
            template <typename Func = std::identity>
            auto OrderByAscending(Func comparator = {}) const {
                auto result = data;
                std::ranges::sort(result, std::less<>{}, comparator);
                return Linq<std::vector<Type>>(std::move(result));
            }

            /// <summary>
            /// Distinct - 重複を排除して一意な要素のシーケンスを取得.
            /// </summary>
            /// <returns>重複を除いた Linq オブジェクト</returns>
            auto Distinct() {
                std::unordered_set<Type> seen;
                std::vector<Type> result;
                for (const auto& item : data) {
                    if (seen.insert(item).second) {
                        result.push_back(item);
                    }
                }
                return Linq<std::vector<Type>>(std::move(result));
            }

            /// <summary>
            /// Take - 先頭の n 個の要素を取得.
            /// </summary>
            /// <param name="n">取得する要素数</param>
            /// <returns>先頭の n 個の要素を持つ Linq オブジェクト</returns>
            auto Take(size_t n) const {
                std::vector<Type> result;
                auto it = data.begin();
                for (size_t i = 0; i < n && it != data.end(); ++i, ++it) {
                    result.push_back(*it);
                }
                return Linq<std::vector<Type>>(std::move(result));
            }

            /// <summary>
            /// Skip - 先頭の n 個の要素をスキップし、残りの要素を取得.
            /// </summary>
            /// <param name="n">スキップする要素数</param>
            /// <returns>スキップ後の Linq オブジェクト</returns>
            auto Skip(size_t n) const {
                std::vector<Type> result;
                auto it = data.begin();
                if (n >= data.size()) {
                    return Linq<std::vector<Type>>(std::move(result));
                }
                std::advance(it, n);
                for (; it != data.end(); ++it) {
                    result.push_back(*it);
                }
                return Linq<std::vector<Type>>(std::move(result));
            }

            /// <summary>
            /// Any - 指定された条件に一致する要素が存在するかを確認する.
            /// </summary>
            /// <typeparam name="Func">条件を判定する関数の型</typeparam>
            /// <param name="predicate">条件を指定する関数</param>
            /// <returns>条件を満たす要素が1つでもあればtrue、それ以外はfalse</returns>
            template<typename Func>
            bool Any(Func predicate) const {
                return std::ranges::any_of(data, predicate);
            }

            /// <summary>
            /// All - 全ての要素が指定された条件を満たすかを確認する.
            /// </summary>
            /// <typeparam name="Func">条件を判定する関数の型</typeparam>
            /// <param name="predicate">条件を指定する関数</param>
            /// <returns>全ての要素が条件を満たす場合true、それ以外はfalse</returns>
            template<typename Func>
            bool All(Func predicate) const {
                return std::ranges::all_of(data, predicate);
            }

            /// <summary>
            /// First - 条件に一致する最初の要素を返す.
            /// </summary>
            /// <typeparam name="Func">条件を判定する関数の型</typeparam>
            /// <param name="predicate">条件を指定する関数</param>
            /// <returns>条件に一致する最初の要素</returns>
            /// <exception cref="std::runtime_error">条件に一致する要素が存在しない場合に例外をスロー</exception>
            template<typename Func>
            Type First(Func predicate) const {
                if (auto it = std::ranges::find_if(data, predicate); it != data.end()) {
                    return *it;
                }
                throw std::runtime_error("No matching element found");
            }

            /// <summary>
            /// Last - 条件に一致する最後の要素を返す.
            /// </summary>
            /// <typeparam name="Func">条件を判定する関数の型</typeparam>
            /// <param name="predicate">条件を指定する関数</param>
            /// <returns>条件に一致する最後の要素</returns>
            /// <exception cref="std::runtime_error">条件に一致する要素が存在しない場合に例外をスロー</exception>
            template<typename Func>
            Type Last(Func predicate) const {
                if (auto it = std::ranges::find_if(data | std::views::reverse, predicate); it != data.rend()) {
                    return *it;
                }
                throw std::runtime_error("No matching element found");
            }

            /// <summary>
            /// GroupBy - 指定されたキーでデータをグループ化する.
            /// </summary>
            /// <typeparam name="Func">キーを選択する関数の型</typeparam>
            /// <param name="keySelector">グループ化の基準となるキーを返す関数</param>
            /// <returns>グループ化されたデータ</returns>
            template<typename Func>
                requires !is_pair_v<Type>
            auto GroupBy(Func keySelector) const {
                using KeyType = std::invoke_result_t<Func, Type>;
                std::vector<Type> sortedData = data;

                // keySelector に基づいてソート
                std::ranges::sort(sortedData, {}, keySelector);

                // GroupBy の結果を保持する
                std::vector<std::pair<KeyType, std::vector<Type>>> result;
                auto it = sortedData.begin();

                while (it != sortedData.end()) {
                    KeyType key = keySelector(*it);
                    auto groupEnd = std::ranges::find_if_not(it, sortedData.end(), [&](const Type& item) {
                        return keySelector(item) == key;
                        });

                    // グループを保存
                    result.emplace_back(key, std::vector<Type>(it, groupEnd));

                    // 次のグループへ
                    it = groupEnd;
                }

                return Linq<std::vector<std::pair<KeyType, std::vector<Type>>>>(std::move(result));
            }

            /// <summary>
            /// GetGroup - 指定されたキーに対するグループを取得する (pairの場合).
            /// </summary>
            /// <param name="key">取得したいグループのキー</param>
            /// <returns>指定されたキーに対するグループ</returns>
            auto GetGroup(const Key key) const
                requires is_pair_v<Type> {
                // グループの型を取得
                using GroupType = typename std::remove_reference<decltype(data)>::type::value_type::second_type;

                for (const auto& [groupKey, group] : data) {
                    if (groupKey == key) {
                        return Linq<GroupType>(group);
                    }
                }

                // 見つからない場合、空のGroupTypeを返す
                return Linq<GroupType>(GroupType{});
            }

            /// <summary>
            /// Aggregate - 要素を集約する.
            /// </summary>
            /// <typeparam name="Func">集約関数の型</typeparam>
            /// <param name="aggregator">集約関数</param>
            /// <param name="initial">集約を開始する初期値</param>
            /// <returns>集約結果</returns>
            template<typename Func>
            auto Aggregate(Func aggregator, Type initial) const {
                std::ranges::for_each(data, [&](const Type& item) {
                    initial = aggregator(initial, item);
                    });
                return initial;
            }

            /// <summary>
            /// Average - 要素の平均を計算する.
            /// </summary>
            /// <typeparam name="Func">選択する関数の型</typeparam>
            /// <param name="selector">選択する関数（デフォルトはそのままの値）</param>
            /// <returns>平均値</returns>
            template<typename Func = std::identity>
            std::optional<double> Average(Func selector = {}) const {
                if (data.empty()) return std::nullopt;
                return static_cast<double>(Sum(selector)) / data.size();
            }

            /// <summary>
            /// Max - 要素の最大値を取得する.
            /// </summary>
            /// <typeparam name="Func">選択する関数の型</typeparam>
            /// <param name="selector">選択する関数（デフォルトはそのままの値）</param>
            /// <returns>最大値</returns>
            template<typename Func = std::identity>
            auto Max(Func selector = {}) const
                requires std::is_arithmetic_v<Type> {
                return *std::ranges::max_element(data);
            }

            /// <summary>
            /// Max - 要素の最大値を取得する（オーバーロード、クラス型の場合）.
            /// </summary>
            /// <typeparam name="Func">選択する関数の型</typeparam>
            /// <param name="selector">選択する関数</param>
            /// <returns>最大値</returns>
            template<typename Func = std::identity>
            auto Max(Func selector) const
                requires std::is_class_v<Type> {
                return *std::ranges::max_element(data, {}, selector);
            }

            /// <summary>
            /// Min - 要素の最小値を取得する.
            /// </summary>
            /// <typeparam name="Func">選択する関数の型</typeparam>
            /// <param name="selector">選択する関数（デフォルトはそのままの値）</param>
            /// <returns>最小値</returns>
            template<typename Func = std::identity>
            auto Min(Func selector = {}) const
                requires std::is_arithmetic_v<Type> {
                return *std::ranges::min_element(data);
            }

            /// <summary>
            /// Min - 要素の最小値を取得する（オーバーロード、クラス型の場合）.
            /// </summary>
            /// <typeparam name="Func">選択する関数の型</typeparam>
            /// <param name="selector">選択する関数</param>
            /// <returns>最小値</returns>
            template<typename Func = std::identity>
            auto Min(Func selector) const
                requires std::is_class_v<Type> {
                return *std::ranges::min_element(data, {}, selector);
            }

            /// <summary>
            /// Sum - 要素の合計を計算する.
            /// </summary>
            /// <typeparam name="Func">選択する関数の型</typeparam>
            /// <param name="selector">選択する関数（デフォルトはそのままの値）</param>
            /// <returns>合計値</returns>
            template<typename Func = std::identity>
            auto Sum(Func selector = {}) const
                requires std::is_arithmetic_v<Type> {
                using ValueType = decltype(selector(data.front()));
                return std::accumulate(data.begin(), data.end(), ValueType{}, [&](ValueType sum, const Type& item) {
                    return sum + selector(item);
                    });
            }

            /// <summary>
            /// Sum - 要素の合計を計算する（オーバーロード、クラス型の場合）.
            /// </summary>
            /// <typeparam name="Func">選択する関数の型</typeparam>
            /// <param name="selector">選択する関数</param>
            /// <returns>合計値</returns>
            template<typename Func = std::identity>
            auto Sum(Func selector) const
                requires std::is_class_v<Type> {
                using ValueType = decltype(selector(data.front()));
                return std::accumulate(data.begin(), data.end(), ValueType{}, [&](ValueType sum, const Type& item) {
                    return sum + selector(item);
                    });
            }

            /// <summary>
            /// Zip - 2つのコレクションを組み合わせる.
            /// </summary>
            /// <typeparam name="OtherContainer">他のコレクションの型</typeparam>
            /// <typeparam name="Func">組み合わせる関数の型</typeparam>
            /// <param name="other">組み合わせるもう一方のコレクション</param>
            /// <param name="zipper">要素を組み合わせる関数</param>
            /// <returns>組み合わせた結果のコレクション</returns>
            template<typename OtherContainer, typename Func>
            auto Zip(const OtherContainer& other, Func zipper) const {
                using OtherType = typename OtherContainer::value_type;
                size_t minSize = min(data.size(), other.size());
                std::vector<std::invoke_result_t<Func, Type, OtherType>> zipped(minSize);
                for (size_t i = 0; i < minSize; ++i) {
                    zipped[i] = zipper(data[i], other[i]);
                }
                return Linq<std::vector<std::invoke_result_t<Func, Type, OtherType>>>(std::move(zipped));
            }

            /// <summary>
            /// Concat - 2つのコレクションを連結する.
            /// </summary>
            /// <typeparam name="OtherContainer">他のコレクションの型</typeparam>
            /// <param name="other">連結するもう一方のコレクション</param>
            /// <returns>連結されたコレクション</returns>
            template<typename OtherContainer>
            auto Concat(const OtherContainer& other) const {
                std::vector<Type> concatenated = data;
                concatenated.insert(concatenated.end(), other.begin(), other.end());
                return Linq<std::vector<Type>>(std::move(concatenated));
            }

            /// <summary>
            /// ForEach - 各要素に対してアクションを実行する.
            /// </summary>
            /// <typeparam name="Func">実行する関数の型</typeparam>
            /// <param name="action">各要素に実行する関数</param>
            template<typename Func>
            void ForEach(Func action) const {
                for (const auto& item : data) {
                    action(item);
                }
            }

            /// <summary>
            /// FirstOrDefault - 条件に一致する最初の要素を取得する（見つからない場合はデフォルト値を返す）.
            /// </summary>
            /// <typeparam name="Func">条件を判定する関数の型</typeparam>
            /// <param name="predicate">条件を指定する関数</param>
            /// <param name="defaultValue">見つからない場合のデフォルト値</param>
            /// <returns>条件に一致する最初の要素またはデフォルト値</returns>
            template<typename Func>
            Type FirstOrDefault(Func predicate, Type defaultValue) const {
                if (auto it = std::ranges::find_if(data, predicate); it != data.end()) {
                    return *it;
                }
                return defaultValue;
            }

            /// <summary>
            /// 条件に一致する最後の要素を返します。見つからなかった場合はデフォルト値を返します。
            /// </summary>
            /// <typeparam name="Func">検索条件を指定する関数型</typeparam>
            /// <param name="predicate">検索条件を指定する関数</param>
            /// <param name="defaultValue">見つからなかった場合に返すデフォルト値</param>
            /// <returns>条件に一致する最後の要素、またはデフォルト値</returns>
            template<typename Func>
            Type LastOrDefault(Func predicate, Type defaultValue) const {
                if (auto it = std::ranges::find_if(data | std::views::reverse, predicate); it != data.rend()) {
                    return *it;
                }
                return defaultValue;
            }

            /// <summary>
            /// コンテナに指定された値が存在するかどうかを確認します。
            /// </summary>
            /// <param name="value">確認する値</param>
            /// <returns>値が存在する場合はtrue、それ以外はfalse</returns>
            bool Contains(const Type& value) const {
                return std::ranges::find(data, value) != data.end();
            }

            /// <summary>
            /// コンテナの順番を逆順にして新しいLinqオブジェクトを返します。
            /// </summary>
            /// <returns>逆順に並べ替えた新しいLinqオブジェクト</returns>
            auto Reverse() const {
                std::vector<Type> reversed(data.rbegin(), data.rend());
                return Linq<std::vector<Type>>(std::move(reversed));
            }

            /// <summary>
            /// 他のコンテナと共通する要素を返します。
            /// </summary>
            /// <typeparam name="OtherContainer">比較する他のコンテナの型</typeparam>
            /// <param name="other">共通する要素を探すための他のコンテナ</param>
            /// <returns>共通する要素のLinqオブジェクト</returns>
            template<typename OtherContainer>
            auto Intersect(const OtherContainer& other) const {
                std::unordered_set<Type> otherSet(other.begin(), other.end());
                std::vector<Type> intersection;
                for (const auto& item : data) {
                    if (otherSet.count(item)) {
                        intersection.push_back(item);
                    }
                }
                return Linq<std::vector<Type>>(std::move(intersection));
            }

            /// <summary>
            /// 他のコンテナと異なる要素を返します。
            /// </summary>
            /// <typeparam name="OtherContainer">比較する他のコンテナの型</typeparam>
            /// <param name="other">異なる要素を探すための他のコンテナ</param>
            /// <returns>異なる要素のLinqオブジェクト</returns>
            template<typename OtherContainer>
            auto Except(const OtherContainer& other) const {
                std::unordered_set<Type> otherSet(other.begin(), other.end());
                std::vector<Type> difference;
                for (const auto& item : data) {
                    if (otherSet.count(item) == 0) {
                        difference.push_back(item);
                    }
                }
                return Linq<std::vector<Type>>(std::move(difference));
            }

            template<typename Func>
                requires is_pair_v<Type>
            auto GroupBy(Func) const = delete;

            template<typename Key>
                requires !is_pair_v<Type>
            auto GetGroup(const Key) const = delete;
#else
            /// <summary>
            /// Print - 要素を指定したフォーマットで出力
            /// </summary>
            /// <typeparam name="Func"></typeparam>
            /// <param name="predicate"></param>
            template <typename Func = identity>
            void Print(Func predicate = {}) {
                for (const auto& item : data) {
                    std::cout << predicate(item) << " ";
                }
                std::cout << std::endl;
            }

            /// <summary>
            /// ToDictionary - キーと値の選択関数を適用して辞書を作成.
            /// </summary>
            /// <typeparam name="FuncKey">キーを選択する関数の型</typeparam>
            /// <typeparam name="FuncValue">値を選択する関数の型</typeparam>
            /// <param name="keySelector">各要素からキーを取得する関数</param>
            /// <param name="valueSelector">各要素から値を取得する関数</param>
            /// <returns>キーと値のペアを持つ std::unordered_map</returns>
            template<typename FuncKey, typename FuncValue>
            auto ToDictionary(FuncKey keySelector, FuncValue valueSelector) const {
                std::unordered_map<
                    typename std::result_of<FuncKey(Type)>::type,
                    typename std::result_of<FuncValue(Type)>::type
                > dictionary;

                for (const auto& item : data) {
                    dictionary[keySelector(item)] = valueSelector(item);
                }
                return dictionary;
            }

            /// <summary>
            /// Where - 条件に合った要素をフィルタリング.
            /// </summary>
            /// <typeparam name="Func">フィルタ条件を判定する関数の型</typeparam>
            /// <param name="predicate">要素をフィルタリングする述語関数</param>
            /// <returns>フィルタ後の要素を持つ Linq オブジェクト</returns>
            template<typename Func>
            auto Where(Func predicate) {
                std::vector<Type> result;
                std::copy_if(data.begin(), data.end(), std::back_inserter(result), predicate);
                return Linq<std::vector<Type>>(std::move(result));
            }

            /// <summary>
            /// Select - 各要素に変換関数を適用し、新しいコレクションを生成.
            /// </summary>
            /// <typeparam name="Func">変換を行う関数の型</typeparam>
            /// <param name="transformer">要素を変換する関数</param>
            /// <returns>変換後の要素を持つ Linq オブジェクト</returns>
            template<typename Func>
            auto Select(Func transformer) const {
                using ResultType = std::invoke_result_t<Func, Type>;
                std::vector<ResultType> transformed;
                std::transform(data.begin(), data.end(), std::back_inserter(transformed), transformer);
                return Linq<std::vector<ResultType>>(std::move(transformed));
            }

            /// <summary>
            /// SelectMany - 各要素を変換し、フラット化されたシーケンスを生成.
            /// </summary>
            /// <typeparam name="Func">変換関数の型</typeparam>
            /// <param name="transformer">各要素をシーケンスに変換する関数</param>
            /// <returns>フラット化された要素を持つ Linq オブジェクト</returns>
            template<typename Func>
            auto SelectMany(Func transformer) const {
                using ResultType = typename std::result_of<Func(Type)>::type;
                using ValueType  = typename ResultType::value_type;
                std::vector<ValueType> flattened;
                for (const auto& item : data) {
                    auto transformed = transformer(item);
                    flattened.insert(flattened.end(), transformed.begin(), transformed.end());
                }

                return Linq<std::vector<ValueType>>(std::move(flattened));
            }

            /// <summary>
            /// OrderBy - 指定した比較関数に基づいて要素を昇順に並び替え.
            /// </summary>
            /// <typeparam name="Func">比較関数の型</typeparam>
            /// <param name="comparator">要素を比較する関数</param>
            /// <returns>並び替え後の Linq オブジェクト</returns>
            template<typename Func = identity>
            auto OrderBy(Func comparator = {}) {
                std::vector<Type> result = data;
                std::sort(result.begin(), result.end(), comparator);
                return Linq<std::vector<Type>>(std::move(result));
            }

            /// <summary>
            /// OrderByDescending - 指定したキーに基づいて要素を降順に並び替え.
            /// </summary>
            /// <typeparam name="Func">キー選択関数の型</typeparam>
            /// <param name="comparator">キーを取得する関数</param>
            /// <returns>降順ソート後の Linq オブジェクト</returns>
            template<typename Func = identity>
            auto OrderByDescending(Func comparator = {}) const {
                std::vector<Type> result = data;
                std::sort(result.begin(), result.end(),
                    [&comparator](const Type& a, const Type& b) {
                        return comparator(a) > comparator(b);
                    });
                return Linq<std::vector<Type>>(std::move(result));
            }

            /// <summary>
            /// OrderByAscending - 指定したキーに基づいて要素を昇順に並び替え.
            /// </summary>
            /// <typeparam name="Func">キー選択関数の型</typeparam>
            /// <param name="comparator">キーを取得する関数</param>
            /// <returns>昇順ソート後の Linq オブジェクト</returns>
            template<typename Func = identity>
            auto OrderByAscending(Func comparator = {}) const {
                std::vector<Type> result = data;
                std::sort(result.begin(), result.end(),
                    [&comparator](const Type& a, const Type& b) {
                        return comparator(a) < comparator(b);
                    });
                return Linq<std::vector<Type>>(std::move(result));
            }

            /// <summary>
            /// Distinct - 重複を排除して一意な要素のシーケンスを取得.
            /// </summary>
            /// <returns>重複を除いた Linq オブジェクト</returns>
            auto Distinct() {
                std::unordered_set<Type> seen;
                std::vector<Type> result;
                for (const auto& item : data) {
                    if (seen.insert(item).second) {
                        result.push_back(item);
                    }
                }
                return Linq<std::vector<Type>>(std::move(result));
            }

            /// <summary>
            /// Take - 先頭の n 個の要素を取得.
            /// </summary>
            /// <param name="n">取得する要素数</param>
            /// <returns>先頭の n 個の要素を持つ Linq オブジェクト</returns>
            auto Take(size_t n) const {
                std::vector<Type> result;
                auto it = data.begin();
                for (size_t i = 0; i < n && it != data.end(); ++i, ++it) {
                    result.push_back(*it);
                }
                return Linq<std::vector<Type>>(std::move(result));
            }

            /// <summary>
            /// Skip - 先頭の n 個の要素をスキップし、残りの要素を取得.
            /// </summary>
            /// <param name="n">スキップする要素数</param>
            /// <returns>スキップ後の Linq オブジェクト</returns>
            auto Skip(size_t n) const {
                std::vector<Type> result;
                auto it = data.begin();
                if (n >= data.size()) {
                    return Linq<std::vector<Type>>(std::move(result));
                }
                std::advance(it, n);
                for (; it != data.end(); ++it) {
                    result.push_back(*it);
                }
                return Linq<std::vector<Type>>(std::move(result));
            }

            /// <summary>
            /// Any - 指定された条件に一致する要素が存在するかを確認する.
            /// </summary>
            /// <typeparam name="Func">条件を判定する関数の型</typeparam>
            /// <param name="predicate">条件を指定する関数</param>
            /// <returns>条件を満たす要素が1つでもあればtrue、それ以外はfalse</returns>
            template<typename Func>
            bool Any(Func predicate) const {
                return std::any_of(data.begin(), data.end(), predicate);
            }

            /// <summary>
            /// All - 全ての要素が指定された条件を満たすかを確認する.
            /// </summary>
            /// <typeparam name="Func">条件を判定する関数の型</typeparam>
            /// <param name="predicate">条件を指定する関数</param>
            /// <returns>全ての要素が条件を満たす場合true、それ以外はfalse</returns>
            template<typename Func>
            bool All(Func predicate) const {
                return std::all_of(data.begin(), data.end(), predicate);
            }

            /// <summary>
            /// First - 条件に一致する最初の要素を返す.
            /// </summary>
            /// <typeparam name="Func">条件を判定する関数の型</typeparam>
            /// <param name="predicate">条件を指定する関数</param>
            /// <returns>条件に一致する最初の要素</returns>
            /// <exception cref="std::runtime_error">条件に一致する要素が存在しない場合に例外をスロー</exception>
            template<typename Func>
            Type First(Func predicate) const {
                auto it = std::find_if(data.begin(), data.end(), predicate);
                if (it != data.end()) {
                    return *it;
                }
                throw std::runtime_error("No matching element found");
            }

            /// <summary>
            /// Last - 条件に一致する最後の要素を返す.
            /// </summary>
            /// <typeparam name="Func">条件を判定する関数の型</typeparam>
            /// <param name="predicate">条件を指定する関数</param>
            /// <returns>条件に一致する最後の要素</returns>
            /// <exception cref="std::runtime_error">条件に一致する要素が存在しない場合に例外をスロー</exception>
            template<typename Func>
            Type Last(Func predicate) const {
                auto it = std::find_if(data.rbegin(), data.rend(), predicate);
                if (it != data.rend()) {
                    return *it;
                }
                throw std::runtime_error("No matching element found");
            }

            /// <summary>
            /// GroupBy - 指定されたキーでデータをグループ化する.
            /// </summary>
            /// <typeparam name="Func">キーを選択する関数の型</typeparam>
            /// <param name="keySelector">グループ化の基準となるキーを返す関数</param>
            /// <returns>グループ化されたデータ</returns>
            template<typename Func>
            auto GroupBy(Func keySelector) const {
                using KeyType = std::invoke_result_t<Func, Type>;
                std::vector<Type> sortedData = data;

                // keySelector に基づいてソート
                std::sort(sortedData.begin(), sortedData.end(), [&](const Type& lhs, const Type& rhs) {
                    return keySelector(lhs) < keySelector(rhs);
                    });

                // GroupBy の結果を保持する
                std::vector<std::pair<KeyType, std::vector<Type>>> result;
                auto it = sortedData.begin();

                while (it != sortedData.end()) {
                    KeyType key = keySelector(*it);
                    auto groupEnd = std::find_if_not(it, sortedData.end(), [&](const Type& item) {
                        return keySelector(item) == key;
                        });

                    // グループを保存
                    result.emplace_back(key, std::vector<Type>(it, groupEnd));

                    // 次のグループへ
                    it = groupEnd;
                }

                return Linq<std::vector<std::pair<KeyType, std::vector<Type>>>>(std::move(result));
            }   

            /// <summary>
            /// GetGroup - 指定されたキーに対するグループを取得する (pairの場合).
            /// </summary>
            /// <param name="key">取得したいグループのキー</param>
            /// <returns>指定されたキーに対するグループ</returns>
            auto GetGroup(const Key key) const {
                // グループの型を取得
                using GroupType = typename std::remove_reference<decltype(data)>::type::value_type::second_type;

                for (const auto& [groupKey, group] : data) {
                    if (groupKey == key) {
                        return Linq<GroupType>(group);
                    }
                }

                // 見つからない場合、空のGroupTypeを返す
                return Linq<GroupType>(GroupType{});
            }

            /// <summary>
            /// Aggregate - 要素を集約する.
            /// </summary>
            /// <typeparam name="Func">集約関数の型</typeparam>
            /// <param name="aggregator">集約関数</param>
            /// <param name="initial">集約を開始する初期値</param>
            /// <returns>集約結果</returns>
            template<typename Func>
            auto Aggregate(Func aggregator, Type initial) const {
                for (const auto& item : data) {
                    initial = aggregator(initial, item);
                }
                return initial;
            }

            /// <summary>
            /// Average - 要素の平均を計算する.
            /// </summary>
            /// <typeparam name="Func">選択する関数の型</typeparam>
            /// <param name="selector">選択する関数（デフォルトはそのままの値）</param>
            /// <returns>平均値</returns>
            template<typename Func  = identity>
            double Average(Func selector = {}) const {
                return static_cast<double>(Sum(selector)) / data.size();
            }   

            /// <summary>
            /// Max - 要素の最大値を取得する（オーバーロード、クラス型の場合）.
            /// </summary>
            /// <typeparam name="Func">選択する関数の型</typeparam>
            /// <param name="selector">選択する関数</param>
            /// <returns>最大値</returns>
            template <typename Func = identity>
            auto Max(Func selector = {}) const {
                return *std::max_element(data.begin(), data.end(), 
                    [&selector](const Type& a, const Type& b) {
                        return selector(a) < selector(b);
                    });
            }

            /// <summary>
            /// Min - 要素の最小値を取得する（オーバーロード、クラス型の場合）.
            /// </summary>
            /// <typeparam name="Func">選択する関数の型</typeparam>
            /// <param name="selector">選択する関数</param>
            /// <returns>最小値</returns>
            template <typename Func = identity>
            auto Min(Func selector = {}) const {
                return *std::min_element(data.begin(), data.end(),
                    [&selector](const Type& a, const Type& b) {
                        return selector(a) < selector(b);
                    });
            }

            /// <summary>
            /// Sum - 要素の合計を計算する（オーバーロード、クラス型の場合）.
            /// </summary>
            /// <typeparam name="Func">選択する関数の型</typeparam>
            /// <param name="selector">選択する関数</param>
            /// <returns>合計値</returns>
            template <typename Func = identity>
            auto Sum(Func selector = {}) const {
                using ValueType = decltype(selector(data.front()));
                return std::accumulate(data.begin(), data.end(), ValueType{}, 
                    [&](ValueType sum, const Type& item) {
                        return sum + selector(item);
                    });
            }

            /// <summary>
            /// Zip - 2つのコレクションを組み合わせる.
            /// </summary>
            /// <typeparam name="OtherContainer">他のコレクションの型</typeparam>
            /// <typeparam name="Func">組み合わせる関数の型</typeparam>
            /// <param name="other">組み合わせるもう一方のコレクション</param>
            /// <param name="zipper">要素を組み合わせる関数</param>
            /// <returns>組み合わせた結果のコレクション</returns>
            template<typename OtherContainer, typename Func>
            auto Zip(const OtherContainer& other, Func zipper) const {
                using OtherType = typename OtherContainer::value_type;
                size_t minSize = min(data.size(), other.size());
                std::vector<std::invoke_result_t<Func, Type, OtherType>> zipped(minSize);
                for (size_t i = 0; i < minSize; ++i) {
                    zipped[i] = zipper(data[i], other[i]);
                }
                return Linq<std::vector<std::invoke_result_t<Func, Type, OtherType>>>(std::move(zipped));
            }

            /// <summary>
            /// Concat - 2つのコレクションを連結する.
            /// </summary>
            /// <typeparam name="OtherContainer">他のコレクションの型</typeparam>
            /// <param name="other">連結するもう一方のコレクション</param>
            /// <returns>連結されたコレクション</returns>
            template<typename OtherContainer>
            auto Concat(const OtherContainer& other) const {
                std::vector<Type> concatenated = data;
                concatenated.insert(concatenated.end(), other.begin(), other.end());
                return Linq<std::vector<Type>>(std::move(concatenated));
            }

            /// <summary>
            /// ForEach - 各要素に対してアクションを実行する.
            /// </summary>
            /// <typeparam name="Func">実行する関数の型</typeparam>
            /// <param name="action">各要素に実行する関数</param>
            template<typename Func>
            void ForEach(Func action) const {
                for (const auto& item : data) {
                    action(item);
                }
            }

            /// <summary>
            /// FirstOrDefault - 条件に一致する最初の要素を取得する（見つからない場合はデフォルト値を返す）.
            /// </summary>
            /// <typeparam name="Func">条件を判定する関数の型</typeparam>
            /// <param name="predicate">条件を指定する関数</param>
            /// <param name="defaultValue">見つからない場合のデフォルト値</param>
            /// <returns>条件に一致する最初の要素またはデフォルト値</returns>
            template<typename Func>
            Type FirstOrDefault(Func predicate, Type defaultValue) const {
                auto it = std::find_if(data.begin(), data.end(), predicate);
                if (it != data.end()) {
                    return *it;
                }
                return defaultValue;
            }

            /// <summary>
            /// 条件に一致する最後の要素を返します。見つからなかった場合はデフォルト値を返します。
            /// </summary>
            /// <typeparam name="Func">検索条件を指定する関数型</typeparam>
            /// <param name="predicate">検索条件を指定する関数</param>
            /// <param name="defaultValue">見つからなかった場合に返すデフォルト値</param>
            /// <returns>条件に一致する最後の要素、またはデフォルト値</returns>
            template<typename Func>
            Type LastOrDefault(Func predicate, Type defaultValue) const {
                auto it = std::find_if(data.rbegin(), data.rend(), predicate);
                if (it != data.rend()) {
                    return *it;
                }
                return defaultValue;
            }

            /// <summary>
            /// コンテナに指定された値が存在するかどうかを確認します。
            /// </summary>
            /// <param name="value">確認する値</param>
            /// <returns>値が存在する場合はtrue、それ以外はfalse</returns>
            bool Contains(const Type& value) const {
                return std::find(data.begin(), data.end(), value) != data.end();
            }

            /// <summary>
            /// コンテナの順番を逆順にして新しいLinqオブジェクトを返します。
            /// </summary>
            /// <returns>逆順に並べ替えた新しいLinqオブジェクト</returns>
            auto Reverse() const {
                std::vector<Type> reversed(data.rbegin(), data.rend());
                return Linq<std::vector<Type>>(std::move(reversed));
            }
#endif// End If 
        };// end class Linq
    } // namespace Linq
} // namespace System

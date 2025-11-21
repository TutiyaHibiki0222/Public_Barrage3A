/*
* JsonValue.hpp
    JSON 文字列を解析するためのクラス群.
    JSON のデータ (数値、文字列、配列、オブジェクト) をメモリ上に持つ.
	*    JsonValue クラスは、JSON の値を表現するためのクラスであり、JsonObject や JsonArray などの型を持つ.
	より高度なSourceが見やすい JSON 解析や操作を行うための基礎となる.
*/

#pragma once
#include <string>
#include <map>
#include <vector>
#include <variant>
#include <cctype>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <unordered_map>

struct JsonValue;
using JsonObject = std::unordered_map<std::string, JsonValue>;
using JsonArray = std::vector<JsonValue>;
using JsonPrimitive = std::variant<std::nullptr_t, bool, double, std::string>;

struct JsonValue {
	std::variant<JsonObject, JsonArray, JsonPrimitive> value;                               // JSON の値はオブジェクト、配列、またはプリミティブ型のいずれか.
	bool IsObject() const noexcept { return std::holds_alternative<JsonObject>(value); }    // オブジェクトかどうかを判定する関数.
	bool IsArray() const noexcept { return std::holds_alternative<JsonArray>(value); }      // 配列かどうかを判定する関数.
    bool IsPrimitive() const noexcept { return std::holds_alternative<JsonPrimitive>(value); }
    const JsonObject& AsObject() const { return std::get<JsonObject>(value); }
    const JsonArray& AsArray() const { return std::get<JsonArray>(value); }
    const JsonPrimitive& AsPrimitive() const { return std::get<JsonPrimitive>(value); }
};

class JsonLexer {
	friend class JsonParser; // JsonParser からアクセスできるようにする
protected:
    const char* data;
    size_t pos;
    size_t length;

    inline void SkipWhitespace() noexcept {
        while (pos < length && std::isspace(static_cast<unsigned char>(data[pos]))) pos++;
    }

public:
    explicit JsonLexer(const std::string& j) : data(j.data()), pos(0), length(j.size()) {}
	// 現在のポジションを取得する関数.
    inline char Peek() noexcept {
        SkipWhitespace();
        return pos < length ? data[pos] : '\0';
    }
	// 文字を取得してポジションを進める関数.
    inline char Get() {
        SkipWhitespace();
        if (pos >= length) throw std::runtime_error("Unexpected end of JSON");
        return data[pos++];
    }
	// 特定の文字を期待している場合に使用する関数.
    inline bool Match(char c) {
        SkipWhitespace();
        if (pos < length && data[pos] == c) { pos++; return true; }
        return false;
    }
	// 文字列を解析するための関数.
    std::string ParseString() {
        if (!Match('"')) throw std::runtime_error("Expected '\"'");
        std::string result;
        result.reserve(64); // 適当な予約
        while (pos < length) {
            char c = data[pos++];
            if (c == '"') break;
            if (c == '\\') {
                if (pos >= length) throw std::runtime_error("Unterminated escape");
                char esc = data[pos++];
                switch (esc) {
                case '"' : result.push_back('"');   break;
                case '\\': result.push_back('\\');  break;
                case '/' : result.push_back('/');   break;
                case 'b' : result.push_back('\b');  break;
                case 'f' : result.push_back('\f');  break;
                case 'n' : result.push_back('\n');  break;
                case 'r' : result.push_back('\r');  break;
                case 't' : result.push_back('\t');  break;
                default: throw std::runtime_error("Invalid escape char");
                }
            }
            else {
                result.push_back(c);
            }
        }
        return result;
    }
	// 数値を解析するための関数.
    double ParseNumber() {
        SkipWhitespace();
        size_t start = pos;
        bool seenDot = false;
        if (data[pos] == '-') pos++;
        while (pos < length && (std::isdigit(static_cast<unsigned char>(data[pos])) || data[pos] == '.')) {
            if (data[pos] == '.') seenDot = true;
            pos++;
        }
        return std::strtod(&data[start], nullptr); // std::strtod を直接使う
    }
	// bool 型の値を解析するための関数.
    bool ParseTrue() {
        if (pos + 4 <= length && std::memcmp(data + pos, "true", 4) == 0) { pos += 4; return true; }
        throw std::runtime_error("Expected true");
    }
	// bool 型の値を解析するための関数.
    bool ParseFalse() {
        if (pos + 5 <= length && std::memcmp(data + pos, "false", 5) == 0) { pos += 5; return false; }
        throw std::runtime_error("Expected false");
    }
	// nullptr_t を返すための関数.
    std::nullptr_t ParseNull() {
        if (pos + 4 <= length && std::memcmp(data + pos, "null", 4) == 0) { pos += 4; return nullptr; }
        throw std::runtime_error("Expected null");
    }
};
// JSON 文字列を解析するためのクラス.
class JsonParser {
private:
    JsonLexer lexer;
	// JSONの値を解析するためのヘルパー関数群.
    JsonValue ParseValue() {
        char c = lexer.Peek();
        switch (c) {
		case '{': return ParseObject();                                     // JSONオブジェクトを解析する.
		case '[': return ParseArray();                                      // JSON配列を解析する.
		case '"': return JsonValue{ JsonPrimitive(lexer.ParseString()) };   // 文字列を解析して JsonPrimitive に格納する.
		case 't': return JsonValue{ JsonPrimitive(lexer.ParseTrue()) };     // 真偽値 true を解析して JsonPrimitive に格納する.
		case 'f': return JsonValue{ JsonPrimitive(lexer.ParseFalse()) };    // 真偽値 false を解析して JsonPrimitive に格納する.
		case 'n': return JsonValue{ JsonPrimitive(lexer.ParseNull()) };     // nullptr_t を解析して JsonPrimitive に格納する.
        default:
			if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) {  // 数値を解析する.
                return JsonValue{ JsonPrimitive(lexer.ParseNumber()) };
            }
            throw std::runtime_error("Unexpected JSON value");
        }
    }
	// JSONオブジェクトを解析するためのヘルパー関数.
    JsonValue ParseObject() {
        lexer.Match('{'); // skip '{'
        JsonObject obj;
        if (lexer.Match('}')) return JsonValue{ obj };
        while (true) {
            std::string key = lexer.ParseString();
            if (!lexer.Match(':')) throw std::runtime_error("Expected ':' after key");
            obj.emplace(std::move(key), ParseValue());
            if (lexer.Match('}')) break;
            if (!lexer.Match(',')) throw std::runtime_error("Expected ',' or '}'");
        }
        return JsonValue{ obj };
    }
	// JSON配列を解析するためのヘルパー関数.
    JsonValue ParseArray() {
        lexer.Match('['); // skip '['
        JsonArray arr;
        if (lexer.Match(']')) return JsonValue{ arr };
        while (true) {
            arr.push_back(ParseValue());
            if (lexer.Match(']')) break;
            if (!lexer.Match(',')) throw std::runtime_error("Expected ',' or ']'");
        }
        return JsonValue{ arr };
    }

public:
    explicit JsonParser(const std::string& json) : lexer(json) {}
    JsonValue Parse() { return ParseValue(); }
};

#pragma once
/*
    JsonUtility.h
*/
#include "Project.h"

// c++ 17 以上の場合.
#include "JsonValue.hpp"
#include "Reflection.h"
class JsonUtility {
public:
    template <typename T>
    static std::string ToJson(const T& obj) {
        try {
            std::ostringstream stream;
            stream << "{";
            bool first = true;
            std::map<std::string, std::string> fields;
            Reflector<T>::Reflect(obj, fields);

            for (const auto& [name, value] : fields) {
                if (!first) stream << ", ";
                // ここは簡略化：ネスト対応はReflectorで文字列化済みと仮定
                if (!value.empty() && (value[0] == '{' || value[0] == '[')) {
                    stream << "\"" << name << "\": " << value;
                }
                else {
                    stream << "\"" << name << "\": \"" << value << "\"";
                }
                first = false;
            }

            stream << "}";
            return stream.str();
        }
        catch (const std::exception& e) {
            std::cerr << "ToJson Error: " << e.what() << std::endl;
            return "{}";
        }
    }

    template <typename T>
    static T FromJson(const std::string& jsonString) {
        try {
            JsonParser parser(jsonString);
            JsonValue root = parser.Parse();
            if (!root.IsObject()) throw std::runtime_error("Root JSON is not an object");

            T obj;
            Reflector<T>::From(obj, root.AsObject());

            return obj;
        }
        catch (const std::exception& e) {
            std::cerr << "FromJson Error: " << e.what() << std::endl;
            return T();
        }
    }
};
#pragma once

#include <string>
#include <vector>
#include <regex>
#include <variant>

// --- TextSegment ---
struct TextSegment {
    std::string text;
    int sizePercent = 100;
    int color = -1;         // RGBA形式で保持（-1はデフォルト色）
    int bgColor = -1;       // -1 は背景色なし
    bool bold = false;
    bool italic = false;
    int outlineWidth = 0;

    float letterSpacing = -1.0f; // -1: デフォルトの間隔を使用
};

static std::string InsertCommas(const std::string& input) {
    std::string number = input;
    // 符号や小数点がある場合はスキップ
    if (number.empty() || number.find_first_not_of("0123456789") != std::string::npos) {
        return input;
    }

    std::string result;
    int count = 0;
    for (int i = static_cast<int>(number.size()) - 1; i >= 0; --i) {
        result.insert(result.begin(), number[i]);
        ++count;
        if (count % 3 == 0 && i != 0) {
            result.insert(result.begin(), ',');
        }
    }
    return result;
}


// --- Rich Text タグ用パーサ ---
// 任意の <size=xx>...</size> タグのほか、将来のタグ拡張も可能な汎用実装
struct RichTextTag { std::string name; std::string value; bool closing; };
static std::vector<std::variant<RichTextTag, std::string>> TokenizeRichText(const std::string& src) {
    std::vector<std::variant<RichTextTag, std::string>> tokens;
    size_t pos = 0;

    auto trim = [](std::string& s) {
        static const char* whitespace = " \t\n\r";
        size_t a = s.find_first_not_of(whitespace);
        if (a == std::string::npos) {
            s.clear();
            return;
        }
        size_t b = s.find_last_not_of(whitespace);
        s = s.substr(a, b - a + 1);
        };

    while (pos < src.size()) {
        size_t lt = src.find('<', pos);
        if (lt == std::string::npos) {
            tokens.emplace_back(src.substr(pos));
            break;
        }
        if (lt > pos)
            tokens.emplace_back(src.substr(pos, lt - pos));
        size_t gt = src.find('>', lt);
        if (gt == std::string::npos) break;
        std::string tag = src.substr(lt + 1, gt - lt - 1);
        RichTextTag rt{ "", "", false };
        if (!tag.empty() && tag[0] == '/') {
            rt.closing = true;
            rt.name = tag.substr(1);
        }
        else {
            rt.closing = false;
            auto eq = tag.find('=');
            if (eq != std::string::npos) {
                rt.name = tag.substr(0, eq);
                rt.value = tag.substr(eq + 1);
            }
            else {
                rt.name = tag;
            }
        }
        trim(rt.name);
        trim(rt.value);
        tokens.emplace_back(rt);
        pos = gt + 1;
    }
    return tokens;
}

static std::vector<TextSegment>  ParseRichText(const std::string& src, bool autoComma = false) {
    auto tokens = TokenizeRichText(src);
    std::vector<TextSegment> segments;

    struct StyleState {
        int size = 100;
        int color = -1;
        bool bold = false;
        bool italic = false;
        int outlineWidth = 0;
        int bgColor = -1;       // -1 は背景色なし
        float letterSpacing = -1.0f;
    };


    struct TaggedStyle {
        std::string tagName;
        StyleState state;
    };

    std::vector<TaggedStyle> styleStack;
    StyleState current;

    enum class TagType { Size, Color, Bold, Italic, Outline, BgColor, Spacing, Unknown };

    auto ToTagType = [](const std::string& name) {
        if (name == "size")    return TagType::Size;
        if (name == "color")   return TagType::Color;
        if (name == "b")       return TagType::Bold;
        if (name == "i")       return TagType::Italic;
        if (name == "outline") return TagType::Outline;
        if (name == "bgcolor") return TagType::BgColor;
        if (name == "spacing") return TagType::Spacing;
        return TagType::Unknown;
        };



    auto ColorNameToInt = [](const std::string& name) -> int {
        if (name == "red")         return 0xFF0000;
        if (name == "green")       return 0x00FF00;
        if (name == "blue")        return 0x0000FF;
        if (name == "white")       return 0xFFFFFF;
        if (name == "black")       return 0x000000;
        if (name == "yellow")      return 0xFFFF00;
        if (name == "cyan")        return 0x00FFFF;
        if (name == "magenta")     return 0xFF00FF;
        if (name == "gray")        return 0x808080;
        if (name == "grey")        return 0x808080;
        if (name == "lightgray")   return 0xD3D3D3;
        if (name == "darkgray")    return 0xA9A9A9;
        if (name == "orange")      return 0xFFA500;
        if (name == "pink")        return 0xFFC0CB;
        if (name == "purple")      return 0x800080;
        if (name == "brown")       return 0xA52A2A;
        if (name == "gold")        return 0xFFD700;
        if (name == "lime")        return 0x00FF00;
        if (name == "navy")        return 0x000080;
        if (name == "teal")        return 0x008080;
        if (name == "olive")       return 0x808000;
        return -1;
        };


    for (auto& out : tokens) {
        if (std::holds_alternative<std::string>(out)) {
            std::string txt = std::get<std::string>(out);
            size_t start = txt.find_first_not_of(" \t\n\r");
            size_t end = txt.find_last_not_of(" \t\n\r");
            if (start == std::string::npos) txt.clear();
            else txt = txt.substr(start, end - start + 1);

            if (autoComma && txt.find_first_not_of("0123456789") == std::string::npos) {
                txt = InsertCommas(txt);
            }

            if (!txt.empty()) {
                segments.push_back({
                    txt,
                    current.size,
                    current.color,
                    current.bgColor,
                    current.bold,
                    current.italic,
                    current.outlineWidth,
                    current.letterSpacing
                    });
            }
        }
        else {
            auto tag = std::get<RichTextTag>(out);
            TagType type = ToTagType(tag.name);

            if (!tag.closing) {
                // 開始タグ：現在の状態をスタックに保存
                styleStack.push_back({ tag.name, current });

                switch (type) {
                case TagType::Size:
                {
                    try { current.size = std::stoi(tag.value); }
                    catch (...) { current.size = 100; }
                    break;
                }
                case TagType::Color:
                {
                    if (!tag.value.empty() && tag.value[0] == '#') {
                        std::string hex = tag.value.substr(1);
                        if (hex.length() == 6) {
                            // #RRGGBB → RRGGBBFF（不透明）
                            hex += "FF";
                        }
                        if (hex.length() == 8) {
                            // #RRGGBBAA → int（RGBA）
                            unsigned int rgba = std::stoul(hex, nullptr, 16);
                            current.color = static_cast<int>(rgba);
                        }
                        else {
                            current.color = -1; // 無効
                        }
                    }
                }
                    break;
                case TagType::Bold:
                {
                    current.bold = true;
                }
                    break;
                case TagType::Italic:
                    current.italic = true;
                    break;
                case TagType::Outline:
                    try { current.outlineWidth = std::stoi(tag.value); }
                    catch (...) { current.outlineWidth = 0; }
                    break;
                case TagType::BgColor:
                    if (!tag.value.empty() && tag.value[0] == '#') {
                        std::string hex = tag.value.substr(1);
                        if (hex.length() == 6) {
                            hex += "FF";
                        }
                        if (hex.length() == 8) {
                            unsigned int rgba = std::stoul(hex, nullptr, 16);
                            current.bgColor = static_cast<int>(rgba);
                        }
                        else {
                            current.bgColor = -1;
                        }
                    }
                    break;
                case TagType::Spacing:
                    try { current.letterSpacing = std::stof(tag.value); }
                    catch (...) { current.letterSpacing = -1.0f; }
                    break;
                default:
                    break;
                }
            }
            else {
                // 終了タグ：同じタグ名のものだけ巻き戻す
                for (auto it = styleStack.rbegin(); it != styleStack.rend(); ++it) {
                    if (it->tagName == tag.name) {
                        current = it->state;
                        styleStack.erase((it + 1).base()); // reverse_iterator → iterator
                        break;
                    }
                }
            }
        }
    }

    return segments;
}

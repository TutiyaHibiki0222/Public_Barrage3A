#include "BulletType.h"
#include "File.hpp"

BulletColor StringToBulletColor(const std::string& str) {
    static const std::unordered_map<std::string, BulletColor> map = {
        // Gray系
        {"Gray", BulletColor::Gray},
        {"Grey", BulletColor::Gray},
        {"グレー", BulletColor::Gray},

        // Red系
        {"Red", BulletColor::Red},
        {"DarkRed", BulletColor::Red},
        {"赤", BulletColor::Red},

        // VividRed系
        {"VividRed", BulletColor::VividRed},
        {"BrightRed", BulletColor::VividRed},
        {"ビビッドレッド", BulletColor::VividRed},

        // Purple系
        {"Purple", BulletColor::Purple},
        {"Violet", BulletColor::Purple},
        {"パープル", BulletColor::Purple},

        // Pink系
        {"Pink", BulletColor::Pink},
        {"LightPinkPurple", BulletColor::Pink},
        {"LightPink", BulletColor::Pink},
        {"ピンク", BulletColor::Pink},

        // Blue系
        {"Blue", BulletColor::Blue},
        {"Navy", BulletColor::Blue},
        {"青", BulletColor::Blue},

        // LightBlue系
        {"LightBlue", BulletColor::LightBlue},
        {"ライトブルー", BulletColor::LightBlue},

        // Cyan系
        {"Cyan", BulletColor::Cyan},
        {"Aqua", BulletColor::Cyan},
        {"シアン", BulletColor::Cyan},
        {"アクア", BulletColor::Cyan},

        // SkyBlue系
        {"SkyBlue", BulletColor::SkyBlue},
        {"空色", BulletColor::SkyBlue},
        {"ライトシアン", BulletColor::SkyBlue},

        // MintGreen系
        {"MintGreen", BulletColor::MintGreen},
        {"ミント", BulletColor::MintGreen},

        // NeonGreen系
        {"NeonGreen", BulletColor::NeonGreen},
        {"ElectricGreen", BulletColor::NeonGreen},
        {"ネオングリーン", BulletColor::NeonGreen},

        // LimeGreen系
        {"LimeGreen", BulletColor::LimeGreen},
        {"ライム", BulletColor::LimeGreen},

        // LemonYellow系
        {"LemonYellow", BulletColor::LemonYellow},
        {"レモン", BulletColor::LemonYellow},

        // PastelYellow系
        {"PastelYellow", BulletColor::PastelYellow},
        {"LightYellow", BulletColor::PastelYellow},
        {"パステルイエロー", BulletColor::PastelYellow},

        // Orange系
        {"Orange", BulletColor::Orange},
        {"オレンジ", BulletColor::Orange},

        // White系
        {"White", BulletColor::White},
        {"白", BulletColor::White},

        // Black系
        {"Black", BulletColor::Black},
        {"黒", BulletColor::Black},
        {"Dark", BulletColor::Black}
    };

    auto it = map.find(str);
    if (it != map.end()) return it->second;
    return BulletColor::Unknown;
}



struct BulletTypeJsonData {
    int id = 0;
    std::string texturePath = "";
    float hitboxSize = 0;
    std::vector<std::string> colors;

    JSON(id, texturePath, hitboxSize, colors)
};

struct BulletTypes {

    std::vector<BulletTypeJsonData> datas;

    JSON(datas)
};

void BulletTypeManager::LoadJson(const std::string& path) {
    try {
        std::string json = System::IO::File::ReadAllText(path);

        // JSON文字列からBulletTypesオブジェクトに変換
        BulletTypes bulletTypes = JsonUtility::FromJson<BulletTypes>(json);

        // 読み込んだ複数のBulletTypeJsonDataを処理
        for (const auto& bulletType : bulletTypes.datas) {
            BulletTypeData data;
            data.imagePath          = bulletType.texturePath;
            data.hitboxSize         = bulletType.hitboxSize;
            for (size_t i = 0; i < bulletType.colors.size(); i++) {
                data.colorToImageIndex[StringToBulletColor(bulletType.colors[i])] = (int)i;
            }
            BulletParentID bulletID = static_cast<BulletParentID>(bulletType.id);
            bulletTypeMap[bulletID] = data;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to load bullet types JSON: " << e.what() << std::endl;
    }
}

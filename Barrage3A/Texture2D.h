#pragma once

#include "DxLib.h"
#include "Path.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace GameEngine {
    
    struct Sprite {
        
        std::string name;   // 名.
        int spriteData;     // 画像データ.
        int width, height;  // 画像サイズ.

        int borderLeft, borderRight, borderTop, borderBottom;
        Sprite() : name(""), spriteData(-1), width(0),height(0),
            borderLeft(0), borderRight(0), borderTop(0), borderBottom(0)
        {}

        Sprite(const std::string& n, int data, int w, int h,int bl = 0,int br = 0,int bt = 0,int bb = 0)
            : name(n), spriteData(data), width(w), height(h),
            borderLeft(bl),borderRight(br), borderTop(bt), borderBottom(bb)
        {}
        ~Sprite() {
            if (spriteData != -1) {
                DeleteGraph(spriteData); // 画像データの解放
                spriteData = -1;         // 再利用防止
			}
        }
	public: // 画像を作成するための関数群.
        // 円画像を作成.
        static std::shared_ptr<Sprite> MakeCircle(int size = 64,bool fill = true,int line = 1.0f) {
			auto sprite = std::make_shared<Sprite>();
            sprite->name = "MaikCircle";
			int screen = GetDrawScreen();
            int make = MakeScreen((int)(size + line), (int)(size + line), TRUE);
			SetDrawScreen(make);
            int size2 = ((size) / 2);
            DrawCircle(size2, size2, size2, 0xFFFFFF, fill, line);
            SetDrawScreen(screen);
            sprite->width   = (int)size;
            sprite->height  = (int)size;
            sprite->borderLeft = 0;
            sprite->borderRight = 0;
            sprite->borderTop = 0;
            sprite->borderBottom = 0;
			sprite->spriteData = make;
			sprite->isMaik = true;
			return sprite;
        }   
        // 四角画像を作成.
        static std::shared_ptr<Sprite> MakeRect(int width = 64, int height = 64) {
			auto sprite = std::make_shared<Sprite>();
			sprite->name = "MaikRect";
			int screen = GetDrawScreen();
			auto make = MakeScreen(width, height, TRUE);
            SetDrawScreen(make);
            DrawBox(0, 0, width, height, 0xFFFFFF, TRUE);
            SetDrawScreen(screen);
            sprite->width = width;
            sprite->height = height;
            sprite->borderLeft = 0;
            sprite->borderRight = 0;
            sprite->borderTop = 0;
			sprite->borderBottom = 0;
			sprite->spriteData = make;
			sprite->isMaik = true;
			return sprite;
        }
        // 星型画像を作成.
        static std::shared_ptr<Sprite> MakeStar(int size = 64) {
            auto sprite = std::make_shared<Sprite>();
            sprite->name = "Star";
            int screen = GetDrawScreen();
            auto make = MakeScreen(size, size, TRUE);
            SetDrawScreen(make);

            float cx = size * 0.5f;
            float cy = size * 0.5f;
            float radiusOuter = size * 0.5f;
            float radiusInner = radiusOuter * 0.5f;

            const int points = 5;
            float angleStep =   DX_PI_F * 2 / (points * 2);
            float startAngle = -DX_PI_F / 2;

            // 星の頂点座標を計算
            float vx[points * 2];
            float vy[points * 2];
            for (int i = 0; i < points * 2; ++i) {
                float r = (i % 2 == 0) ? radiusOuter : radiusInner;
                float angle = startAngle + angleStep * i;
                vx[i] = cx + cosf(angle) * r;
                vy[i] = cy + sinf(angle) * r;
            }

            // 多角形描画
            for (int i = 0; i < points * 2; ++i) {
                int next = (i + 1) % (points * 2);
                DrawTriangle(
                    (int)cx, (int)cy,
                    (int)vx[i], (int)vy[i],
                    (int)vx[next], (int)vy[next],
                    GetColor(255, 255, 255), TRUE
                );
            }

            SetDrawScreen(screen);

            sprite->width = size;
            sprite->height = size;
            sprite->borderLeft = 0;
            sprite->borderRight = 0;
            sprite->borderTop = 0;
            sprite->borderBottom = 0;
            sprite->spriteData = make;
            sprite->isMaik = true;
            return sprite;
        }
        // 扇形画像を作成.
        static std::shared_ptr<Sprite> MakeFan(int size = 64, float startAngleDeg = 0, float sweepAngleDeg = 90) {
            auto sprite = std::make_shared<Sprite>();
            sprite->name = "Fan";
            int screen = GetDrawScreen();
            auto make = MakeScreen(size, size, TRUE);
            SetDrawScreen(make);

            float cx = size * 0.5f;
            float cy = size * 0.5f;
            float radius = size * 0.5f;

            float startRad = startAngleDeg * DX_PI_F / 180.0f;
            float sweepRad = sweepAngleDeg * DX_PI_F / 180.0f;
            int segments = 32;

            // 扇形を三角形で分割描画
            for (int i = 0; i < segments; ++i) {
                float angle1 = startRad + sweepRad * (float)i / segments;
                float angle2 = startRad + sweepRad * (float)(i + 1) / segments;

                DrawTriangle(
                    (int)cx, (int)cy,
                    (int)(cx + cosf(angle1) * radius), (int)(cy + sinf(angle1) * radius),
                    (int)(cx + cosf(angle2) * radius), (int)(cy + sinf(angle2) * radius),
                    GetColor(255, 255, 255), TRUE
                );
            }

            SetDrawScreen(screen);

            sprite->width = size;
            sprite->height = size;
            sprite->borderLeft = 0;
            sprite->borderRight = 0;
            sprite->borderTop = 0;
            sprite->borderBottom = 0;
            sprite->spriteData = make;
            sprite->isMaik = true;
            return sprite;
        }
        // 楕円描画を作成.
        static std::shared_ptr<Sprite> MakeEllipse(int width = 64, int height = 32, int segments = (64)) {
            auto sprite = std::make_shared<Sprite>();
            sprite->name = "Ellipse";
            int screen = GetDrawScreen();
            auto make = MakeScreen(width, height, TRUE);
            SetDrawScreen(make);

            int cx = width / 2;
            int cy = height / 2;

            for (int i = 0; i < segments; ++i) {
                float angle1 = DX_PI_F * 2 * i / segments;
                float angle2 = DX_PI_F * 2 * (i + 1) / segments;
                int x1 = cx + (int)(cosf(angle1) * cx);
                int y1 = cy + (int)(sinf(angle1) * cy);
                int x2 = cx + (int)(cosf(angle2) * cx);
                int y2 = cy + (int)(sinf(angle2) * cy);
                DrawTriangle(cx, cy, x1, y1, x2, y2, GetColor(255, 255, 255), TRUE);
            }

            SetDrawScreen(screen);

            sprite->width = width;
            sprite->height = height;
            sprite->borderLeft = 0;
            sprite->borderRight = 0;
            sprite->borderTop = 0;
            sprite->borderBottom = 0;
            sprite->spriteData = make;
            sprite->isMaik = true;
            return sprite;
        }
        // 角が丸い四角画像を作成.
        static std::shared_ptr<Sprite> MakeRoundRect(int width = 64, int height = 64, int radiusX = 10, int radiusY = 10) {
            auto sprite = std::make_shared<Sprite>();
            sprite->name = "RoundRect";
            int screen = GetDrawScreen();
            auto make = MakeScreen(width, height, TRUE);
            SetDrawScreen(make);

            DrawRoundRect(0, 0, width, height, radiusX, radiusY, GetColor(255, 255, 255), TRUE);

            SetDrawScreen(screen);

            sprite->width = width;
            sprite->height = height;
            sprite->borderLeft = 0;
            sprite->borderRight = 0;
            sprite->borderTop = 0;
            sprite->borderBottom = 0;
            sprite->spriteData = make;
            sprite->isMaik = true;
            return sprite;
        }
        // ノイズ画像を作成.
        static std::shared_ptr<Sprite> MakeNoise(int size = 32, float density = 0.1f) {
            auto sprite = std::make_shared<Sprite>();
            sprite->name = "Noise";
            int screen = GetDrawScreen();
            auto make = MakeScreen(size, size, TRUE);
            SetDrawScreen(make);
            ClearDrawScreen();
            for (int i = 0; i < (int)(size * size * density); ++i) {
                int x = rand() % size;
                int y = rand() % size;
                DrawPixel(x, y, GetColor(255, 255, 255));
            }
            SetDrawScreen(screen);
            sprite->width = size;
            sprite->height = size;
            sprite->spriteData = make;
            sprite->isMaik = true;
            return sprite;
        }
        // ドーナツ型画像を作成.
        static std::shared_ptr<Sprite> MakeRipple(int size = 64, int thickness = 4) {
            auto sprite = std::make_shared<Sprite>();
            sprite->name = "RippleAA";
            int screen = GetDrawScreen();
            int make = MakeScreen(size, size, TRUE);
            SetDrawScreen(make);

            ClearDrawScreen();
            int cx = size / 2;
            int cy = size / 2;
            int radiusOuter = size / 2 - 1;
            int radiusInner = radiusOuter - thickness;

            for (int y = 0; y < size; ++y) {
                for (int x = 0; x < size; ++x) {
                    float dx = (float)(x - cx);
                    float dy = (float)(y - cy);
                    float dist = sqrtf(dx * dx + dy * dy);
                    if (dist <= radiusOuter && dist >= radiusInner) {
                        DrawPixel(x, y, GetColor(255, 255, 255));
                    }
                }
            }
            SetDrawScreen(screen);
            sprite->width = size;
            sprite->height = size;
            sprite->spriteData = make;
            sprite->isMaik = true;
            return sprite;
        }
        // laser的な画像を作成.
        static std::shared_ptr<Sprite> MakeLine(int length = 64, int thickness = 4, int shadow = 2) {
            auto sprite = std::make_shared<Sprite>();
            sprite->name = "Line";
            int screen = GetDrawScreen();
            int make = MakeScreen(length + shadow * 2, thickness + shadow * 2, TRUE); // 枠を含むサイズに拡張
            SetDrawScreen(make);

            ClearDrawScreen();

            // 外枠（薄い透明色）を影っぽく描く
            unsigned int shadowColor = GetColor(255, 255, 255);
            for (int i = 1; i <= shadow; ++i) {
                int alpha = 64 / i; // 徐々に薄く
                SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
                DrawBox(i, i, length + i - 1, thickness + i - 1, shadowColor, FALSE);
            }
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

            // 中央の白い塗りつぶし長方形
            DrawBox(shadow, shadow, length + shadow - 1, thickness + shadow - 1, GetColor(255, 255, 255), TRUE);

            SetDrawScreen(screen);
            sprite->width = length + shadow * 2;
            sprite->height = thickness + shadow * 2;
            sprite->spriteData = make;
            sprite->isMaik = true;
            return sprite;
        }
        // グラデーション円型の画像を作成.
        static std::shared_ptr<Sprite> MakeGradientCircle(int size = 64) {
            auto sprite = std::make_shared<Sprite>();
            sprite->name = "GradientCircle";
            int screen = GetDrawScreen();
            int make = MakeScreen(size, size, TRUE);
            SetDrawScreen(make);

            ClearDrawScreen();

            int cx = size / 2;
            int cy = size / 2;
            int radius = size / 2 - 1;

            for (int y = 0; y < size; ++y) {
                for (int x = 0; x < size; ++x) {
                    float dx = (float)(x - cx);
                    float dy = (float)(y - cy);
                    float dist = sqrtf(dx * dx + dy * dy);
                    if (dist <= radius) {
                        float t = dist / radius; // 0(center)～1(edge)
                        int alpha = (int)((1.0f - t) * 255);
                        SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
                        DrawPixel(x, y, GetColor(255, 255, 255));
                        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
                    }
                }
            }

            SetDrawScreen(screen);
            sprite->width = size;
            sprite->height = size;
            sprite->spriteData = make;
            sprite->isMaik = true;
            return sprite;
        }
        // カプセル形状の画像を作成.
        static std::shared_ptr<Sprite> MakeCapsule(int width = 32, int height = 64) {
            auto sprite = std::make_shared<Sprite>();
            sprite->name = "Capsule";
            int screen = GetDrawScreen();
            int make = MakeScreen(width, height, TRUE);
            SetDrawScreen(make);

            ClearDrawScreen();

            int radius = width / 2;
            int bodyHeight = height - radius * 2;

            // 縦長の長方形部分
            DrawBox(0, radius, width, radius + bodyHeight, GetColor(255, 255, 255), TRUE);

            // 上の半円
            DrawCircle(width / 2, radius, radius, GetColor(255, 255, 255), TRUE);

            // 下の半円
            DrawCircle(width / 2, radius + bodyHeight, radius, GetColor(255, 255, 255), TRUE);

            SetDrawScreen(screen);

            sprite->width = width;
            sprite->height = height;
            sprite->spriteData = make;
            sprite->isMaik = true;

            return sprite;
        }
        // Polygon (4以上から )形状の画像を作成.
        static std::shared_ptr<Sprite> MakePolygon(int sides = 5, int size = 64) {
            if (sides < 3) sides = 3;

            auto sprite = std::make_shared<Sprite>();
            sprite->name = "Polygon_" + std::to_string(sides);

            int screen = GetDrawScreen();
            int make = MakeScreen(size, size, TRUE);
            SetDrawScreen(make);

            ClearDrawScreen();

            float cx = size / 2.0f;
            float cy = size / 2.0f;
            float radius = size / 2.0f - 2;

            // 頂点座標を計算
            std::vector<VECTOR> vertices(sides);
            for (int i = 0; i < sides; ++i) {
                float angle = DX_PI_F * 2.0f * i / sides - DX_PI_F / 2.0f;  // 上を向く形に調整
                vertices[i].x = cx + cosf(angle) * radius;
                vertices[i].y = cy + sinf(angle) * radius;
                vertices[i].z = 0;
            }

            // 三角形ごとに枠線を描画
            for (int i = 1; i < sides - 1; ++i) {
                DrawTriangle(
                    (int)vertices[0].x, (int)vertices[0].y,
                    (int)vertices[i].x, (int)vertices[i].y,
                    (int)vertices[i + 1].x, (int)vertices[i + 1].y,
                    GetColor(255, 255, 255),TRUE
                );
            }

            SetDrawScreen(screen);

            sprite->width = size;
            sprite->height = size;
            sprite->spriteData = make;
            sprite->isMaik = true;

            return sprite;
        }

        static std::shared_ptr<Sprite> MakeLoadImage(const std::string& path) {
            auto sprite = std::make_shared<Sprite>();
            int handle = LoadGraph(path.c_str());
            if (handle != -1) {
                sprite->spriteData = handle;
            }
            else {
                return nullptr;
            }
            int w = 0, h = 0;
            GetGraphSize(handle, &w, &h);
            sprite->width   = w;
            sprite->height  = h;
            sprite->name    = System::IO::Path::GetFileNameWithoutExtension(path);
            sprite->isMaik  = true;
            return sprite;
        }
    private:
        bool isMaik = false; // メイクで作られた画像かどうか.
    };

    struct Texture2D {
        std::unordered_map<std::string,std::shared_ptr<Sprite>> textureMap;
        
        Texture2D()
        {}
		// テクスチャデータを追加する.
        void AddTextureData(const Sprite& data) {
            textureMap[data.name] = std::make_shared<Sprite>(data);
        }
        void AddTextureData(const std::string& name,int handle, int spriteWidth, int spriteHeight) {
            textureMap[name] = std::make_shared<Sprite>(name,handle,spriteWidth,spriteHeight);
        }
		// 画像データを追加する.
        void AddSpritesFromHandles(const std::string& baseName, const std::vector<int>& handles, int spriteWidth, int spriteHeight) {
            for (size_t i = 0; i < handles.size(); ++i) {
                std::string name = baseName + "_" + std::to_string(i);
                AddTextureData(name, handles[i], spriteWidth, spriteHeight);
            }
        }
		// 画像データを追加する.
        void AddImageData(const std::string key, int img) {
            int x = 0, y = 0;
            GetGraphSize(img, &x, &y);
            textureMap[key] = std::make_shared<Sprite>(key,img,x,y);
        }
		// 画像サイズを取得する.
        std::shared_ptr<Sprite> GetTextureData(const std::string& name) const {
            auto it = textureMap.find(name);
            if (it != textureMap.end()) {
                return it->second;
            }
            return nullptr;
        }
		// すべてのテクスチャを取得する.
        std::vector<std::shared_ptr<Sprite>> GetAllTextures() const {
            std::vector<std::shared_ptr<Sprite>> result;
            result.reserve(textureMap.size());
            for (const auto& pair : textureMap) {
                result.push_back(pair.second);
            }
            return result;
        }
		// 指定されたプレフィックスに基づいてテクスチャを取得する.
        std::vector<std::shared_ptr<Sprite>> GetTexturesByPrefix(const std::string& prefix) const {
            std::vector<std::shared_ptr<Sprite>> result;
            for (const auto& pair : textureMap) {
                // prefix がキーの先頭にあるかどうか判定
                if (pair.first.rfind(prefix + "_", 0) == 0) {
                    result.push_back(pair.second);
                }
            }
            return result;
        }

        std::vector<std::shared_ptr<Sprite>> GetTexturesByPrefixRange(const std::string& prefix, int startIndex, int endIndex) const {
            std::vector<std::shared_ptr<Sprite>> result;
            int step = (startIndex <= endIndex) ? 1 : -1;

            for (int i = startIndex; i != endIndex + step; i += step) {
                std::string key = prefix + "_" + std::to_string(i);
                auto it = textureMap.find(key);
                if (it != textureMap.end()) result.push_back(it->second);
            }
            return result;
        }

        std::vector<std::shared_ptr<Sprite>> GetTexturesByPrefixRangePingPong(const std::string& prefix, int startIndex, int endIndex) const {
            std::vector<std::shared_ptr<Sprite>> result;
            int step = (startIndex <= endIndex) ? 1 : -1;

            // 行き
            for (int i = startIndex; i != endIndex + step; i += step) {
                std::string key = prefix + "_" + std::to_string(i);
                auto it = textureMap.find(key);
                if (it != textureMap.end()) result.push_back(it->second);
            }

            // 戻り（端を除いて逆方向）
            for (int i = endIndex - step; i != startIndex; i -= step) {
                std::string key = prefix + "_" + std::to_string(i);
                auto it = textureMap.find(key);
                if (it != textureMap.end()) result.push_back(it->second);
            }

            return result;
        }
    };
}
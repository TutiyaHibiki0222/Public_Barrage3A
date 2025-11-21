/*
    SystemImage.hpp

    :class
        - ImageCropper

    作成日         : 2025/04/16
    最終変更日     : 2025/04/16
*/
#pragma once

#include "DxLib.h"
#include "Mathf.h"
#include <vector>
#include <string>
#include <locale>

#ifndef _ImageHelper
#define _ImageHelper

namespace System::ImageHelper {

    class ImageCropper {
    public:

        static bool IsImageAllTransparent(int handle, int _x, int _y, int _w, int _h) {
            // 画像のサイズを取得
            int imgWidth, imgHeight;
            if (GetSoftImageSize(handle, &imgWidth, &imgHeight) == -1) {
                return false; // サイズ取得失敗
            }

            // 範囲が画像の範囲内に収まっているか確認
            if (_x < 0 || _y < 0 || (_x + _w) > imgWidth || (_y + _h) > imgHeight) {
                return false; // 範囲外
            }

            for (int i = 0; i < Mathf::Min(_w, _h); i++) {
                for (int xy = 0; xy < Mathf::Min(_w, _h) - i; xy++) {
                    int x = _x + xy + i;
                    int y = _y + i;
                    int a = 0;
                    if (x < _x + _w && y < _y + _h) {
                        GetPixelSoftImage(handle, x, y, nullptr, nullptr, nullptr, &a);
                        if (a > 0) {
                            return true; // 少なくとも1ピクセルが透過ではない.
                        }
                    }
                    x = _x + i;
                    y = _y + xy + i;
                    if (x < _x + _w && y < _y + _h) {
                        GetPixelSoftImage(handle, x, y, nullptr, nullptr, nullptr, &a);
                        if (a > 0) {
                            return true; // 少なくとも1ピクセルが透過ではない.
                        }
                    }
                }
            }

            return false;
        }

        static bool LoadImageFile(int& image, const char* fname) {
            int handle = LoadGraph(fname);
            if (handle == -1) {
                throw std::runtime_error("Failed to load image: " + std::string(fname));
                return false;
            }
            image = handle;
            return true;
        }
        static bool LoadDivImageFile(std::vector<int>& image, const char* fname, int allNum, int xNum, int yNum, int xSize, int ySize) {
            std::vector<int> handles(allNum, -1);
            if (LoadDivGraph(fname, allNum, xNum, yNum, xSize, ySize, handles.data()) == -1) {
                throw std::runtime_error("Failed to load divided image: " + std::string(fname));
                return false;
            }
            image.assign(handles.begin(), handles.end());
            return true;
        }

        static void LoadOffsetPaddingFile(
            std::vector<int>& image,
            const char* fname,
            int CellX = 1,
            int CellY = 1,
            int offsetX = 0,
            int offsetY = 0,
            int paddingX = 0,
            int paddingY = 0,
            bool excludeTransparent = false)
        {
            // グラフィック読み込み
            int img1 = LoadGraph(fname);
            if (img1 == -1) {
                throw std::runtime_error("LoadOffsetPaddingFile: Failed to load graph: " + std::string(fname));
            }

            int img2 = -1;
            if (excludeTransparent) {
                img2 = LoadSoftImage(fname);
                if (img2 == -1) {
                    DeleteGraph(img1);
                    throw std::runtime_error("LoadOffsetPaddingFile: Failed to load soft image: " + std::string(fname));
                }
            }

            // 元画像サイズ取得
            int width = 0, height = 0;
            if (GetGraphSize(img1, &width, &height) == -1) {
                DeleteGraph(img1);
                if (img2 != -1) DeleteSoftImage(img2);
                throw std::runtime_error("LoadOffsetPaddingFile: Failed to get graph size: " + std::string(fname));
            }

            // 各画像サイズ計算
            int imageWidth  = (width - offsetX * 2 - paddingX * (CellX - 1)) / CellX;
            int imageHeight = (height - offsetY * 2 - paddingY * (CellY - 1)) / CellY;

            for (int y = 0; y < CellY; ++y) {
                for (int x = 0; x < CellX; ++x) {
                    int left = offsetX + x * (imageWidth + paddingX);
                    int top = offsetY + y * (imageHeight + paddingY);

                    int handle = DerivationGraph(left, top, imageWidth, imageHeight, img1);
                    if (handle == -1) {
                        DeleteGraph(img1);
                        if (img2 != -1) DeleteSoftImage(img2);
                        throw std::runtime_error("LoadOffsetPaddingFile: Failed to derive sub-image [" +
                            std::to_string(x) + ", " + std::to_string(y) + "] from: " + fname);
                    }

                    if (excludeTransparent) {
                        if (IsImageAllTransparent(img2, left, top, imageWidth, imageHeight)) {
                            DeleteGraph(handle); // 完全に透明ならスキップ
                        }
                        else {
                            image.push_back(handle);
                        }
                    }
                    else {
                        image.push_back(handle);
                    }
                }
            }

            DeleteGraph(img1);
            if (img2 != -1) DeleteSoftImage(img2);
        }

        static bool LoadDivImageFile(int* imageArray, const char* fname, int allNum, int xNum, int yNum, int xSize, int ySize) {
            std::vector<int> handles(allNum, -1);
            if (LoadDivGraph(fname, allNum, xNum, yNum, xSize, ySize, handles.data()) == -1) {
                return false;
            }
            std::copy(handles.begin(), handles.end(), imageArray);
            return true;
        }
        static bool LoadOffsetPaddingFile(int* imageArray, const char* fname, int CellX = 1, int CellY = 1, int offsetX = 0, int offsetY = 0, int paddingX = 0, int paddingY = 0, bool transparentPassFlg = false) {
            int img1 = LoadGraph(fname);
            if (img1 == -1) {
                return false;
            }

            int img2 = -1;
            if (transparentPassFlg) {
                img2 = LoadSoftImage(fname);
                if (img2 == -1) {
                    DeleteGraph(img1);
                    return false;
                }
            }

            int width, height;
            if (GetGraphSize(img1, &width, &height) == -1) {
                DeleteGraph(img1);
                if (img2 != -1) {
                    DeleteGraph(img2);
                }
                return false;
            }

            int imageWidth = (width - offsetX * 2 - paddingX * (CellX - 1)) / CellX;
            int imageHeight = (height - offsetY * 2 - paddingY * (CellY - 1)) / CellY;

            int index = 0;
            for (int y = 0; y < CellY; ++y) {
                for (int x = 0; x < CellX; ++x) {
                    int left = offsetX + x * (imageWidth + paddingX);
                    int top = offsetY + y * (imageHeight + paddingY);
                    int handle = DerivationGraph(left, top, imageWidth, imageHeight, img1);
                    if (handle == -1) {
                        DeleteGraph(img1);
                        if (img2 != -1) {
                            DeleteGraph(img2);
                        }
                        return false;
                    }

                    if (transparentPassFlg) {
                        if (IsImageAllTransparent(img2, left, top, imageWidth, imageHeight)) {
                            imageArray[index++] = handle;
                        }
                        else {
                            DeleteGraph(handle); // 透明でないなら捨てる
                        }
                    }
                    else {
                        imageArray[index++] = handle;
                    }
                }
            }

            DeleteGraph(img1);
            if (img2 != -1) {
                DeleteGraph(img2);
            }

            return true;
        }
    };

  

}

#endif
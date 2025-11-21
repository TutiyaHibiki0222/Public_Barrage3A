#pragma once
#include "headers.h"
#include "DxLib.h"
#include <string>

class Font {
private:
    std::string fontName = "‚l‚r ƒSƒVƒbƒN";
    int fontSize = 24;
    int thick = 1;               // ‘¾Žš‘Š“–
    int fontType = DX_FONTTYPE_ANTIALIASING;
    int fontHandle = -1;

public:
    Font() {
        CreateHandle();
    }

    Font(const std::string& name, int size, int thickVal = -1, int type = DX_FONTTYPE_ANTIALIASING)
        : fontName(name), fontSize(size), thick(thickVal), fontType(type) {
        CreateHandle();
    }

    ~Font() {
        ReleaseHandle();
    }

    void SetFontName(const std::string& name) {
        fontName = name;
        RecreateHandle();
    }

    void SetFontSize(int size) {
        fontSize = size;
        RecreateHandle();
    }

    void SetThick(int thickVal) {
        thick = thickVal;
        RecreateHandle();
    }

    void SetFontType(int type) {
        fontType = type;
        RecreateHandle();
    }

    int GetFontHandle() const {
        if (fontHandle == -1) {
            const_cast<Font*>(this)->CreateHandle();
        }
        return fontHandle;
    }

    int GetFontSize() const {
        return fontSize;
	}

private:
    void CreateHandle() {
        if(fontName.empty()) {
            fontHandle = CreateFontToHandle(null, fontSize, thick, fontType);
		}
        else {
            fontHandle = CreateFontToHandle(fontName.c_str(), fontSize, thick, fontType);
        }
    }

    void ReleaseHandle() {
        if (fontHandle != -1) {
            DeleteFontToHandle(fontHandle);
            fontHandle = -1;
        }
    }

    void RecreateHandle() {
        ReleaseHandle();
        CreateHandle();
    }
};

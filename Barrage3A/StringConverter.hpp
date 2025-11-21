#pragma once

#include <windows.h>
#include <string>
#include <vector>

namespace System {
    /// <summary>
    /// String ŠÖŒW‚ÌŒ`®•ÏŠ·.
    /// </summary>
    class StringConverter {
    public:
        // wstringŒ^ ‚©‚ç stringŒ^‚É•ÏŠ·.
        static std::string WStringToString(const std::wstring& wstr, UINT codepage = CP_UTF8) {
            if (wstr.empty()) return {};

            int size_needed = WideCharToMultiByte(codepage, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
            if (size_needed <= 0) return {};

            std::vector<char> buffer(size_needed);
            char defaultChar = '?';
            WideCharToMultiByte(codepage, 0, wstr.c_str(), -1, buffer.data(), size_needed, &defaultChar, nullptr);

            // I’[•¶š‚ÍŠÜ‚ß‚È‚¢
            return std::string(buffer.data(), size_needed - 1);
        }
        // stringŒ^ ‚©‚ç wstringŒ^‚É•ÏŠ·.
        static std::wstring StringToWString(const std::string& str, UINT codepage = CP_UTF8) {
            if (str.empty()) return {};

            int size_needed = MultiByteToWideChar(codepage, 0, str.c_str(), -1, nullptr, 0);
            if (size_needed <= 0) return {};

            std::vector<wchar_t> buffer(size_needed);
            MultiByteToWideChar(codepage, 0, str.c_str(), -1, buffer.data(), size_needed);

            // I’[•¶š‚ÍŠÜ‚ß‚È‚¢
            return std::wstring(buffer.data(), size_needed - 1);
        }
    };
}

#include "Project.h"
#include "PlayerPrefs.h"
#include "File.hpp"
#include "EditorUtility.hpp"
#include <windows.h>
#include <wincrypt.h>    // SHA256用
#include <sstream>
#include <iomanip>
#include <stdexcept>
#pragma comment(lib, "Crypt32.lib")  // これを追加

using namespace System::IO;

// static変数の初期化
std::unordered_map<std::string, std::string> PlayerPrefs::prefs;
#if _DEBUG
std::string PlayerPrefs::filename = "_prefs_debug.dat";
#else
std::string PlayerPrefs::filename = "_prefs.dat";
#endif
bool PlayerPrefs::initialized   = false;
bool PlayerPrefs::needsSave     = false; // 暗号化フラグ

void PlayerPrefs::Init()
{
    if (!initialized) {
        Load();
        initialized = true;
    }
}

// XOR暗号化／復号（同じ処理）
std::string SimpleXOREncrypt(const std::string& input, char key = 0x5A) {
    std::string output = input;
    for (char& c : output) c ^= key;
    return output;
}
// Base64エンコード
std::string Base64Encode(const std::string& input) {
    if (input.empty()) return "";

    DWORD encodedLen = 0;
    if (!CryptBinaryToStringA(
        reinterpret_cast<const BYTE*>(input.data()),
        (DWORD)input.size(),
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
        NULL,
        &encodedLen))
    {
        throw std::runtime_error("CryptBinaryToStringA get length failed");
    }

    std::string encoded;
    encoded.resize(encodedLen);

    if (!CryptBinaryToStringA(
        reinterpret_cast<const BYTE*>(input.data()),
        (DWORD)input.size(),
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
        &encoded[0],
        &encodedLen))
    {
        throw std::runtime_error("CryptBinaryToStringA encode failed");
    }

    // CryptBinaryToStringAはnull終端つけるので、resizeでnull除去
    if (!encoded.empty() && encoded.back() == '\0') {
        encoded.pop_back();
    }

    return encoded;
}

// Base64デコード
std::string Base64Decode(const std::string& input) {
    if (input.empty()) return "";

    DWORD decodedLen = 0;
    if (!CryptStringToBinaryA(
        input.c_str(),
        (DWORD)input.size(),
        CRYPT_STRING_BASE64,
        NULL,
        &decodedLen,
        NULL,
        NULL))
    {
        throw std::runtime_error("CryptStringToBinaryA get length failed");
    }

    std::string decoded;
    decoded.resize(decodedLen);

    if (!CryptStringToBinaryA(
        input.c_str(),
        (DWORD)input.size(),
        CRYPT_STRING_BASE64,
        reinterpret_cast<BYTE*>(&decoded[0]),
        &decodedLen,
        NULL,
        NULL))
    {
        throw std::runtime_error("CryptStringToBinaryA decode failed");
    }

    return decoded;
}

// 改行やバックスラッシュをエスケープ（保存時）
std::string EscapeString(const std::string& s) {
    std::string result;
    for (char c : s) {
        switch (c) {
        case '\n': result += "\\n"; break;
        case '\\': result += "\\\\"; break;
        case '\r': /* 無視するか \\r にしても良い */ break;
        default:   result += c; break;
        }
    }
    return result;
}

// エスケープ文字列を元に戻す（読み込み時）
std::string UnescapeString(const std::string& s) {
    std::string result;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            char next = s[i + 1];
            if (next == 'n') {
                result += '\n';
                ++i;
            }
            else if (next == '\\') {
                result += '\\';
                ++i;
            }
            else {
                // 不明なエスケープはそのまま
                result += s[i];
            }
        }
        else {
            result += s[i];
        }
    }
    return result;
}

// SHA256ハッシュ（バイナリ32バイト）を計算して返す
std::string CalcSHA256Bin(const std::string& data) {
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    BYTE hash[32];
    DWORD hashSize = sizeof(hash);

    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        throw std::runtime_error("CryptAcquireContext failed");
    }
    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
        CryptReleaseContext(hProv, 0);
        throw std::runtime_error("CryptCreateHash failed");
    }
    if (!CryptHashData(hHash, reinterpret_cast<const BYTE*>(data.data()), (DWORD)data.size(), 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        throw std::runtime_error("CryptHashData failed");
    }
    if (!CryptGetHashParam(hHash, HP_HASHVAL, hash, &hashSize, 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        throw std::runtime_error("CryptGetHashParam failed");
    }

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);

    return std::string(reinterpret_cast<const char*>(hash), hashSize);
}

// ファイルを隠し属性に設定する関数（失敗したらfalseを返す）
bool SetFileHiddenAttribute(const std::string& filepath) {
    DWORD attrs = GetFileAttributesA(filepath.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        return false; // ファイルが存在しない等
    }

    // 既存の属性に隠し属性を加える
    attrs |= FILE_ATTRIBUTE_HIDDEN;

    return SetFileAttributesA(filepath.c_str(), attrs) != 0;
}

// --- PlayerPrefs メソッド ---
void PlayerPrefs::Save()
{
    if (!needsSave) return;
    needsSave = false;

    try {
        std::ostringstream oss;
        for (const auto& pair : prefs) {
            std::string encodedValue = EscapeString(pair.second); // ← エスケープして一行に
            oss << pair.first << "=" << encodedValue << "\n";
        }
        std::string plainData = oss.str();

#ifdef _DEBUG
        // デバッグ時は平文テキスト保存
        File::WriteAllText(filename, plainData);
#else
        // 暗号化
        std::string encrypted = SimpleXOREncrypt(plainData);

        // ハッシュ計算（平文データ）
        std::string hashBin = CalcSHA256Bin(plainData);

        // ファイル = 暗号化データ + ハッシュ32バイト
        std::string saveData = encrypted + hashBin;

        File::WriteAllBytes(filename, saveData);

        SetFileHiddenAttribute(filename);
#endif
    }
    catch (const std::exception& e) {
        std::cerr << "PlayerPrefs::Save failed: " << e.what() << std::endl;
    }
}

void PlayerPrefs::Load()
{
    try {
        if (!File::Exists(filename)) return;

#ifdef _DEBUG
        std::string content = File::ReadAllText(filename);
        prefs.clear();
        std::istringstream iss(content);
        std::string line;
        while (std::getline(iss, line)) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string escapedValue = line.substr(pos + 1);
                std::string decodedValue = UnescapeString(escapedValue);  // ← エスケープ解除
                prefs[key] = decodedValue;
            }
        }
#else
        std::string fileData = File::ReadAllBytes(filename);
        if (fileData.size() < 32) throw std::runtime_error("Prefs file size too small");

        std::string encrypted = fileData.substr(0, fileData.size() - 32);
        std::string savedHash = fileData.substr(fileData.size() - 32, 32);

        std::string plainData = SimpleXOREncrypt(encrypted);
        std::string calcHash = CalcSHA256Bin(plainData);

        if (savedHash != calcHash) {
            GameEditor::EditorUtility::DisplayErrorMessage("Error", "PlayerPrefs file hash mismatch! File corrupted or modified.");
            throw std::runtime_error("Prefs file hash mismatch! File corrupted or modified.");
        }

        prefs.clear();
        std::istringstream iss(plainData);
        std::string line;
        while (std::getline(iss, line)) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string escapedValue = line.substr(pos + 1);
                std::string decodedValue = UnescapeString(escapedValue);  // ← エスケープ解除
                prefs[key] = decodedValue;
            }
        }
#endif
    }
    catch (const std::exception& e) {
        std::cerr << "PlayerPrefs::Load failed: " << e.what() << std::endl;
        prefs.clear();
    }
}

void PlayerPrefs::SetInt(const std::string& key, int value)
{
    Init();
    prefs[key] = std::to_string(value);
    needsSave = true;
}

void PlayerPrefs::SetFloat(const std::string& key, float value)
{
    Init();
    prefs[key] = std::to_string(value);
    needsSave = true;
}

void PlayerPrefs::SetString(const std::string& key, const std::string& value)
{
    Init();
    prefs[key] = value;
    needsSave = true;
}

int PlayerPrefs::GetInt(const std::string& key, int defaultValue)
{
    Init();
    if (prefs.find(key) != prefs.end()) {
        try {
            return std::stoi(prefs[key]);
        }
        catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

float PlayerPrefs::GetFloat(const std::string& key, float defaultValue)
{
    Init();
    if (prefs.find(key) != prefs.end()) {
        try {
            return std::stof(prefs[key]);
        }
        catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

std::string PlayerPrefs::GetString(const std::string& key, const std::string& defaultValue)
{
    Init();
    auto it = prefs.find(key);
    if (it != prefs.end()) {
        try {
            return it->second; // 取得時にデコード
        }
        catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

bool PlayerPrefs::HasKey(const std::string& key)
{
    Init();
    return prefs.find(key) != prefs.end();
}

void PlayerPrefs::DeleteAll()
{
    Init();
    prefs.clear();
    needsSave = true;
    //Save();
}

void PlayerPrefs::DeleteKey(const std::string& key)
{
    Init();
    prefs.erase(key);
    needsSave = true;
    //Save();
}

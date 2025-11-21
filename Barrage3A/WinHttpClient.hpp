#pragma once
#include <windows.h>
#include <winhttp.h>
#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <vector>
#include <codecvt>
#include <locale>

#include "StringConverter.hpp"

#pragma comment(lib, "winhttp.lib")

namespace System::Net {

    class WinHttpClient {
    public:
        WinHttpClient() = default;
        ~WinHttpClient() { Cleanup(); }

        // GET
        bool Get(const std::wstring& url, std::wstring& response) {
            return SendRequest(L"GET", url, L"", response);
        }

        // POST
        bool Post(const std::wstring& url, const std::wstring& data, std::wstring& response) {
            return SendRequest(L"POST", url, data, response);
        }

        const std::map<std::wstring, std::wstring>& GetResponseHeaders() const {
            return responseHeaders;
        }

        void AddHeader(const std::wstring& header) {
            customHeaders += header + L"\r\n";
        }

    private:
        HINTERNET hSession = nullptr;
        HINTERNET hConnect = nullptr;
        HINTERNET hRequest = nullptr;
        std::wstring customHeaders;
        std::map<std::wstring, std::wstring> responseHeaders;

        void Cleanup() {
            if (hRequest) { WinHttpCloseHandle(hRequest); hRequest = nullptr; }
            if (hConnect) { WinHttpCloseHandle(hConnect); hConnect = nullptr; }
            if (hSession) { WinHttpCloseHandle(hSession); hSession = nullptr; }
            customHeaders.clear();
            responseHeaders.clear();
        }

        // ヘルパー：16進数4桁を数値に変換
        uint16_t Hex4ToInt(const std::wstring& hex) {
            uint16_t val = 0;
            for (int i = 0; i < 4; ++i) {
                val <<= 4;
                wchar_t c = hex[i];
                if (c >= L'0' && c <= L'9') val += (c - L'0');
                else if (c >= L'a' && c <= L'f') val += (c - L'a' + 10);
                else if (c >= L'A' && c <= L'F') val += (c - L'A' + 10);
                else return 0; // 不正な文字
            }
            return val;
        }


        std::wstring DecodeUnicodeEscapes(const std::wstring& input) {
            std::wstring output;
            for (size_t i = 0; i < input.size();) {
                if (input[i] == L'\\' && i + 5 < input.size() && input[i + 1] == L'u') {
                    std::wstring hexStr = input.substr(i + 2, 4);
                    uint16_t code = Hex4ToInt(hexStr);

                    // サロゲートペア対応は今は省略

                    output += static_cast<wchar_t>(code);
                    i += 6; // \uXXXX の分だけ進める
                }
                else {
                    output += input[i];
                    i++;
                }
            }
            return output;
        }

        bool ParseUrl(const std::wstring& url, std::wstring& host, std::wstring& path, bool& isHttps, INTERNET_PORT& port) const {
            isHttps = false;
            port = 0;
            const std::wstring httpsPrefix = L"https://";
            const std::wstring httpPrefix = L"http://";
            size_t hostStart = 0;

            if (url.compare(0, httpsPrefix.size(), httpsPrefix) == 0) {
                isHttps = true;
                hostStart = httpsPrefix.size();
                port = INTERNET_DEFAULT_HTTPS_PORT;
            }
            else if (url.compare(0, httpPrefix.size(), httpPrefix) == 0) {
                hostStart = httpPrefix.size();
                port = INTERNET_DEFAULT_HTTP_PORT;
            }
            else {
                return false;
            }

            size_t pathPos = url.find(L'/', hostStart);
            std::wstring hostPortPart = (pathPos == std::wstring::npos)
                ? url.substr(hostStart)
                : url.substr(hostStart, pathPos - hostStart);

            size_t colonPos = hostPortPart.find(L':');
            if (colonPos != std::wstring::npos) {
                host = hostPortPart.substr(0, colonPos);
                port = static_cast<INTERNET_PORT>(std::stoi(hostPortPart.substr(colonPos + 1)));
            }
            else {
                host = hostPortPart;
            }

            path = (pathPos == std::wstring::npos) ? L"/" : url.substr(pathPos);
            return true;
        }

        bool SendRequest(const std::wstring& method, const std::wstring& url,
            const std::wstring& data, std::wstring& response) {
            Cleanup();

            std::wstring host, path;
            bool isHttps;
            INTERNET_PORT port;
            if (!ParseUrl(url, host, path, isHttps, port)) {
                std::wcerr << L"Invalid URL: " << url << std::endl;
                return false;
            }

            hSession = WinHttpOpen(L"WinHttpClient/1.0",
                WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                WINHTTP_NO_PROXY_NAME,
                WINHTTP_NO_PROXY_BYPASS, 0);
            if (!hSession) {
                std::wcerr << L"WinHttpOpen failed.\n";
                return false;
            }

            hConnect = WinHttpConnect(hSession, host.c_str(), port, 0);
            if (!hConnect) {
                std::wcerr << L"WinHttpConnect failed.\n";
                return false;
            }

            DWORD flags = isHttps ? WINHTTP_FLAG_SECURE : 0;
            hRequest = WinHttpOpenRequest(hConnect, method.c_str(),
                path.c_str(), nullptr,
                WINHTTP_NO_REFERER,
                WINHTTP_DEFAULT_ACCEPT_TYPES,
                flags);
            if (!hRequest) {
                std::wcerr << L"WinHttpOpenRequest failed.\n";
                return false;
            }

            if (method == L"POST" && customHeaders.find(L"Content-Type:") == std::wstring::npos) {
                AddHeader(L"Content-Type: application/x-www-form-urlencoded; charset=utf-8");
            }

            LPCWSTR headers = customHeaders.empty() ? nullptr : customHeaders.c_str();

            std::string utf8Data;
            const void* pData = nullptr;
            DWORD dwDataSize = 0;
            if (!data.empty()) {
                utf8Data = StringConverter::WStringToString(data);
                pData = utf8Data.data();
                dwDataSize = static_cast<DWORD>(utf8Data.size());
            }

            if (!WinHttpSendRequest(hRequest, headers, headers ? -1L : 0,
                (LPVOID)pData, dwDataSize, dwDataSize, 0)) {
                std::wcerr << L"WinHttpSendRequest failed. Error: " << GetLastError() << std::endl;
                return false;
            }

            if (!WinHttpReceiveResponse(hRequest, nullptr)) {
                std::wcerr << L"WinHttpReceiveResponse failed.\n";
                return false;
            }

            QueryResponseHeaders();

            // Content-Type charset取得
            std::wstring charset = L"utf-8";
            auto it = responseHeaders.find(L"Content-Type");
            if (it != responseHeaders.end()) {
                size_t pos = it->second.find(L"charset=");
                if (pos != std::wstring::npos) {
                    charset = it->second.substr(pos + 8);
                    for (auto& c : charset) c = towlower(c);
                }
            }

            response.clear();
            std::vector<BYTE> responseData;
            while (true) {
                DWORD size = 0;
                if (!WinHttpQueryDataAvailable(hRequest, &size)) {
                    std::wcerr << L"WinHttpQueryDataAvailable failed.\n";
                    return false;
                }
                if (size == 0) break;

                std::vector<BYTE> buffer(size);
                DWORD downloaded = 0;
                if (!WinHttpReadData(hRequest, buffer.data(), size, &downloaded)) {
                    std::wcerr << L"WinHttpReadData failed.\n";
                    return false;
                }
                responseData.insert(responseData.end(), buffer.begin(), buffer.begin() + downloaded);
            }

            // vector<BYTE> をまとめて文字列変換
            std::string rawResponse(reinterpret_cast<const char*>(responseData.data()), responseData.size());
            if (charset == L"utf-8" || charset == L"utf8") {
                response = StringConverter::StringToWString(rawResponse);
                response = DecodeUnicodeEscapes(response);
            }

            return true;
        }

        void QueryResponseHeaders() {
            responseHeaders.clear();

            DWORD size = 0;
            WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF,
                WINHTTP_HEADER_NAME_BY_INDEX,
                WINHTTP_NO_OUTPUT_BUFFER, &size,
                WINHTTP_NO_HEADER_INDEX);

            if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) return;

            std::vector<wchar_t> buffer(size / sizeof(wchar_t), 0);
            if (WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF,
                WINHTTP_HEADER_NAME_BY_INDEX,
                buffer.data(), &size,
                WINHTTP_NO_HEADER_INDEX)) {
                std::wistringstream stream(buffer.data());
                std::wstring line;
                while (std::getline(stream, line)) {
                    size_t pos = line.find(L": ");
                    if (pos != std::wstring::npos) {
                        responseHeaders[line.substr(0, pos)] = line.substr(pos + 2);
                    }
                }
            }
        }
    };

} // namespace System::Net

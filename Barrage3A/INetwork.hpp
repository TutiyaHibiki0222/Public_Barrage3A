/*
    INetwork.hpp

    :class
        - INetwork

    作成日         : 2025/04/02
    最終変更日     : 2025/04/04
*/
#pragma once

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <queue>
#include <mutex>

#pragma comment(lib, "ws2_32.lib")

namespace System::Net::Utilite {

    class INetwork {
    public:
        virtual ~INetwork() {
            if (sock != -1) {
                closesocket(sock);
            }
            WSACleanup(); // Windows の場合のみ
        }

        virtual bool Start(const std::string& host, int port) = 0;   // サーバー側の開始処理
        virtual void Stop() = 0;    // サーバー側の停止処理
        virtual void Send(const std::string& msg) = 0; // メッセージ送信

    protected:
        INetwork() : sock(-1) {}

        bool Connect(const std::string& host, int port) {
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                std::cerr << "WSAStartup failed!" << std::endl;
                return false;
            }

            sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock == -1) {
                std::cerr << "Failed to create socket!" << std::endl;
                return false;
            }

            sockaddr_in serverAddr{};
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(port);
            if (inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr) <= 0) {
                std::cerr << "Invalid address or address not supported!" << std::endl;
                return false;
            }

            if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
                std::cerr << "Connection failed!" << std::endl;
                return false;
            }

            return true;
        }

    protected:
        int sock;
        std::thread recvThread;
        std::queue<std::string> recvQueue;
        std::mutex queueMutex;
        bool running = false;
    };
}


/*
    NetworkServer .hpp

    :class
        - NetworkServer -> INetwork

    作成日         : 2025/04/02
    最終変更日     : 2025/04/04
*/
#pragma once

#include "INetwork.hpp"

using namespace System::Net::Utilite;

#include <unordered_map>
#include <iostream>
#include <thread>
#include <future>
#include <vector>
#include <mutex>

namespace System::Net {

    class NetworkServer : public INetwork {
    private:
        int port;
        std::vector<int> clients;

    public:
        NetworkServer() : port(0) {}

        bool Start(const std::string& host, int port) override {
            this->port = port;

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

            sockaddr_in serverAddr;
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(port);
            serverAddr.sin_addr.s_addr = INADDR_ANY;

            if (bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
                std::cerr << "Bind failed!" << std::endl;
                return false;
            }

            if (listen(sock, 5) == -1) {
                std::cerr << "Listen failed!" << std::endl;
                return false;
            }

            std::cout << "Server started on port " << port << "..." << std::endl;

            running = true;
            recvThread = std::thread(&NetworkServer::AcceptConnections, this);
            return true;
        }

        void Stop() override {
            running = false;
            if (recvThread.joinable()) {
                recvThread.join();
            }
            closesocket(sock);
        }

        void Send(const std::string& msg) override {
            // サーバーからすべてのクライアントに送信する例
            for (int clientSock : clients) {
                send(clientSock, msg.c_str(), msg.length(), 0);
            }
        }

    private:
        void AcceptConnections() {
            while (running) {
                sockaddr_in clientAddr;
                int clientLen = sizeof(clientAddr);
                int clientSock = accept(sock, (struct sockaddr*)&clientAddr, &clientLen);
                if (clientSock != -1) {
                    std::cout << "Client connected!" << std::endl;
                    clients.push_back(clientSock);
                    std::thread(&NetworkServer::HandleClient, this, clientSock).detach();
                }
            }
        }

        void HandleClient(int clientSock) {
            char buffer[1024];
            while (true) {
                int bytesReceived = recv(clientSock, buffer, sizeof(buffer), 0);
                if (bytesReceived <= 0) {
                    std::cout << "Client disconnected!" << std::endl;
                    closesocket(clientSock);
                    break;
                }
                buffer[bytesReceived] = '\0';
                std::cout << "Received: " << buffer << std::endl;
            }
        }

    };
}
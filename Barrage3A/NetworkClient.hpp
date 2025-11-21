/*
    NetworkClient.hpp

    :class
        - NetworkClient -> INetwork

    ì¬“ú         : 2025/04/02
    ÅI•ÏX“ú     : 2025/04/04
*/
#pragma once
#include <winsock2.h>
#include <iostream>

#include "INetwork.hpp"

using namespace System::Net::Utilite;

namespace System::Net {

    class NetworkClient : public INetwork {
    public:
        bool Start(const std::string& host, int port) override {
            if (!Connect(host, port)) {
                return false;
            }

            std::cout << "Connected to server!" << std::endl;
            running = true;
            recvThread = std::thread(&NetworkClient::ReceiveMessages, this);  // ”ñ“¯Šú‚ÅóM
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
            send(sock, msg.c_str(), msg.length(), 0);
        }

    private:
        void ReceiveMessages() {
            char buffer[1024];
            while (running) {
                int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
                if (bytesReceived > 0) {
                    buffer[bytesReceived] = '\0';
                    std::cout << "Received: " << buffer << std::endl;
                }
                else if (bytesReceived == 0) {
                    std::cout << "Server disconnected!" << std::endl;
                    break;
                }
                else {
                    std::cerr << "Error receiving message!" << std::endl;
                    break;
                }
            }
        }
    };

}


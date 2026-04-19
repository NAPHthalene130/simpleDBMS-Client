#include "NetworkManager.h"

#include <iostream>

#include <asio/connect.hpp>

NetworkManager::NetworkManager(MainWindow *mainWindow)
    : mainWindow(mainWindow),
      netReceiver(new NetReceiver(mainWindow)),
      netSender(new NetSender(mainWindow))
{
}

NetworkManager::~NetworkManager()
{
    stop();
    delete netReceiver;
    delete netSender;
    netReceiver = nullptr;
    netSender = nullptr;
}

bool NetworkManager::start(const std::string &serverAddress, unsigned short serverPort)
{
    if (!connectToServer(serverAddress, serverPort)) {
        return false;
    }

    if (netReceiver != nullptr) {
        netReceiver->start();
    }
    return true;
}

void NetworkManager::stop()
{
    if (netReceiver != nullptr) {
        netReceiver->stop();
    }

    if (socket != nullptr && socket->is_open()) {
        std::error_code errorCode;
        socket->shutdown(asio::ip::tcp::socket::shutdown_both, errorCode);
        socket->close(errorCode);
    }

    socket.reset();
    ioContext.reset();
}

bool NetworkManager::connectToServer(const std::string &serverAddress, unsigned short serverPort)
{
    try {
        if (socket != nullptr && socket->is_open()) {
            return true;
        }

        ioContext = std::make_unique<asio::io_context>();
        socket = std::make_shared<asio::ip::tcp::socket>(*ioContext);
        asio::ip::tcp::resolver resolver(*ioContext);
        const auto endpoints = resolver.resolve(serverAddress, std::to_string(serverPort));
        asio::connect(*socket, endpoints);
        return true;
    } catch (const std::exception &exception) {
        std::cerr << "NetworkManager::connectToServer failed: " << exception.what() << std::endl;
        socket.reset();
        ioContext.reset();
        return false;
    }
}

void NetworkManager::disconnected(std::shared_ptr<asio::ip::tcp::socket> serverSocket)
{
    // TODO: Handle reconnect and UI notification when server connection is lost.
    static_cast<void>(serverSocket);
}

NetReceiver *NetworkManager::getNetReceiver()
{
    return netReceiver;
}

NetSender *NetworkManager::getNetSender()
{
    return netSender;
}

std::shared_ptr<asio::ip::tcp::socket> NetworkManager::getSocket() const
{
    return socket;
}

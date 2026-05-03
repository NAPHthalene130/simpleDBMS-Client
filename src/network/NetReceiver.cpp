#include "NetReceiver.h"

#include <array>

#include <asio/read.hpp>
#include <QMetaObject>
#include <QString>

#include "mainwindow.h"
#include "NetworkManager.h"
#include "models/network/NetworkTransferData.h"

namespace {
QString buildDisplayMessage(const NetworkTransferData &networkTransferData)
{
    if (!networkTransferData.getMessage().empty()) {
        return QString::fromStdString(networkTransferData.getMessage());
    }

    if (!networkTransferData.getSql().empty()) {
        return QString::fromStdString(networkTransferData.getSql());
    }

    return QString::fromStdString(networkTransferData.toJson());
}
}

NetReceiver::NetReceiver(MainWindow *mainWindow)
    : mainWindow(mainWindow),
      isRunning(false)
{
}

NetReceiver::~NetReceiver()
{
    stop();
}

void NetReceiver::start()
{
    if (isRunning.exchange(true)) {
        return;
    }

    serviceThread = std::thread(&NetReceiver::runService, this);
}

void NetReceiver::stop()
{
    if (!isRunning.exchange(false)) {
        return;
    }

    if (mainWindow != nullptr) {
        NetworkManager *networkManager = mainWindow->getNetworkManager();
        std::shared_ptr<asio::ip::tcp::socket> serverSocket =
            networkManager != nullptr ? networkManager->getSocket() : nullptr;
        if (serverSocket != nullptr && serverSocket->is_open()) {
            std::error_code errorCode;
            serverSocket->shutdown(asio::ip::tcp::socket::shutdown_both, errorCode);
        }
    }

    if (serviceThread.joinable()) {
        serviceThread.join();
    }
}

void NetReceiver::processMsg(const NetworkTransferData &networkTransferData)
{
    {
        std::lock_guard<std::mutex> lock(messageMutex);
        lastReceivedMessage = networkTransferData.toJson();
    }

    const QString messageText = buildDisplayMessage(networkTransferData);

    if (mainWindow != nullptr) {
        QMetaObject::invokeMethod(mainWindow,
                                  [window = mainWindow, messageText]() {
                                      window->setProperty("lastReceivedMessage", messageText);
                                  },
                                  Qt::QueuedConnection);
    }
}

std::string NetReceiver::getLastReceivedMessage() const
{
    std::lock_guard<std::mutex> lock(messageMutex);
    return lastReceivedMessage;
}

void NetReceiver::runService()
{
    if (mainWindow == nullptr) {
        return;
    }

    NetworkManager *networkManager = mainWindow->getNetworkManager();
    if (networkManager == nullptr) {
        return;
    }

    handleServerSession(networkManager->getSocket());
}

void NetReceiver::handleServerSession(std::shared_ptr<asio::ip::tcp::socket> serverSocket)
{
    if (serverSocket == nullptr || !serverSocket->is_open()) {
        return;
    }

    while (isRunning.load()) {
        std::error_code errorCode;
        std::array<unsigned char, 4> lengthHeader{};
        asio::read(*serverSocket, asio::buffer(lengthHeader), errorCode);

        if (errorCode == asio::error::eof) {
            break;
        }

        if (errorCode) {
            break;
        }

        const std::uint32_t messageLength = parseLengthHeader(lengthHeader);
        std::string msg(messageLength, '\0');
        asio::read(*serverSocket, asio::buffer(msg.data(), msg.size()), errorCode);

        if (errorCode == asio::error::eof) {
            break;
        }

        if (errorCode) {
            break;
        }

        try {
            const NetworkTransferData networkTransferData = NetworkTransferData::fromJson(msg);
            processMsg(networkTransferData);
        } catch (const std::exception &) {
            {
                std::lock_guard<std::mutex> lock(messageMutex);
                lastReceivedMessage = msg;
            }

            if (mainWindow != nullptr) {
                const QString messageText = QString::fromStdString(msg);
                QMetaObject::invokeMethod(mainWindow,
                                          [window = mainWindow, messageText]() {
                                              window->setProperty("lastReceivedMessage", messageText);
                                          },
                                          Qt::QueuedConnection);
            }
        }
    }

    if (mainWindow != nullptr) {
        NetworkManager *networkManager = mainWindow->getNetworkManager();
        if (networkManager != nullptr) {
            networkManager->disconnected(serverSocket);
        }
    }
}

std::uint32_t NetReceiver::parseLengthHeader(const std::array<unsigned char, 4> &lengthHeader) const
{
    return (static_cast<std::uint32_t>(lengthHeader[0]) << 24U)
           | (static_cast<std::uint32_t>(lengthHeader[1]) << 16U)
           | (static_cast<std::uint32_t>(lengthHeader[2]) << 8U)
           | static_cast<std::uint32_t>(lengthHeader[3]);
}

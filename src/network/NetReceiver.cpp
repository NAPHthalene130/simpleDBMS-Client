#include "NetReceiver.h"

#include <array>

#include <asio/read.hpp>
#include <QMetaObject>
#include <QString>

#include "mainwindow.h"
#include "NetworkManager.h"
#include "models/network/NetworkTransferData.h"
#include "ui/OpePanelWidget.h"
#include "ui/opePanel/TableWidget.h"
#include "ui/opePanel/TerminalWidget.h"

namespace {
QString buildCommandDisplay(const NetworkTransferData &networkTransferData)
{
    if (!networkTransferData.getSql().empty()) {
        return QString::fromStdString(networkTransferData.getSql());
    }
    return QString();
}

QString buildMessageText(const NetworkTransferData &networkTransferData)
{
    if (!networkTransferData.getMessage().empty()) {
        return QString::fromStdString(networkTransferData.getMessage());
    }
    return QString::fromStdString(networkTransferData.toJson());
}

QString buildExecResultText(const NetworkTransferData &networkTransferData)
{
    QString text = buildMessageText(networkTransferData);
    if (networkTransferData.getSuccess()) {
        text += QString(" (Affected rows: %1)").arg(networkTransferData.getAffectedRows());
    }
    return text;
}

QString buildQueryResultText(const NetworkTransferData &networkTransferData)
{
    const auto &columns = networkTransferData.getColumns();
    const auto &rows = networkTransferData.getRows();

    if (columns.empty() && rows.empty()) {
        return buildMessageText(networkTransferData);
    }

    QString result;

    if (!columns.empty()) {
        QStringList columnList;
        for (const auto &col : columns) {
            columnList.append(QString::fromStdString(col));
        }
        result += columnList.join(" | ") + "\n";
        result += QString("-").repeated(result.length()) + "\n";
    }

    for (const auto &row : rows) {
        QStringList rowList;
        for (const auto &cell : row) {
            rowList.append(QString::fromStdString(cell));
        }
        result += rowList.join(" | ") + "\n";
    }

    if (!networkTransferData.getMessage().empty()) {
        result += "\n" + QString::fromStdString(networkTransferData.getMessage());
    }

    return result.trimmed();
}
} // namespace

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

    auto *terminalWidget = getTerminalWidget();

    if (networkTransferData.getType() == NetworkTransferData::SQL_EXEC_RESPONSE) {
        const QString msg = buildExecResultText(networkTransferData);
        if (networkTransferData.getSuccess()) {
            invokeTerminalAppend(
                terminalWidget,
                [msg](TerminalWidget *tw) { tw->appendMessage(msg); });
        } else {
            invokeTerminalAppend(
                terminalWidget,
                [msg](TerminalWidget *tw) { tw->appendError(msg); });
        }
        return;
    }

    if (networkTransferData.getType() == NetworkTransferData::SQL_QUERY_RESPONSE) {
        const QString text = buildQueryResultText(networkTransferData);
        if (networkTransferData.getSuccess()) {
            const std::vector<std::string> columns = networkTransferData.getColumns();
            const std::vector<std::vector<std::string>> rows = networkTransferData.getRows();
            QMetaObject::invokeMethod(
                mainWindow,
                [window = mainWindow, terminalWidget, text, columns, rows]() {
                    if (window == nullptr) {
                        return;
                    }

                    OpePanelWidget *opePanelWidget = window->getOpePanelWidget();
                    if (opePanelWidget != nullptr) {
                        TableWidget *tableWidget = opePanelWidget->getTableWidget();
                        if (tableWidget != nullptr) {
                            tableWidget->setQueryResult(columns, rows);
                        }
                    }

                    if (terminalWidget != nullptr) {
                        terminalWidget->appendMessage(text);
                    }
                },
                Qt::QueuedConnection);
        } else {
            invokeTerminalAppend(
                terminalWidget,
                [text](TerminalWidget *tw) { tw->appendError(text); });
        }
        return;
    }

    if (networkTransferData.getType() == NetworkTransferData::LOGIN_RESPONSE) {
        // TODO: 处理登录响应
        storeAsProperty(buildMessageText(networkTransferData));
        return;
    }

    if (networkTransferData.getType() == NetworkTransferData::VERIFY_RESPONSE) {
        // TODO: 处理连接验证响应
        storeAsProperty(buildMessageText(networkTransferData));
        return;
    }

    if (networkTransferData.getType() == NetworkTransferData::USE_DATABASE_RESPONSE) {
        // TODO: 处理数据库切换响应
        storeAsProperty(buildMessageText(networkTransferData));
        return;
    }

    if (networkTransferData.getType() == NetworkTransferData::DIRECTORY_RESPONSE) {
        // TODO: 处理数据库目录响应
        storeAsProperty(buildMessageText(networkTransferData));
        return;
    }

    if (networkTransferData.getType() == NetworkTransferData::ERROR_RESPONSE) {
        const QString msg = buildMessageText(networkTransferData);
        invokeTerminalAppend(
            terminalWidget,
            [msg](TerminalWidget *tw) { tw->appendError(msg); });
        return;
    }

    storeAsProperty(buildMessageText(networkTransferData));
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

TerminalWidget *NetReceiver::getTerminalWidget() const
{
    if (mainWindow == nullptr) {
        return nullptr;
    }
    auto *opePanel = mainWindow->getOpePanelWidget();
    if (opePanel == nullptr) {
        return nullptr;
    }
    return opePanel->getTerminalWidget();
}

void NetReceiver::invokeTerminalAppend(TerminalWidget *terminalWidget,
                                       const std::function<void(TerminalWidget *)> &action)
{
    if (mainWindow == nullptr) {
        return;
    }

    QMetaObject::invokeMethod(
        mainWindow,
        [action, terminalWidget]() {
            if (terminalWidget != nullptr) {
                action(terminalWidget);
            }
        },
        Qt::QueuedConnection);
}

void NetReceiver::storeAsProperty(const QString &messageText)
{
    if (mainWindow != nullptr) {
        QMetaObject::invokeMethod(mainWindow,
                                  [window = mainWindow, messageText]() {
                                      window->setProperty("lastReceivedMessage", messageText);
                                  },
                                  Qt::QueuedConnection);
    }
}

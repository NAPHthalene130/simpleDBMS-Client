#include "NetReceiver.h"

#include <algorithm>
#include <array>
#include <vector>

#include <asio/read.hpp>
#include <QMetaObject>
#include <QString>
#include <QStringList>

#include "mainwindow.h"
#include "NetworkManager.h"
#include "models/network/NetworkTransferData.h"
#include "ui/OpePanelWidget.h"
#include "ui/opePanel/DirectoryWidget.h"
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

    std::size_t columnCount = columns.size();
    for (const auto &row : rows) {
        columnCount = std::max(columnCount, row.size());
    }

    if (columnCount == 0) {
        return buildMessageText(networkTransferData);
    }

    std::vector<int> columnWidths(columnCount, 0);
    QStringList headerCells;
    if (!columns.empty()) {
        headerCells.reserve(static_cast<qsizetype>(columnCount));
        for (std::size_t i = 0; i < columnCount; ++i) {
            const QString headerCell =
                i < columns.size() ? QString::fromStdString(columns[i]) : QString();
            headerCells.append(headerCell);
            columnWidths[i] = std::max(columnWidths[i], static_cast<int>(headerCell.size()));
        }
    }

    for (const auto &row : rows) {
        for (std::size_t i = 0; i < columnCount; ++i) {
            const QString cell = i < row.size() ? QString::fromStdString(row[i]) : QString();
            columnWidths[i] = std::max(columnWidths[i], static_cast<int>(cell.size()));
        }
    }

    auto formatCells = [&columnWidths](const QStringList &cells) {
        QStringList paddedCells;
        paddedCells.reserve(static_cast<qsizetype>(columnWidths.size()));
        for (std::size_t i = 0; i < columnWidths.size(); ++i) {
            const QString cell = i < static_cast<std::size_t>(cells.size()) ? cells[static_cast<qsizetype>(i)] : QString();
            paddedCells.append(cell.leftJustified(columnWidths[i], ' '));
        }
        return paddedCells.join(" | ");
    };

    auto buildSeparator = [&columnWidths]() {
        QStringList segments;
        segments.reserve(static_cast<qsizetype>(columnWidths.size()));
        for (const int width : columnWidths) {
            segments.append(QString("-").repeated(std::max(width, 1)));
        }
        return segments.join("-+-");
    };

    QStringList lines;
    if (!columns.empty()) {
        lines.append(formatCells(headerCells));
        lines.append(buildSeparator());
    }

    for (const auto &row : rows) {
        QStringList rowCells;
        rowCells.reserve(static_cast<qsizetype>(columnCount));
        for (std::size_t i = 0; i < columnCount; ++i) {
            rowCells.append(i < row.size() ? QString::fromStdString(row[i]) : QString());
        }
        lines.append(formatCells(rowCells));
    }

    if (!networkTransferData.getMessage().empty()) {
        if (!lines.isEmpty()) {
            lines.append(QString());
        }
        lines.append(QString::fromStdString(networkTransferData.getMessage()));
    }

    return lines.join("\n").trimmed();
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
        /**
         * @brief 处理服务端返回的目录结构数据，刷新左侧数据库目录树
         * @author NAPH130
         * @details
         * 1. 解析 NetworkTransferData 中的 databases 字段获取完整 库-表-字段 层级结构。
         * 2. 通过 Qt 线程安全机制投递到主线程，调用 DirectoryWidget::refreshFromServer 刷新界面。
         */
        const auto databases = networkTransferData.getDatabases();
        QMetaObject::invokeMethod(
            mainWindow,
            [window = mainWindow, databases]() {
                if (window == nullptr) {
                    return;
                }
                OpePanelWidget *opePanelWidget = window->getOpePanelWidget();
                if (opePanelWidget != nullptr) {
                    DirectoryWidget *directoryWidget = opePanelWidget->getDirectoryWidget();
                    if (directoryWidget != nullptr) {
                        directoryWidget->refreshFromServer(databases);
                    }
                }
            },
            Qt::QueuedConnection);
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

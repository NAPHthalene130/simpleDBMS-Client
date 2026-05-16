#include "NetReceiver.h"

#include <algorithm>
#include <array>
#include <vector>

#include <asio/read.hpp>
#include <QMetaObject>
#include <QRegularExpression>
#include <QSettings>
#include <QStackedWidget>
#include <QString>
#include <QStringList>

#include "mainwindow.h"
#include "NetworkManager.h"
#include "models/network/NetworkTransferData.h"
#include "ui/AuthWidget.h"
#include "ui/OpePanelWidget.h"
#include "ui/TopNavigationWidget.h"
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
        const std::uint64_t serverVersion = networkTransferData.getDbVersion();
        const std::string dbName = networkTransferData.getDbName();
        if (networkTransferData.getSuccess()) {
            const QString dbNameStr = QString::fromStdString(dbName);
            if (!dbName.empty()) {
                QMetaObject::invokeMethod(mainWindow,
                    [window = mainWindow, dbNameStr]() {
                        if (window != nullptr) {
                            window->setProperty("currentDatabase", dbNameStr);
                            QSettings settings(QStringLiteral("simpleDBMS"), QStringLiteral("simpleDBMS-Client"));
                            if (settings.value("habit/rememberDatabase", false).toBool()) {
                                settings.setValue("session/lastDatabase", dbNameStr);
                                settings.sync();
                            }
                        }
                    },
                    Qt::QueuedConnection);
            }
            if (serverVersion > 0 && !dbNameStr.isEmpty()) {
                QMetaObject::invokeMethod(mainWindow,
                    [window = mainWindow, dbNameStr, serverVersion]() {
                        if (window == nullptr) return;
                        OpePanelWidget *ope = window->getOpePanelWidget();
                        if (ope && ope->getDirectoryWidget()) {
                            ope->getDirectoryWidget()->setDbVersion(dbNameStr, serverVersion);
                        }
                    },
                    Qt::QueuedConnection);
            }
            invokeTerminalAppend(
                terminalWidget,
                [msg](TerminalWidget *tw) { tw->appendMessage(msg); });

            // 执行 DDL 后自动刷新目录
            const QString lastSql = mainWindow->property("lastExecutedSql").toString();
            if (!lastSql.isEmpty()) {
                const QRegularExpression ddlRegex("\\b(CREATE|DROP|ALTER)\\b",
                                                  QRegularExpression::CaseInsensitiveOption);
                if (ddlRegex.match(lastSql).hasMatch()) {
                    QMetaObject::invokeMethod(mainWindow,
                                              [window = mainWindow]() {
                                                  if (window != nullptr) {
                                                      window->sendDirectoryRequest();
                                                  }
                                              },
                                              Qt::QueuedConnection);
                }
            }
        } else {
            // 版本号不一致时的强制刷新
            // 作者：NAPH130
            if (serverVersion > 0 && !dbName.empty()) {
                const QString dbNameStr = QString::fromStdString(dbName);
                QMetaObject::invokeMethod(mainWindow,
                    [window = mainWindow, dbNameStr, serverVersion]() {
                        if (window == nullptr) return;
                        OpePanelWidget *ope = window->getOpePanelWidget();
                        if (ope && ope->getDirectoryWidget()) {
                            ope->getDirectoryWidget()->setDbVersion(dbNameStr, serverVersion);
                        }
                        if (ope && ope->getTableWidget()) {
                            ope->getTableWidget()->clearTable();
                        }
                        window->sendDirectoryRequest();
                    },
                    Qt::QueuedConnection);
            }
            invokeTerminalAppend(
                terminalWidget,
                [msg](TerminalWidget *tw) { tw->appendError(msg); });
        }
        return;
    }

    if (networkTransferData.getType() == NetworkTransferData::SQL_QUERY_RESPONSE) {
        const QString text = buildQueryResultText(networkTransferData);
        const std::uint64_t serverVersion = networkTransferData.getDbVersion();
        const std::string dbName = networkTransferData.getDbName();
        const QString dbNameStr = QString::fromStdString(dbName);
        if (networkTransferData.getSuccess()) {
            const std::vector<std::string> columns = networkTransferData.getColumns();
            const std::vector<std::vector<std::string>> rows = networkTransferData.getRows();
            const std::string responseDbName = networkTransferData.getDbName();
            QMetaObject::invokeMethod(
                mainWindow,
                [window = mainWindow, terminalWidget, text, columns, rows, responseDbName, dbNameStr, serverVersion]() {
                    if (window == nullptr) {
                        return;
                    }

                    const QString tableName = window->property("lastQueryTable").toString();
                    const QString dbName = responseDbName.empty()
                        ? window->property("currentDatabase").toString()
                        : QString::fromStdString(responseDbName);

                    OpePanelWidget *opePanelWidget = window->getOpePanelWidget();
                    if (opePanelWidget != nullptr) {
                        TableWidget *tableWidget = opePanelWidget->getTableWidget();
                        QStackedWidget *displayStack = opePanelWidget->getMainDisplayStackedWidget();
                        if (tableWidget != nullptr) {
                            tableWidget->setQueryResult(columns, rows,
                                                        tableName.toStdString(),
                                                        dbName.toStdString(),
                                                        opePanelWidget->getDirectoryData());
                        }
                        const QSettings settings(QStringLiteral("simpleDBMS"), QStringLiteral("simpleDBMS-Client"));
                        if (settings.value("habit/autoSwitchToTable", true).toBool()
                            && displayStack != nullptr && tableWidget != nullptr) {
                            displayStack->setCurrentWidget(tableWidget);
                        }
                    }

                    if (terminalWidget != nullptr) {
                        terminalWidget->appendMessage(text);
                    }

                    if (serverVersion > 0 && !dbNameStr.isEmpty() && opePanelWidget != nullptr
                        && opePanelWidget->getDirectoryWidget() != nullptr) {
                        opePanelWidget->getDirectoryWidget()->setDbVersion(dbNameStr, serverVersion);
                    }
                },
                Qt::QueuedConnection);
        } else {
            // 查询失败的版本不一致处理
            // 作者：NAPH130
            if (serverVersion > 0 && !dbName.empty()) {
                QMetaObject::invokeMethod(mainWindow,
                    [window = mainWindow, dbNameStr, serverVersion]() {
                        if (window == nullptr) return;
                        OpePanelWidget *ope = window->getOpePanelWidget();
                        if (ope && ope->getDirectoryWidget()) {
                            ope->getDirectoryWidget()->setDbVersion(dbNameStr, serverVersion);
                        }
                        if (ope && ope->getTableWidget()) {
                            ope->getTableWidget()->clearTable();
                        }
                        window->sendDirectoryRequest();
                    },
                    Qt::QueuedConnection);
            }
            invokeTerminalAppend(
                terminalWidget,
                [text](TerminalWidget *tw) { tw->appendError(text); });
        }
        return;
    }

    if (networkTransferData.getType() == NetworkTransferData::LOGIN_RESPONSE) {
        /**
         * @brief 处理登录响应
         * @author NAPH130
         * @details 成功则切换至工作区并设置用户名；失败则显示错误提示
         */
        const bool success = networkTransferData.getSuccess();
        const QString message = QString::fromStdString(networkTransferData.getMessage());
        const QString userId = QString::fromStdString(networkTransferData.getId());
        QMetaObject::invokeMethod(
            mainWindow,
            [window = mainWindow, success, message, userId]() {
                if (window == nullptr) {
                    return;
                }
                if (success) {
                    // 登录成功，设置用户名并切换至工作区
                    // 作者：NAPH130
                    window->getTopNavigationWidget()->setUserName(userId);
                    window->showWorkspacePage();
                } else {
                    // 登录失败，在登录页显示错误消息
                    // 作者：NAPH130
                    AuthWidget *authWidget = window->getAuthWidget();
                    if (authWidget != nullptr) {
                        authWidget->setConnectionStatus(message);
                    }
                }
            },
            Qt::QueuedConnection);
        return;
    }

    if (networkTransferData.getType() == NetworkTransferData::VERIFY_RESPONSE) {
        /**
         * @brief 处理连接验证响应
         * @author NAPH130
         * @details 在登录页显示验证结果消息
         */
        const bool success = networkTransferData.getSuccess();
        const QString message = QString::fromStdString(networkTransferData.getMessage());
        QMetaObject::invokeMethod(
            mainWindow,
            [window = mainWindow, success, message]() {
                if (window == nullptr) {
                    return;
                }
                AuthWidget *authWidget = window->getAuthWidget();
                if (authWidget != nullptr) {
                    QString displayMsg = success
                        ? QStringLiteral("验证成功 - ") + message
                        : QStringLiteral("验证失败 - ") + message;
                    authWidget->setConnectionStatus(displayMsg);
                }
            },
            Qt::QueuedConnection);
        return;
    }

    if (networkTransferData.getType() == NetworkTransferData::DB_VERSION_RESPONSE) {
        /**
         * @brief 处理服务端返回的数据库版本号数据
         * @details 解析 databases 字段中每个 DatabaseNode 的 dbVersion，更新 DirectoryWidget 本地缓存。
         * @author NAPH130
         */
        const auto databases = networkTransferData.getDatabases();
        QMetaObject::invokeMethod(mainWindow,
            [window = mainWindow, databases]() {
                if (window == nullptr) return;
                OpePanelWidget *ope = window->getOpePanelWidget();
                if (ope && ope->getDirectoryWidget()) {
                    for (const auto &db : databases) {
                        ope->getDirectoryWidget()->setDbVersion(
                            QString::fromStdString(db.getName()), db.getDbVersion());
                    }
                }
            },
            Qt::QueuedConnection);
        return;
    }

    if (networkTransferData.getType() == NetworkTransferData::USE_DATABASE_RESPONSE) {
        if (networkTransferData.getSuccess() && !networkTransferData.getDbName().empty()) {
            const QString dbName = QString::fromStdString(networkTransferData.getDbName());
            QMetaObject::invokeMethod(mainWindow,
                                      [window = mainWindow, dbName]() {
                                          if (window != nullptr) {
                                              window->setProperty("currentDatabase", dbName);
                                          }
                                      },
                                      Qt::QueuedConnection);
        }
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
                    opePanelWidget->setDirectoryData(databases);
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

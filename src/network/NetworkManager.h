#pragma once

#include <memory>
#include <string>

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>

#include "NetReceiver.h"
#include "NetSender.h"

class MainWindow;

/**
 * @class NetworkManager
 * @brief 客户端网络管理器
 * @details 统一管理客户端套接字、接收器与发送器，作为客户端网络模块的中控入口。
 * @author NAPH130
 */
class NetworkManager
{
public:
    static constexpr unsigned short DEFAULT_SERVER_PORT = 10086;

    /**
     * @brief 构造函数
     * @author NAPH130
     * @param mainWindow 主窗口对象
     */
    explicit NetworkManager(MainWindow *mainWindow);

    /**
     * @brief 析构函数
     * @author NAPH130
     */
    ~NetworkManager();

    /**
     * @brief 启动网络模块（需先调用 connectToServer）
     * @author NAPH130
     * @details 仅启动 NetReceiver 监听线程，不再自动连接。
     */
    void start();

    /**
     * @brief 连接到服务端并启动接收线程
     * @author NAPH130
     * @param serverAddress 服务端地址
     * @param serverPort 服务端端口
     * @return 是否连接成功
     */
    bool connectAndStart(const std::string &serverAddress = "127.0.0.1",
                         unsigned short serverPort = DEFAULT_SERVER_PORT);

    /**
     * @brief 停止网络模块
     * @author NAPH130
     */
    void stop();

    /**
     * @brief 连接到服务端（仅建立 TCP 连接）
     * @author NAPH130
     * @param serverAddress 服务端地址
     * @param serverPort 服务端端口
     * @return 是否连接成功
     */
    bool connectToServer(const std::string &serverAddress, unsigned short serverPort);

    /**
     * @brief 断开与服务端的连接
     * @author NAPH130
     */
    void disconnectFromServer();

    /**
     * @brief 处理连接断开事件
     * @author NAPH130
     * @param serverSocket 丢失连接的套接字
     */
    void disconnected(std::shared_ptr<asio::ip::tcp::socket> serverSocket);

    /**
     * @brief 获取网络接收器
     * @author NAPH130
     * @return 网络接收器指针
     */
    NetReceiver *getNetReceiver();

    /**
     * @brief 获取网络发送器
     * @author NAPH130
     * @return 网络发送器指针
     */
    NetSender *getNetSender();

    /**
     * @brief 获取当前服务端套接字
     * @author NAPH130
     * @return 当前服务端套接字
     */
    std::shared_ptr<asio::ip::tcp::socket> getSocket() const;

private:
    MainWindow *mainWindow;
    std::unique_ptr<asio::io_context> ioContext;
    std::shared_ptr<asio::ip::tcp::socket> socket;
    NetReceiver *netReceiver;
    NetSender *netSender;
};

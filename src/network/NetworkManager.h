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
 * @author NAPH1301
 */
class NetworkManager
{
public:
    static constexpr unsigned short DEFAULT_SERVER_PORT = 10086;

    /**
     * @brief 构造函数
     * @author NAPH1301
     * @param mainWindow 主窗口对象
     */
    explicit NetworkManager(MainWindow *mainWindow);

    /**
     * @brief 析构函数
     * @author NAPH1301
     */
    ~NetworkManager();

    /**
     * @brief 启动网络模块
     * @author NAPH1301
     * @param serverAddress 服务端地址
     * @param serverPort 服务端端口
     * @return 是否启动成功
     */
    bool start(const std::string &serverAddress = "127.0.0.1", unsigned short serverPort = DEFAULT_SERVER_PORT);

    /**
     * @brief 停止网络模块
     * @author NAPH1301
     */
    void stop();

    /**
     * @brief 连接到服务端
     * @author NAPH1301
     * @param serverAddress 服务端地址
     * @param serverPort 服务端端口
     * @return 是否连接成功
     */
    bool connectToServer(const std::string &serverAddress, unsigned short serverPort);

    /**
     * @brief 处理连接断开事件
     * @author NAPH1301
     * @param serverSocket 丢失连接的套接字
     */
    void disconnected(std::shared_ptr<asio::ip::tcp::socket> serverSocket);

    /**
     * @brief 获取网络接收器
     * @author NAPH1301
     * @return 网络接收器指针
     */
    NetReceiver *getNetReceiver();

    /**
     * @brief 获取网络发送器
     * @author NAPH1301
     * @return 网络发送器指针
     */
    NetSender *getNetSender();

    /**
     * @brief 获取当前服务端套接字
     * @author NAPH1301
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

#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <string>

#include <asio/ip/tcp.hpp>

class MainWindow;

/**
 * @class NetSender
 * @brief 客户端网络发送服务类
 * @details 负责向服务端套接字发送长度前缀协议消息。
 * @author NAPH130
 */
class NetSender
{
public:
    /**
     * @brief 构造函数
     * @author NAPH130
     * @param mainWindow 主窗口指针，用于后续界面交互扩展
     */
    explicit NetSender(MainWindow *mainWindow);

    /**
     * @brief 析构函数
     * @author NAPH130
     */
    ~NetSender();

    /**
     * @brief 发送一条字符串消息
     * @author NAPH130
     * @param serverSocket 服务端套接字
     * @param msg 待发送的字符串内容
     */
    void send(std::shared_ptr<asio::ip::tcp::socket> serverSocket, const std::string &msg);

private:
    /**
     * @brief 构建 4 字节长度前缀
     * @author NAPH130
     * @param messageLength 消息长度
     * @return 长度前缀字节数组
     */
    std::array<unsigned char, 4> buildLengthHeader(std::uint32_t messageLength) const;

    MainWindow *mainWindow;
};

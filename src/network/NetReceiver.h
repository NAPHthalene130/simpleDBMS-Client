#pragma once

#include <array>
#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include <asio/ip/tcp.hpp>

class MainWindow;

/**
 * @class NetReceiver
 * @brief 客户端网络接收服务类
 * @details 负责从客户端维护的单个服务端套接字中按“4 字节长度 + 消息体”的协议读取消息。
 * @author NAPH1301
 */
class NetReceiver
{
public:
    /**
     * @brief 构造函数
     * @author NAPH1301
     * @param mainWindow 主窗口指针，用于后续与界面层联动
     */
    explicit NetReceiver(MainWindow *mainWindow);

    /**
     * @brief 析构函数
     * @author NAPH1301
     */
    ~NetReceiver();

    /**
     * @brief 启动接收服务
     * @author NAPH1301
     */
    void start();

    /**
     * @brief 停止接收服务
     * @author NAPH1301
     */
    void stop();

    /**
     * @brief 处理最终接收到的完整消息
     * @author NAPH1301
     * @param msg 最终处理后的接收字符串
     */
    void processMsg(const std::string &msg);

    /**
     * @brief 获取最近一次处理的消息内容
     * @author NAPH1301
     * @return 最近一次处理的消息
     */
    std::string getLastReceivedMessage() const;

private:
    /**
     * @brief 执行接收服务主循环
     * @author NAPH1301
     */
    void runService();

    /**
     * @brief 处理与服务端的单个会话
     * @author NAPH1301
     * @param serverSocket 服务端套接字
     */
    void handleServerSession(std::shared_ptr<asio::ip::tcp::socket> serverSocket);

    /**
     * @brief 解析 4 字节长度前缀
     * @author NAPH1301
     * @param lengthHeader 长度前缀字节数组
     * @return 消息体长度
     */
    std::uint32_t parseLengthHeader(const std::array<unsigned char, 4> &lengthHeader) const;

    MainWindow *mainWindow;
    std::atomic<bool> isRunning;
    std::thread serviceThread;
    mutable std::mutex messageMutex;
    std::string lastReceivedMessage;
};

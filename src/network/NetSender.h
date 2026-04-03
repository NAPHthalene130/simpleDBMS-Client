#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

class MainWindow;

/**
 * @class NetSender
 * @brief 客户端网络发送服务类
 * @details 负责在独立线程中处理待发送消息队列，并向目标地址建立短连接发送数据
 * @author NAPH130
 */
class NetSender
{
public:
    static constexpr unsigned short DEFAULT_TARGET_PORT = 10086;

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
     * @brief 启动发送服务
     * @author NAPH130
     */
    void start();

    /**
     * @brief 停止发送服务
     * @author NAPH130
     */
    void stop();

    /**
     * @brief 发送一条字符串消息
     * @author NAPH130
     * @param str 待发送的字符串内容
     */
    void send(const std::string &str);

    /**
     * @brief 提交一条待发送消息
     * @author NAPH130
     * @param message 待发送消息
     * @param targetAddress 目标地址
     * @param targetPort 目标端口
     */
    void sendMessage(const std::string &message,
                     const std::string &targetAddress = "127.0.0.1",
                     unsigned short targetPort = DEFAULT_TARGET_PORT);

private:
    /**
     * @brief 发送线程主循环
     * @author NAPH130
     */
    void processQueue();

    /**
     * @brief 真正执行网络发送
     * @author NAPH130
     * @param message 待发送消息
     * @param targetAddress 目标地址
     * @param targetPort 目标端口
     */
    void sendNow(const std::string &message, const std::string &targetAddress, unsigned short targetPort);

    struct PendingMessage {
        std::string message;
        std::string targetAddress;
        unsigned short targetPort;
    };

    MainWindow *mainWindow;
    std::atomic<bool> isRunning;
    std::thread serviceThread;
    std::mutex queueMutex;
    std::condition_variable conditionVariable;
    std::queue<PendingMessage> pendingMessages;
};

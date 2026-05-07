/**
 * @file SettingWidget.cpp
 * @author YuzhSong
 * @brief 设置页面组件实现文件
 * @details 仅实现设置页展示与退出登录请求信号，不包含业务认证逻辑
 * @module ui
 */

#include "SettingWidget.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

SettingWidget::SettingWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent)
    , mainWindow(mainWindow)
    , titleLabel(new QLabel(tr("设置"), this))
    , descriptionLabel(new QLabel(tr("可在此管理客户端偏好与账号状态。"), this))
    , logoutButton(new QPushButton(tr("退出登录"), this))
{
    initUI();
    initStyle();
    initConnections();
}

MainWindow *SettingWidget::getMainWindow() const
{
    return mainWindow;
}

void SettingWidget::onLogoutButtonClicked()
{
    // 作者：YuzhSong
    // 设置页仅声明“退出登录意图”，不直接执行页面跳转，保持与 MainWindow 的解耦。
    emit logoutRequested();
}

void SettingWidget::initUI()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(28, 24, 28, 24);
    layout->setSpacing(12);

    titleLabel->setObjectName("settingTitleLabel");
    descriptionLabel->setObjectName("settingDescriptionLabel");
    descriptionLabel->setWordWrap(true);

    logoutButton->setObjectName("logoutButton");
    logoutButton->setCursor(Qt::PointingHandCursor);
    logoutButton->setFixedHeight(38);
    logoutButton->setFixedWidth(140);

    layout->addWidget(titleLabel, 0, Qt::AlignLeft);
    layout->addWidget(descriptionLabel, 0, Qt::AlignLeft);
    layout->addSpacing(16);
    layout->addWidget(logoutButton, 0, Qt::AlignLeft);
    layout->addStretch(1);
}

void SettingWidget::initConnections()
{
    connect(logoutButton, &QPushButton::clicked, this, &SettingWidget::onLogoutButtonClicked);
}

void SettingWidget::initStyle()
{
    // 作者：YuzhSong
    // 设置页采用统一深灰风格，突出标题层级与退出按钮反馈，不改动登出业务流程。
    setStyleSheet(QString(
        "QWidget {"
        "    background-color: #1E1F22;"
        "    color: #F0F0F0;"
        "}"
        "QLabel#settingTitleLabel {"
        "    color: #F0F0F0;"
        "    font-size: 22px;"
        "    font-weight: 700;"
        "}"
        "QLabel#settingDescriptionLabel {"
        "    color: #A8A8A8;"
        "    font-size: 13px;"
        "}"
        "QPushButton#logoutButton {"
        "    background-color: #2B2D30;"
        "    color: #F0F0F0;"
        "    border: 1px solid #A14B4B;"
        "    border-radius: 6px;"
        "    padding: 6px 14px;"
        "}"
        "QPushButton#logoutButton:hover {"
        "    background-color: #4A2E2E;"
        "}"
        "QPushButton#logoutButton:pressed {"
        "    background-color: #5A3232;"
        "}"
    ));
}

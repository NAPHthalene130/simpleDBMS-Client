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
    setStyleSheet(QString(
        "QWidget {"
        "    background-color: #1E1E1E;"
        "    color: #D4D4D4;"
        "}"
        "QLabel#settingTitleLabel {"
        "    color: #E8E8E8;"
        "    font-size: 22px;"
        "    font-weight: 600;"
        "}"
        "QLabel#settingDescriptionLabel {"
        "    color: #AFAFAF;"
        "    font-size: 13px;"
        "}"
        "QPushButton#logoutButton {"
        "    background-color: #2D2D30;"
        "    color: #F2F2F2;"
        "    border: 1px solid #C75050;"
        "    border-radius: 6px;"
        "    padding: 6px 14px;"
        "}"
        "QPushButton#logoutButton:hover {"
        "    background-color: #3A2A2A;"
        "}"
        "QPushButton#logoutButton:pressed {"
        "    background-color: #4A2E2E;"
        "}"
    ));
}

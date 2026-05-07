/**
 * @file TopNavigationWidget.cpp
 * @author YuzhSong
 * @brief 顶部导航栏组件实现文件
 * @details 仅负责导航栏 UI 与请求信号，不包含具体页面切换逻辑
 * @module ui
 */

#include "TopNavigationWidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStyle>

TopNavigationWidget::TopNavigationWidget(QWidget *parent)
    : QWidget(parent)
    , currentPageType(PageType::None)
    , brandLabel(new QLabel(tr("simpleDBMS"), this))
    , workspaceButton(new QPushButton(tr("工作区"), this))
    , settingButton(new QPushButton(tr("设置"), this))
    , userNamePrefixLabel(new QLabel(tr("用户："), this))
    , userNameValueLabel(new QLabel(tr("Guest"), this))
{
    initUI();
    initStyle();
    initConnections();
    updateTabStyle();
}

void TopNavigationWidget::setCurrentPage(PageType pageType)
{
    currentPageType = pageType;
    updateTabStyle();
}

void TopNavigationWidget::setUserName(const QString &userName)
{
    // 作者：YuzhSong
    // 用户名为空时回退占位文本，避免导航栏右侧出现空白。
    userNameValueLabel->setText(userName.trimmed().isEmpty() ? tr("Guest") : userName.trimmed());
}

void TopNavigationWidget::onWorkspaceButtonClicked()
{
    emit workspaceRequested();
}

void TopNavigationWidget::onSettingButtonClicked()
{
    emit settingRequested();
}

void TopNavigationWidget::initUI()
{
    setFixedHeight(52);

    auto *layout = new QHBoxLayout(this);
    // 作者：YuzhSong
    // 仅优化顶部导航的视觉留白与组件间距，不改变导航按钮业务交互。
    layout->setContentsMargins(14, 8, 14, 8);
    layout->setSpacing(8);

    brandLabel->setObjectName("topNavBrandLabel");

    workspaceButton->setObjectName("topNavWorkspaceButton");
    workspaceButton->setProperty("active", false);
    workspaceButton->setCursor(Qt::PointingHandCursor);
    workspaceButton->setFixedHeight(32);

    settingButton->setObjectName("topNavSettingButton");
    settingButton->setProperty("active", false);
    settingButton->setCursor(Qt::PointingHandCursor);
    settingButton->setFixedHeight(32);

    userNamePrefixLabel->setObjectName("topNavUserPrefixLabel");
    userNameValueLabel->setObjectName("topNavUserValueLabel");

    layout->addWidget(brandLabel, 0, Qt::AlignVCenter);
    layout->addSpacing(10);
    layout->addWidget(workspaceButton, 0, Qt::AlignVCenter);
    layout->addWidget(settingButton, 0, Qt::AlignVCenter);
    layout->addStretch(1);
    layout->addWidget(userNamePrefixLabel, 0, Qt::AlignVCenter);
    layout->addWidget(userNameValueLabel, 0, Qt::AlignVCenter);
}

void TopNavigationWidget::initConnections()
{
    connect(workspaceButton, &QPushButton::clicked, this, &TopNavigationWidget::onWorkspaceButtonClicked);
    connect(settingButton, &QPushButton::clicked, this, &TopNavigationWidget::onSettingButtonClicked);
}

void TopNavigationWidget::initStyle()
{
    // 作者：YuzhSong
    // 将“工作区/设置”改为标签页视觉：激活态与下方内容区连接，未激活态保持暗色未选中外观。
    setStyleSheet(QString(
        "QWidget {"
        "    background-color: #1F2023;"
        "    color: #F0F0F0;"
        "    border: none;"
        "}"
        "QLabel#topNavBrandLabel {"
        "    color: #F2F3F5;"
        "    font-size: 14px;"
        "    font-weight: 700;"
        "    letter-spacing: 0;"
        "}"
        "QPushButton#topNavWorkspaceButton, QPushButton#topNavSettingButton {"
        "    background-color: #2A2C31;"
        "    color: #C8CBD2;"
        "    border: 1px solid #3A3D42;"
        "    border-bottom-color: #3A3D42;"
        "    border-top-left-radius: 8px;"
        "    border-top-right-radius: 8px;"
        "    border-bottom-left-radius: 0px;"
        "    border-bottom-right-radius: 0px;"
        "    padding: 5px 14px;"
        "    font-size: 13px;"
        "}"
        "QPushButton#topNavWorkspaceButton:hover, QPushButton#topNavSettingButton:hover {"
        "    background-color: #353942;"
        "}"
        "QPushButton#topNavWorkspaceButton[active=\"true\"], QPushButton#topNavSettingButton[active=\"true\"] {"
        "    background-color: #252629;"
        "    border: 1px solid #4A4E55;"
        "    border-bottom-color: #252629;"
        "    color: #FFFFFF;"
        "    font-weight: 600;"
        "}"
        "QLabel#topNavUserPrefixLabel {"
        "    color: #A8A8A8;"
        "    font-size: 12px;"
        "}"
        "QLabel#topNavUserValueLabel {"
        "    color: #F0F0F0;"
        "    font-size: 12px;"
        "    font-weight: 600;"
        "    background-color: #2B2D30;"
        "    border: 1px solid #3A3D42;"
        "    border-radius: 6px;"
        "    padding: 2px 8px;"
        "}"
    ));
}

void TopNavigationWidget::updateTabStyle()
{
    const bool workspaceActive = currentPageType == PageType::Workspace;
    const bool settingActive = currentPageType == PageType::Setting;

    workspaceButton->setProperty("active", workspaceActive);
    settingButton->setProperty("active", settingActive);

    // 作者：YuzhSong
    // 动态属性变化后需重新 polish，才能立即刷新按钮选中态样式。
    workspaceButton->style()->unpolish(workspaceButton);
    workspaceButton->style()->polish(workspaceButton);
    settingButton->style()->unpolish(settingButton);
    settingButton->style()->polish(settingButton);
    workspaceButton->update();
    settingButton->update();
}

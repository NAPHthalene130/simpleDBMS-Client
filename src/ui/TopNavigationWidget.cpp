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
    updateButtonState();
}

void TopNavigationWidget::setCurrentPage(PageType pageType)
{
    currentPageType = pageType;
    updateButtonState();
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
    layout->setContentsMargins(14, 10, 14, 10);
    layout->setSpacing(8);

    brandLabel->setObjectName("topNavBrandLabel");

    workspaceButton->setObjectName("topNavWorkspaceButton");
    workspaceButton->setProperty("active", false);
    workspaceButton->setCursor(Qt::PointingHandCursor);
    workspaceButton->setFixedHeight(30);

    settingButton->setObjectName("topNavSettingButton");
    settingButton->setProperty("active", false);
    settingButton->setCursor(Qt::PointingHandCursor);
    settingButton->setFixedHeight(30);

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
    setStyleSheet(QString(
        "QWidget {"
        "    background-color: #252526;"
        "    color: #D4D4D4;"
        "    border: none;"
        "}"
        "QLabel#topNavBrandLabel {"
        "    color: #EAEAEA;"
        "    font-size: 14px;"
        "    font-weight: 700;"
        "    letter-spacing: 0;"
        "}"
        "QPushButton#topNavWorkspaceButton, QPushButton#topNavSettingButton {"
        "    background-color: #333337;"
        "    color: #D9D9D9;"
        "    border: 1px solid #444448;"
        "    border-radius: 6px;"
        "    padding: 4px 14px;"
        "    font-size: 13px;"
        "}"
        "QPushButton#topNavWorkspaceButton:hover, QPushButton#topNavSettingButton:hover {"
        "    background-color: #3D3D42;"
        "}"
        "QPushButton#topNavWorkspaceButton[active=\"true\"], QPushButton#topNavSettingButton[active=\"true\"] {"
        "    background-color: #0E639C;"
        "    border: 1px solid #0E639C;"
        "    color: #FFFFFF;"
        "    font-weight: 600;"
        "}"
        "QLabel#topNavUserPrefixLabel {"
        "    color: #9A9A9A;"
        "    font-size: 12px;"
        "}"
        "QLabel#topNavUserValueLabel {"
        "    color: #E0E0E0;"
        "    font-size: 12px;"
        "    font-weight: 600;"
        "}"
    ));
}

void TopNavigationWidget::updateButtonState()
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

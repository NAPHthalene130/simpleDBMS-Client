/**
 * @file TopNavigationWidget.cpp
 * @author YuzhSong
 * @brief 顶部导航栏组件实现文件
 * @details 负责品牌、标签页与用户信息展示，并发出页面切换请求信号，不直接执行页面切换。
 * @module ui
 */

#include "TopNavigationWidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QStyle>
#include <QTabBar>

TopNavigationWidget::TopNavigationWidget(QWidget *parent)
    : QWidget(parent)
    , currentPageType(PageType::None)
    , brandLabel(new QLabel(tr("simpleDBMS"), this))
    , navigationTabBar(new QTabBar(this))
    , userNamePrefixLabel(new QLabel(tr("用户："), this))
    , userNameValueLabel(new QLabel(tr("Guest"), this))
{
    setAttribute(Qt::WA_StyledBackground, true);
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
    userNameValueLabel->setText(userName.trimmed().isEmpty() ? tr("Guest") : userName.trimmed());
}

void TopNavigationWidget::onTabChanged(int index)
{
    // 作者：YuzhSong
    // 顶部导航使用真实标签页控件；这里只负责转发“请求”，具体页面切换仍由 MainWindow 处理。
    if (index == 0) {
        emit workspaceRequested();
    } else if (index == 1) {
        emit settingRequested();
    }
}

void TopNavigationWidget::initUI()
{
    // 作者：YuzhSong
    // 标签页需要上圆角与下方内容区连通，因此提高导航栏高度，给标签留出稳定的视觉空间。
    setFixedHeight(35);

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 2, 12, 0);
    layout->setSpacing(8);

    brandLabel->setObjectName("topNavBrandLabel");

    navigationTabBar->setObjectName("topNavTabBar");
    navigationTabBar->setDrawBase(false);
    navigationTabBar->setExpanding(false);
    navigationTabBar->setUsesScrollButtons(false);
    navigationTabBar->setElideMode(Qt::ElideNone);
    navigationTabBar->addTab(tr("工作区"));
    navigationTabBar->addTab(tr("设置"));
    navigationTabBar->setCurrentIndex(0);
    navigationTabBar->setCursor(Qt::PointingHandCursor);

    userNamePrefixLabel->setObjectName("topNavUserPrefixLabel");
    userNameValueLabel->setObjectName("topNavUserValueLabel");

    layout->addWidget(brandLabel, 0, Qt::AlignVCenter);
    layout->addSpacing(12);
    layout->addWidget(navigationTabBar, 0, Qt::AlignBottom);
    layout->addStretch(1);
    layout->addWidget(userNamePrefixLabel, 0, Qt::AlignVCenter);
    layout->addWidget(userNameValueLabel, 0, Qt::AlignVCenter);
}

void TopNavigationWidget::initConnections()
{
    connect(navigationTabBar, &QTabBar::currentChanged, this, &TopNavigationWidget::onTabChanged);
}

void TopNavigationWidget::initStyle()
{
    // 作者：YuzhSong
    // 通过“选中标签底边与内容区同色”建立视觉连通，形成 IDE/浏览器标签页风格而非普通按钮风格。
    setStyleSheet(QString(
        "TopNavigationWidget {"
        "    background-color: #111315;"
        "}"
        "QWidget {"
        "    background-color: #111315;"
        "    color: #F0F0F0;"
        "    border: none;"
        "}"
        "QLabel#topNavBrandLabel {"
        "    color: #F2F3F5;"
        "    font-size: 14px;"
        "    font-weight: 700;"
        "    letter-spacing: 0;"
        "}"
        "QTabBar#topNavTabBar::tab {"
        "    background-color: #111315;"
        "    color: #A8A8A8;"
        "    border: 1px solid #2F3136;"
        "    border-bottom: 1px solid #111315;"
        "    border-top-left-radius: 11px;"
        "    border-top-right-radius: 11px;"
        "    min-width: 70px;"
        "    max-width: 70px;"
        "    qproperty-alignment: AlignCenter;"
        "    padding: 7px 15px;"
        "    margin-right: 4px;"
        "    font-size: 13px;"
        "}"
        "QTabBar#topNavTabBar::tab:hover {"
        "    background-color: #111315;"
        "    color: #E6E6E6;"
        "}"
        "QTabBar#topNavTabBar::tab:selected {"
        "    background-color: #2B2B2B;"
        "    border: 1px solid #1B1D20;"
        "    border-bottom-color: #2B2B2B;"
        "    color: #FFFFFF;"
        "    font-weight: 700;"
        "}"
        "QLabel#topNavUserPrefixLabel {"
        "    color: #A8A8A8;"
        "    font-size: 12px;"
        "}"
        "QLabel#topNavUserValueLabel {"
        "    color: #F0F0F0;"
        "    font-size: 12px;"
        "    font-weight: 600;"
        "    background-color: #111315;"
        "    border: 1px solid #3A3D42;"
        "    border-radius: 6px;"
        "    padding: 2px 8px;"
        "}"
    ));
}

void TopNavigationWidget::updateTabStyle()
{
    int targetIndex = -1;
    if (currentPageType == PageType::Workspace) {
        targetIndex = 0;
    } else if (currentPageType == PageType::Setting) {
        targetIndex = 1;
    }

    if (targetIndex >= 0 && navigationTabBar->currentIndex() != targetIndex) {
        navigationTabBar->blockSignals(true);
        navigationTabBar->setCurrentIndex(targetIndex);
        navigationTabBar->blockSignals(false);
    }

    navigationTabBar->style()->unpolish(navigationTabBar);
    navigationTabBar->style()->polish(navigationTabBar);
    navigationTabBar->update();
}

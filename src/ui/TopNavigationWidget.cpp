/**
 * @file TopNavigationWidget.cpp
 * @author YuzhSong
 * @brief 顶部导航栏组件实现文件
 * @details 负责品牌、标签页与用户信息展示，并发出页面切换请求信号，不直接执行页面切换。
 * @module ui
 */

#include "TopNavigationWidget.h"
#include "ThemeManager.h"

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
    // 顶部导航使用真实标签页控件；这里只负责转发"请求"，具体页面切换仍由 MainWindow 处理。
    if (index == 0) {
        emit workspaceRequested();
    } else if (index == 1) {
        emit tableViewRequested();
    } else if (index == 2) {
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
    navigationTabBar->addTab(tr("表内容"));
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
    setStyleSheet(ThemeManager::topNavigation());
}

void TopNavigationWidget::updateTabStyle()
{
    int targetIndex = -1;
    if (currentPageType == PageType::Workspace) {
        targetIndex = 0;
    } else if (currentPageType == PageType::TableView) {
        targetIndex = 1;
    } else if (currentPageType == PageType::Setting) {
        targetIndex = 2;
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

void TopNavigationWidget::refreshTheme()
{
    setStyleSheet(ThemeManager::topNavigation());
}

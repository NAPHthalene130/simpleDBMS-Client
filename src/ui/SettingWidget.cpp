#include "SettingWidget.h"

#include "mainwindow.h"
#include "AuthWidget.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>

SettingWidget::SettingWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      mainWindow(mainWindow)
{
    auto *mainLayout = new QVBoxLayout(this);
    auto *headerLayout = new QHBoxLayout();
    auto *titleLayout = new QVBoxLayout();
    auto *titleLabel = new QLabel(tr("偏好设置"), this);
    auto *subtitleLabel = new QLabel(tr("做一些细节调整，让使用体验更顺手。"), this);
    auto *backButton = new QPushButton(tr("返回"), this);
    auto *contentLayout = new QHBoxLayout();
    auto *leftCard = new QFrame(this);
    auto *rightCard = new QFrame(this);
    auto *leftLayout = new QVBoxLayout(leftCard);
    auto *rightLayout = new QVBoxLayout(rightCard);
    auto *themeLabel = new QLabel(tr("主题"), leftCard);
    auto *themeComboBox = new QComboBox(leftCard);
    auto *fontSizeLabel = new QLabel(tr("编辑器缩放"), leftCard);
    auto *fontSizeValueLabel = new QLabel(tr("100%"), leftCard);
    auto *fontSizeSlider = new QSlider(Qt::Horizontal, leftCard);
    auto *notificationCheckBox = new QCheckBox(tr("启用桌面通知"), leftCard);
    auto *autoConnectCheckBox = new QCheckBox(tr("启动时自动重连上次服务器"), leftCard);
    auto *rightTitleLabel = new QLabel(tr("工作区状态"), rightCard);
    auto *statusLayout = new QGridLayout();
    auto *statusHintLabel = new QLabel(tr("今日状态"), rightCard);
    auto *statusValueLabel = new QLabel(tr("已准备好本地开发环境"), rightCard);
    auto *shortcutHintLabel = new QLabel(tr("快捷键"), rightCard);
    auto *shortcutValueLabel = new QLabel(tr("Ctrl+L 可快速回到登录"), rightCard);
    auto *moodLabel = new QLabel(tr("一个小巧思：设置页使用了更柔和的强调色，让配置过程看起来更安静。"), rightCard);
    auto *actionLayout = new QHBoxLayout();
    auto *saveButton = new QPushButton(tr("保存"), this);
    auto *resetButton = new QPushButton(tr("重置"), this);

    setObjectName("settingWidget");
    leftCard->setObjectName("settingCard");
    rightCard->setObjectName("settingCard");
    titleLabel->setObjectName("settingTitleLabel");
    subtitleLabel->setObjectName("settingSubtitleLabel");
    rightTitleLabel->setObjectName("settingSectionTitleLabel");
    themeLabel->setObjectName("settingFieldTitleLabel");
    fontSizeLabel->setObjectName("settingFieldTitleLabel");
    moodLabel->setObjectName("settingMoodLabel");
    saveButton->setObjectName("settingPrimaryButton");
    resetButton->setObjectName("settingSecondaryButton");
    backButton->setObjectName("settingSecondaryButton");

    titleLabel->setTextFormat(Qt::PlainText);
    subtitleLabel->setWordWrap(true);
    moodLabel->setWordWrap(true);
    fontSizeSlider->setRange(80, 140);
    fontSizeSlider->setValue(100);
    themeComboBox->addItems({tr("浅色"), tr("深色"), tr("跟随系统")});
    notificationCheckBox->setChecked(true);
    autoConnectCheckBox->setChecked(true);

    headerLayout->addLayout(titleLayout, 1);
    headerLayout->addWidget(backButton, 0, Qt::AlignTop);

    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(subtitleLabel);

    leftLayout->setContentsMargins(24, 24, 24, 24);
    leftLayout->setSpacing(14);
    leftLayout->addWidget(themeLabel);
    leftLayout->addWidget(themeComboBox);
    leftLayout->addWidget(fontSizeLabel);
    leftLayout->addWidget(fontSizeSlider);
    leftLayout->addWidget(fontSizeValueLabel, 0, Qt::AlignRight);
    leftLayout->addSpacing(8);
    leftLayout->addWidget(notificationCheckBox);
    leftLayout->addWidget(autoConnectCheckBox);
    leftLayout->addStretch();

    statusLayout->addWidget(statusHintLabel, 0, 0);
    statusLayout->addWidget(statusValueLabel, 0, 1);
    statusLayout->addWidget(shortcutHintLabel, 1, 0);
    statusLayout->addWidget(shortcutValueLabel, 1, 1);
    statusLayout->setHorizontalSpacing(18);
    statusLayout->setVerticalSpacing(12);

    rightLayout->setContentsMargins(24, 24, 24, 24);
    rightLayout->setSpacing(16);
    rightLayout->addWidget(rightTitleLabel);
    rightLayout->addLayout(statusLayout);
    rightLayout->addWidget(moodLabel);
    rightLayout->addStretch();

    contentLayout->addWidget(leftCard, 3);
    contentLayout->addWidget(rightCard, 2);
    contentLayout->setSpacing(18);

    actionLayout->addStretch();
    actionLayout->addWidget(resetButton);
    actionLayout->addWidget(saveButton);

    mainLayout->setContentsMargins(32, 24, 32, 24);
    mainLayout->setSpacing(18);
    mainLayout->addLayout(headerLayout);
    mainLayout->addLayout(contentLayout, 1);
    mainLayout->addLayout(actionLayout);

    setStyleSheet(
        "#settingWidget {"
        "    background-color: #f5f7fb;"
        "}"
        "#settingCard {"
        "    background-color: white;"
        "    border: 1px solid #e5eaf5;"
        "    border-radius: 20px;"
        "}"
        "#settingTitleLabel {"
        "    font-size: 26px;"
        "    font-weight: 700;"
        "    color: #1f2937;"
        "}"
        "#settingSubtitleLabel, #settingMoodLabel {"
        "    font-size: 13px;"
        "    color: #667085;"
        "}"
        "#settingSectionTitleLabel, #settingFieldTitleLabel {"
        "    font-size: 15px;"
        "    font-weight: 700;"
        "    color: #344054;"
        "}"
        "QComboBox, QSlider {"
        "    min-height: 36px;"
        "}"
        "QComboBox {"
        "    padding: 0 12px;"
        "    border: 1px solid #d8e0ef;"
        "    border-radius: 12px;"
        "    background-color: #fbfcff;"
        "}"
        "QCheckBox, QLabel {"
        "    color: #475467;"
        "}"
        "#settingPrimaryButton {"
        "    min-width: 96px;"
        "    min-height: 40px;"
        "    border: none;"
        "    border-radius: 12px;"
        "    background-color: #6985ff;"
        "    color: white;"
        "    font-weight: 700;"
        "}"
        "#settingSecondaryButton {"
        "    min-width: 96px;"
        "    min-height: 40px;"
        "    border-radius: 12px;"
        "    border: 1px solid #d8e0ef;"
        "    background-color: white;"
        "    color: #344054;"
        "    font-weight: 600;"
        "}"
    );

    connect(fontSizeSlider, &QSlider::valueChanged, this, [fontSizeValueLabel](int value) {
        fontSizeValueLabel->setText(QObject::tr("%1%").arg(value));
    });

    connect(backButton, &QPushButton::clicked, this, [this]() {
        if (this->mainWindow != nullptr) {
            this->mainWindow->switchWidget(this->mainWindow->getAuthWidget());
        }
    });
}

MainWindow *SettingWidget::getMainWindow() const
{
    return mainWindow;
}

/**
 * @file SettingWidget.cpp
 * @author Qi
 * @brief 设置页面组件实现文件
 * @details 实现设置页 UI 展示与本地设置保存，不包含后端业务逻辑
 * @module ui
 */

#include "SettingWidget.h"
#include "ThemeManager.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

SettingWidget::SettingWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent)
    , mainWindow(mainWindow)
    , titleLabel(new QLabel(tr("设置"), this))
    , statusLabel(new QLabel(this))
    , categoryListWidget(new QListWidget(this))
    , contentStackedWidget(new QStackedWidget(this))
    , themeComboBox(new QComboBox(this))
    , uiFontSizeComboBox(new QComboBox(this))
    , uiScaleComboBox(new QComboBox(this))
    , editorFontFamilyComboBox(new QComboBox(this))
    , editorFontSizeComboBox(new QComboBox(this))
    , editorAutoWrapCheckBox(new QCheckBox(tr("自动换行"), this))
    , editorShowLineNumberCheckBox(new QCheckBox(tr("显示行号"), this))
    , autoSwitchToTableCheckBox(new QCheckBox(tr("执行 SQL 后自动切到结果表格"), this))
    , keepEditorContentCheckBox(new QCheckBox(tr("执行 SQL 后保留编辑器内容"), this))
    , dangerousSqlWarningCheckBox(new QCheckBox(tr("危险 SQL 提醒（DROP/DELETE/TRUNCATE）"), this))
    , rememberWindowSizeCheckBox(new QCheckBox(tr("记住上次窗口大小"), this))
    , rememberDatabaseCheckBox(new QCheckBox(tr("记住上次使用的数据库"), this))
    , currentUserValueLabel(new QLabel(this))
    , connectionStatusValueLabel(new QLabel(this))
    , currentDatabaseValueLabel(new QLabel(this))
    , logoutButton(new QPushButton(tr("退出登录"), this))
    , resetButton(new QPushButton(tr("恢复默认"), this))
    , cancelButton(new QPushButton(tr("取消"), this))
    , applyButton(new QPushButton(tr("应用"), this))
    , okButton(new QPushButton(tr("确定"), this))
{
    initUI();
    initStyle();
    initConnections();
    reloadSettingsFromLocal();
}

MainWindow *SettingWidget::getMainWindow() const
{
    return mainWindow;
}

void SettingWidget::refreshAccountInfo(const QString &currentUser,
                                       const QString &connectionStatus,
                                       const QString &currentDatabase)
{
    currentUserValueLabel->setText(currentUser.trimmed().isEmpty() ? tr("Guest") : currentUser.trimmed());
    connectionStatusValueLabel->setText(connectionStatus.trimmed().isEmpty() ? tr("Unknown") : connectionStatus.trimmed());
    currentDatabaseValueLabel->setText(currentDatabase.trimmed().isEmpty() ? tr("未选择") : currentDatabase.trimmed());
}

void SettingWidget::reloadSettingsFromLocal()
{
    loadSettingsToUi();
    setStatusMessage(tr("已加载本地设置。"));
}

void SettingWidget::onLogoutButtonClicked()
{
    emit logoutRequested();
}

void SettingWidget::onCategoryChanged(int categoryIndex)
{
    contentStackedWidget->setCurrentIndex(categoryIndex);
}

void SettingWidget::onApplyButtonClicked()
{
    if (isThemeChanged()) {
        QSettings oldSettings = buildSettingsStorage();
        const QString oldTheme = oldSettings.value("appearance/theme", tr("深色")).toString();
        const QMessageBox::StandardButton reply = QMessageBox::question(
            this, tr("切换主题"),
            tr("切换主题需要重启客户端才能生效。是否立即重启？"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            saveSettingsFromUi();
            emit restartRequested();
            return;
        }
        saveSettingsFromUiExceptTheme();
        themeComboBox->setCurrentText(oldTheme);
        setStatusMessage(tr("已取消主题切换，其他设置已保存。"));
        return;
    }
    saveSettingsFromUi();
    setStatusMessage(tr("设置已保存到本地。"));
    emit settingsApplied();
}

void SettingWidget::onOkButtonClicked()
{
    if (isThemeChanged()) {
        QSettings oldSettings = buildSettingsStorage();
        const QString oldTheme = oldSettings.value("appearance/theme", tr("深色")).toString();
        const QMessageBox::StandardButton reply = QMessageBox::question(
            this, tr("切换主题"),
            tr("切换主题需要重启客户端才能生效。是否立即重启？"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            saveSettingsFromUi();
            emit restartRequested();
            return;
        }
        saveSettingsFromUiExceptTheme();
        themeComboBox->setCurrentText(oldTheme);
        setStatusMessage(tr("已取消主题切换，其他设置已保存。"));
        emit backToWorkspaceRequested();
        return;
    }
    saveSettingsFromUi();
    setStatusMessage(tr("设置已保存。"));
    emit settingsApplied();
    emit backToWorkspaceRequested();
}

void SettingWidget::onCancelButtonClicked()
{
    loadSettingsToUi();
    setStatusMessage(tr("已取消未保存修改。"));
    emit backToWorkspaceRequested();
}

void SettingWidget::onResetButtonClicked()
{
    resetUiToDefaults();
    setStatusMessage(tr("已恢复默认配置，点击“应用”或“确定”保存。"));
}

void SettingWidget::initUI()
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(28, 22, 28, 18);
    rootLayout->setSpacing(14);

    auto *headerWidget = new QWidget(this);
    auto *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(12);

    titleLabel->setObjectName("settingTitleLabel");
    statusLabel->setObjectName("settingStatusLabel");
    statusLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    headerLayout->addWidget(titleLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);
    headerLayout->addStretch(1);
    headerLayout->addWidget(statusLabel, 0, Qt::AlignRight | Qt::AlignVCenter);

    auto *bodyLayout = new QHBoxLayout();
    bodyLayout->setSpacing(18);

    categoryListWidget->setObjectName("settingCategoryList");
    categoryListWidget->setFixedWidth(175);
    categoryListWidget->addItem(tr("外观"));
    categoryListWidget->addItem(tr("编辑器"));
    categoryListWidget->addItem(tr("使用习惯"));
    categoryListWidget->addItem(tr("账户"));
    categoryListWidget->setCurrentRow(0);

    contentStackedWidget->setObjectName("settingContentStack");
    contentStackedWidget->addWidget(buildAppearancePage());
    contentStackedWidget->addWidget(buildEditorPage());
    contentStackedWidget->addWidget(buildHabitPage());
    contentStackedWidget->addWidget(buildAccountPage());

    bodyLayout->addWidget(categoryListWidget, 0);
    bodyLayout->addWidget(contentStackedWidget, 1);

    auto *footerWidget = new QWidget(this);
    auto *footerLayout = new QHBoxLayout(footerWidget);
    footerLayout->setContentsMargins(0, 2, 0, 0);
    footerLayout->setSpacing(8);

    resetButton->setObjectName("secondaryButton");
    cancelButton->setObjectName("secondaryButton");
    applyButton->setObjectName("primaryButton");
    okButton->setObjectName("primaryButton");
    logoutButton->setObjectName("dangerButton");

    footerLayout->addWidget(logoutButton, 0, Qt::AlignLeft);
    footerLayout->addStretch(1);
    footerLayout->addWidget(resetButton, 0);
    footerLayout->addWidget(cancelButton, 0);
    footerLayout->addWidget(applyButton, 0);
    footerLayout->addWidget(okButton, 0);

    auto *separator = new QFrame(this);
    separator->setObjectName("settingFooterSeparator");
    separator->setFrameShape(QFrame::HLine);
    separator->setFixedHeight(1);

    rootLayout->addWidget(headerWidget, 0);
    rootLayout->addLayout(bodyLayout, 1);
    rootLayout->addWidget(separator, 0);
    rootLayout->addWidget(footerWidget, 0);
}

void SettingWidget::initConnections()
{
    connect(categoryListWidget, &QListWidget::currentRowChanged, this, &SettingWidget::onCategoryChanged);
    connect(applyButton, &QPushButton::clicked, this, &SettingWidget::onApplyButtonClicked);
    connect(okButton, &QPushButton::clicked, this, &SettingWidget::onOkButtonClicked);
    connect(cancelButton, &QPushButton::clicked, this, &SettingWidget::onCancelButtonClicked);
    connect(resetButton, &QPushButton::clicked, this, &SettingWidget::onResetButtonClicked);
    connect(logoutButton, &QPushButton::clicked, this, &SettingWidget::onLogoutButtonClicked);
}

void SettingWidget::initStyle()
{
    setStyleSheet(ThemeManager::settingWidget());
}

QWidget *SettingWidget::buildAppearancePage()
{
    auto *pageWidget = new QWidget(this);
    pageWidget->setObjectName("settingContentPage");
    auto *pageLayout = new QVBoxLayout(pageWidget);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->setSpacing(12);

    themeComboBox->addItems({tr("深色"), tr("浅色")});
    uiFontSizeComboBox->addItems({QStringLiteral("12"), QStringLiteral("13"), QStringLiteral("14"), QStringLiteral("15"), QStringLiteral("16")});
    uiScaleComboBox->addItems({QStringLiteral("100%"), QStringLiteral("125%"), QStringLiteral("150%"), QStringLiteral("175%"), QStringLiteral("200%")});

    auto *formLayout = new QFormLayout();
    formLayout->setSpacing(12);
    formLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    formLayout->addRow(tr("界面主题"), themeComboBox);
    formLayout->addRow(tr("界面字号"), uiFontSizeComboBox);
    formLayout->addRow(tr("界面缩放"), uiScaleComboBox);

    auto *noteLabel = new QLabel(tr("注：切换主题需立即重启客户端，缩放需重启后生效。"), this);
    noteLabel->setObjectName("settingStatusLabel");
    noteLabel->setWordWrap(true);

    pageLayout->addWidget(buildGroupCard(tr("外观"), formLayout));
    pageLayout->addWidget(noteLabel, 0);
    pageLayout->addStretch(1);
    return pageWidget;
}

QWidget *SettingWidget::buildEditorPage()
{
    auto *pageWidget = new QWidget(this);
    pageWidget->setObjectName("settingContentPage");
    auto *pageLayout = new QVBoxLayout(pageWidget);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->setSpacing(12);

    editorFontFamilyComboBox->addItems({tr("默认"), QStringLiteral("Consolas"), QStringLiteral("Courier New"), QStringLiteral("微软雅黑"), QStringLiteral("JetBrains Mono")});
    editorFontSizeComboBox->addItems({QStringLiteral("12"), QStringLiteral("13"), QStringLiteral("14"), QStringLiteral("15"), QStringLiteral("16"), QStringLiteral("18"), QStringLiteral("20")});

    auto *formLayout = new QFormLayout();
    formLayout->setSpacing(12);
    formLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    formLayout->addRow(tr("编辑器字体"), editorFontFamilyComboBox);
    formLayout->addRow(tr("编辑器字号"), editorFontSizeComboBox);
    formLayout->addRow(QString(), editorAutoWrapCheckBox);
    formLayout->addRow(QString(), editorShowLineNumberCheckBox);

    pageLayout->addWidget(buildGroupCard(tr("编辑器"), formLayout));
    pageLayout->addStretch(1);
    return pageWidget;
}

QWidget *SettingWidget::buildHabitPage()
{
    auto *pageWidget = new QWidget(this);
    pageWidget->setObjectName("settingContentPage");
    auto *pageLayout = new QVBoxLayout(pageWidget);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->setSpacing(12);

    auto *habitLayout = new QVBoxLayout();
    habitLayout->setSpacing(12);
    habitLayout->addWidget(autoSwitchToTableCheckBox);
    habitLayout->addWidget(keepEditorContentCheckBox);
    habitLayout->addWidget(dangerousSqlWarningCheckBox);
    habitLayout->addWidget(rememberWindowSizeCheckBox);
    habitLayout->addWidget(rememberDatabaseCheckBox);
    habitLayout->addStretch(1);

    pageLayout->addWidget(buildGroupCard(tr("使用习惯"), habitLayout));
    pageLayout->addStretch(1);
    return pageWidget;
}

QWidget *SettingWidget::buildAccountPage()
{
    auto *pageWidget = new QWidget(this);
    pageWidget->setObjectName("settingContentPage");
    auto *pageLayout = new QVBoxLayout(pageWidget);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->setSpacing(12);

    auto *accountLayout = new QFormLayout();
    accountLayout->setSpacing(12);

    auto *userLabel = new QLabel(tr("当前用户"), this);
    auto *connectionLabel = new QLabel(tr("连接状态"), this);
    auto *databaseLabel = new QLabel(tr("当前数据库"), this);
    userLabel->setObjectName("accountKeyLabel");
    connectionLabel->setObjectName("accountKeyLabel");
    databaseLabel->setObjectName("accountKeyLabel");

    currentUserValueLabel->setObjectName("accountValueLabel");
    connectionStatusValueLabel->setObjectName("accountValueLabel");
    currentDatabaseValueLabel->setObjectName("accountValueLabel");

    accountLayout->addRow(userLabel, currentUserValueLabel);
    accountLayout->addRow(connectionLabel, connectionStatusValueLabel);
    accountLayout->addRow(databaseLabel, currentDatabaseValueLabel);

    pageLayout->addWidget(buildGroupCard(tr("账户信息"), accountLayout));
    pageLayout->addStretch(1);
    return pageWidget;
}

QSettings SettingWidget::buildSettingsStorage() const
{
    return QSettings(QStringLiteral("simpleDBMS"), QStringLiteral("simpleDBMS-Client"));
}

void SettingWidget::loadSettingsToUi()
{
    QSettings settings = buildSettingsStorage();

    const QString themeValue = settings.value("appearance/theme", tr("深色")).toString();
    themeComboBox->setCurrentText(themeValue);
    uiFontSizeComboBox->setCurrentText(settings.value("appearance/uiFontSize", QStringLiteral("14")).toString());
    uiScaleComboBox->setCurrentText(settings.value("appearance/uiScale", QStringLiteral("100%")).toString());

    editorFontFamilyComboBox->setCurrentText(settings.value("editor/fontFamily", tr("默认")).toString());
    editorFontSizeComboBox->setCurrentText(settings.value("editor/fontSize", QStringLiteral("14")).toString());
    editorAutoWrapCheckBox->setChecked(settings.value("editor/autoWrap", true).toBool());
    editorShowLineNumberCheckBox->setChecked(settings.value("editor/showLineNumber", true).toBool());

    autoSwitchToTableCheckBox->setChecked(settings.value("habit/autoSwitchToTable", true).toBool());
    keepEditorContentCheckBox->setChecked(settings.value("habit/keepEditorContent", true).toBool());
    dangerousSqlWarningCheckBox->setChecked(settings.value("habit/dangerousSqlWarning", true).toBool());
    rememberWindowSizeCheckBox->setChecked(settings.value("habit/rememberWindowSize", true).toBool());
    rememberDatabaseCheckBox->setChecked(settings.value("habit/rememberDatabase", false).toBool());
}

void SettingWidget::saveSettingsFromUi()
{
    QSettings settings = buildSettingsStorage();

    settings.setValue("appearance/theme", themeComboBox->currentText());
    settings.setValue("appearance/uiFontSize", uiFontSizeComboBox->currentText());
    settings.setValue("appearance/uiScale", uiScaleComboBox->currentText());

    settings.setValue("editor/fontFamily", editorFontFamilyComboBox->currentText());
    settings.setValue("editor/fontSize", editorFontSizeComboBox->currentText());
    settings.setValue("editor/autoWrap", editorAutoWrapCheckBox->isChecked());
    settings.setValue("editor/showLineNumber", editorShowLineNumberCheckBox->isChecked());

    settings.setValue("habit/autoSwitchToTable", autoSwitchToTableCheckBox->isChecked());
    settings.setValue("habit/keepEditorContent", keepEditorContentCheckBox->isChecked());
    settings.setValue("habit/dangerousSqlWarning", dangerousSqlWarningCheckBox->isChecked());
    settings.setValue("habit/rememberWindowSize", rememberWindowSizeCheckBox->isChecked());
    settings.setValue("habit/rememberDatabase", rememberDatabaseCheckBox->isChecked());

    settings.sync();
}

void SettingWidget::resetUiToDefaults()
{
    themeComboBox->setCurrentText(tr("深色"));
    uiFontSizeComboBox->setCurrentText(QStringLiteral("14"));
    uiScaleComboBox->setCurrentText(QStringLiteral("100%"));

    editorFontFamilyComboBox->setCurrentText(tr("默认"));
    editorFontSizeComboBox->setCurrentText(QStringLiteral("14"));
    editorAutoWrapCheckBox->setChecked(true);
    editorShowLineNumberCheckBox->setChecked(true);

    autoSwitchToTableCheckBox->setChecked(true);
    keepEditorContentCheckBox->setChecked(true);
    dangerousSqlWarningCheckBox->setChecked(true);
    rememberWindowSizeCheckBox->setChecked(true);
    rememberDatabaseCheckBox->setChecked(false);
}

void SettingWidget::saveSettingsFromUiExceptTheme()
{
    QSettings settings = buildSettingsStorage();

    settings.setValue("appearance/uiFontSize", uiFontSizeComboBox->currentText());
    settings.setValue("appearance/uiScale", uiScaleComboBox->currentText());

    settings.setValue("editor/fontFamily", editorFontFamilyComboBox->currentText());
    settings.setValue("editor/fontSize", editorFontSizeComboBox->currentText());
    settings.setValue("editor/autoWrap", editorAutoWrapCheckBox->isChecked());
    settings.setValue("editor/showLineNumber", editorShowLineNumberCheckBox->isChecked());

    settings.setValue("habit/autoSwitchToTable", autoSwitchToTableCheckBox->isChecked());
    settings.setValue("habit/keepEditorContent", keepEditorContentCheckBox->isChecked());
    settings.setValue("habit/dangerousSqlWarning", dangerousSqlWarningCheckBox->isChecked());
    settings.setValue("habit/rememberWindowSize", rememberWindowSizeCheckBox->isChecked());
    settings.setValue("habit/rememberDatabase", rememberDatabaseCheckBox->isChecked());

    settings.sync();
}

void SettingWidget::setStatusMessage(const QString &message)
{
    statusLabel->setText(message);
}

bool SettingWidget::isThemeChanged() const
{
    QSettings settings = buildSettingsStorage();
    const QString savedTheme = settings.value("appearance/theme", tr("深色")).toString();
    return themeComboBox->currentText() != savedTheme;
}

QFrame *SettingWidget::buildGroupCard(const QString &title, QLayout *contentLayout) const
{
    auto *cardFrame = new QFrame();
    cardFrame->setObjectName("settingGroupCard");

    auto *cardLayout = new QVBoxLayout(cardFrame);
    cardLayout->setContentsMargins(16, 14, 16, 14);
    cardLayout->setSpacing(12);

    auto *titleTextLabel = new QLabel(title, cardFrame);
    titleTextLabel->setObjectName("settingGroupTitle");
    cardLayout->addWidget(titleTextLabel, 0, Qt::AlignLeft);
    cardLayout->addLayout(contentLayout);
    return cardFrame;
}

void SettingWidget::refreshTheme()
{
    setStyleSheet(ThemeManager::settingWidget());
}

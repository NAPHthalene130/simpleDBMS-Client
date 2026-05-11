/**
 * @file ThemeManager.h
 * @author Qi
 * @brief 全局主题管理器
 * @details 集中管理所有面板的深色/浅色 stylesheet，支持热切换
 * @module ui
 */

#pragma once

#include <QSettings>
#include <QString>

class ThemeManager
{
public:
    enum class Theme { Dark, Light };

    static Theme currentTheme()
    {
        const QSettings settings(QStringLiteral("simpleDBMS"), QStringLiteral("simpleDBMS-Client"));
        const QString themeValue = settings.value("appearance/theme", QStringLiteral("深色")).toString();
        if (themeValue.contains(QStringLiteral("浅")) || themeValue.contains("Light", Qt::CaseInsensitive)) {
            return Theme::Light;
        }
        return Theme::Dark;
    }

    static bool isDark() { return currentTheme() == Theme::Dark; }

    // ── MainWindow / 全局基础 ──
    static QString globalBase()
    {
        if (isDark()) {
            return QStringLiteral(
                "QMainWindow, QWidget#appContainerWidget { background-color: #2B2B2B; }"
                "QStackedWidget { background-color: #2B2B2B; }"
                "QSplitter::handle { background-color: #3C3F41; }"
                "QScrollBar:vertical { background: #313335; width: 10px; }"
                "QScrollBar::handle:vertical { background: #555; border-radius: 4px; min-height: 30px; }"
                "QScrollBar:horizontal { background: #313335; height: 10px; }"
                "QScrollBar::handle:horizontal { background: #555; border-radius: 4px; min-width: 30px; }"
            );
        }
        return QStringLiteral(
            "QMainWindow, QWidget#appContainerWidget { background-color: #EDF0F4; }"
            "QStackedWidget { background-color: #EDF0F4; }"
            "QSplitter::handle { background-color: #D0D7E2; }"
            "QScrollBar:vertical { background: #EDF0F4; width: 10px; }"
            "QScrollBar::handle:vertical { background: #BEC6D2; border-radius: 4px; min-height: 30px; }"
            "QScrollBar:horizontal { background: #EDF0F4; height: 10px; }"
            "QScrollBar::handle:horizontal { background: #BEC6D2; border-radius: 4px; min-width: 30px; }"
        );
    }

    // ── OpePanelWidget ──
    static QString opePanel()
    {
        if (isDark()) {
            return QStringLiteral(
                "QWidget { background-color: #2B2B2B; color: #F0F0F0; }"
                "QSplitter::handle { background-color: #2B2B2B; border-radius: 2px; }"
                "QSplitter::handle:hover { background-color: #353638; }"
            );
        }
        return QStringLiteral(
            "QWidget { background-color: #EDF0F4; color: #1C1E21; }"
            "QSplitter::handle { background-color: #EDF0F4; border-radius: 2px; }"
            "QSplitter::handle:hover { background-color: #D0D7E2; }"
        );
    }

    // ── 通用侧栏面板 (Directory / File / Log / AI) ──
    static QString sidePanel()
    {
        if (isDark()) {
            return QStringLiteral(
                "QWidget { background-color: #111315; border: 1px solid #1B1D20; border-radius: 14px; }"
                "QLabel[objectName$=\"itleLabel\"] { color: #D5D8DD; font-weight: 700; background-color: #2F3134;"
                "  border: 1px solid #3A3D42; border-radius: 8px; padding: 8px 10px; }"
                "QTreeWidget,QListWidget { background-color: #111315; color: #F0F0F0; border: 1px solid #1B1D20;"
                "  border-radius: 10px; padding: 6px; outline: none; }"
                "QTreeWidget::item,QListWidget::item { padding: 3px 6px; margin: 1px 2px; border-radius: 4px; }"
                "QTreeWidget::item:hover,QListWidget::item:hover { background-color: #34373C; }"
                "QTreeWidget::item:selected,QListWidget::item:selected { background-color: #2F64A8; color: #FFFFFF; }"
            );
        }
        return QStringLiteral(
            "QWidget { background-color: #FFFFFF; border: 1px solid #C5CFDA; border-radius: 14px; }"
            "QLabel[objectName$=\"itleLabel\"] { color: #3B4A60; font-weight: 700; background-color: #F4F6F8;"
            "  border: 1px solid #D8E0E8; border-radius: 8px; padding: 8px 10px; }"
            "QTreeWidget,QListWidget { background-color: #FFFFFF; color: #1C1E21; border: 1px solid #C5CFDA;"
            "  border-radius: 10px; padding: 6px; outline: none; }"
            "QTreeWidget::item,QListWidget::item { padding: 3px 6px; margin: 1px 2px; border-radius: 4px; }"
            "QTreeWidget::item:hover,QListWidget::item:hover { background-color: #E8EDF4; }"
            "QTreeWidget::item:selected,QListWidget::item:selected { background-color: #3B71CA; color: #FFFFFF; }"
        );
    }

    // ── ActivityBarWidget ──
    static QString activityBar()
    {
        if (isDark()) {
            return QStringLiteral(
                "ActivityBarWidget { background-color: #111315; border: 1px solid #1B1D20; border-radius: 14px; }"
                "QPushButton { min-width:30px; min-height:30px; border:1px solid transparent; border-radius:7px;"
                "  background-color:#1A1C1E; color:#C8CBD2; font-weight:600; font-size:11px; }"
                "QPushButton:hover { background-color:#26292D; border-color:#2B3035; }"
                "QPushButton:checked { background-color:#2F64A8; border:1px solid #2F64A8; color:#FFFFFF; }"
            );
        }
        return QStringLiteral(
            "ActivityBarWidget { background-color: #FFFFFF; border: 1px solid #C5CFDA; border-radius: 14px; }"
            "QPushButton { min-width:30px; min-height:30px; border:1px solid transparent; border-radius:7px;"
            "  background-color:#F4F6F8; color:#6B7A90; font-weight:600; font-size:11px; }"
            "QPushButton:hover { background-color:#E3E9F0; border-color:#C5CFDA; }"
            "QPushButton:checked { background-color:#3B71CA; border:1px solid #3B71CA; color:#FFFFFF; }"
        );
    }

    // ── EditorWidget 工具栏 ──
    static QString editorPanel()
    {
        if (isDark()) {
            return QStringLiteral(
                "EditorWidget#editorWidgetRoot { background-color:#111315; border:1px solid #1B1D20;"
                "  border-radius:14px; }"
                "QToolBar#editorToolbar { background-color:#111315; border:none; border-top-left-radius:14px;"
                "  border-top-right-radius:14px; padding:4px 6px; spacing:4px; }"
                "QToolBar#editorToolbar QToolButton { background-color:#1A1C1E; border:1px solid #1B1D20;"
                "  border-radius:8px; padding:4px 10px; color:#F0F0F0; }"
                "QToolBar#editorToolbar QToolButton:hover { background-color:#25292D; }"
                "QToolBar#editorToolbar QToolButton#btnRun { background-color:#3FB950; border:1px solid #2F9E44;"
                "  color:#FFFFFF; font-weight:600; }"
                "QWidget#editorToolbarSpacer { background-color:#111315; border:none; }"
                "QPlainTextEdit#editorSqlInput { background-color:#111315; border:none;"
                "  border-bottom-left-radius:14px; border-bottom-right-radius:14px; }"
            );
        }
        return QStringLiteral(
            "EditorWidget#editorWidgetRoot { background-color:#FFFFFF; border:1px solid #C5CFDA;"
            "  border-radius:14px; }"
            "QToolBar#editorToolbar { background-color:#F8F9FB; border:none; border-top-left-radius:14px;"
            "  border-top-right-radius:14px; padding:4px 6px; spacing:4px; }"
            "QToolBar#editorToolbar QToolButton { background-color:#FFFFFF; border:1px solid #D0D7E2;"
            "  border-radius:8px; padding:4px 10px; color:#1C1E21; }"
            "QToolBar#editorToolbar QToolButton:hover { background-color:#E8EDF4; border-color:#C5CFDA; }"
            "QToolBar#editorToolbar QToolButton#btnRun { background-color:#3FB950; border:1px solid #2F9E44;"
            "  color:#FFFFFF; font-weight:600; }"
            "QWidget#editorToolbarSpacer { background-color:#F8F9FB; border:none; }"
            "QPlainTextEdit#editorSqlInput { background-color:#FFFFFF; color:#1C1E21;"
            "  border:none; border-bottom-left-radius:14px; border-bottom-right-radius:14px; }"
        );
    }

    // ── SqlEditor ──
    static QString sqlEditor()
    {
        if (isDark()) {
            return QStringLiteral(
                "QPlainTextEdit { background-color:#111315; color:#E6E6E6; border:none; }"
            );
        }
        return QStringLiteral(
            "QPlainTextEdit { background-color:#FFFFFF; color:#1C1E21; border:none; }"
        );
    }

    // ── TableWidget ──
    static QString tableWidget()
    {
        if (isDark()) {
            return QStringLiteral(
                "QWidget { background-color: #2b2d30; color: #c9ccd1; }"
                "QLabel { color: #8f949d; font-size: 11px; padding: 2px 4px; }"
                "QTableWidget { background-color: #31343a; alternate-background-color: #2f3238;"
                "  color: #d3d6dc; gridline-color: #3b3e45; selection-background-color: #355a85;"
                "  selection-color: #f2f4f8; border: 1px solid #3b3e45; font-size: 12px; }"
                "QTableWidget::item { padding: 2px 6px; }"
                "QHeaderView::section { background-color: #2a2d31; color: #c1c6cf;"
                "  border: 1px solid #3b3e45; padding: 3px 6px; font-size: 11px; }"
                "QHeaderView::section:horizontal { text-align: left; }"
                "QScrollBar:vertical, QScrollBar:horizontal { background-color: #2c2f34; }"
                "QPushButton#tableBackButton { background-color: #3a3d43; color: #a8adb8;"
                "  border: 1px solid #4a4d54; border-radius: 4px; padding: 2px 10px; font-size: 11px; }"
                "QPushButton#tableBackButton:hover { background-color: #4a4d54; color: #d3d6dc; }"
                "QPushButton#tableBackButton:pressed { background-color: #50545c; }"
            );
        }
        return QStringLiteral(
            "QWidget { background-color: #FFFFFF; color: #1C1E21; }"
            "QLabel { color: #6B7A90; font-size: 11px; padding: 2px 4px; }"
            "QTableWidget { background-color: #FFFFFF; alternate-background-color: #F7F8FA;"
            "  color: #1C1E21; gridline-color: #D0D7E2; selection-background-color: #3B71CA;"
            "  selection-color: #FFFFFF; border: 1px solid #C5CFDA; font-size: 12px; }"
            "QTableWidget::item { padding: 2px 6px; }"
            "QHeaderView::section { background-color: #F4F6F8; color: #3B4A60;"
            "  border: 1px solid #D0D7E2; padding: 3px 6px; font-size: 11px; font-weight:600; }"
            "QHeaderView::section:horizontal { text-align: left; }"
            "QScrollBar:vertical, QScrollBar:horizontal { background-color: #F4F6F8; }"
            "QPushButton#tableBackButton { background-color: #F0F2F5; color: #3B4A60;"
            "  border: 1px solid #C5CFDA; border-radius: 4px; padding: 2px 10px; font-size: 11px; }"
            "QPushButton#tableBackButton:hover { background-color: #E3E9F0; color: #1C1E21;"
            "  border-color: #3B71CA; }"
            "QPushButton#tableBackButton:pressed { background-color: #D0D7E2; }"
        );
    }

    // ── TerminalWidget ──
    static QString terminal()
    {
        if (isDark()) {
            return QStringLiteral(
                "TerminalWidget { background-color:#111315; color:#F0F0F0; border:1px solid #1B1D20;"
                "  border-radius:14px; }"
                "QPlainTextEdit#terminalOutputTextEdit { background-color:#111315; color:#F0F0F0;"
                "  border:1px solid #1B1D20; border-radius:10px; selection-background-color:#3D6FB6; }"
                "QLineEdit#terminalInputLineEdit { background-color:#111315; color:#F0F0F0;"
                "  border:1px solid #1B1D20; border-radius:10px; padding:8px 10px; }"
                "QPushButton#terminalExecuteButton { background-color:#111315; color:#ffffff;"
                "  border:1px solid #1B1D20; border-radius:10px; padding:8px 14px; }"
                "QPushButton#terminalExecuteButton:hover { background-color:#1A1C1E; }"
            );
        }
        return QStringLiteral(
            "TerminalWidget { background-color:#FFFFFF; color:#1C1E21; border:1px solid #C5CFDA;"
            "  border-radius:14px; }"
            "QPlainTextEdit#terminalOutputTextEdit { background-color:#F8F9FB; color:#1C1E21;"
            "  border:1px solid #D0D7E2; border-radius:10px; selection-background-color:#BBD3F5; }"
            "QLineEdit#terminalInputLineEdit { background-color:#FFFFFF; color:#1C1E21;"
            "  border:1px solid #C5CFDA; border-radius:10px; padding:8px 10px; }"
            "QLineEdit#terminalInputLineEdit:focus { border:1px solid #3B71CA; }"
            "QPushButton#terminalExecuteButton { background-color:#3B71CA; color:#FFFFFF;"
            "  border:1px solid #3B71CA; border-radius:10px; padding:8px 14px; }"
            "QPushButton#terminalExecuteButton:hover { background-color:#4D82D6; }"
        );
    }

    // ── TopNavigationWidget ──
    static QString topNavigation()
    {
        if (isDark()) {
            return QStringLiteral(
                "TopNavigationWidget,QWidget { background-color:#111315; color:#F0F0F0; border:none; }"
                "QLabel#topNavBrandLabel { color:#F2F3F5; font-size:14px; font-weight:700; }"
                "QTabBar#topNavTabBar::tab { background-color:#111315; color:#A8A8A8;"
                "  border:1px solid #2F3136; border-bottom:1px solid #111315;"
                "  border-top-left-radius:11px; border-top-right-radius:11px;"
                "  min-width:70px; max-width:70px; padding:7px 15px;"
                "  margin-right:4px; font-size:13px; }"
                "QTabBar#topNavTabBar::tab:hover { background-color:#111315; color:#E6E6E6; }"
                "QTabBar#topNavTabBar::tab:selected { background-color:#2B2B2B; border:1px solid #1B1D20;"
                "  border-bottom-color:#2B2B2B; color:#FFFFFF; font-weight:700; }"
                "QLabel#topNavUserPrefixLabel { color:#A8A8A8; font-size:12px; }"
                "QLabel#topNavUserValueLabel { color:#F0F0F0; font-size:12px; font-weight:600;"
                "  background-color:#111315; border:1px solid #3A3D42; border-radius:6px; padding:2px 8px; }"
            );
        }
        return QStringLiteral(
            "TopNavigationWidget,QWidget { background-color:#FFFFFF; color:#1C1E21; border:none; }"
            "QLabel#topNavBrandLabel { color:#1C1E21; font-size:14px; font-weight:700; }"
            "QTabBar#topNavTabBar::tab { background-color:#F4F6F8; color:#8A92A0;"
            "  border:1px solid #D0D7E2; border-bottom:1px solid #F4F6F8;"
            "  border-top-left-radius:11px; border-top-right-radius:11px;"
            "  min-width:70px; max-width:70px; padding:7px 15px;"
            "  margin-right:4px; font-size:13px; }"
            "QTabBar#topNavTabBar::tab:hover { background-color:#F4F6F8; color:#3B4A60; }"
            "QTabBar#topNavTabBar::tab:selected { background-color:#EDF0F4; border:1px solid #C5CFDA;"
            "  border-bottom-color:#EDF0F4; color:#1C1E21; font-weight:700; }"
            "QLabel#topNavUserPrefixLabel { color:#8A92A0; font-size:12px; }"
            "QLabel#topNavUserValueLabel { color:#1C1E21; font-size:12px; font-weight:600;"
            "  background-color:#F4F6F8; border:1px solid #D0D7E2; border-radius:6px; padding:2px 8px; }"
        );
    }

    // ── AuthWidget ──
    static QString authWidget()
    {
        if (isDark()) {
            return QStringLiteral(
                "QWidget { background-color:#1E1E1E; color:#D4D4D4; }"
                "QWidget#authCard { background-color:#2D2D30; border:1px solid #3E3E42; border-radius:8px; }"
                "QWidget#authCard QLabel { background-color:#2D2D30; color:#CCCCCC; }"
                "QLabel#authHeaderLabel { font-size:18px; font-weight:bold; color:#E0E0E0; padding-bottom:4px; }"
                "QLabel#formLabel { font-size:13px; min-width:70px; }"
                "QLineEdit { background-color:#3C3C3C; border:1px solid #555555; border-radius:4px;"
                "  padding:6px 10px; font-size:13px; color:#E0E0E0; selection-background-color:#264F78; }"
                "QLineEdit:focus { border:1px solid #007ACC; }"
                "QLineEdit:disabled { background-color:#2A2A2A; color:#666666; }"
                "QPushButton { font-size:13px; border:1px solid #555555; border-radius:4px; padding:6px 16px; }"
                "QPushButton:hover { background-color:#505050; }"
                "QPushButton:pressed { background-color:#404040; }"
                "QPushButton#btnTestConnection { background-color:#3C3C3C; color:#E0E0E0;"
                "  border:1px solid #555555; }"
                "QPushButton#btnTestConnection:hover { background-color:#505050; border:1px solid #007ACC; }"
                "QPushButton#btnTestConnection:pressed { background-color:#404040; }"
                "QPushButton#btnConfirm { background-color:#0E639C; color:#FFFFFF; border:1px solid #1177BB; }"
                "QPushButton#btnConfirm:hover { background-color:#1177BB; }"
                "QPushButton#btnConfirm:pressed { background-color:#0D5689; }"
            );
        }
        return QStringLiteral(
            "QWidget { background-color:#EDF0F4; color:#1C1E21; }"
            "QWidget#authCard { background-color:#FFFFFF; border:1px solid #C5CFDA; border-radius:8px; }"
            "QWidget#authCard QLabel { background-color:#FFFFFF; color:#3B4A60; }"
            "QLabel#authHeaderLabel { font-size:18px; font-weight:bold; color:#1C1E21; padding-bottom:4px; }"
            "QLabel#formLabel { font-size:13px; min-width:70px; color:#3B4A60; }"
            "QLineEdit { background-color:#FFFFFF; border:1px solid #C5CFDA; border-radius:4px;"
            "  padding:6px 10px; font-size:13px; color:#1C1E21; selection-background-color:#BBD3F5; }"
            "QLineEdit:focus { border:1px solid #3B71CA; }"
            "QLineEdit:disabled { background-color:#F4F6F8; color:#8A92A0; }"
            "QPushButton { font-size:13px; border:1px solid #C5CFDA; border-radius:4px;"
            "  padding:6px 16px; color:#1C1E21; }"
            "QPushButton:hover { background-color:#E8EDF4; }"
            "QPushButton:pressed { background-color:#D0D7E2; }"
            "QPushButton#btnTestConnection { background-color:#F4F6F8; color:#3B4A60;"
            "  border:1px solid #C5CFDA; }"
            "QPushButton#btnTestConnection:hover { background-color:#E3E9F0; border:1px solid #3B71CA; }"
            "QPushButton#btnTestConnection:pressed { background-color:#D0D7E2; }"
            "QPushButton#btnConfirm { background-color:#3B71CA; color:#FFFFFF; border:1px solid #3B71CA; }"
            "QPushButton#btnConfirm:hover { background-color:#4D82D6; }"
            "QPushButton#btnConfirm:pressed { background-color:#2E5CAD; }"
        );
    }

    // ── SettingWidget ──
    static QString settingWidget()
    {
        if (isDark()) {
            return QStringLiteral(
                "QWidget { background-color:#1F2125; color:#D7DAE0; }"
                "QLabel#settingTitleLabel { color:#EEF1F6; font-size:22px; font-weight:700; }"
                "QLabel#settingStatusLabel { color:#8F96A3; font-size:11px; }"
                "QFrame#settingFooterSeparator { background-color:#333842; border:none; }"
                "QListWidget#settingCategoryList { background-color:#24272D; border:1px solid #333842;"
                "  border-radius:8px; outline:none; padding:4px; }"
                "QListWidget#settingCategoryList::item { padding:9px 12px; margin:1px 0px; border-radius:6px;"
                "  color:#BDC4D0; font-size:12px; }"
                "QListWidget#settingCategoryList::item:selected { background-color:#1D4F91; color:#FFFFFF;"
                "  font-weight:600; }"
                "QListWidget#settingCategoryList::item:hover:!selected { background-color:#2A2D34; }"
                "QWidget#settingContentPage { background-color:transparent; }"
                "QFrame#settingGroupCard { background-color:#262A31; border:1px solid #363C47;"
                "  border-radius:10px; }"
                "QLabel#settingGroupTitle { color:#E6EAF2; font-size:13px; font-weight:700; }"
                "QLabel#accountKeyLabel { color:#9AA2AF; font-size:12px; }"
                "QLabel#accountValueLabel { color:#E0E4EB; font-size:12px; font-weight:500; }"
                "QComboBox { min-height:30px; min-width:130px; background-color:#2E333C;"
                "  border:1px solid #3A404B; border-radius:6px; padding:4px 10px; font-size:12px; }"
                "QComboBox:hover { border:1px solid #4E84C4; }"
                "QComboBox::drop-down { subcontrol-origin:padding; subcontrol-position:top right; width:24px;"
                "  border-left:1px solid #3A404B; border-top-right-radius:6px;"
                "  border-bottom-right-radius:6px; }"
                "QCheckBox { spacing:8px; color:#D7DAE0; font-size:12px; }"
                "QCheckBox::indicator { width:16px; height:16px; }"
                "QCheckBox::indicator:unchecked { border:1px solid #5D6674; background-color:#2A2F38;"
                "  border-radius:3px; }"
                "QCheckBox::indicator:checked { border:1px solid #3B78C8; background-color:#2F6EC4;"
                "  border-radius:3px; }"
                "QPushButton { min-width:82px; min-height:34px; border-radius:6px; padding:6px 16px;"
                "  font-size:12px; }"
                "QPushButton#secondaryButton { background-color:#2E333C; border:1px solid #434A57;"
                "  color:#D8DCE4; }"
                "QPushButton#secondaryButton:hover { background-color:#383F4B; }"
                "QPushButton#primaryButton { background-color:#2F6EC4; border:1px solid #3A7AD0;"
                "  color:#FFFFFF; }"
                "QPushButton#primaryButton:hover { background-color:#3A7AD0; }"
                "QPushButton#dangerButton { background-color:#3B2A2D; border:1px solid #8F4A53;"
                "  color:#F3D9DD; }"
                "QPushButton#dangerButton:hover { background-color:#4A3136; }"
                "QPushButton:pressed { padding-top:7px; padding-bottom:5px; }"
            );
        }
        return QStringLiteral(
            "QWidget { background-color:#EDF0F4; color:#1C1E21; }"
            "QLabel#settingTitleLabel { color:#1C1E21; font-size:22px; font-weight:700; }"
            "QLabel#settingStatusLabel { color:#6B7A90; font-size:11px; }"
            "QFrame#settingFooterSeparator { background-color:#D0D7E2; border:none; }"
            "QListWidget#settingCategoryList { background-color:#FFFFFF; border:1px solid #C5CFDA;"
            "  border-radius:8px; outline:none; padding:4px; }"
            "QListWidget#settingCategoryList::item { padding:9px 12px; margin:1px 0px; border-radius:6px;"
            "  color:#3B4A60; font-size:12px; }"
            "QListWidget#settingCategoryList::item:selected { background-color:#3B71CA; color:#FFFFFF;"
            "  font-weight:600; }"
            "QListWidget#settingCategoryList::item:hover:!selected { background-color:#E8EDF4; }"
            "QWidget#settingContentPage { background-color:transparent; }"
            "QFrame#settingGroupCard { background-color:#FFFFFF; border:1px solid #C5CFDA;"
            "  border-radius:10px; }"
            "QLabel#settingGroupTitle { color:#3B4A60; font-size:13px; font-weight:700; }"
            "QLabel#accountKeyLabel { color:#6B7A90; font-size:12px; }"
            "QLabel#accountValueLabel { color:#1C1E21; font-size:12px; font-weight:500; }"
            "QComboBox { min-height:30px; min-width:130px; background-color:#FFFFFF;"
            "  border:1px solid #C5CFDA; border-radius:6px; padding:4px 10px; font-size:12px;"
            "  color:#1C1E21; }"
            "QComboBox:hover { border:1px solid #3B71CA; }"
            "QComboBox::drop-down { subcontrol-origin:padding; subcontrol-position:top right; width:24px;"
            "  border-left:1px solid #C5CFDA; border-top-right-radius:6px;"
            "  border-bottom-right-radius:6px; }"
            "QCheckBox { spacing:8px; color:#1C1E21; font-size:12px; }"
            "QCheckBox::indicator { width:16px; height:16px; }"
            "QCheckBox::indicator:unchecked { border:1px solid #BEC6D2; background-color:#FFFFFF;"
            "  border-radius:3px; }"
            "QCheckBox::indicator:checked { border:1px solid #3B71CA; background-color:#3B71CA;"
            "  border-radius:3px; }"
            "QPushButton { min-width:82px; min-height:34px; border-radius:6px; padding:6px 16px;"
            "  font-size:12px; }"
            "QPushButton#secondaryButton { background-color:#F4F6F8; border:1px solid #C5CFDA;"
            "  color:#3B4A60; }"
            "QPushButton#secondaryButton:hover { background-color:#E8EDF4; }"
            "QPushButton#primaryButton { background-color:#3B71CA; border:1px solid #3B71CA;"
            "  color:#FFFFFF; }"
            "QPushButton#primaryButton:hover { background-color:#4D82D6; }"
            "QPushButton#dangerButton { background-color:#FFF0F0; border:1px solid #E8A0A0;"
            "  color:#C03030; }"
            "QPushButton#dangerButton:hover { background-color:#FFE4E4; }"
            "QPushButton:pressed { padding-top:7px; padding-bottom:5px; }"
        );
    }
};

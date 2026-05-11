/**
 * @file ActivityBarWidget.h
 * @author YuzhSong
 * @brief IDE 左侧 ActivityBar 头文件
 * @details 提供 Directory/File/Log/AI/Terminal 五个入口按钮与状态同步接口
 * @module ui/opePanel
 */

#pragma once

#include <QWidget>

class QPushButton;

class ActivityBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ActivityBarWidget(QWidget* parent = nullptr);

    void setDirectoryChecked(bool checked);
    void setFileChecked(bool checked);
    void setLogChecked(bool checked);
    void setAiChecked(bool checked);
    void setTerminalChecked(bool checked);

    void refreshTheme();

signals:
    void directoryPanelRequested();
    void filePanelRequested();
    void logPanelRequested();
    void aiPanelRequested();
    void terminalPanelRequested();

private:
    void initUI();
    void initStyle();
    void initConnections();

private:
    QPushButton* directoryButton; ///< Directory/DB 入口按钮
    QPushButton* fileButton;      ///< File 入口按钮
    QPushButton* logButton;       ///< Log 入口按钮
    QPushButton* aiButton;        ///< AI 入口按钮
    QPushButton* terminalButton;  ///< Terminal 入口按钮
};


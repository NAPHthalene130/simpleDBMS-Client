#include "TableWidget.h"

#include <QFrame>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>

TableWidget::TableWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      mainWindow(mainWindow)
{
    auto *layout = new QVBoxLayout(this);
    auto *titleLabel = new QLabel(tr("结果表"), this);
    auto *hintLabel = new QLabel(tr("交替行背景可以让大结果集的浏览更轻松。"), this);
    auto *tableFrame = new QFrame(this);
    auto *tableLayout = new QVBoxLayout(tableFrame);
    auto *tableWidget = new QTableWidget(4, 3, tableFrame);

    titleLabel->setObjectName("tableTitleLabel");
    hintLabel->setObjectName("tableHintLabel");
    tableFrame->setObjectName("tableCard");

    tableWidget->setHorizontalHeaderLabels({tr("编号"), tr("用户"), tr("状态")});
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->setShowGrid(false);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    tableWidget->setItem(0, 0, new QTableWidgetItem("1001"));
    tableWidget->setItem(0, 1, new QTableWidgetItem("alice"));
    tableWidget->setItem(0, 2, new QTableWidgetItem("活跃"));
    tableWidget->setItem(1, 0, new QTableWidgetItem("1002"));
    tableWidget->setItem(1, 1, new QTableWidgetItem("bob"));
    tableWidget->setItem(1, 2, new QTableWidgetItem("空闲"));
    tableWidget->setItem(2, 0, new QTableWidgetItem("1003"));
    tableWidget->setItem(2, 1, new QTableWidgetItem("cindy"));
    tableWidget->setItem(2, 2, new QTableWidgetItem("活跃"));
    tableWidget->setItem(3, 0, new QTableWidgetItem("1004"));
    tableWidget->setItem(3, 1, new QTableWidgetItem("david"));
    tableWidget->setItem(3, 2, new QTableWidgetItem("锁定"));

    tableLayout->setContentsMargins(12, 12, 12, 12);
    tableLayout->addWidget(tableWidget);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    layout->addWidget(titleLabel);
    layout->addWidget(hintLabel);
    layout->addWidget(tableFrame, 1);

    setStyleSheet(
        "#tableTitleLabel {"
        "    color: #1f2937;"
        "    font-size: 18px;"
        "    font-weight: 700;"
        "}"
        "#tableHintLabel {"
        "    color: #667085;"
        "    font-size: 12px;"
        "}"
        "#tableCard {"
        "    background-color: white;"
        "    border: 1px solid #e2e8f3;"
        "    border-radius: 16px;"
        "}"
        "QTableWidget {"
        "    border: none;"
        "    background-color: white;"
        "    alternate-background-color: #f8faff;"
        "    color: #344054;"
        "    gridline-color: transparent;"
        "}"
        "QHeaderView::section {"
        "    background-color: #f8faff;"
        "    color: #475467;"
        "    border: none;"
        "    padding: 8px;"
        "    font-weight: 700;"
        "}"
    );
}

MainWindow *TableWidget::getMainWindow() const
{
    return mainWindow;
}

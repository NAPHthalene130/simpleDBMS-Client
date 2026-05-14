#include "TableWidget.h"
#include "ui/ThemeManager.h"

#include <algorithm>

#include <QLabel>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

TableWidget::TableWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      mainWindow(mainWindow),
      messageLabel(new QLabel(this)),
      backButton(new QPushButton(tr("← 返回编辑器"), this)),
      tableViewWidget(new QTableWidget(this))
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 6, 6, 4);
    layout->setSpacing(4);

    messageLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    messageLabel->setText(tr("Ready."));

    backButton->setObjectName("tableBackButton");
    backButton->setCursor(Qt::PointingHandCursor);
    backButton->setFixedHeight(26);

    auto *bottomLayout = new QHBoxLayout();
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    bottomLayout->setSpacing(0);
    bottomLayout->addWidget(messageLabel, 1);
    bottomLayout->addWidget(backButton, 0);

    tableViewWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableViewWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    tableViewWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableViewWidget->setAlternatingRowColors(true);
    tableViewWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    tableViewWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    tableViewWidget->horizontalHeader()->setStretchLastSection(false);
    tableViewWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    tableViewWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    tableViewWidget->verticalHeader()->setVisible(true);
    tableViewWidget->verticalHeader()->setDefaultSectionSize(24);
    tableViewWidget->verticalHeader()->setMinimumSectionSize(20);
    tableViewWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    layout->addWidget(tableViewWidget, 1);
    layout->addLayout(bottomLayout);

    connect(backButton, &QPushButton::clicked, this, &TableWidget::backToEditorRequested);

    applyTableStyle();
}

MainWindow *TableWidget::getMainWindow() const
{
    return mainWindow;
}

void TableWidget::setQueryResult(const std::vector<std::string> &columns,
                                 const std::vector<std::vector<std::string>> &rows)
{
    this->columns = columns;
    this->rows = rows;
    rebuildTableView();
    showMessage(tr("Query OK, %1 rows returned.").arg(static_cast<int>(rows.size())));
}

void TableWidget::clearTable()
{
    columns.clear();
    rows.clear();
    tableViewWidget->clear();
    tableViewWidget->setRowCount(0);
    tableViewWidget->setColumnCount(0);
}

void TableWidget::rebuildTableView()
{
    tableViewWidget->clear();
    tableViewWidget->setRowCount(static_cast<int>(rows.size()));
    tableViewWidget->setColumnCount(static_cast<int>(columns.size()));

    QStringList headerLabels;
    for (const std::string &column : columns) {
        headerLabels.append(QString::fromStdString(column));
    }
    tableViewWidget->setHorizontalHeaderLabels(headerLabels);

    for (int rowIndex = 0; rowIndex < static_cast<int>(rows.size()); ++rowIndex) {
        const std::vector<std::string> &rowValues = rows[rowIndex];
        const int fillColumnCount = std::min(static_cast<int>(rowValues.size()), static_cast<int>(columns.size()));
        for (int columnIndex = 0; columnIndex < fillColumnCount; ++columnIndex) {
            auto *tableItem = new QTableWidgetItem(QString::fromStdString(rowValues[columnIndex]));
            tableViewWidget->setItem(rowIndex, columnIndex, tableItem);
        }
    }
}

void TableWidget::showMessage(const QString &message)
{
    messageLabel->setText(message);
}

void TableWidget::applyTableStyle()
{
    setStyleSheet(ThemeManager::tableWidget());
}

void TableWidget::refreshTheme()
{
    setStyleSheet(ThemeManager::tableWidget());
}

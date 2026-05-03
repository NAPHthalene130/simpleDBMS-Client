#include "TableWidget.h"

#include <algorithm>

#include <QLabel>
#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

TableWidget::TableWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      mainWindow(mainWindow),
      messageLabel(new QLabel(this)),
      tableViewWidget(new QTableWidget(this))
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 6, 6, 4);
    layout->setSpacing(4);

    messageLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    messageLabel->setText(tr("Ready."));

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
    layout->addWidget(messageLabel);

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
    setStyleSheet(
        "QWidget { background-color: #2b2d30; color: #c9ccd1; }"
        "QLabel {"
        "  color: #8f949d;"
        "  font-size: 11px;"
        "  padding: 2px 4px;"
        "}"
        "QTableWidget {"
        "  background-color: #31343a;"
        "  alternate-background-color: #2f3238;"
        "  color: #d3d6dc;"
        "  gridline-color: #3b3e45;"
        "  selection-background-color: #355a85;"
        "  selection-color: #f2f4f8;"
        "  border: 1px solid #3b3e45;"
        "  font-size: 12px;"
        "}"
        "QTableWidget::item { padding: 2px 6px; }"
        "QHeaderView::section {"
        "  background-color: #2a2d31;"
        "  color: #c1c6cf;"
        "  border: 1px solid #3b3e45;"
        "  padding: 3px 6px;"
        "  font-size: 11px;"
        "}"
        "QHeaderView::section:horizontal { text-align: left; }"
        "QScrollBar:vertical, QScrollBar:horizontal {"
        "  background-color: #2c2f34;"
        "}"
    );
}

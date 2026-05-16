/**
 * @file TableWidget.cpp
 * @author Qi
 * @brief 数据表展示组件实现文件
 * @details 实现查询结果展示、单元格编辑、插入行、删除行与 UPDATE/INSERT/DELETE 提交
 * @module ui/opePanel
 */

#include "TableWidget.h"
#include "ui/ThemeManager.h"
#include "mainwindow.h"
#include "models/network/NetworkTransferData.h"

#include <algorithm>
#include <sstream>

#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QLabel>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextStream>
#include <QVBoxLayout>

TableWidget::TableWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      mainWindow(mainWindow),
      messageLabel(new QLabel(this)),
      backButton(new QPushButton(tr("← 返回编辑器"), this)),
      submitButton(new QPushButton(tr("提交修改"), this)),
      tableViewWidget(new QTableWidget(this)),
      isEditingAllowed(false),
      isSelectAll(false),
      hasInsertRow(false),
      isContentLoading(false)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 6, 6, 4);
    layout->setSpacing(4);

    messageLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    messageLabel->setText(tr("Ready."));

    backButton->setObjectName("tableBackButton");
    backButton->setCursor(Qt::PointingHandCursor);
    backButton->setFixedHeight(26);

    submitButton->setObjectName("tableSubmitButton");
    submitButton->setCursor(Qt::PointingHandCursor);
    submitButton->setFixedHeight(26);
    submitButton->setVisible(false);

    auto *bottomLayout = new QHBoxLayout();
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    bottomLayout->setSpacing(8);
    bottomLayout->addWidget(messageLabel, 1);
    bottomLayout->addWidget(submitButton, 0);
    bottomLayout->addWidget(backButton, 0);

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
    tableViewWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    layout->addWidget(tableViewWidget, 1);
    layout->addLayout(bottomLayout);

    connect(backButton, &QPushButton::clicked, this, &TableWidget::backToEditorRequested);
    connect(submitButton, &QPushButton::clicked, this, &TableWidget::onSubmitClicked);
    connect(tableViewWidget, &QTableWidget::cellChanged, this, &TableWidget::onCellChanged);
    connect(tableViewWidget, &QTableWidget::customContextMenuRequested, this, &TableWidget::onCustomContextMenu);

    applyTableStyle();
}

MainWindow *TableWidget::getMainWindow() const
{
    return mainWindow;
}

void TableWidget::setQueryResult(const std::vector<std::string> &columns,
                                 const std::vector<std::vector<std::string>> &rows,
                                 const std::string &tableName,
                                 const std::string &dbName,
                                 const std::vector<DatabaseNode> &directoryData)
{
    isContentLoading = true;
    isEditingAllowed = false;
    isSelectAll = false;
    hasInsertRow = false;
    pendingChanges.clear();
    submitButton->setVisible(false);
    primaryKeyColumn.clear();

    this->columns = columns;
    this->rows = rows;
    this->tableName = tableName;
    this->dbName = dbName;

    if (!tableName.empty() && !directoryData.empty()) {
        QString pkCol;
        if (tryDetectPrimaryKey(directoryData, tableName, pkCol)) {
            primaryKeyColumn = pkCol;
            isSelectAll = isSelectAllQuery(directoryData);
            isEditingAllowed = true;
            hasInsertRow = isSelectAll;
        }
    }

    rebuildTableView();

    if (isEditingAllowed) {
        tableViewWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
        applyEditRestrictions();
        const int displayRows = static_cast<int>(rows.size());
        if (hasInsertRow) {
            showMessage(tr("Query OK, %1 rows. Double-click to edit, right-click for more.").arg(displayRows));
        } else {
            showMessage(tr("Query OK, %1 rows. Double-click to edit, PK column locked.").arg(displayRows));
        }
    } else {
        tableViewWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        showMessage(tr("Query OK, %1 rows returned.").arg(static_cast<int>(rows.size())));
    }

    isContentLoading = false;
}

void TableWidget::clearTable()
{
    isContentLoading = true;
    columns.clear();
    rows.clear();
    pendingChanges.clear();
    isEditingAllowed = false;
    isSelectAll = false;
    hasInsertRow = false;
    submitButton->setVisible(false);
    primaryKeyColumn.clear();
    tableViewWidget->clear();
    tableViewWidget->setRowCount(0);
    tableViewWidget->setColumnCount(0);
    tableViewWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    isContentLoading = false;
}

void TableWidget::rebuildTableView()
{
    isContentLoading = true;
    tableViewWidget->clear();

    const int dataRows = static_cast<int>(rows.size());
    const int totalRows = hasInsertRow ? dataRows + 1 : dataRows;
    const int colCount = static_cast<int>(columns.size());

    tableViewWidget->setRowCount(totalRows);
    tableViewWidget->setColumnCount(colCount);

    QStringList headerLabels;
    for (const std::string &column : columns) {
        headerLabels.append(QString::fromStdString(column));
    }
    tableViewWidget->setHorizontalHeaderLabels(headerLabels);

    if (hasInsertRow) {
        auto *starItem = new QTableWidgetItem(QStringLiteral("*"));
        starItem->setFlags(Qt::NoItemFlags);
        starItem->setTextAlignment(Qt::AlignCenter);
        tableViewWidget->setVerticalHeaderItem(kInsertRowIndex, starItem);

        for (int colIndex = 0; colIndex < colCount; ++colIndex) {
            const QString colName = QString::fromStdString(
                colIndex < colCount ? columns[colIndex] : "");
            auto *item = new QTableWidgetItem();
            if (colName.compare(primaryKeyColumn, Qt::CaseInsensitive) == 0) {
                item->setFlags(item->flags() | Qt::ItemIsEditable);
            } else {
                item->setFlags(item->flags() | Qt::ItemIsEditable);
            }
            tableViewWidget->setItem(kInsertRowIndex, colIndex, item);
        }
    }

    for (int rowIndex = 0; rowIndex < dataRows; ++rowIndex) {
        const int tableRow = hasInsertRow ? rowIndex + kDataOffset : rowIndex;
        const std::vector<std::string> &rowValues = rows[rowIndex];
        const int fillColCount = std::min(static_cast<int>(rowValues.size()), colCount);
        for (int colIndex = 0; colIndex < fillColCount; ++colIndex) {
            auto *item = new QTableWidgetItem(QString::fromStdString(rowValues[colIndex]));
            tableViewWidget->setItem(tableRow, colIndex, item);
        }
    }

    if (hasInsertRow) {
        auto *vHeader = tableViewWidget->verticalHeader();
        for (int i = 1; i < totalRows; ++i) {
            auto *label = new QTableWidgetItem(QString::number(i));
            label->setFlags(Qt::NoItemFlags);
            label->setTextAlignment(Qt::AlignCenter);
            tableViewWidget->setVerticalHeaderItem(i, label);
        }
    }

    isContentLoading = false;
}

bool TableWidget::isSelectAllQuery(const std::vector<DatabaseNode> &directoryData) const
{
    if (directoryData.empty() || tableName.empty()) {
        return false;
    }

    const QString targetTable = QString::fromStdString(tableName);
    for (const DatabaseNode &dbNode : directoryData) {
        for (const TableNode &tblNode : dbNode.getTables()) {
            if (QString::fromStdString(tblNode.getName()).compare(targetTable, Qt::CaseInsensitive) == 0) {
                const auto &allFields = tblNode.getFields();
                if (allFields.size() != columns.size()) {
                    return false;
                }
                for (size_t i = 0; i < columns.size(); ++i) {
                    if (QString::fromStdString(columns[i]).compare(
                            QString::fromStdString(allFields[i]), Qt::CaseInsensitive) != 0) {
                        return false;
                    }
                }
                return true;
            }
        }
    }
    return false;
}

QStringList TableWidget::collectInsertRowValues() const
{
    QStringList values;
    for (int c = 0; c < tableViewWidget->columnCount(); ++c) {
        QTableWidgetItem *item = tableViewWidget->item(kInsertRowIndex, c);
        values.append(item != nullptr ? item->text().trimmed() : QString());
    }
    return values;
}

bool TableWidget::validateNotEmpty()
{
    if (!hasInsertRow) {
        return true;
    }

    bool allFilled = true;
    for (int c = 0; c < tableViewWidget->columnCount(); ++c) {
        QTableWidgetItem *item = tableViewWidget->item(kInsertRowIndex, c);
        if (item != nullptr && item->text().trimmed().isEmpty()) {
            allFilled = false;
            item->setBackground(QColor("#FFE0E0"));
        }
    }
    if (!allFilled) {
        QMessageBox::warning(this, tr("数据不完整"),
            tr("插入行所有列都必须填写值，红色标记列为空。"));
    }
    return allFilled;
}

bool TableWidget::tryDetectPrimaryKey(const std::vector<DatabaseNode> &directoryData,
                                      const std::string &tableName,
                                      QString &outPkColumnName)
{
    if (directoryData.empty() || tableName.empty()) {
        return false;
    }

    const QString targetTable = QString::fromStdString(tableName);

    for (const DatabaseNode &dbNode : directoryData) {
        for (const TableNode &tblNode : dbNode.getTables()) {
            if (QString::fromStdString(tblNode.getName()).compare(targetTable, Qt::CaseInsensitive) == 0) {
                const auto &fields = tblNode.getFields();
                if (fields.empty()) {
                    return false;
                }

                const QString firstCol = QString::fromStdString(fields.front()).trimmed();
                if (firstCol.isEmpty()) {
                    return false;
                }

                for (const std::string &col : columns) {
                    if (QString::fromStdString(col).compare(firstCol, Qt::CaseInsensitive) == 0) {
                        outPkColumnName = QString::fromStdString(col);
                        return true;
                    }
                }

                outPkColumnName = firstCol;
                return true;
            }
        }
    }

    return false;
}

void TableWidget::applyEditRestrictions()
{
    for (int rowIdx = 0; rowIdx < tableViewWidget->rowCount(); ++rowIdx) {
        if (hasInsertRow && rowIdx == kInsertRowIndex) {
            continue;
        }
        for (int colIdx = 0; colIdx < tableViewWidget->columnCount(); ++colIdx) {
            QTableWidgetItem *item = tableViewWidget->item(rowIdx, colIdx);
            if (item == nullptr) {
                continue;
            }
            if (!primaryKeyColumn.isEmpty()) {
                const QString colName = QString::fromStdString(
                    colIdx < static_cast<int>(columns.size()) ? columns[colIdx] : "");
                if (colName.compare(primaryKeyColumn, Qt::CaseInsensitive) == 0) {
                    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                } else {
                    item->setFlags(item->flags() | Qt::ItemIsEditable);
                }
            }
        }
    }
}

void TableWidget::onCellChanged(int row, int col)
{
    if (isContentLoading) {
        return;
    }

    QTableWidgetItem *item = tableViewWidget->item(row, col);
    if (item == nullptr) {
        return;
    }

    const QString newValue = item->text().trimmed();
    if (newValue.isEmpty()) {
        item->setBackground(QColor("#FFE0E0"));
        if (hasInsertRow && row == kInsertRowIndex) {
            submitButton->setVisible(true);
            showMessage(tr("红色单元格为空，填写后提交。"));
            return;
        }
        return;
    }
    item->setBackground(QBrush());

    if (hasInsertRow && row == kInsertRowIndex) {
        submitButton->setVisible(true);
        showMessage(tr("Insert row modified. Click 'Submit' to insert."));
        return;
    }

    if (!isEditingAllowed) {
        return;
    }

    const QString colName = QString::fromStdString(
        col < static_cast<int>(columns.size()) ? columns[col] : "");

    if (colName.compare(primaryKeyColumn, Qt::CaseInsensitive) == 0) {
        return;
    }

    const int dataRow = hasInsertRow ? row - kDataOffset : row;
    const QString oldValue = dataRow >= 0 && dataRow < static_cast<int>(rows.size())
                                 && col < static_cast<int>(rows[dataRow].size())
                                 ? QString::fromStdString(rows[dataRow][col])
                                 : QString();

    if (newValue == oldValue) {
        pendingChanges.erase({row, col});
    } else {
        int pkColIndex = -1;
        QString pkValue;
        for (int c = 0; c < static_cast<int>(columns.size()); ++c) {
            if (QString::fromStdString(columns[c]).compare(primaryKeyColumn, Qt::CaseInsensitive) == 0) {
                pkColIndex = c;
                break;
            }
        }

        if (pkColIndex >= 0 && dataRow >= 0 && dataRow < static_cast<int>(rows.size())
            && pkColIndex < static_cast<int>(rows[dataRow].size())) {
            pkValue = QString::fromStdString(rows[dataRow][pkColIndex]);
        }

        CellChange change{row, col, colName, oldValue, newValue, primaryKeyColumn, pkValue};
        pendingChanges[{row, col}] = change;
    }

    submitButton->setVisible(!pendingChanges.empty() || hasInsertRow);
    if (!pendingChanges.empty()) {
        showMessage(tr("%1 pending change(s). Click 'Submit' to save.")
                        .arg(static_cast<int>(pendingChanges.size())));
    }
}

void TableWidget::onSubmitClicked()
{
    if (tableName.empty() || primaryKeyColumn.isEmpty()) {
        return;
    }

    bool hasInsert = false;
    if (hasInsertRow) {
        const QStringList insertValues = collectInsertRowValues();
        const bool anyFilled = std::any_of(insertValues.begin(), insertValues.end(),
                                           [](const QString &v) { return !v.isEmpty(); });
        if (anyFilled) {
            if (!validateNotEmpty()) return;

            int pkColIdx = -1;
            for (int c = 0; c < static_cast<int>(columns.size()); ++c) {
                if (QString::fromStdString(columns[c]).compare(primaryKeyColumn, Qt::CaseInsensitive) == 0) {
                    pkColIdx = c;
                    break;
                }
            }
            if (pkColIdx >= 0 && pkColIdx < insertValues.size()) {
                const QString newPk = insertValues[pkColIdx];
                for (const auto &r : rows) {
                    if (pkColIdx < static_cast<int>(r.size())
                        && QString::fromStdString(r[pkColIdx]) == newPk) {
                        QMessageBox::warning(this, tr("主键重复"),
                            tr("主键 '%1' = '%2' 已存在，无法插入重复行。")
                                .arg(primaryKeyColumn, newPk));
                        return;
                    }
                }
            }

            hasInsert = true;
        }
    }

    const bool hasEdits = !pendingChanges.empty();
    if (!hasInsert && !hasEdits) return;

    QStringList sqls;
    QString confirmMsg;

    if (hasInsert) {
        const QStringList values = collectInsertRowValues();
        QStringList quoted;
        for (const QString &v : values) {
            quoted.append(QString("'%1'").arg(v));
        }
        sqls.append(QString("INSERT INTO %1 VALUES (%2);")
                        .arg(QString::fromStdString(tableName), quoted.join(", ")));
        confirmMsg = tr("Insert 1 row into '%1'?").arg(QString::fromStdString(tableName));
    }

    if (hasEdits) {
        const int count = static_cast<int>(pendingChanges.size());
        if (hasInsert) {
            confirmMsg += "\n" + tr("Also submit %1 update(s)?").arg(count);
        } else {
            confirmMsg = tr("Submit %1 update(s) to table '%2'?")
                             .arg(count).arg(QString::fromStdString(tableName));
        }
        for (const auto &entry : pendingChanges) {
            const CellChange &change = entry.second;
            if (change.pkValue.isEmpty() || change.columnName.isEmpty()) continue;
            sqls.append(QString("UPDATE %1 SET %2 = '%3' WHERE %4 = '%5';")
                            .arg(QString::fromStdString(tableName),
                                 change.columnName, change.newValue,
                                 change.pkColumnName, change.pkValue));
        }
    }

    if (QMessageBox::question(this, tr("确认提交"), confirmMsg,
                              QMessageBox::Yes | QMessageBox::No,
                              QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    for (const QString &sql : sqls) {
        emit sqlExecutionRequested(sql);
    }

    const QString refreshSql = QString("SELECT * FROM %1;")
                                   .arg(QString::fromStdString(tableName));
    emit sqlExecutionRequested(refreshSql);

    pendingChanges.clear();
    submitButton->setVisible(false);
    if (hasInsert && hasEdits) {
        showMessage(tr("Insert + %1 update(s) submitted. Re-query to refresh.")
                        .arg(static_cast<int>(sqls.size()) - 1));
    } else if (hasInsert) {
        showMessage(tr("Insert submitted. Re-query to refresh."));
    } else {
        showMessage(tr("Update submitted. Re-query to refresh."));
    }
}

void TableWidget::onCustomContextMenu(const QPoint &pos)
{
    QTableWidgetItem *item = tableViewWidget->itemAt(pos);
    if (item == nullptr) {
        return;
    }

    const int row = item->row();
    const bool isInsertRow = hasInsertRow && row == kInsertRowIndex;
    const bool isDataRow = !isInsertRow && isEditingAllowed;

    QMenu menu(this);

    if (isDataRow) {
        menu.addAction(tr("复制单元格"), this, [this, item]() {
            QApplication::clipboard()->setText(item->text());
            showMessage(tr("已复制单元格内容。"));
        });
        menu.addAction(tr("复制整行 (TSV)"), this, [this, row]() {
            QStringList rowValues;
            for (int col = 0; col < tableViewWidget->columnCount(); ++col) {
                QTableWidgetItem *cell = tableViewWidget->item(row, col);
                rowValues.append(cell != nullptr ? cell->text() : QString());
            }
            QApplication::clipboard()->setText(rowValues.join('\t'));
            showMessage(tr("已复制整行数据 (TSV)。"));
        });
        menu.addSeparator();
        menu.addAction(tr("导出为 CSV"), this, [this]() {
            const QString filePath = QFileDialog::getSaveFileName(this, tr("导出 CSV"),
                QStringLiteral("export.csv"), tr("CSV 文件 (*.csv)"));
            if (filePath.isEmpty()) return;
            QFile file(filePath);
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                showMessage(tr("无法写入文件。"));
                return;
            }
            QTextStream out(&file);
            for (int c = 0; c < tableViewWidget->columnCount(); ++c) {
                if (c > 0) out << ',';
                out << '"' << tableViewWidget->horizontalHeaderItem(c)->text() << '"';
            }
            out << '\n';
            for (int r = (hasInsertRow ? 1 : 0); r < tableViewWidget->rowCount(); ++r) {
                for (int c = 0; c < tableViewWidget->columnCount(); ++c) {
                    if (c > 0) out << ',';
                    QTableWidgetItem *cell = tableViewWidget->item(r, c);
                    out << '"' << (cell ? cell->text() : QString()) << '"';
                }
                out << '\n';
            }
            showMessage(tr("已导出到 CSV。"));
        });
        menu.addSeparator();
        menu.addAction(tr("删除此行"), this, [this, row]() {
            if (!isEditingAllowed || tableName.empty() || primaryKeyColumn.isEmpty()) return;
            int pkColIndex = -1;
            for (int c = 0; c < static_cast<int>(columns.size()); ++c) {
                if (QString::fromStdString(columns[c]).compare(primaryKeyColumn, Qt::CaseInsensitive) == 0) {
                    pkColIndex = c;
                    break;
                }
            }
            if (pkColIndex < 0) return;
            QTableWidgetItem *pkItem = tableViewWidget->item(row, pkColIndex);
            if (pkItem == nullptr) return;
            const QString pkValue = pkItem->text().trimmed();
            if (pkValue.isEmpty()) return;

            if (QMessageBox::question(this, tr("确认删除"),
                    tr("删除 %1 行 WHERE %2 = '%3'?").arg(
                        QString::fromStdString(tableName), primaryKeyColumn, pkValue),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::No) != QMessageBox::Yes) {
                return;
            }

            const QString sql = QString("DELETE FROM %1 WHERE %2 = '%3';")
                                    .arg(QString::fromStdString(tableName),
                                         primaryKeyColumn, pkValue);
            emit sqlExecutionRequested(sql);
            const QString refreshSql = QString("SELECT * FROM %1;")
                                           .arg(QString::fromStdString(tableName));
            emit sqlExecutionRequested(refreshSql);
            showMessage(tr("Delete submitted. Re-query to refresh."));
        });
    } else if (isInsertRow) {
        menu.addAction(tr("复制单元格"), this, [this, item]() {
            QApplication::clipboard()->setText(item->text());
            showMessage(tr("已复制单元格内容。"));
        });
    } else {
        menu.addAction(tr("复制单元格"), this, [this, item]() {
            QApplication::clipboard()->setText(item->text());
            showMessage(tr("已复制单元格内容。"));
        });
    }

    menu.exec(tableViewWidget->viewport()->mapToGlobal(pos));
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

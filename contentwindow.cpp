#include "ContentWindow.h"
#include "PositiveIntDelegate.h"
#include "CellEditCommand.h"

// Optional commands for undoing add/remove:
#include <QUndoCommand>
#include <QStandardItem>

class AddRowCommand : public QUndoCommand {
public:
    AddRowCommand(QStandardItemModel* m) 
      : model(m), row(m->rowCount()) {
        setText("Add Row");
    }
    void undo() override {
        model->removeRow(row);
    }
    void redo() override {
        model->insertRow(row);
    }
private:
    QStandardItemModel* model;
    int                 row;
};

class RemoveRowsCommand : public QUndoCommand {
public:
    RemoveRowsCommand(QStandardItemModel* m, const QVector<int>& rows)
      : model(m), rowsToRemove(rows) {
        setText("Remove Rows");
        // store the data
        for (int r : rowsToRemove) {
            QVector<QVariant> rowData;
            for (int c = 0; c < model->columnCount(); ++c) {
                rowData << model->item(r, c)->data(Qt::EditRole);
            }
            backup << rowData;
        }
    }
    void undo() override {
        for (int i = 0; i < rowsToRemove.size(); ++i) {
            int r = rowsToRemove[i];
            model->insertRow(r);
            for (int c = 0; c < backup[i].size(); ++c) {
                model->setData(model->index(r, c), backup[i][c]);
            }
        }
    }
    void redo() override {
        // remove from highest to lowest
        auto rows = rowsToRemove;
        std::sort(rows.begin(), rows.end(), std::greater<>());
        for (int r : rows) {
            model->removeRow(r);
        }
    }
private:
    QStandardItemModel*       model;
    QVector<int>              rowsToRemove;
    QVector<QVector<QVariant>> backup;
};

#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QClipboard>
#include <QItemSelectionModel>

ContentWindow::ContentWindow(QWidget* parent)
    : QWidget(parent)
{
    initialize();
    connectSignals();
}

void ContentWindow::initialize()
{
    // 1) Model & Proxy
    m_model = new QStandardItemModel(0, 3, this);
    m_model->setHeaderData(0, Qt::Horizontal, tr("Name"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("Author"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Pages"));

    m_proxy = new QSortFilterProxyModel(this);
    m_proxy->setSourceModel(m_model);
    m_proxy->setSortCaseSensitivity(Qt::CaseInsensitive);

    // 2) Table
    m_table = new QTableView(this);
    m_table->setModel(m_proxy);
    m_table->setSortingEnabled(true);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    // numeric delegate for column 2
    m_table->setItemDelegateForColumn(2, new PositiveIntDelegate(this));

    // 3) ListView showing just the "Name" column
    m_listView = new QListView(this);
    m_listView->setModel(m_proxy);
    m_listView->setModelColumn(0);

    // 4) Buttons & Status
    m_addButton   = new QPushButton(tr("Add"),    this);
    m_delButton   = new QPushButton(tr("Delete"), this);
    m_statusLabel = new QLabel(tr("Ready"), this);

    // 5) Layout
    auto buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_delButton);
    buttonLayout->addStretch();

    auto mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_table,    3);
    mainLayout->addLayout(buttonLayout, 1);
    mainLayout->addWidget(m_listView, 3);

    auto bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(m_statusLabel);
    bottomLayout->addStretch();

    auto outer = new QVBoxLayout(this);
    outer->addLayout(mainLayout);
    outer->addLayout(bottomLayout);
    setLayout(outer);
    setWindowTitle(tr("Content Window"));

    // Undo stack
    m_undoStack = new QUndoStack(this);
}

void ContentWindow::connectSignals()
{
    // Record old value on *any* delegate commit
    connect(m_table->itemDelegate(), &QAbstractItemDelegate::commitData,
        this, [this](QWidget* /*editor*/) {
        QModelIndex idx = m_table->currentIndex();
        m_lastOldValue = m_proxy->data(idx, Qt::EditRole).toString();
    });

    // When itemChanged -> push edit command
    connect(m_model, &QStandardItemModel::itemChanged,
            this, [this](QStandardItem* item){
        if (m_blockUndo) return;
        int r = item->row();
        int c = item->column();
        QString newVal = item->text();
        m_undoStack->push(new CellEditCommand(m_model, r, c,
                                              m_lastOldValue, newVal));
        setModified(true);
    });

    // Add row
    connect(m_addButton, &QPushButton::clicked, this, [this](){
        m_undoStack->push(new AddRowCommand(m_model));
        setModified(true);
    });

    // Delete rows
    connect(m_delButton, &QPushButton::clicked, this, [this](){
        auto sel = m_table->selectionModel()->selectedRows();
        if (sel.isEmpty()) return;
        // map to source and collect unique rows
        QSet<int> rows;
        for (auto idx : sel)
            rows.insert(m_proxy->mapToSource(idx).row());
        QVector<int> rowList = rows.values().toVector();
        std::sort(rowList.begin(), rowList.end());
        m_undoStack->push(new RemoveRowsCommand(m_model, rowList));
        setModified(true);
    });
}

void ContentWindow::setModified(bool on)
{
    m_isModified = on;
    m_statusLabel->setText(on ? tr("Modified") : tr("Ready"));
}

void ContentWindow::copy()
{
    auto sel = m_table->selectionModel()->selectedIndexes();
    if (sel.isEmpty()) return;
    QString txt = m_proxy->data(sel.first(), Qt::DisplayRole).toString();
    QApplication::clipboard()->setText(txt);
}

void ContentWindow::cut()
{
    copy();
    auto sel = m_table->selectionModel()->selectedIndexes();
    if (sel.isEmpty()) return;
    m_blockUndo = true;
    for (auto idx : sel) {
        auto src = m_proxy->mapToSource(idx);
        m_model->setData(src, QString());
    }
    m_blockUndo = false;
    setModified(true);
}

void ContentWindow::paste()
{
    auto sel = m_table->selectionModel()->selectedIndexes();
    if (sel.isEmpty()) return;
    QString txt = QApplication::clipboard()->text();
    auto src = m_proxy->mapToSource(sel.first());
    m_model->setData(src, txt);
    setModified(true);
}

void ContentWindow::undo() { m_undoStack->undo(); }
void ContentWindow::redo() { m_undoStack->redo(); }
void ContentWindow::clear()
{
    m_model->removeRows(0, m_model->rowCount());
    setModified(false);
}

// Simple tab‐delimited serialization
void ContentWindow::write(QTextStream& out)
{
    for (int r = 0; r < m_model->rowCount(); ++r) {
        QStringList row;
        for (int c = 0; c < m_model->columnCount(); ++c) {
            row << m_model->item(r, c)->text();
        }
        out << row.join('\t') << '\n';
    }
}

void ContentWindow::read(QTextStream& in)
{
    m_model->removeRows(0, m_model->rowCount());
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.isEmpty()) continue;
        QStringList fields = line.split('\t');
        int newRow = m_model->rowCount();
        m_model->insertRow(newRow);
        for (int c = 0; c < fields.size() && c < m_model->columnCount(); ++c) {
            m_model->setData(m_model->index(newRow, c), fields[c]);
        }
    }
    setModified(false);
}
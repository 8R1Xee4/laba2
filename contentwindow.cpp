#include "contentwindow.h"


ContentWindow::ContentWindow(QWidget* parent)
    : QWidget(parent)
{
    initialize();
    connectSignals();
}

void ContentWindow::initialize()
{
    m_model = new QStandardItemModel(0, 3, this);
    m_model->setHeaderData(0, Qt::Horizontal, tr("Name"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("Author"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Pages"));

    m_proxy = new QSortFilterProxyModel(this);
    m_proxy->setSourceModel(m_model);
    m_proxy->setSortCaseSensitivity(Qt::CaseInsensitive);

    m_table = new QTableView(this);
    m_table->setModel(m_proxy);
    m_table->setSortingEnabled(true);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_table->setItemDelegateForColumn(2, new PositiveIntDelegate(this));
    m_table->setContextMenuPolicy(Qt::CustomContextMenu);

    m_listView = new QListView(this);
    m_listView->setModel(m_proxy);
    m_listView->setModelColumn(0);

    m_statusLabel = new QLabel(tr("Ready"),       this);  

    m_addButton   = new QPushButton(tr("Add"),    this);
    m_delButton   = new QPushButton(tr("Delete"), this);

    m_addButton->setIcon(QIcon(":/icons/add-row.png"));
    m_addButton->setText(QString());
    m_addButton->setIconSize(QSize(24,24));

    m_delButton->setIcon(QIcon(":/icons/delete-row.png"));
    m_delButton->setText(QString());
    m_delButton->setIconSize(QSize(24,24));

    if(!m_addButton->icon().isNull())
        qDebug(logInfo()) << "add-row.png loaded.";
    else
        qDebug(logWarning()) << "Couldn\'t load add-row.png.";

    if(!m_delButton->icon().isNull())
        qDebug(logInfo()) << "delete-row.png loaded.";
    else
        qDebug(logWarning()) << "Couldn\'t load delete-row.png.";

    m_addButton->setIcon(QIcon(":/icons/add-row.png"));
    m_addButton->setText(QString());
    m_delButton->setIcon(QIcon(":/icons/delete-row.png"));
    m_delButton->setText(QString());
    

    auto topLayout = new QHBoxLayout;
    topLayout->addWidget(m_table, 3);
    topLayout->addWidget(m_listView, 2);

    auto bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(m_addButton);
    bottomLayout->addWidget(m_delButton);
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_statusLabel);

    auto outer = new QVBoxLayout(this);
    outer->addLayout(topLayout,    5);
    outer->addLayout(bottomLayout, 1);

    setLayout(outer);
    setWindowTitle(tr("Content Window"));

    m_undoStack = new QUndoStack(this);
    qDebug(logInfo()) << "Content window initialized.";
}

void ContentWindow::connectSignals()
{
    connect(m_table->itemDelegate(), &QAbstractItemDelegate::commitData,
        this, [this](QWidget*) {
        QModelIndex idx = m_table->currentIndex();
        m_lastOldValue = m_proxy->data(idx, Qt::EditRole).toString();
    });

    connect(m_model, &QStandardItemModel::itemChanged,
            this, [this](QStandardItem* item)
    {
        if (m_blockUndo) return;
        int r = item->row();
        int c = item->column();
        QString newVal = item->text();
        m_undoStack->push(new CellEditCommand(m_model, r, c, m_lastOldValue, newVal));
        setModified(true);
    });

    connect(m_addButton, &QPushButton::clicked, this, [this]()
    {
        qDebug(logInfo()) << "New row added.";
        m_undoStack->push(new AddRowCommand(m_model));
        setModified(true);
    });

    connect(m_delButton, &QPushButton::clicked, this, [this]()
    {
        auto sel = m_table->selectionModel()->selectedRows();
        if (sel.isEmpty()) return;
        qDebug(logInfo()) << sel.size() << " rows deleted.";
        QSet<int> rows;
        for (auto idx : sel)
            rows.insert(m_proxy->mapToSource(idx).row());
        QVector<int> rowList = rows.values().toVector();
        std::sort(rowList.begin(), rowList.end());
        m_undoStack->push(new RemoveRowsCommand(m_model, rowList));
        setModified(true);
    });

    connect(m_table, &QWidget::customContextMenuRequested, this, [this](const QPoint& pos){
        QMenu menu;
        QAction* actCopy  = menu.addAction(tr("Copy"));
        QAction* actCut   = menu.addAction(tr("Cut"));
        QAction* actPaste = menu.addAction(tr("Paste"));
        QAction* chosen = menu.exec(m_table->viewport()->mapToGlobal(pos));
        if (chosen == actCopy)  copy();
        if (chosen == actCut)   cut();
        if (chosen == actPaste) paste();
    });

    qDebug(logInfo()) << "Content window connected";
}

void ContentWindow::setModified(bool on)
{
    m_isModified = on;
    m_statusLabel->setText(on ? tr("Modified") : tr("Saved"));
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
    for (auto idx : sel) 
    {
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

void ContentWindow::undo()
{
  m_blockUndo = true;
  m_undoStack->undo();
  m_blockUndo = false;
  setModified(true);
}

void ContentWindow::redo()
{
  m_blockUndo = true;
  m_undoStack->redo();
  m_blockUndo = false;
  setModified(true);
}
void ContentWindow::clear()
{
    m_model->removeRows(0, m_model->rowCount());
    setModified(false);
}

void ContentWindow::write(QTextStream& out)
{
    const int rows = m_model->rowCount();
    const int cols = m_model->columnCount();

    for (int r = 0; r < rows; ++r) 
    {
        QStringList rowText;
        for (int c = 0; c < cols; ++c) {
            QModelIndex idx = m_model->index(r, c);
            rowText << idx.data(Qt::EditRole).toString();
        }
        out << rowText.join('\t') << '\n';
    }
}

void ContentWindow::read(QTextStream& in)
{
    m_model->removeRows(0, m_model->rowCount());
    while (!in.atEnd()) 
    {
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
#ifndef CONTENTWINDOW_H
#define CONTENTWINDOW_H

#include <QWidget>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QUndoStack>
#include <QTableView>
#include <QListView>
#include <QPushButton>
#include <QLabel>
#include <QTextStream>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QClipboard>
#include <QItemSelectionModel>
#include <QMenu>
#include <QIcon>
#include <QSize>

#include "positiveintdelegate.h"
#include "celleditcommand.h"
#include "loghandler.h"
#include "addremoverows.h"

class AddRowCommand;
class RemoveRowsCommand;

class ContentWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ContentWindow(QWidget* parent = nullptr);

    bool isModified() const    { return m_isModified; }
    void setModified(bool on);

    void copy();
    void cut();
    void paste();
    void undo();
    void redo();
    void clear();
    void write(QTextStream& out);
    void read(QTextStream& in);

private:
    void initialize();
    void connectSignals();

    bool                      m_isModified  = false;
    bool                      m_blockUndo   = false;
    QString                   m_lastOldValue;
    QStandardItemModel*       m_model       = nullptr;
    QSortFilterProxyModel*    m_proxy       = nullptr;
    QUndoStack*               m_undoStack   = nullptr;

    QTableView*               m_table       = nullptr;
    QListView*                m_listView    = nullptr;
    QPushButton*              m_addButton   = nullptr;
    QPushButton*              m_delButton   = nullptr;
    QLabel*                   m_statusLabel = nullptr;
};

#endif // CONTENTWINDOW_H
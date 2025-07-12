#ifndef ADDREMOVEROWS_H
#define ADDREMOVEROWS_H

#include <QUndoCommand>
#include <QStandardItem>

class AddRowCommand : public QUndoCommand {
public:
    AddRowCommand(QStandardItemModel* m);
    void undo() override;
    void redo() override;
private:
    QStandardItemModel* model;
    int                 row;
};

class RemoveRowsCommand : public QUndoCommand {
public:
    RemoveRowsCommand(QStandardItemModel* m, const QVector<int>& rows);
    
    void undo() override;
    void redo() override;
private:
    QStandardItemModel*         model;
    QVector<int>                rowsToRemove;
    QVector<QVector<QVariant>>  backup;
};

#endif // ADDREMOVEROWS_H
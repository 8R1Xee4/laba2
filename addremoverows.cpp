#include "addremoverows.h"

AddRowCommand::AddRowCommand(QStandardItemModel* m)  : model(m), row(m->rowCount()) 
{
    setText("Add Row");
}
void AddRowCommand::undo() 
{
    model->removeRow(row);
}
void AddRowCommand::redo() 
{
    QList<QStandardItem*> items;
    items << new QStandardItem(QObject::tr("New book"))
          << new QStandardItem(QObject::tr("Author"))
          << new QStandardItem(QString::number(1));
    model->insertRow(row, items);
}

RemoveRowsCommand::RemoveRowsCommand(QStandardItemModel* m, const QVector<int>& rows) : model(m), rowsToRemove(rows) 
{
    setText("Remove Rows");
    for (auto r : rowsToRemove) 
    {
        QVector<QVariant> rowData;
        for (int c = 0; c < model->columnCount(); c++) 
        {
           QModelIndex idx = model->index(r, c);
            rowData << idx.data(Qt::EditRole);
        }
        backup << rowData;
    }
}

void RemoveRowsCommand::undo() 
{
    for (int i = 0; i < rowsToRemove.size(); i++) 
    {
        int r = rowsToRemove[i];
        model->insertRow(r);
        for (int c = 0; c < backup[i].size(); c++) 
        {
            model->setData(model->index(r, c), backup[i][c], Qt::EditRole);
        }
    }
}
    
void RemoveRowsCommand::redo()
{
    auto rows = rowsToRemove;
    std::sort(rows.begin(), rows.end(), std::greater<>());
    for (int r : rows) 
    {
        model->removeRow(r);
    }
}
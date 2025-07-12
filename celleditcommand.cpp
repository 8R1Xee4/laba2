#include "celleditcommand.h"

CellEditCommand::CellEditCommand(QStandardItemModel* model,
                  int row, int col,
                  const QString& before,
                  const QString& after,
                  QUndoCommand* parent)
    : QUndoCommand(parent)
    , m(model), r(row), c(col)
    , oldValue(before), newValue(after)
{
  setText(QString("Edit cell (%1,%2)").arg(r).arg(c));
}

void CellEditCommand::undo()
{
  m->setData(m->index(r,c), oldValue);
}

void CellEditCommand::redo() 
{
  m->setData(m->index(r,c), newValue);
}
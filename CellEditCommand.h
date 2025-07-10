// CellEditCommand.h
#pragma once
#include <QUndoCommand>
#include <QStandardItemModel>

class CellEditCommand : public QUndoCommand {
  QStandardItemModel*  m;
  int                  r, c;
  QString              oldValue, newValue;
public:
  CellEditCommand(QStandardItemModel* model,
                  int row, int col,
                  const QString& before,
                  const QString& after,
                  QUndoCommand* parent = nullptr)
    : QUndoCommand(parent)
    , m(model), r(row), c(col)
    , oldValue(before), newValue(after)
  {
    setText(QString("Edit cell (%1,%2)").arg(r).arg(c));
  }
  void undo() override {
    m->setData(m->index(r,c), oldValue);
  }
  void redo() override {
    m->setData(m->index(r,c), newValue);
  }
};
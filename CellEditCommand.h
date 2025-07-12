#ifndef CELLEDITCOMMAND_H
#define CELLEDITCOMMAND_H
#include <QUndoCommand>
#include <QStandardItemModel>

class CellEditCommand : public QUndoCommand {
public:
  CellEditCommand(QStandardItemModel* model,
                  int row, int col,
                  const QString& before,
                  const QString& after,
                  QUndoCommand* parent = nullptr);
            
  void undo() override;
  void redo() override;

private:
  QStandardItemModel*  m;
  int                  r, c;
  QString              oldValue, newValue;
};
#endif // CELLEDITCOMMAND_H
#ifndef POSITIVEINTDELEGATE_H
#define POSITIVEINTDELEGATE_H

#include <QStyledItemDelegate>
#include <QLineEdit>
#include <QIntValidator>

class PositiveIntDelegate : public QStyledItemDelegate {
public:
  PositiveIntDelegate(QObject* parent=nullptr);
  
  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const override;
};

#endif // POSITIVEINTDELEGATE_H
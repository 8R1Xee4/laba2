#include "positiveintdelegate.h"

PositiveIntDelegate::PositiveIntDelegate(QObject* parent) : QStyledItemDelegate(parent) 
{
  
}

QWidget* PositiveIntDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const
{
    if(index.column()==2) {
        QLineEdit* e = new QLineEdit(parent);
        e->setValidator(new QIntValidator(1, 1000000000, e));
        return e;
    }
    return QStyledItemDelegate::createEditor(parent, {}, index);
}


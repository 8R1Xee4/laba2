#include "loghandler.h"
#include "infodialog.h"
#include "ui_infodialog.h"

InfoDialog::InfoDialog(QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::InfoDialog)
{
  initializeMainWindow();
  connectSlots();
}

InfoDialog::~InfoDialog()
{
  delete ui;
}

void InfoDialog::closeEvent(QCloseEvent *event)
{
  qDebug(logInfo()) << "Closing dialog window...";
  event->accept();
}

void InfoDialog::initializeMainWindow()
{
  this->setModal(true);
  ui->setupUi(this);
  this->setWindowTitle("Info");
  ui->label->setText("<html><head/><body><p>Это программа для работы с таблицей, представляющей собой набор информации о различных книгах, такой как название, имя автора и количество страниц. Инструкция по использованию:</p><p>1. С помощью кнопок снизу можно создать или удалить строку.</p><p>2. В новой строке дважды кликните по ячейке: таким образом вы сможете изменить содержимое ячейки.</p></body></html>");
}

void InfoDialog::connectSlots()
{
  connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &buttonPress);
  connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &buttonPress);
}

void InfoDialog::buttonPress()
{
  qDebug(logInfo()) << "InfoDialog was closed.";
  this->close();
}
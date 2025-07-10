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
  ui->label->setText("<html><head/><body><p>Это программа для расчёта степени, факториала и корня числа. Инструкция по использованию:</p><p>1. Выберите поле для ввода и введите число.</p><p>2. Выберите в комбинированном списке что будете считать.</p><p>3. Нажмите кнопку рассчитать, результат появится в текстовом виде под кнопкой.</p></body></html>");
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
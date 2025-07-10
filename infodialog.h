#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>
#include<QCloseEvent>

namespace Ui {
  class InfoDialog;    // forward-declare the uic-generated class
}

class InfoDialog : public QDialog {
    Q_OBJECT
  public:
    explicit InfoDialog(QWidget *parent = nullptr);
    ~InfoDialog();

  protected:
    void initializeMainWindow();
    void connectSlots();
    void closeEvent(QCloseEvent *event);

  public slots:
    void buttonPress();

  private:
    Ui::InfoDialog *ui;
};

#endif // INFODIALOG_H
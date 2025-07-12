#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBoxLayout>
#include <QtWidgets>
#include <QDebug>
#include <QDialog>
#include "infodialog.h"
#include "loghandler.h"
#include "contentwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

public slots:
    void slotNewFileAct();
    void slotOpenFileAct();
    void slotSaveFileAct();
    void slotSaveFileAsAct();
    void slotExitAct();

    void slotUndoAct();
    void slotRedoAct();
    void slotCutAct();
    void slotPasteAct();
    void slotCopyAct();

    void slotAboutAct();
protected:
    void initializeLogHandler();
    void initializeMainWindow();
    void initializeMenuBar();
    void initializeApp();

    void closeEvent(QCloseEvent *ev);

    void showAppInfo();
    void onExit();
    void newDocument();

    bool     maybeSave();
    bool     saveFile();
    bool     saveFileAs();
    bool     saveToFile(const QString &fileName);


private:
    QString appName = "Laba1";

    Ui::MainWindow *ui;
    ContentWindow* app;

    QString  currentFile;       // empty == untitled

    QMenu* fileMenu;
    QMenu* editMenu;
    QMenu* helpMenu;

    QAction* newFileAct;
    QAction* openFileAct;
    QAction* saveFileAct;
    QAction* saveFileAsAct;
    QAction* exitAct;

    QAction* undoAct;
    QAction* redoAct;
    QAction* cutAct;
    QAction* pasteAct;
    QAction* copyAct;

    QAction* aboutAct;
};
#endif // MAINWINDOW_H

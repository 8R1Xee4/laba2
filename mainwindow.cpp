#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
  {
    initializeLogHandler();
    initializeMainWindow();
    initializeApp();
    initializeMenuBar();
    this->newDocument();
  }

MainWindow::~MainWindow()
  {
    delete ui;
  }

// ----------------------------------------------

void MainWindow::initializeLogHandler()
{
  logHandler.addFile(QString("logs/") + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh.mm.ss.log"));
  logHandler.addTextStream(*(new QTextStream(stdout)));
  qInstallMessageHandler(&LogHandler::messageHandler);
  qDebug(logInfo()) << "LogHandler initialized.";
}

void MainWindow::initializeMainWindow()
{
  ui->setupUi(this);
  this->setFixedSize(500,300);
  qDebug(logInfo()) << "Main window initialized.";
}

void MainWindow::initializeMenuBar()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  newFileAct  = fileMenu->addAction(tr("&New"), QKeySequence::New, this, &MainWindow::slotNewFileAct);
  openFileAct = fileMenu->addAction(tr("&Open…"), QKeySequence::Open, this, &MainWindow::slotOpenFileAct);
  saveFileAct = fileMenu->addAction(tr("&Save"), QKeySequence::Save, this, &MainWindow::slotSaveFileAct);
  saveFileAsAct = fileMenu->addAction(tr("&Save as…"), QKeySequence::SaveAs, this, &MainWindow::slotSaveFileAsAct);
  fileMenu->addSeparator();
  exitAct     = fileMenu->addAction(tr("E&xit"), QKeySequence::Quit, this, &MainWindow::slotExitAct);

  editMenu = menuBar()->addMenu(tr("&Edit"));
  undoAct  = editMenu->addAction(tr("&Undo"), QKeySequence::Undo, this, &MainWindow::slotUndoAct);
  redoAct  = editMenu->addAction(tr("&Redo"), QKeySequence::Redo, this, &MainWindow::slotRedoAct);
  editMenu->addSeparator();
  copyAct  = editMenu->addAction(tr("&Copy"), QKeySequence::Copy, this, &MainWindow::slotCopyAct);
  pasteAct = editMenu->addAction(tr("&Paste"), QKeySequence::Paste, this, &MainWindow::slotPasteAct);
  cutAct   = editMenu->addAction(tr("Cu&t"), QKeySequence::Cut, this, &MainWindow::slotCutAct);

  helpMenu = menuBar()->addMenu(tr("&Help"));
  aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::slotAboutAct);
  
  menuBar()->show();
  qDebug(logInfo()) << "Menu bar initialized.";
}

void MainWindow::initializeApp()
{
  app = new ContentWindow(this);
  this->setCentralWidget(app);
  qDebug(logInfo()) << "App initialized.";
}

// ------------------------------------------------------------

void MainWindow::slotNewFileAct()
{
  qDebug(logInfo()) << "New file action called.";
  this->newDocument();
  qDebug(logInfo()) << "New document created.";
}

void MainWindow::slotSaveFileAct()
{
  qDebug(logInfo()) << "Save file action called.";
  if (saveFile())
    qDebug(logInfo()) << "Document saved.";
  else
    qDebug(logWarning()) << "Save cancelled or failed.";
}

void MainWindow::slotSaveFileAsAct()
{
  qDebug(logInfo()) << "Save file as action called.";
  if(saveFileAs())
    qDebug(logInfo()) << "Saved successfully.";
  else 
    qDebug(logWarning()) << "Save cancelled or failed.";
}

bool MainWindow::saveFileAs()
{
  QString fn = QFileDialog::getSaveFileName(this, tr("Save As"), QString(), tr("Text Files (*.txt);;All Files (*)"));
  if (fn.isEmpty())
    return false;

  return saveToFile(fn);
}

bool MainWindow::saveFile()
{
  if (currentFile.isEmpty())
    return saveFileAs();
  return saveToFile(currentFile);
}

bool MainWindow::saveToFile(const QString &fileName)
{
  qDebug(logInfo()) << "Trying to save to " << fileName;
  QFile file(fileName);
  if (!file.open(QFile::WriteOnly | QFile::Text)) {
    QMessageBox::warning( this, tr("Error"), tr("Cannot write file %1:\n%2").arg(QDir::toNativeSeparators(fileName), file.errorString()));
    qDebug(logWarning) << tr("Cannot write file %1:\n%2").arg(QDir::toNativeSeparators(fileName), file.errorString());
    return false;
  }

  QTextStream out(&file);
  app->write(out);
  file.close();

  currentFile = fileName;
  app->setModified(false);
  setWindowTitle(QFileInfo(currentFile).fileName() + tr(" - ") + appName);
  return true;
}

bool MainWindow::maybeSave()
{
  if(!app->isModified())
    return true;
  qDebug(logInfo()) << "Asking user to save.";

  auto ret = QMessageBox::warning(
    this, appName,
    tr("The document has been modified.\nDo you want to save your changes?"),
    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save
  );

  switch (ret) {
    case QMessageBox::Save:
      return saveFile();
    case QMessageBox::Cancel:
      return false;
    default:
      return true;
  }
}

void MainWindow::slotExitAct()
{
  qDebug(logInfo()) << "Exit action.";
  this->close();
}

void MainWindow::slotOpenFileAct()
{
  qDebug(logInfo()) << "Open file action called.";
  QString path = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), tr("Text Files (*.txt);;All Files (*)"));
  if(path.isEmpty()) return;

  QFile f(path);
  if(!f.open(QIODevice::ReadOnly | QIODevice::Text)){
    QMessageBox::warning(this, tr("Error"), tr("Cannot open file %1").arg(path));
    qDebug(logWarning()) << tr("Cannot open file %1").arg(path);
    return;
  }

  QTextStream in(&f);
  app->read(in);
  f.close();

  currentFile = path;
  app->setModified(false);
  this->setWindowTitle(QFileInfo(currentFile).fileName() + tr(" - ") + appName);

  qDebug(logInfo()) << "Opened: " << path;
}

// ---------------------------------------------

void MainWindow::slotUndoAct()   
{ 
  qDebug(logInfo()) << "Undo action.";
  app->undo(); 
}
void MainWindow::slotRedoAct()   
{ 
  qDebug(logInfo()) << "Redo action.";
  app->redo(); 
}
void MainWindow::slotCutAct()    
{ 
  qDebug(logInfo()) << "Cut action.";
  app->cut(); 
}
void MainWindow::slotPasteAct()  
{ 
  qDebug(logInfo()) << "Paste action.";
  app->paste(); 
}
void MainWindow::slotCopyAct()  
{ 
  qDebug(logInfo()) << "Copy action.";
  app->copy(); 
}

// ---------------------------------------------

void MainWindow::slotAboutAct()
{
  qDebug(logInfo()) << "About action.";
  showAppInfo();
}

// ------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent *event)
{
  qDebug(logInfo()) << "Close event called.";
  if (!maybeSave()) 
  {
    event->ignore();
    return;
  }
  onExit();
  qDebug(logInfo()) << "Closing in action.";
  event->accept();
}

// ---------------------------------------------------------------

void MainWindow::onExit()
{
  qDebug(logInfo()) << "On exit event called.";
}

void MainWindow::showAppInfo()
{
  qDebug(logInfo()) << "Dialog called.";
  InfoDialog* dlg = new InfoDialog(this);
  dlg->setAttribute(Qt::WA_DeleteOnClose);
  dlg->show();
}

void MainWindow::newDocument()
{
  if (!maybeSave()) return;
  app->clear();
  app->setModified(false);
  currentFile.clear();
  setWindowTitle(tr("Untitled - ") + appName);
}
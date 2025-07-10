#include"loghandler.h"

Q_LOGGING_CATEGORY(logDebug,    "Debug")
Q_LOGGING_CATEGORY(logInfo,     "Info")
Q_LOGGING_CATEGORY(logWarning,  "Warning")
Q_LOGGING_CATEGORY(logCritical, "Critical")

LogHandler* LogHandler::l_instance = nullptr;
LogHandler logHandler;

LogHandler::LogHandler() : l_mutex()
{
    if(l_instance)
    {
        qFatal("LogHandler: only one instance allowed.");
    }
    l_instance = this;
}

LogHandler::~LogHandler()
{
    close();
    l_instance = nullptr;
}

void LogHandler::write(QTextStream& out, QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");
    switch (type)
    {
        case QtInfoMsg:     out << "INF "; break;
        case QtDebugMsg:    out << "DBG "; break;
        case QtWarningMsg:  out << "WRN "; break;
        case QtCriticalMsg: out << "CRT "; break;
        case QtFatalMsg:    out << "FTL "; break;
        default: type = QtDebugMsg;
    }
    out << context.category << ": "
        << msg << Qt::endl;
    out.flush();
    if(type == QtFatalMsg)
    {
        qDebug(logInfo()) << "Fatal error: can't continue the work. Closing the application...";
        QApplication::exit(-1);
    }
}

void LogHandler::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if(l_instance)
    {
        l_instance->handle(type, context, msg);
    }
}

void LogHandler::handle(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QMutexLocker locker(&l_mutex);
    for(int i = 0; i < l_streams.size(); i++)
    {       
        this->write(*l_streams[i], type, context, msg);
    }   
}

void LogHandler::addTextStream(QTextStream& s)
{
    QMutexLocker locker(&l_mutex);
    l_streams.append(&s);
}

void LogHandler::addFile(const QString& filepath)
{
    QMutexLocker locker(&l_mutex);
    QFileInfo fi(filepath);
    QDir dir = fi.absoluteDir();
    if(!dir.exists()) 
    {
        if(!QDir().mkpath(dir.absolutePath()))
        {
            qDebug(logWarning()) << "Failed to create log directory: " << dir.absolutePath();
            return;
        }
    }


    QFile* file = new QFile(filepath);
    if(!file->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug(logWarning()) << "Could not open file " << filepath << " for writing.";
        delete file;
    } else {
        l_files.append(file);
        l_ownedStreams.append(new QTextStream(file));
        l_streams.append(*l_ownedStreams.rbegin());
    }
}

void LogHandler::close()
{
    QMutexLocker locker(&l_mutex);
    for(int i = 0; i < l_files.size(); i++)
    {
        if(l_files[i]->isOpen()) l_files[i]->close();
    }
    qDeleteAll(l_ownedStreams);
    qDeleteAll(l_files);

    l_files.clear();
    l_ownedStreams.clear();
    l_streams.clear();
}
#ifndef LOGHANDLER_H
#define LOGHANDLER_H

#include <QLoggingCategory>
#include <QMessageLogContext>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDateTime>
#include <QApplication>
#include <QFileInfo>
#include <QDir>

Q_DECLARE_LOGGING_CATEGORY(logDebug)
Q_DECLARE_LOGGING_CATEGORY(logInfo)
Q_DECLARE_LOGGING_CATEGORY(logWarning)
Q_DECLARE_LOGGING_CATEGORY(logCritical)

class LogHandler
{
public:
    LogHandler();
    ~LogHandler();

    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    void addTextStream(QTextStream& s);
    void addFile(const QString& filename);
    void close();

    LogHandler(const LogHandler&) = delete;
    LogHandler& operator=(const LogHandler&) = delete;
protected:
    void write(QTextStream& out, QtMsgType type, const QMessageLogContext &context, const QString &msg);
    void handle(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private:
    QMutex                  l_mutex;
    QVector<QTextStream*>   l_streams;
    QVector<QTextStream*>   l_ownedStreams;
    QVector<QFile*>         l_files;

    static LogHandler*      l_instance;

};

extern LogHandler logHandler;

#endif // LOGHANDLER_H
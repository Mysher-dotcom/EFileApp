#ifndef LOGGERHELPER_H
#define LOGGERHELPER_H

#include <qapplication.h>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QtMsgHandler>
#include <QMessageLogContext>
#include <QMutex>
#include <QStandardPaths>
#include <QDir>

void myMsgOutput(QtMsgType type, const QMessageLogContext &context, const QString& msg)
{
    QString logFolderPath = QString("/%1/%2")
                            .arg(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation))
                            .arg("EFile_log");
    QDir newFolder;
    if(newFolder.exists(logFolderPath) == false)
    {
        newFolder.mkpath(logFolderPath);
    }
    QString logFilePath = QString("/%1/%2.log")
                         .arg(logFolderPath)
                         .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd"));

    static QMutex mutex;
    mutex.lock();
    QString time=QDateTime::currentDateTime().toString(QString("[yyyy-MM-dd HH:mm:ss:zzz]"));
    QString mmsg;
    switch(type)
    {
    case QtDebugMsg:
        mmsg=QString("%1: Debug:\t%2 (file:%3, line:%4, func: %5)").arg(time).arg(msg).arg(QString(context.file)).arg(context.line).arg(QString(context.function));
        break;
    case QtInfoMsg:
        mmsg=QString("%1: Info:\t%2 (file:%3, line:%4, func: %5)").arg(time).arg(msg).arg(QString(context.file)).arg(context.line).arg(QString(context.function));
        break;
    case QtWarningMsg:
        mmsg=QString("%1: Warning:\t%2 (file:%3, line:%4, func: %5)").arg(time).arg(msg).arg(QString(context.file)).arg(context.line).arg(QString(context.function));
        break;
    case QtCriticalMsg:
        mmsg=QString("%1: Critical:\t%2 (file:%3, line:%4, func: %5)").arg(time).arg(msg).arg(QString(context.file)).arg(context.line).arg(QString(context.function));
        break;
    case QtFatalMsg:
        mmsg=QString("%1: Fatal:\t%2 (file:%3, line:%4, func: %5)").arg(time).arg(msg).arg(QString(context.file)).arg(context.line).arg(QString(context.function));
        abort();
    }
    QFile file(logFilePath);
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << mmsg << "\r\n";
    file.flush();
    file.close();
    mutex.unlock();
}






#endif // LOGGERHELPER_H

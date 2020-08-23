#ifndef MERGEPDFTHREAD_H
#define MERGEPDFTHREAD_H

#include <QObject>
#include "Jpeg.h"


class MergePDFThread : public QObject
{
    Q_OBJECT
public:
    explicit MergePDFThread(QObject *parent = nullptr);

    QStringList fileList;
    void setFileList(QStringList list);

    //信号
signals:
    void signalSingleFileMergeOver(QString filePath,int fileIndex);
    void signalOver();

    //槽
public slots:
    void startMerge();

};

#endif // MERGEPDFTHREAD_H

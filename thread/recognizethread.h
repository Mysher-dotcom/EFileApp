#ifndef RECOGNIZETHREAD_H
#define RECOGNIZETHREAD_H

#include <QObject>

class RecognizeThread : public QObject
{
    Q_OBJECT
public:
    explicit RecognizeThread(QObject *parent = nullptr);
    QString regcognizeBarcode(QString path);//识别条码


    //信号
signals:
    void signalSendCode(QString filePath,QString code,QStringList codeInfo);
    void signalOver();


    //槽
public slots:
    void startRecognize();

};

#endif // RECOGNIZETHREAD_H

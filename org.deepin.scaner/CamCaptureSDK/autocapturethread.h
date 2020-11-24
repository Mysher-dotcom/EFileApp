#ifndef AUTOCAPTURETHREAD_H
#define AUTOCAPTURETHREAD_H

#include <QObject>
class AutoCaptureThread;
class AutoCaptureThread : public QObject
{
    Q_OBJECT
public:
    explicit AutoCaptureThread(QObject *parent = nullptr);

    //static AutoCaptureThread *g_autoCaptureThread;
signals:
    void AutoCapture();
public slots:
    void AutoCaptureFun();
};

#endif // AUTOCAPTURETHREAD_H

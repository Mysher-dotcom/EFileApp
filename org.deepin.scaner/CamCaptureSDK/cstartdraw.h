#ifndef CSTARTDRAW_H
#define CSTARTDRAW_H

#include <QObject>

class CStartDraw : public QObject
{
    Q_OBJECT
public:
    explicit CStartDraw(QObject *parent = nullptr);

signals:
    void SendVideoBuffer(void *data, int size,int w,int h);
public slots:
    void handleVideoBuffer(void *data, int size,int w,int h);
};

#endif // CSTARTDRAW_H

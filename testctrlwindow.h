#ifndef TESTCTRLWINDOW_H
#define TESTCTRLWINDOW_H

#include <DMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QComboBox>

DWIDGET_USE_NAMESPACE

namespace Ui {
class TestCtrlWindow;
}

class TestCtrlWindow : public DMainWindow
{
    Q_OBJECT

public:
    explicit TestCtrlWindow(QWidget *parent = nullptr);
    ~TestCtrlWindow();

    void initUI();
    void initConnection();
    void writeMsg(QString msg);

    static int doScanReceiveCB(uchar*data,int size,int w,int h,int nBpp,int nDPI);
    static int doScanStatuCB(int nStatus);


    QPushButton *initSANEBtn;
    QPushButton *getParBtn;
    QPushButton *scanBtn;
    QComboBox *colorCb;
    QComboBox *scanmodeCb;
    QComboBox *dpiCb;
    QTextEdit *logText;

private:
    Ui::TestCtrlWindow *ui;


private slots:
    //槽
    void slotInitSANE();
    void slotGetPar();
    void slotScan();

};

#endif // TESTCTRLWINDOW_H

#ifndef SCANWINDOW_H
#define SCANWINDOW_H

#include <QMainWindow>
#include <DMainWindow>
#include "DTitlebar"
#include <QPushButton>
#include <QListWidget>
#include <QGraphicsDropShadowEffect>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QThread>
#include <DListView>
#include <QSortFilterProxyModel>
#include <DSpinner>
#include <DLabel>
#include "thread/doscanthread.h"
#include <DSpinner>
#include "camscansdk.h"
#include "cmimage.h"
#include "MImage.h"
#include "errortype.h"
#include <QStackedLayout>
#include <QFile>
#include <QDesktopServices>
#include <QUrl>
#include <QFontDatabase>

DWIDGET_USE_NAMESPACE

namespace Ui {
class ScanWindow;
}

class ScanWindow : public DMainWindow
{
    Q_OBJECT

public:
    explicit ScanWindow(int devIndex,QMap<int ,QString>,QWidget *parent = nullptr);
    ~ScanWindow();    
    void closeEvent(QCloseEvent *event);//窗口关闭

    //函数
    void initUI();//初始化界面
    void successScanUI();//成功扫描UI，出现图像列表和底部扫描按钮
    void showImageList();//显示图像列表
    void changeScanBtnStyle(bool isScanning);//修改扫描按钮样式，true=扫描中样式，false=普通样式    
    void startScanThread();//开始扫描线程
    void copyFile(QString oldFilePath, QString newFilePath);//拷贝文件
    QString getCopyFileName(QString filePath);//获取拷贝文件名，用于重名文件覆盖
    char* substrend(char * str, int n);
    int WriteFile(unsigned char* srcData,int len,FILE * destfb);
private:
    Ui::ScanWindow *ui;

    //变量
    int deviceIndex;//设备编号
    QMap<int ,QString> parMap;//扫描参数
    bool listViewIsShow = false;//图片列表是否已显示
    QStringList fileNames;//图像路径集合
    DoScanThread * doScanThread;//开始扫描
    QThread * subThread;//扫描线程

    //控件
    QPushButton *scanBtn;//扫描按钮
    QHBoxLayout* winLayout;//窗口容器布局
    QWidget *mainWidget;//主容器
    QWidget *tWidget;//上部容器
    QWidget *bWidget;//下部容器
    QVBoxLayout *mainVLayout;//容器布局
    QVBoxLayout *tVLayout;//上部容器布局
    QHBoxLayout *bHLayout;//下部容器布局
    QWidget *tipWidget;//扫描中图标提示容器
    QVBoxLayout *pVLayout ;//扫描中图标提示布局
    QLabel *pScanningFontTip ;//扫描中文字提示
    DSpinner *pScanningIconTip;//扫描中图标提示
    QListView *imgList;//图像列表
    QStandardItemModel *pModel;//图像Item
    QSortFilterProxyModel *proxyModel;    
    DSpinner *scanBtnSpinner;//扫描按钮上的动画
    DLabel *scanBtnText;//扫描按钮上的文字

signals:
    //信号

private slots:
    //槽;
    void onBtnScanClicked();//扫描按钮点击
    void addItem(QString path);//增加图像Item到列表
    void closeThread();//关闭线程
    void slotFinishThread();//线程停止触发
    void slotScanError(QString);//扫描中错误信号
    void slotListDoubleClicked(const QModelIndex);//列表项双击
    void slotScanSaveImage(char* data,int nSize,int w,int h,int nBpp,int nDPI);//存图

};

#endif // SCANWINDOW_H

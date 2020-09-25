#ifndef CAMERAWINDOW_H
#define CAMERAWINDOW_H

#include <DMainWindow>
#include "DTitlebar"
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QWidget>
#include <QPushButton>
#include "DIconButton"
#include <DListView>
#include "listview/picitemdelegate.h"
#include "listview/listviewitemdata.h"
#include <QSortFilterProxyModel>
#include <QLabel>
#include <QWheelEvent>
#include <QTimer>
#include <QTimerEvent>
#include <DWidget>
#include <DLabel>

DWIDGET_USE_NAMESPACE

namespace Ui {
class CameraWindow;
}

class CameraWindow : public DMainWindow
{
    Q_OBJECT

public:
    explicit CameraWindow(int devIndex,QMap<QString ,QString> map,QWidget *parent = nullptr);
    ~CameraWindow();

     void paintEvent(QPaintEvent *event);//窗口渲染
     void closeEvent(QCloseEvent *event);//窗口关闭     
     void wheelEvent(QWheelEvent *event);//鼠标滑轮事件(禁用)     
     bool isDarkType();//当前系统是否为深色主题
     /*
     void mousePressEvent(QMouseEvent *event);//窗口中鼠标按下
     void mouseMoveEvent(QMouseEvent *event);//窗口中鼠标移动
     void mouseReleaseEvent(QMouseEvent *event);//窗口中鼠标抬起
     */

     void copyFile(QString oldFilePath, QString newFilePath);//拷贝文件
     QString getCopyFileName(QString filePath);//获取拷贝文件名，用于重名文件覆盖
    void DecodeString(char* pszin,char (*pszout)[100],int& num,char mark);

private:
    Ui::CameraWindow *ui;
    //函数
    void initUI();//初始化界面
    void initConnection();//按钮信号槽连接
    void initSDK();//初始化SDK
    void shot();//拍摄图像
    void addItem(QString path);//增加图像Item到列表
    static int ReceiveImage(void *data, int size,int w,int h,int nFormatType);//视频绘制回调
    static int callBackAutoCaptureFun(long nState);
    void timerEvent(QTimerEvent *event);

    //变量
    int nDeviceIndex;//拍摄仪设备下标
    QMap<QString ,QString> parMap;//拍摄参数
    int formatIndex;//视频格式
    int videoWidth;//视频宽
    int videoHeight;//视频高
    int isCut;//0-no crop 1-autocrop 2-MultiCrop
    qreal nVideoRotateAngle;//视频旋转角度
    qreal nVideoScaleSize;//视频缩放尺寸
    QStringList fileNameList;//图像路径集合
    int VIDEO_MAX_SIZE = 1000;//视频最大缩放值
    int VIDEO_MIN_SIZE = -100;//视频最小缩放值
    /*
    bool isMousePressed;
    QPoint mousePressedPoint;
    int nMoveX;
    int nMoveY;
    */
    int nShotType;//拍摄类型（0=手动，1=定时拍）
    int nShotTime;//定时拍秒
    QTimer *shotTimer;//定时拍计时器
    bool isStartShotTimer;//是否已经开始定时拍
    //是否支持拍书功能
    bool m_bIsBook;
    //记录拍书按钮次数
    int m_nBookCount;
    //控件
    QVBoxLayout *winVLayout;//窗口布局
    QHBoxLayout *mainVLayout;//主布局
    QVBoxLayout *leftVLayout;//左侧布局
    QHBoxLayout *videoHLayout;//视频布局
    QVBoxLayout *leftBottomVLayout;//左侧下部布局
    QVBoxLayout *rightVLayout;//右侧布局
    QVBoxLayout *rightTopVLayout;//右侧上部布局
    QVBoxLayout *rightBottomVLayout;//右侧下部布局
    QHBoxLayout *toolBarHLayout;//工具栏布局
    DWidget *mainWidget;//主容器
    DWidget *leftWidget;//左侧容器
    DWidget *videoWidget;//视频容器
    DWidget *leftBottomWidget;//左侧下部容器
    DWidget *rightWidget;//右侧容器
    DWidget *rightTopWidget;//右侧上部容器
    DWidget *rightBottomWidget;//右侧下部容器
    DWidget *toolBarWidget;//工具栏容器
    QPushButton *scanBtn;//扫描按钮
    DIconButton *oneTOneBtn;//1:1按钮
    DIconButton *zoomInBtn;//放大按钮
    DIconButton *zoomOutBtn;//缩小按钮
    DIconButton *rotateLeftBtn;//左旋按钮
    DIconButton *rotateRightBtn;//右旋按钮
    DIconButton *cutBtn;//裁切按钮
    DIconButton *bookBtn;//拍书按钮
    DListView *imgListView;//图像列表
    QStandardItemModel *listViewModel;//缩略图Item数据
    QSortFilterProxyModel *listViewProxyModel;//代理数据，用于列表的筛选和排序
    QPushButton *saveBtn;//保存按钮
    //QFrame *previewImageFrame;//预览的图像
    QWidget *timerWidget;//定时拍容器
    DLabel *timerIconLabel;//定时拍图标
    DLabel *timerLabel;//定时拍秒


signals:
    //信号
    void signalWindowClosed();//窗口关闭信号

private slots:
    //槽
    void slotScanBtnClicked();//扫描按钮
    void slotOneTOneBtnClicked();//1:1按钮
    void slotZoomInBtnClicked();//放大按钮
    void slotZoomOutBtnClicked();//缩小按钮
    void slotRotateLeftBtnClicked();//左旋按钮
    void slotRotateRightBtnClicked();//右旋按钮
    void slotCutBtnClicked();//裁切按钮
    void slotBookBtnClicked();//拍书
    void slotSaveBtnClicked();//保存按钮
    void slotImgListViewPressed(const QModelIndex);//图片列表点击
    void slotListDoubleClicked(const QModelIndex);//列表项双击
    void slotStartShotTime();
    void slotShotTimerUpdate();

};

#endif // CAMERAWINDOW_H

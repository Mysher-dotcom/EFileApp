#ifndef SCANMANAGERWINDOW_H
#define SCANMANAGERWINDOW_H


#include <DMainWindow>
#include "DTitlebar"
#include "DIconButton"
#include "dlabel.h"
#include "dstyle.h"
#include "dtextedit.h"
#include <QPushButton>
#include "scanwindow.h"
#include <DListWidget>
#include "thread/getscannerinfothread.h"
#include <QThread>
#include <QObject>
#include <DListView>
#include <DRadioButton>
#include <DScrollArea>
#include <QMetaType>
#include <QVariant>
#include <DSpinner>
#include "camerawindow.h"
#include "thread/getcamerainfothread.h"
#include "camerawindow.h"

DWIDGET_USE_NAMESPACE

namespace Ui {
class ScanManagerWindow;
}

class ScanManagerWindow : public DMainWindow
{
    Q_OBJECT

public:
    explicit ScanManagerWindow(QWidget *parent = nullptr);
    ~ScanManagerWindow();


private:
    Ui::ScanManagerWindow *ui;

    //函数
    void initUI();//初始化界面
    void showDeviceListUI();//显示设备列表控件UI
    void openScannerThread();//开启SANE线程
    void closeScannerThread();//关闭SANE线程
    void openCameraThread();//开启拍摄仪线程
    void closeCameraThread();//关闭拍摄仪线程

    /*
     * 新增设备Item
     * type=设备类型(0=拍摄仪，1=扫描仪)
     * type=设备名称
     * model=设备型号
     * status=设备状态(空闲或者忙碌)
     * index=设备下标
     */
    void addDeviceItem(int type,QString name,QString model,QString status,int index);

    /*
     * 获取指定设备参数
     * devType=设备类型，0=拍摄仪，1=扫描仪
     * devIndex=设备下标index
     */
    void getDevicePar(int devType,int devIndex);

    /*
     * 显示设备参数UI
     * titleName=抬头名称(如：扫描设置),
     * parName=参数名称(如：分辨率)，
     * parIndex=参数下标，从设备中读取到的顺序
     * parType=参数类型(0=文本框，1=下拉框))
     * parValue=参数值（如：200DPI<<300DPI）
     * isAddTitleTopMargin=抬头是否需要加入上边距
     * isLastPar=是否为最后一个参数,布局需要加一个addStretch来控制参数行居上
     */
    void showDeviceParUI(QString titleName,QString parName,QString parIndex,int parType, QStringList parValue,bool isAddTitleTopMargin=false,bool isLastPar=false);
    void showImgEditParUI();//图像处理参数UI
    void getImgEditPar();//获取图像处理参数
    void showImgFormatAndTypeUI(bool isCamera = false);//图像格式和颜色模式UI
    void showShotTypeUI();//拍摄方式UI


    //变量
    bool isInitStart;//是否初始化，用于获取设备参数后结束线程的判断
    bool isHaveScanner;//是否拥有扫描仪设备
    bool isHaveCamera;//是否拥有拍摄仪设备
    int nCurrentDeviceType;//当前设备类型，0=拍摄仪，1=扫描仪
    int nCurrentDeviceIndex;//当前设备下标

    /* 扫描仪参数键值对集合
     * int=参数的下标
     * QStringList=参数的值集合
     * 如：<1,"Color24,Gary8,Lineart">
     */
    QMap<int ,QStringList> scannerParMap;

    /* 扫描仪选中参数键值对集合
     * int=参数的下标
     * QString=参数的值
     * 如：<1,"Color24">
     */
    QMap<int ,QString> scannerChoiseParMap;
    QMap<QString ,QStringList> cameraParMap;//拍摄仪参数键值对集合<参数名称,参数值集合>，如：<颜色模式,"Color24,Gary8,Lineart">
    QMap<QString ,QString> cameraChoiseParMap;//拍摄仪选中参数键值对集合<参数名称,参数值>，如：<颜色模式,"Color24">
    GetScannerInfoThread *getScannerInfoThread;//获取扫描仪信息
    QThread *subSANEThread;//SANE扫描仪线程
    GetCameraInfoThread *getCameraInfoThread;//获取拍摄仪信息
    QThread *subCameraThread;//拍摄仪线程

    //控件
    ScanWindow *scanWindow;//扫描窗口
    CameraWindow *cameraWindow;//拍摄窗口
    QPushButton *scanBtn;//扫描按钮
    QHBoxLayout* mainLayout ;//主布局
    QWidget *lWidget;//左侧容器
    QWidget *rWidget;//右侧容器
    QWidget *rtWidget;//右侧-上部容器
    QWidget *rbWidget;//右侧-下部容器
    QVBoxLayout *lVLayout;//左侧容器布局
    QVBoxLayout *rVLayout;//右侧容器布局
    QVBoxLayout *rtVLayout;//右侧-上部容器布局
    QHBoxLayout *rbHLayout;//右侧-下部容器布局
    QWidget *lTipWidget;//左侧提示容器
    QHBoxLayout *lTipHLayout;//左侧提示布局
    DSpinner *lSpinner;//左侧提示信息动画
    QLabel *lLbl;//左侧提示信息("正在检测扫描设备...")
    QWidget * rtipWidget;//右上提示信息容器
    QVBoxLayout *rtipLayout ;//右上提示信息布局
    QLabel *rtNLbl;//右上占位空提示
    QLabel *rtLbl;//右上提示信息("正在检测")
    QLabel *rbLbl;//右上提示信息2("若长时间没有反应，请更换USB接口，重启设备或电脑尝试。");
    DListView *devListView;//左侧设备列表容器
    QStandardItemModel *pModel;//设备Item
    QSortFilterProxyModel *proxyModel;//过滤模板
    DRadioButton *noCutrbBtn;//不裁剪单选按钮
    DRadioButton *singleCutrbBtn;//单图裁剪单选按钮
    DRadioButton *mulCutrbBtn;//多图裁剪单选按钮
    DRadioButton *noWaterrbBtn;//无水印单选按钮
    DRadioButton *fontWaterrbBtn;//文字水印单选按钮
    DTextEdit *fontWaterText;//文字水印文本
    DIconButton *colorBtn;//文字水印颜色选择按钮
    DRadioButton *imageWaterBtn;//图片水印单选按钮
    DComboBox *docTypeCBB;//文档类型下拉框(0=原始文档，1=文档优化，2=彩色优化，3=红印文档优化，4=反色，5=滤红)
    DCheckBox *filterDenoisingCKB;//去噪复选框
    DCheckBox *repairCKB;//缺角修复复选框
    DComboBox *imgFormatCBB;//图片格式下拉框
    DComboBox *imgTypeCBB;//图片颜色模式下拉框（彩色，灰度，黑白）
    DRadioButton *shotTypeManualrbBtn;//手动拍摄单选按钮
    DRadioButton *shotTypeTimerrbBtn;//定时拍摄单选按钮
    DComboBox *shotTypeTimerCBB;//定时拍摄时间下拉框
    DRadioButton *shotTypeAutoCaptureBtn;//自动拍摄单选按钮

signals:
    //信号

private slots:
    //槽
    void slotScanButtonClicked();//扫描按钮点击
    void slotFinishThread();//SANE线程停止触发
    void slotDevListPressed(const QModelIndex index);//设备列表点击事件,获取指定设备的参数
    void slotComboBoxCurrentIndexChanged(const int index);//下拉框选项改变
    void slotColorButtonClicked();//颜色按钮点击
    void slotNoScannerUI();//无设备UI

    /*
     * SANE设备信息(信号槽不可以传复杂类型的参数，需要转化成QVariant作为参数)
     * devInfo 设备参数键值对集合
     * int=设备类型，0=拍摄仪，1=扫描仪
     * QStringList=设备信息的值集合
     * 如：<1,"扫描仪,UNIS F3135,空闲">
     */
    void slotScannerInfoUI(QVariant qv, const QString &str);

    /*
     * SANE设备参数信息
     * parInfo 设备参数键值对集合
     * int=参数的下标
     * QStringList=参数的值集合
     * 如：<1,"Color24,Gary8,Lineart">
     */
    void slotScannerParInfoUI(QVariant qv, const QString &str);
    void slotNoCameraUI();//无拍摄仪UI
    void slotCameraInfoUI(QVariant qv, const QString &str);//拍摄仪信息
    void slotCameraParInfoUI(QVariant qv,  const QString &str);//拍摄仪参数信息

    void slotGetDeviceList();//获取设备列表并显示到UI

};

#endif // SCANMANAGERWINDOW_H

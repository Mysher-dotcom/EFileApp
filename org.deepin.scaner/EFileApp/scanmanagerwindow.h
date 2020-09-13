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
#include <QStackedLayout>
#include <QGridLayout>

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
    void dragEnterEvent(QDragEnterEvent *e);//筛选拖拽事件
    void dropEvent(QDropEvent *e);//处理拖拽事件
    void closeEvent(QCloseEvent *event);

private:
    Ui::ScanManagerWindow *ui;

    //***函数
    void initUI();//初始化界面
    void initNODeviceUI();//加载无设备UI
    void initCheckingDeviceUI();//加载检测设备中UI
    void initDeviceInfoUI();//加载设备信息UI
    void initInstallDeviceUI();//加载安装设备UI

    //新增设备Item(设备类型(0=拍摄仪，1=扫描仪),设备名称,设备型号,设备状态(空闲或者忙碌),设备下标,是否授权)
    void addDeviceItem(int type,QString name,QString model,QString status,int index,int license);

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
    void showImgEditParUI(bool isLicense=true);//图像处理参数UI
    void getImgEditPar();//获取图像处理参数
    void showImgFormatAndTypeUI(bool isCamera = false,bool isLicense=true);//图像格式和颜色模式UI
    void showShotTypeUI(bool isLicense=true);//拍摄方式UI
    void setUILastSetting();//设置UI的LastSetting


    //***变量
    QLocale locale;//获取系统信息，判断语言是否要翻译
    bool isScanClose;//是否为点击扫描按钮关闭窗口,窗口关闭时不用再次记录参数
    int nCurrentDeviceType;//当前设备类型，0=拍摄仪，1=扫描仪
    int nCurrentDeviceIndex;//当前设备下标
    QString currentDeviceModel;//当前设备型号,如Canon dr-c230
    bool currentDeviceIsCanUse;//当前设备是否可用

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
    int emitAdder = 0;//由哪个页面点击新增按钮（0=无设备UI，1=设备信息UI）
    QString localDriverFilePath;//本地驱动路径

    //***控件
    ScanWindow *scanWindow;//扫描窗口
    CameraWindow *cameraWindow;//拍摄窗口
    QPushButton *scanBtn;//扫描按钮
    QStackedLayout *mainSLayout;//主布局
    DIconButton *addDeviceBtn;//新增设备按钮
    DIconButton *addDeviceBtn2;//新增设备按钮-无设备UI里
    DListView *devListView;//左侧设备列表容器
    QStandardItemModel *pModel;//设备Item
    QSortFilterProxyModel *proxyModel;//过滤模板
    DSpinner *lSpinner;//左侧提示信息动画
    QVBoxLayout *rtVLayout;//右侧-上部容器布局(参数容器布局)
    QWidget *rtWidget;//右侧-上部容器(参数容器)
    QHBoxLayout *rbHLayout;//右侧-下部容器布局(扫描按钮)
    DIconButton *refreshDeviceBtn;//刷新设备
    DIconButton *backBtn;//返回按钮
    QPushButton *installOnlineBtn;//在线安装按钮
    QPushButton *installLocalBtn;//本地安装按钮
    QLabel *installTitleLabel;//标题栏
    QStackedLayout *installSLayout;//安装控件区域
    QWidget *localParWidget;//本地安装区域
    DComboBox *vendorCBB;//厂商
    DComboBox *modelCBB;//型号
    DComboBox *driverCBB;//驱动
    QLabel *localDriverTipLabel;//导入本地驱动文本提示
    QLabel *localDriverTipLabel2;//导入本地驱动文本提示2
    QPushButton *importDriverBtn;//导入本地驱动按钮
    QPushButton *installBtn;//安装按钮


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
    void signalSearchDevice();//搜索设备

private slots:
    //槽
    void slotAddDeviceButtonClicked();//新增设备按钮
    void slotAddDeviceButtonClicked2();//新增设备按钮-无设备UI里
    void slotScanButtonClicked();//扫描按钮点击
    void slotDevListPressed(const QModelIndex index);//设备列表点击事件,获取指定设备的参数
    void slotComboBoxCurrentIndexChanged(const int index);//下拉框选项改变
    void slotFontWaterTextChanged();//水印文本框文本改变事件
    void slotColorButtonClicked();//颜色按钮点击
    void slotNoScannerUI();//无设备UI
    void slotGetDeviceList();//获取设备列表并显示到UI
    void slotBackBtnClicked();//返回按钮
    void slotInstallOnlineBtnClicked();//在线安装按钮
    void slotInstallLocalBtnClicked();//本地安装按钮
    void slotImportDriverBtnClicked();//导入本地驱动按钮
    void slotInstallBtnClicked();//安装驱动
    void slotRefreshDevice();//更新设备列表

};

#endif // SCANMANAGERWINDOW_H

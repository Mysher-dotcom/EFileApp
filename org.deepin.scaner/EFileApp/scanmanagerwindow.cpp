#include "scanmanagerwindow.h"
#include "ui_scanmanagerwindow.h"

#include <QFile>
#include <qdir.h>
#include <QListWidget>
#include <QGraphicsDropShadowEffect>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <dbaseline.h>
#include <QDesktopWidget>
#include <DListWidget>
#include <QDebug>
//#include <sanehelper/sanehelper.h>
#include <DComboBox>
#include "listview/listviewitemdata.h"
#include "listview/deviceitemdelegate.h"
#include "camscansdk.h"
#include "helper/globalhelper.h"
#include <QStandardPaths>
#include "mainwindow.h"
#include <DColorDialog>
#include <DCheckBox>
#include <QButtonGroup>
#include "camscansdk.h"
#include "cmimage.h"
#include "MImage.h"
#include <QDateTime>
#include "helper/deviceinfohelper.h"
#include <QFileDialog>
#include <QDesktopServices>
#include <QDragEnterEvent>//用于拖拽
#include <QMimeData>//用于拖拽
#include <QScreen>


ScanManagerWindow::ScanManagerWindow(QWidget *parent) :
    DMainWindow(parent),
    ui(new Ui::ScanManagerWindow)
{
    ui->setupUi(this);

    initUI();
    this->setAcceptDrops(true);//整个窗口支持拖拽，暂时
    isScanClose = false;//是否为点击扫描按钮关闭窗口,窗口关闭时不用再次记录参数    
    isClickedInstallBtn = false;//是否点击了安装按钮

    QString defaultDeviceModelFilePath =  DeviceInfoHelper::readValue(DeviceInfoHelper::getDeviceListInfoFilePath(),
                                             "default",
                                             "config");
    int nIsWaterMark= DeviceInfoHelper::readValue(defaultDeviceModelFilePath,"imgset","isWaterMark").toInt();
    if(nIsWaterMark == 1)
    {
        isNoWatermark = false;
    }
    else
    {
        isNoWatermark = true;
    }

    //线程执行中，不加载设备列表
    if( GlobalHelper::getDeviceInfoIsOver == true)
    {
        qDebug()<<"加载设备列表";
        slotGetDeviceList();
    }
    else
    {
        qDebug()<<"后台正在加载设备参数，等待...";
        mainSLayout->setCurrentIndex(1);
    }

}

ScanManagerWindow::~ScanManagerWindow()
{
    delete ui;
}

//当前系统是否为深色主题
bool ScanManagerWindow::isDarkType()
{
    DGuiApplicationHelper *guiAppHelp = DGuiApplicationHelper::instance();
    if(guiAppHelp->themeType() == DGuiApplicationHelper::ColorType::DarkType)
    {
        //深色主题
        return true;
    }
    else
    {
        //浅色主题
        return false;
    }
}

void ScanManagerWindow::closeEvent(QCloseEvent *event)
{
    this->hide();//先隐藏窗口
    if(isScanClose == false && GlobalHelper::getDeviceInfoIsOver == true && isNoDevice == false)
    {
         getImgEditPar();//从UI上获取图像处理参数
         isScanClose = true;
    }
    if(isClickedInstallBtn)
    {
         slotRefreshDevice();//更新设备列表
     }

}

//筛选拖拽事件
void ScanManagerWindow::dragEnterEvent(QDragEnterEvent *e)
{
    e->acceptProposedAction();//放行，执行dropEvent()
}

//处理拖拽事件
void ScanManagerWindow::dropEvent(QDropEvent *e)
{
    QList<QUrl> urls = e->mimeData()->urls();
    if(!urls.isEmpty())
    {
        QString filePath = urls.first().toLocalFile();
        QFileInfo fi(filePath);
        if(fi.exists())
        {
            localDriverFilePath = filePath;
            localDriverTipLabel->setText(tr("Driver path:")+fi.fileName());
            localDriverTipLabel->setToolTip(tr("Driver path:")+localDriverFilePath);
            installBtn->setEnabled(true);
            localDriverTipLabel2->setText("");
        }
    }
}

//初始化界面
void ScanManagerWindow::initUI()
{
    //*****窗口设置*******
    this->resize(850,680);//窗口初始尺寸
    this->setMinimumSize(QSize(850,680));//窗口最小尺寸
    this->titlebar()->setTitle("");//标题栏文字设为空
    this->titlebar()->setMenuVisible(false);//隐藏标题栏
    setWindowIcon(QIcon(":/img/logo/logo-16.svg"));// 状态栏图标
    this->titlebar()->setIcon(QIcon(":/img/logo/logo-16.svg"));//标题栏图标
    this->setWindowTitle(tr("Scan Assistant"));//开始菜单栏上鼠标悬浮在窗口上显示的名称
    this->setWindowFlags(this->windowFlags()&~Qt::WindowMinimizeButtonHint);//最小化按钮隐藏

    mainSLayout = new QStackedLayout();//窗口布局
    ui->centralwidget->setLayout(mainSLayout);

    initNODeviceUI();//加载无设备UI
    initCheckingDeviceUI();//加载检测设备中UI
    initDeviceInfoUI();//加载设备信息UI
    initInstallDeviceUI();//加载安装设备UI


}

//加载无设备UI
void ScanManagerWindow::initNODeviceUI()
{
    //左侧
    DLabel *leftTopLbl=new DLabel ();
    leftTopLbl->setText(tr("Scanners"));
    if(isDarkType()==true)
    {
        leftTopLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgb(192,198,212);font-size:14px");
    }
    else
    {
        leftTopLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgba(65,77,104,1);font-size:14px");
    }
    //刷新设备-无设备UI里
    refreshDeviceBtn2 = new DIconButton (nullptr);
    if(isDarkType()==true)
    {
        refreshDeviceBtn2->setIcon(QIcon(":/img/refresh_device_dark.svg"));
    }
    else
    {
        refreshDeviceBtn2->setIcon(QIcon(":/img/refresh_device.svg"));
    }
    refreshDeviceBtn2->setFixedSize(QSize(30, 30));
    //refreshDeviceBtn2->setIconSize(QSize(20,20));
    refreshDeviceBtn2->setToolTip(tr("Refresh"));
    connect(refreshDeviceBtn2, SIGNAL(clicked()), this, SLOT(slotRefreshDevice()));

    DWidget *leftTopGroupWidget = new DWidget();
    QHBoxLayout *leftTopGroupLayout = new QHBoxLayout();
    leftTopGroupWidget->setLayout(leftTopGroupLayout);
    leftTopGroupLayout->addWidget(leftTopLbl,0,Qt::AlignLeft);
    leftTopGroupLayout->addWidget(refreshDeviceBtn2,0,Qt::AlignRight);

    DLabel *leftLbl=new DLabel ();
    leftLbl->setText(tr("No scanners found"));
    if(isDarkType()==true)
    {
        leftLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgb(192,198,212);font-size:14px");
    }
    else
    {
        leftLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgba(65,77,104,1);font-size:14px");
    }
    //新增设备按钮-无设备UI里
    addDeviceBtn2 = new DIconButton(nullptr);
    if(isDarkType()==true)
    {
        addDeviceBtn2->setIcon(QIcon(":/img/addDevice_dark.svg"));//图标
    }
    else
    {
        addDeviceBtn2->setIcon(QIcon(":/img/addDevice.svg"));//图标
    }
    addDeviceBtn2->setFixedSize(QSize(48, 48));//按钮尺寸
    addDeviceBtn2->setIconSize(QSize(48,48));//图标尺寸
    addDeviceBtn2->raise();//置顶
    addDeviceBtn2->setStyleSheet("background:transparent;");
    connect(addDeviceBtn2, SIGNAL(clicked()), this, SLOT(slotAddDeviceButtonClicked2()));

    DWidget *leftTopWidget = new DWidget();
    QVBoxLayout *leftTopLayout = new QVBoxLayout();
    leftTopWidget->setLayout(leftTopLayout);
    leftTopLayout->addWidget(leftTopGroupWidget,0,Qt::AlignTop);
    leftTopLayout->addWidget(leftLbl,1,Qt::AlignHCenter);

    DWidget *leftBottomWidget = new DWidget();
    QGridLayout *leftGridLayout = new QGridLayout();
    leftGridLayout->addWidget(leftTopWidget, 0, 0, 1, 1);
    leftGridLayout->addWidget(addDeviceBtn2, 0, 0, 1, 1,Qt::AlignBottom | Qt::AlignCenter);
    leftBottomWidget->setLayout(leftGridLayout);


    //右侧
    DLabel *rightTopLbl=new DLabel();//右上空白占位提示信息

    DLabel *rightLbl=new DLabel ();
    rightLbl->setText(tr("Make sure your scanner is powered on and connected"));
    if(isDarkType()==true)
    {
        rightLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgb(192,198,212);font-size:14px");
    }
    else
    {
        rightLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgba(65,77,104,1);font-size:14px");
    }
    //控件加入布局中
    QHBoxLayout *mainLayout = new QHBoxLayout();
    QVBoxLayout *leftLayout = new QVBoxLayout();
    QVBoxLayout *rightLayout = new QVBoxLayout();
    DWidget *mainWidget = new DWidget();
    DWidget *leftWidget = new DWidget();
    DWidget *rightWidget = new DWidget();
    leftWidget->setFixedWidth(300);//左侧容器固定宽度300px
    if(isDarkType()==true)
    {
        GlobalHelper::setWidgetBackgroundColor(leftWidget,QColor(255,255,255,12),false);//左侧容器设置背景白色
        GlobalHelper::setWidgetBackgroundColor(rightWidget,QColor(255,255,255,12),false);//右侧容器设置背景白色
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(leftWidget,QColor(255,255,255,255),false);//左侧容器设置背景白色
        GlobalHelper::setWidgetBackgroundColor(rightWidget,QColor(255,255,255,255),false);//右侧容器设置背景白色
    }
    mainLayout->addWidget(leftWidget);
    mainLayout->addWidget(rightWidget);
    mainWidget->setLayout(mainLayout);
    leftWidget->setLayout(leftLayout);
    rightWidget->setLayout(rightLayout);
    //leftLayout->addWidget(leftTopLbl,0,Qt::AlignTop);
    //leftLayout->addWidget(leftLbl,1,Qt::AlignHCenter);
    leftLayout->addWidget(leftBottomWidget);
    rightLayout->addWidget(rightTopLbl);//右侧-上部加一个空白的label，用于和左侧的“扫描仪设备”提示保持一致，否则右侧的提示信息比左侧提示信息下移一些
    rightLayout->addWidget(rightLbl,1,Qt::AlignCenter);

    mainSLayout->addWidget(mainWidget);

}

//加载检测设备中UI
void ScanManagerWindow::initCheckingDeviceUI()
{
    //左侧
    DLabel *leftTopLbl=new DLabel ();
    leftTopLbl->setText(tr("Scanners"));
    if(isDarkType()==true)
    {
        leftTopLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgb(192,198,212);font-size:14px");
    }
    else
    {
        leftTopLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgba(65,77,104,1);font-size:14px");
    }

    lSpinner = new DSpinner();//左侧提示动画
    lSpinner->setFixedSize(QSize(28,28));
    lSpinner->show();
    lSpinner->start();
    DLabel *leftLbl=new DLabel ();
    leftLbl->setText(tr("Detecting..."));
    if(isDarkType()==true)
    {
        leftLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgb(192,198,212);font-size:14px");
    }
    else
    {
        leftLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgba(65,77,104,1);font-size:14px");
    }

    //右侧
    DLabel *rightTopLbl=new DLabel();//右上空白占位提示信息

    DLabel *rightLbl=new DLabel ();
    //rightLbl->setText(tr("正在检测"));
    if(isDarkType()==true)
    {
        rightLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgb(192,198,212);font-size:14px");
    }
    else
    {
        rightLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgba(65,77,104,1);font-size:14px");
    }
    DLabel *rightLbl2=new DLabel ();
    rightLbl2->setText(tr("If it takes a long time, use another USB port, or restart your scanner and PC"));
    if(isDarkType()==true)
    {
        rightLbl2->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgb(192,198,212);font-size:14px");
    }
    else
    {
        rightLbl2->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgba(65,77,104,1);font-size:14px");
    }

    //控件加入布局中
    QHBoxLayout *mainLayout = new QHBoxLayout();
    QVBoxLayout *leftLayout = new QVBoxLayout();
    QVBoxLayout *rightLayout = new QVBoxLayout();
    DWidget *mainWidget = new DWidget();
    DWidget *leftWidget = new DWidget();
    DWidget *rightWidget = new DWidget();
    leftWidget->setFixedWidth(300);//左侧容器固定宽度300px
    if(isDarkType()==true)
    {
        GlobalHelper::setWidgetBackgroundColor(leftWidget,QColor(255,255,255,12),false);//左侧容器设置背景色
        GlobalHelper::setWidgetBackgroundColor(rightWidget,QColor(255,255,255,12),false);//右侧容器设置背景色
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(leftWidget,QColor(255,255,255,255),false);//左侧容器设置背景白色
        GlobalHelper::setWidgetBackgroundColor(rightWidget,QColor(255,255,255,255),false);//右侧容器设置背景白色
    }
    mainLayout->addWidget(leftWidget);
    mainLayout->addWidget(rightWidget);
    mainWidget->setLayout(mainLayout);
    leftWidget->setLayout(leftLayout);
    rightWidget->setLayout(rightLayout);

    QHBoxLayout *leftLayout2 = new QHBoxLayout();
    QVBoxLayout *rightLayout2 = new QVBoxLayout();
    DWidget *leftWidget2 = new DWidget();
    DWidget *rightWidget2 = new DWidget();
    leftWidget2->setLayout(leftLayout2);
    rightWidget2->setLayout(rightLayout2);
    leftLayout2->addWidget(lSpinner);
    leftLayout2->addWidget(leftLbl);
    rightLayout2->addWidget(rightLbl,0,Qt::AlignHCenter);
    rightLayout2->addWidget(rightLbl2,1,Qt::AlignHCenter);

    leftLayout->addWidget(leftTopLbl,0,Qt::AlignTop);
    leftLayout->addWidget(leftWidget2,1,Qt::AlignHCenter);
    rightLayout->addWidget(rightTopLbl);//右侧-上部加一个空白的label，用于和左侧的“扫描仪设备”提示保持一致，否则右侧的提示信息比左侧提示信息下移一些
    rightLayout->addWidget(rightWidget2,1,Qt::AlignCenter);

    mainSLayout->addWidget(mainWidget);
}

//加载设备信息UI
void ScanManagerWindow::initDeviceInfoUI()
{
    //***左侧
    DLabel *leftTopLbl=new DLabel ();
    leftTopLbl->setText(tr("Scanners"));
    if(isDarkType()==true)
    {
        leftTopLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgb(192,198,212);font-size:14px");
    }
    else
    {
        leftTopLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgba(65,77,104,1);font-size:14px");
    }
    refreshDeviceBtn = new DIconButton (nullptr);//刷新设备
    refreshDeviceBtn->setIcon(QIcon(":/img/refresh_device.svg"));
    refreshDeviceBtn->setFixedSize(QSize(30, 30));
    refreshDeviceBtn->setIconSize(QSize(20,20));
    refreshDeviceBtn->setToolTip(tr("Refresh"));
    connect(refreshDeviceBtn, SIGNAL(clicked()), this, SLOT(slotRefreshDevice()));


    DWidget *leftTopWidget = new DWidget();
    QHBoxLayout *leftTopLayout = new QHBoxLayout();
    leftTopWidget->setLayout(leftTopLayout);
    leftTopLayout->addWidget(leftTopLbl,0,Qt::AlignLeft);
    leftTopLayout->addWidget(refreshDeviceBtn,0,Qt::AlignRight);


    //设备列表容器
    devListView = new DListView();
    devListView->setSelectionMode(QAbstractItemView::SingleSelection);
    devListView->setEditTriggers(QAbstractItemView::NoEditTriggers);//双击Item屏蔽可编辑
    devListView->setMovement(QListView::Static);//禁止拖拽Item
    devListView->setViewMode(QListView::IconMode);
    devListView->setResizeMode(QListView::Adjust);
    pModel = new QStandardItemModel ();
    DeviceItemDelegate *pItemDelegate=new DeviceItemDelegate (this);
    devListView->setItemDelegate(pItemDelegate);
    proxyModel = new QSortFilterProxyModel (devListView);
    proxyModel->setSourceModel(pModel);
    devListView->setModel(proxyModel);
    connect(devListView,SIGNAL(pressed(const QModelIndex)),this,SLOT(slotDevListPressed(const QModelIndex)));//设备列表点击信号槽

    //新增设备按钮
    addDeviceBtn = new DIconButton(nullptr);
    if(isDarkType()==true)
    {
        addDeviceBtn->setIcon(QIcon(":/img/addDevice_dark.svg"));//图标
    }
    else
    {
        addDeviceBtn->setIcon(QIcon(":/img/addDevice.svg"));//图标
    }
    addDeviceBtn->setFixedSize(QSize(48, 48));//按钮尺寸
    addDeviceBtn->setIconSize(QSize(48,48));//图标尺寸
    addDeviceBtn->raise();//置顶
    addDeviceBtn->setStyleSheet("background:transparent;");
    connect(addDeviceBtn, SIGNAL(clicked()), this, SLOT(slotAddDeviceButtonClicked()));

    DWidget *leftBottomWidget = new DWidget();
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(devListView, 0, 0, 1, 1);
    gridLayout->addWidget(addDeviceBtn, 0, 0, 1, 1,Qt::AlignBottom | Qt::AlignCenter);
    leftBottomWidget->setLayout(gridLayout);

    //***右侧
    //扫描按钮
    scanBtn=new QPushButton ();
    scanBtn->setText(tr("Start"));
    scanBtn->setFixedSize(QSize(240,36));
    connect(scanBtn, SIGNAL(clicked()), this, SLOT(slotScanButtonClicked()));

    //控件加入布局中
    QHBoxLayout *mainLayout = new QHBoxLayout();//主布局
    QVBoxLayout *leftLayout = new QVBoxLayout();//左布局
    QVBoxLayout *rightLayout = new QVBoxLayout();//右布局
    rtVLayout = new QVBoxLayout();//参数布局
    rbHLayout = new QHBoxLayout();//扫描按钮布局
    DWidget *mainWidget = new DWidget();//主容器
    DWidget *leftWidget = new DWidget();//左容器
    DWidget *rightWidget = new DWidget();//右容器
    DWidget *rightTopWidget = new DWidget();//右容器-参数容器
    DWidget *rightBottomWidget = new DWidget();//右容器-扫描按钮容器

    //滚动条，右侧参数容器，参数可能过多，需要滚动条
    DScrollArea *rtScrollArea = new DScrollArea (this);
    rtScrollArea->setWidgetResizable(true);

    leftWidget->setFixedWidth(300);//左侧容器固定宽度300px
    if(isDarkType()==true)
    {
        GlobalHelper::setWidgetBackgroundColor(leftWidget,QColor(255,255,255,12),false);//左侧容器设置背景色
        //GlobalHelper::setWidgetBackgroundColor(rightWidget,QColor(255,255,255,12),false);//右侧容器不要背景色
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(leftWidget,QColor(255,255,255,255),false);//左侧容器设置背景白色
        GlobalHelper::setWidgetBackgroundColor(rightWidget,QColor(255,255,255,255),false);//右侧容器设置背景白色
    }
    //容器设置布局
    mainWidget->setLayout(mainLayout);
    leftWidget->setLayout(leftLayout);
    rightWidget->setLayout(rightLayout);
    rightTopWidget->setLayout(rtVLayout);
    rightBottomWidget->setLayout(rbHLayout);
    //布局加入子控件
    mainLayout->addWidget(leftWidget);
    mainLayout->addWidget(rightWidget);
    leftLayout->addWidget(leftTopWidget,0,Qt::AlignTop);
    leftLayout->addWidget(leftBottomWidget);
    rightLayout->addWidget(rtScrollArea);
    rightLayout->addWidget(rightBottomWidget,0,Qt::AlignBottom);
    rtScrollArea->setWidget(rightTopWidget);
    rbHLayout->addWidget(scanBtn,0,Qt::AlignCenter);

    mainSLayout->addWidget(mainWidget);

    //去除布局的间隙
    gridLayout->setMargin(0);
    gridLayout->setSpacing(0);
    rightLayout->setMargin(0);
    rightLayout->setSpacing(0);

}

//加载安装设备UI
void ScanManagerWindow::initInstallDeviceUI()
{
    //标题栏返回按钮
    backBtn = new DIconButton(nullptr);
    if(isDarkType()==true)
    {
        backBtn->setIcon(QIcon(":/img/back_dark.svg"));//图标
    }
    else
    {
        backBtn->setIcon(QIcon(":/img/back.svg"));//图标
    }
    backBtn->setFixedSize(QSize(36, 36));//按钮尺寸
    backBtn->setToolTip(tr("Back"));
    this->titlebar()->addWidget(backBtn,Qt::AlignLeft);
    backBtn->setVisible(false);
    connect(backBtn, SIGNAL(clicked()), this, SLOT(slotBackBtnClicked()));

    //***左侧
    DLabel *leftTopLbl=new DLabel ();
    leftTopLbl->setText(tr("Add a driver"));
    if(isDarkType()==true)
    {
        leftTopLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgb(192,198,212);font-size:14px");
    }
    else
    {
        leftTopLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgba(65,77,104,1);font-size:14px");
    }
    //在线安装按钮
    installOnlineBtn = new QPushButton ();
    installOnlineBtn->setFixedSize(QSize(288, 38));
    installOnlineBtn->setText(tr("Find drivers"));
    installOnlineBtn->setCheckable(true);//开启可选中模式状态
    installOnlineBtn->setDown(true);
    connect(installOnlineBtn, SIGNAL(clicked()), this, SLOT(slotInstallOnlineBtnClicked()));
    //本地安装按钮
    installLocalBtn = new QPushButton ();
    installLocalBtn->setFixedSize(QSize(288, 38));
    installLocalBtn->setText(tr("Local drivers"));
    installLocalBtn->setCheckable(true);//开启可选中模式状态
    connect(installLocalBtn, SIGNAL(clicked()), this, SLOT(slotInstallLocalBtnClicked()));
    //产生互斥效果
    QButtonGroup *g=new QButtonGroup ();
    g->setExclusive(true);
    g->addButton(installOnlineBtn);
    g->addButton(installLocalBtn);

    //***右侧
    installTitleLabel = new DLabel();//标题栏
    installTitleLabel->setText(tr("Select a driver"));
    if(isDarkType()==true)
    {
        installTitleLabel->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgb(192,198,212);font-size:14px");
    }
    else
    {
        installTitleLabel->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgba(65,77,104,1);font-size:14px");
    }
    installSLayout = new QStackedLayout();//安装控件区域
    DWidget *installWidget = new DWidget();
    //installWidget->setStyleSheet("background:rgba(247,247,247,255);border-radius: 10px;");
    if(isDarkType()==true)
    {
        //GlobalHelper::setWidgetBackgroundColor(installWidget,QColor(255,255,255,12),false);//左侧容器不要设置背景色
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(installWidget,QColor(255,255,255,255),false);//左侧容器设置背景白色
    }
    installWidget->setLayout(installSLayout);
    vendorCBB = new DComboBox();//厂商
    vendorCBB->addItem(tr("None"));
    modelCBB = new DComboBox();//型号
    modelCBB->addItem(tr("None"));
    driverCBB = new DComboBox();//驱动
    driverCBB->addItem(tr("No drivers detected, but you can install a local driver"));

    //在线安装区域
    DWidget *onlineParWidget = new DWidget();
   /* if(isDarkType()==true)
    {
        GlobalHelper::setWidgetBackgroundColor(onlineParWidget,QColor(255,255,255,12),false);
    }*/
    QVBoxLayout *onlineParLayout = new QVBoxLayout();
    onlineParWidget->setLayout(onlineParLayout);
    for(int i=0;i<3;i++)
    {
        DWidget *parWidget = new DWidget ();//每一个参数行的容器
        parWidget->setFixedHeight(48);
        if(isDarkType()==true)
        {
            GlobalHelper::setWidgetBackgroundColor(parWidget,QColor(255,255,255,12),false);//容器设置背景
        }
        else
        {
            GlobalHelper::setWidgetBackgroundColor(parWidget,QColor(249,249,249,255),false);//容器设置背景
        }
        QHBoxLayout *parHLayout = new QHBoxLayout ();//每一个参数行的布局
        DLabel  *pnameLbl= new DLabel ();//参数名称
        pnameLbl->setFixedWidth(80);
        if(isDarkType()==true)
        {
            pnameLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgb(192,198,212);font-size:14px");
        }
        else
        {
            pnameLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgba(65,77,104,1);font-size:14px");
        }
        parHLayout->addWidget(pnameLbl);
        if(i == 0)
        {
            parHLayout->addWidget(vendorCBB);
            pnameLbl->setText(tr("Vendor"));
            parHLayout->setStretchFactor(vendorCBB,1);//设置下拉框的拉伸系数，达到铺满布局效果
            connect(vendorCBB,SIGNAL(currentIndexChanged(const int)),this,SLOT(slotComboBoxCurrentIndexChanged(const int)));
            parWidget->setLayout(parHLayout);//参数行的容器加入布局
        }
        else if(i == 1)
        {
            parHLayout->addWidget(modelCBB);
            pnameLbl->setText(tr("Model"));
            parHLayout->setStretchFactor(modelCBB,1);
            connect(modelCBB,SIGNAL(currentIndexChanged(const int)),this,SLOT(slotComboBoxCurrentIndexChanged(const int)));
            parWidget->setLayout(parHLayout);
        }
        else if(i == 2)
        {
            parHLayout->addWidget(driverCBB);
            pnameLbl->setText(tr("Driver"));
            parHLayout->setStretchFactor(driverCBB,1);
            connect(driverCBB,SIGNAL(currentIndexChanged(const int)),this,SLOT(slotComboBoxCurrentIndexChanged(const int)));
            parWidget->setLayout(parHLayout);
        }
        onlineParLayout->addWidget(parWidget);
    }
    onlineParLayout->addStretch(0);
    installSLayout->addWidget(onlineParWidget);//在线安装区域加入右侧区域[0]

    //本地安装区域
    localParWidget = new DWidget();
    localParWidget->setAcceptDrops(true);//支持接收拖拽事件
    QVBoxLayout *localParLayout = new QVBoxLayout();
    localParWidget->setLayout(localParLayout);
    //localParWidget->setStyleSheet("border-radius: 10px;");
    if(isDarkType()==true)
    {
        GlobalHelper::setWidgetBackgroundColor(localParWidget,QColor(255,255,255,12),false);
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(localParWidget,QColor(247,247,247,255),false);
    }

    localDriverTipLabel = new DLabel();//导入本地驱动文本提示
    localDriverTipLabel->setText(tr("Drag a local driver here"));
    if(isDarkType()==true)
    {
        localDriverTipLabel->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgb(192,198,212);font-size:14px");
    }
    else
    {
        localDriverTipLabel->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgba(65,77,104,1);font-size:14px");
    }
    localDriverTipLabel2 = new DLabel();//导入本地驱动文本提示2
    localDriverTipLabel2->setText(tr("or"));
    if(isDarkType()==true)
    {
        localDriverTipLabel2->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgb(192,198,212);font-size:14px");
    }
    else
    {
        localDriverTipLabel2->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgba(65,77,104,1);font-size:14px");
    }
    importDriverBtn = new QPushButton();//导入本地驱动按钮
    importDriverBtn->setFixedSize(QSize(160, 36));
    importDriverBtn->setText(tr("Import Local Driver"));
    connect(importDriverBtn, SIGNAL(clicked()), this, SLOT(slotImportDriverBtnClicked()));
    localParLayout->addStretch();
    localParLayout->addWidget(localDriverTipLabel,0,Qt::AlignCenter);
    localParLayout->addWidget(localDriverTipLabel2,0,Qt::AlignCenter);
    localParLayout->addWidget(importDriverBtn,0,Qt::AlignCenter);
    localParLayout->addStretch();
    installSLayout->addWidget(localParWidget);//本地安装区域加入右侧区域[1]

    //安装按钮
    installBtn = new QPushButton();
    installBtn->setFixedSize(QSize(300, 36));
    installBtn->setText(tr("Install Driver"));
    installBtn->setEnabled(false);
    connect(installBtn, SIGNAL(clicked()), this, SLOT(slotInstallBtnClicked()));

    //控件加入布局中
    QHBoxLayout *mainLayout = new QHBoxLayout();
    QVBoxLayout *leftLayout = new QVBoxLayout();
    QVBoxLayout *rightLayout = new QVBoxLayout();
    DWidget *mainWidget = new DWidget();
    DWidget *leftWidget = new DWidget();
    DWidget *rightWidget = new DWidget();
    leftWidget->setFixedWidth(300);//左侧容器固定宽度300px
    if(isDarkType()==true)
    {
        GlobalHelper::setWidgetBackgroundColor(leftWidget,QColor(255,255,255,12),false);//左侧容器设置背景色
        GlobalHelper::setWidgetBackgroundColor(rightWidget,QColor(255,255,255,12),false);//右侧容器设置背景色
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(leftWidget,QColor(255,255,255,255),false);//左侧容器设置背景白色
        GlobalHelper::setWidgetBackgroundColor(rightWidget,QColor(255,255,255,255),false);//右侧容器设置背景白色
    }
    mainWidget->setLayout(mainLayout);
    leftWidget->setLayout(leftLayout);
    rightWidget->setLayout(rightLayout);
    mainLayout->addWidget(leftWidget);
    mainLayout->addWidget(rightWidget);
    leftLayout->addWidget(leftTopLbl,0,Qt::AlignTop);
    leftLayout->addWidget(installOnlineBtn,0,Qt::AlignTop);
    leftLayout->addWidget(installLocalBtn,0,Qt::AlignTop);
    leftLayout->addStretch(0);
    rightLayout->addWidget(installTitleLabel,0,Qt::AlignTop);//右侧-上部加标题
    rightLayout->addWidget(installWidget);
    rightLayout->addWidget(installBtn,0,Qt::AlignCenter | Qt::AlignBottom);
    rightLayout->setSpacing(10);//设置内部控件之间的间距10,（重要）中间区域会随着窗口大小自适应

    mainSLayout->addWidget(mainWidget);

}


//新增设备按钮点击
void ScanManagerWindow::slotAddDeviceButtonClicked()
{
    emitAdder = 1;
    mainSLayout->setCurrentIndex(3);//显示安装设备UI
    backBtn->setVisible(true);
}

//新增设备按钮-无设备UI里
void ScanManagerWindow::slotAddDeviceButtonClicked2()
{
    emitAdder = 0;
    mainSLayout->setCurrentIndex(3);//显示安装设备UI
    backBtn->setVisible(true);
}

//返回按钮点击
void ScanManagerWindow::slotBackBtnClicked()
{
    backBtn->setVisible(false);
    //未点击安装按钮
    if(!isClickedInstallBtn)
    {
        if(emitAdder == 0)
        {
             mainSLayout->setCurrentIndex(0);//无设备UI
        }
        else
        {
            mainSLayout->setCurrentIndex(2);//显示设备信息UI
        }
    }
    //点击安装按钮
    else
    {
        slotRefreshDevice();//更新设备列表
    }
}

//在线安装按钮
void ScanManagerWindow::slotInstallOnlineBtnClicked()
{
    installOnlineBtn->setDown(true);
    installLocalBtn->setDown(false);
    installSLayout->setCurrentIndex(0);
    installTitleLabel->setText(tr("Select a driver"));
    installBtn->setEnabled(false);//在线安装禁用安装按钮，暂时没有此功能
}

//本地安装按钮
void ScanManagerWindow::slotInstallLocalBtnClicked()
{
    installOnlineBtn->setDown(false);
    installLocalBtn->setDown(true);
    installSLayout->setCurrentIndex(1);
    installTitleLabel->setText(tr("Select a local driver"));
    if(localDriverFilePath.isEmpty())//本地驱动不为空
    {
        installBtn->setEnabled(false);
    }
    else
    {
        installBtn->setEnabled(true);
    }
}

//导入本地驱动按钮
void ScanManagerWindow::slotImportDriverBtnClicked()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString filePath = QFileDialog::getOpenFileName(NULL,
                                                    tr("Select a local driver"),
                                                    desktopPath,
                                                    "Driver(*.deb);;All files(*.*)",
                                                    Q_NULLPTR,
                                                    QFileDialog::DontResolveSymlinks);
    QFileInfo fi(filePath);
    if(fi.exists())
    {
        localDriverFilePath = filePath;
        localDriverTipLabel->setText(tr("Driver path:")+fi.fileName());
        localDriverTipLabel->setToolTip(tr("Driver path:")+localDriverFilePath);
        installBtn->setEnabled(true);
        localDriverTipLabel2->setText("");
    }

}

//安装驱动
void ScanManagerWindow::slotInstallBtnClicked()
{
    QFileInfo fi(localDriverFilePath);
    if(fi.exists())
    {
        isClickedInstallBtn = true;//点击了安装按钮
        //驱动，以系统默认方式打开
         QDesktopServices::openUrl(QUrl::fromLocalFile(localDriverFilePath));
    }
}

//更新设备列表
void ScanManagerWindow::slotRefreshDevice()
{
    isClickedInstallBtn = false;//是否点击了安装按钮
    mainSLayout->setCurrentIndex(1);
    emit signalSearchDevice();//发送搜索设备信号
}

//获取设备列表并显示到UI
void ScanManagerWindow::slotGetDeviceList()
{
    QList<DeviceInfoData> devList = DeviceInfoHelper::getDeviceListInfo();
    qDebug()<<"获取到的设备数量："<<devList.size();
    if(devList.size() <= 0)
    {
        slotNoScannerUI();
        return;
    }

    isNoDevice = false;//是否无设备
    pModel->clear();//清空原有设备项
    mainSLayout->setCurrentIndex(2); //显示设备信息UI
    backBtn->setVisible(false);//返回按钮隐藏

    for(int i=0;i<devList.length();i++)
    {
        QString strStatus = tr("Disconnected");
        if(devList.at(i).status == 0) strStatus = tr("Idle");
        else if(devList.at(i).status == 1) strStatus = tr("Busy");

        /*
         * 新增设备Item
         * type=设备类型(0=拍摄仪，1=扫描仪)
         * name=设备名称
         * model=设备型号
         * status=设备状态(空闲或者忙碌)
         * index=设备下标
         */
        addDeviceItem(devList.at(i).type,
                      devList.at(i).name,
                      devList.at(i).model,
                      strStatus,
                      devList.at(i).index,
                      devList.at(i).isLicense);
    }

    //默认选中第0项
    QModelIndex index = proxyModel->index(0,0);
    devListView->setCurrentIndex(index);
    slotDevListPressed(index);

}

/*
 * 新增设备Item
 * type=设备类型(0=拍摄仪，1=扫描仪)
 * name=设备名称
 * model=设备型号
 * status=设备状态(空闲或者忙碌)
 * index=设备下标
 * license=是否授权
 */
void ScanManagerWindow::addDeviceItem(int type,QString name,QString model,QString status,int index,int license)
{
    QStandardItem *pItem = new QStandardItem ();
    DeviceItemData itemData;
    itemData.devType = type;
    itemData.devName = name;
    itemData.devModel = model;
    itemData.devStatus = status;
    itemData.devIndex = index;
    itemData.devLicense = license;
    itemData.devModelTitle = tr("Model:");
    itemData.devStatusTitle = tr("Status:");
    pItem->setData(QVariant::fromValue(itemData),Qt::UserRole+1);
    pModel->appendRow(pItem);

}

/*
 * 设备参数UI,显示时要根据配置文件翻译一下
 * titleName=抬头名称(如：扫描设置),
 * parName=参数名称(如：分辨率)，
 * parIndex=参数下标，从设备中读取到的顺序
 * parType=参数类型(0=文本框，1=下拉框))
 * parValue=参数值（如：200DPI<<300DPI）
 * isAddTitleTopMargin=抬头是否需要加入上边距
 * isLastPar=是否为最后一个参数,布局需要加一个addStretch来控制参数行居上
 */
void ScanManagerWindow::showDeviceParUI(QString titleName,QString parName,QString parIndex,int parType, QStringList parValue,bool isAddTitleTopMargin,bool isLastPar)
{
    if(titleName.length() > 0)
    {
        DLabel *lbl = new DLabel ();
        lbl->setText(titleName);//("扫描设置");
        //lbl->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;color:rgba(0,26,46,1);font-size:17px");
        lbl->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;font-size:17px");
        if(isAddTitleTopMargin == true)
        {
            DLabel *tmplbl=new DLabel(); //用一个空白的label，达到上边距距离
            rtVLayout->addWidget(tmplbl,0,Qt::AlignTop);
        }
        rtVLayout->addWidget(lbl,0,Qt::AlignTop);
    }
    DWidget *parWidget = new DWidget ();//每一个参数行的容器
    parWidget->setFixedHeight(48);
    if(isDarkType()==true)
    {
        GlobalHelper::setWidgetBackgroundColor(parWidget,QColor(255,255,255,12),false);//左侧容器设置背景色
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(parWidget,QColor(249,249,249,255),false);//左侧容器设置背景白色
    }

    QHBoxLayout *parHLayout = new QHBoxLayout ();//每一个参数行的布局
    DLabel  *pnameLbl= new DLabel ();//参数名称，显示配置文件中的翻译文本
    pnameLbl->setFixedWidth(80);

    if(locale.language() == QLocale::Chinese )
    {
        pnameLbl->setText(GlobalHelper::readSettingValue("lan",parName));
    }
    else
    {
        pnameLbl->setText(parName);
    }
   // pnameLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgba(65,77,104,1);font-size:14px");
    pnameLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;font-size:14px");
    parHLayout->addWidget(pnameLbl);

    bool isContainsGroup = DeviceInfoHelper::isContainsGroup(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset");
    QString recordedParValue = "";
    int valueIndex = 0;
    //设置选中项
    if(isContainsGroup == true)
    {
        recordedParValue = DeviceInfoHelper::readValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),
                                                              "imgset",parName);
    }
    qDebug()<<"参数："<<parName<<"，上一次选中："<<recordedParValue<<",参数值："<<parValue;
    DTextEdit *ptypeTxt;//参数类型控件 文本框
    DComboBox *ptypeCbb;//参数类型控件 下拉框
    if(parType == 0)
    {
        ptypeTxt = new DTextEdit ();
        parHLayout->addWidget(ptypeTxt);
    }
    else
    {
        ptypeCbb = new DComboBox ();
        if(parIndex.toInt() < 0)//小于0,表示是摄像头的参数
        {
            ptypeCbb->setObjectName(parName);//一定要设置名称，否则无法获取到具体的控件,以参数下标命名
        }
        else
        {
            ptypeCbb->setObjectName(parIndex);//一定要设置名称，否则无法获取到具体的控件,以参数下标命名
        }
        //for(QString pvalue : parValue)
        for(int i = 0;i < parValue.size();i++)
        {
            if(parValue.at(i).isNull() || parValue.at(i).isEmpty()) continue;
            QString tmpParValue = parValue.at(i);
            if(locale.language() == QLocale::Chinese )
            {
                tmpParValue = GlobalHelper::readSettingValue("lan",parValue.at(i));
            }
            if(tmpParValue.isNull() || tmpParValue.isEmpty())
            {
                tmpParValue = parValue.at(i);
            }
            ptypeCbb->addItem(tmpParValue);

            qDebug()<<"对比："<<parValue.at(i)<<","<<recordedParValue;
            if(parValue.at(i) == recordedParValue)
            {
                valueIndex = i;
            }
        }
        parHLayout->addWidget(ptypeCbb);
        parHLayout->setStretchFactor(ptypeCbb,1);//设置下拉框的拉伸系数，达到铺满布局效果

        //设置上一次选中参数
        if(!recordedParValue.isEmpty())
        {
            qDebug()<<parName<<"上一次选中参数,"<<recordedParValue<<",下拉框中的下标："<<valueIndex;
            ptypeCbb->setCurrentIndex(valueIndex);
            //设置参数集合
            if(parIndex.toInt() < 0)//小于0,表示是摄像头的参数
            {

            }
            else
            {
                //选中参数集合是否包含此参数名
                if(scannerChoiseParMap.contains(recordedParValue.toInt()))
                {
                    scannerChoiseParMap[parIndex.toInt()] = recordedParValue;//修改值
                }
                else
                {
                    scannerChoiseParMap.insert(parIndex.toInt(),recordedParValue);//加入值
                }
            }
        }
        //连接信号槽
        connect(ptypeCbb,SIGNAL(currentIndexChanged(const int)),this,SLOT(slotComboBoxCurrentIndexChanged(const int)));
    }
    parWidget->setLayout(parHLayout);//参数行的容器加入布局
    rtVLayout->addWidget(parWidget,0,Qt::AlignTop);//右上局部加入参数行容器

}

//图像处理参数UI
void ScanManagerWindow::showImgEditParUI(bool isLicense)
{
    DLabel *lblNULLCut=new DLabel();//用一个空白的label，达到上边距距离
    DLabel *lblCut = new DLabel ();
    lblCut->setText(tr("Crop"));//裁剪
    if(isDarkType() == true)
    {
        lblCut->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;font-size:17px;color:rgb(192,198,212);");
    }
    else
    {
        lblCut->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;font-size:17px;color:rgba(0,26,46,1);");
    }

    DWidget *parWidgetNoCut = new DWidget ();//每一个参数行的容器
    parWidgetNoCut->setFixedHeight(48);
    if(isDarkType()==true)
    {
        GlobalHelper::setWidgetBackgroundColor(parWidgetNoCut,QColor(255,255,255,12),false);//容器设置背景色
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(parWidgetNoCut,QColor(249,249,249,255),false);//容器设置背景色
    }
    QHBoxLayout *parHLayoutNoCut = new QHBoxLayout ();
    noCutrbBtn=new DRadioButton (nullptr);
    noCutrbBtn->setText(tr("No crop"));//不裁剪
    noCutrbBtn->setChecked(true);//选中
    parHLayoutNoCut->addWidget(noCutrbBtn);
    parWidgetNoCut->setLayout(parHLayoutNoCut);//不裁切参数行的容器加入布局

    DWidget *parWidgetSingleCut = new DWidget ();
    parWidgetSingleCut->setFixedHeight(48);
    if(isDarkType()==true)
    {
        GlobalHelper::setWidgetBackgroundColor(parWidgetSingleCut,QColor(255,255,255,12),false);//容器设置背景色
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(parWidgetSingleCut,QColor(249,249,249,255),false);//容器设置背景色
    }
    QHBoxLayout *parHLayoutSingleCut = new QHBoxLayout ();
    singleCutrbBtn=new DRadioButton (nullptr);
    singleCutrbBtn->setText(tr("Single area"));//单图裁剪
    parHLayoutSingleCut->addWidget(singleCutrbBtn);
    parWidgetSingleCut->setLayout(parHLayoutSingleCut);

    DWidget *parWidgetMulCut = new DWidget ();
    parWidgetMulCut->setFixedHeight(48);
    if(isDarkType()==true)
    {
        GlobalHelper::setWidgetBackgroundColor(parWidgetMulCut,QColor(255,255,255,12),false);//容器设置背景色
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(parWidgetMulCut,QColor(249,249,249,255),false);//容器设置背景色
    }
    QHBoxLayout *parHLayoutMulCut = new QHBoxLayout ();
    mulCutrbBtn=new DRadioButton (nullptr);
    mulCutrbBtn->setText(tr("Multiple areas"));//多图裁剪
    parHLayoutMulCut->addWidget(mulCutrbBtn);
    parWidgetMulCut->setLayout(parHLayoutMulCut);

    //单选按钮加入组，互斥效果
    QButtonGroup *btnGroup=new QButtonGroup ();
    btnGroup->addButton(noCutrbBtn);
    btnGroup->addButton(singleCutrbBtn);
    btnGroup->addButton(mulCutrbBtn);
    btnGroup->setExclusive(true);

    DLabel *lblNULLWM=new DLabel(); //用一个空白的label，达到上边距距离
    DLabel *lblWM = new DLabel ();
    lblWM->setText(tr("Watermark"));//水印设置
    if(isDarkType() == true)
    {
        lblWM->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;font-size:17px;color:rgb(192,198,212);");
    }
    else
    {
        lblWM->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;font-size:17px;color:rgba(0,26,46,1);");
    }
    DWidget *parWidgetNoWM = new DWidget ();
    parWidgetNoWM->setFixedHeight(48);
    if(isDarkType()==true)
    {
        GlobalHelper::setWidgetBackgroundColor(parWidgetNoWM,QColor(255,255,255,12),false);//容器设置背景色
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(parWidgetNoWM,QColor(249,249,249,255),false);//容器设置背景色
    }
    QHBoxLayout *parHLayoutNoWM = new QHBoxLayout ();
    noWaterrbBtn=new DRadioButton (nullptr);
    noWaterrbBtn->setText(tr("No watermark"));//无水印
    noWaterrbBtn->setChecked(true);
    parHLayoutNoWM->addWidget(noWaterrbBtn);
    parWidgetNoWM->setLayout(parHLayoutNoWM);

    DWidget *parWidgetFontWM = new DWidget ();
    parWidgetFontWM->setFixedHeight(48);
    if(isDarkType()==true)
    {
        GlobalHelper::setWidgetBackgroundColor(parWidgetFontWM,QColor(255,255,255,12),false);//容器设置背景色
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(parWidgetFontWM,QColor(249,249,249,255),false);//容器设置背景色
    }
    QHBoxLayout *parHLayoutFontWM = new QHBoxLayout ();
    fontWaterrbBtn=new DRadioButton (nullptr);
    fontWaterrbBtn->setText(tr("Text"));//文字水印
    fontWaterText = new DTextEdit();
    colorBtn = new DIconButton (nullptr);
    colorBtn->setIcon(QIcon(":/img/watermarkColor.svg"));
    colorBtn->setFixedSize(QSize(32,32));
    parHLayoutFontWM->addWidget(fontWaterrbBtn);
    parHLayoutFontWM->addWidget(fontWaterText);
    parHLayoutFontWM->addWidget(colorBtn);
    parWidgetFontWM->setLayout(parHLayoutFontWM);

    /*
    QWidget *parWidgetImage = new QWidget (rtWidget);
    parWidgetImage->setFixedHeight(48);
    GlobalHelper::setWidgetBackgroundColor(parWidgetImage,QColor(249,249,249,255));//容器设置背景
    QHBoxLayout *parHLayoutImage = new QHBoxLayout ();
    imageWaterBtn=new DRadioButton (nullptr);
    imageWaterBtn->setText(tr("图片水印"));
    parHLayoutImage->addWidget(imageWaterBtn);
    parWidgetImage->setLayout(parHLayoutImage);
    */

    //单选按钮加入组，互斥效果
    QButtonGroup *btnGroup2=new QButtonGroup ();
    btnGroup2->addButton(noWaterrbBtn);
    btnGroup2->addButton(fontWaterrbBtn);
   // btnGroup2->addButton(imageWaterBtn);
    btnGroup2->setExclusive(true);

    DLabel *lblNULLIE=new DLabel();//用一个空白的label，达到上边距距离
    DLabel *lblIE = new DLabel ();
    lblIE->setText(tr("Images"));//图像处理
    if(isDarkType() == true)
    {
        lblIE->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;font-size:17px;color:rgb(192,198,212);");
    }
    else
    {
        lblIE->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;font-size:17px;color:rgba(0,26,46,1);");
    }
    DWidget *parWidgetBB = new DWidget ();
    parWidgetBB->setFixedHeight(48);
    if(isDarkType()==true)
    {
        GlobalHelper::setWidgetBackgroundColor(parWidgetBB,QColor(255,255,255,12),false);//容器设置背景色
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(parWidgetBB,QColor(249,249,249,255),false);//容器设置背景色
    }

    QHBoxLayout *parHLayoutBB = new QHBoxLayout ();
    DLabel  *pnameLbl= new DLabel ();
    pnameLbl->setFixedWidth(100);
    pnameLbl->setText(tr("Optimization"));//图像类型
    /*if(isDarkType() == true)
    {
        pnameLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;font-size:14px;color:rgb(192,198,212);");
    }
    else
    {
        pnameLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;font-size:14px;color:rgba(0,26,46,1);");
    }*/
    //文档类型下拉框
    docTypeCBB = new DComboBox ();
    docTypeCBB->addItem(tr("Original"));//原始文档
    docTypeCBB->addItem(tr("Document"));//文档优化
    docTypeCBB->addItem(tr("Color"));//彩色优化
    docTypeCBB->addItem(tr("Red seal"));//红印文档优化
    docTypeCBB->addItem(tr("Invert colors"));//反色
    //docTypeCBB->addItem(tr("滤红"));
    parHLayoutBB->addWidget(pnameLbl);
    parHLayoutBB->addWidget(docTypeCBB);
    parWidgetBB->setLayout(parHLayoutBB);

    /*
    QWidget *parWidgetFD = new QWidget (rtWidget);
    parWidgetFD->setFixedHeight(48);
    GlobalHelper::setWidgetBackgroundColor(parWidgetFD,QColor(249,249,249,255));//容器设置背景
    QHBoxLayout *parHLayoutFD = new QHBoxLayout ();
    filterDenoisingCKB=new DCheckBox (nullptr);
    filterDenoisingCKB->setText(tr("去噪"));
    parHLayoutFD->addWidget(filterDenoisingCKB);
    parWidgetFD->setLayout(parHLayoutFD);
    */

    DWidget *parWidgetRepair = new DWidget ();
    parWidgetRepair->setFixedHeight(48);
    if(isDarkType()==true)
    {
        GlobalHelper::setWidgetBackgroundColor(parWidgetRepair,QColor(255,255,255,12),false);//容器设置背景色
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(parWidgetRepair,QColor(249,249,249,255),false);//容器设置背景色
    }
    QHBoxLayout *parHLayoutRepair = new QHBoxLayout ();
    repairCKB=new DCheckBox (nullptr);
    repairCKB->setText(tr("Fill missing corners"));//缺角修复
    parHLayoutRepair->addWidget(repairCKB);
    parWidgetRepair->setLayout(parHLayoutRepair);

    //裁切设置
    rtVLayout->addWidget(lblNULLCut,0,Qt::AlignTop);
    rtVLayout->addWidget(lblCut,0,Qt::AlignTop);
    rtVLayout->addWidget(parWidgetNoCut,0,Qt::AlignTop);
    rtVLayout->addWidget(parWidgetSingleCut,0,Qt::AlignTop);
    rtVLayout->addWidget(parWidgetMulCut,0,Qt::AlignTop);

    //水印设置
    rtVLayout->addWidget(lblNULLWM,0,Qt::AlignTop);
    rtVLayout->addWidget(lblWM,0,Qt::AlignTop);
    rtVLayout->addWidget(parWidgetNoWM,0,Qt::AlignTop);
    rtVLayout->addWidget(parWidgetFontWM,0,Qt::AlignTop);
    //rtVLayout->addWidget(parWidgetImage,0,Qt::AlignTop);

    //文档处理设置
    rtVLayout->addWidget(lblNULLIE,0,Qt::AlignTop);
    rtVLayout->addWidget(lblIE,0,Qt::AlignTop);
    rtVLayout->addWidget(parWidgetBB,0,Qt::AlignTop);
    //rtVLayout->addWidget(parWidgetFD,0,Qt::AlignTop);
    rtVLayout->addWidget(parWidgetRepair,0,Qt::AlignTop);

    connect(fontWaterText, SIGNAL(textChanged()), this, SLOT(slotFontWaterTextChanged()));//水印文本框文本改变事件
    connect(colorBtn, SIGNAL(clicked()), this, SLOT(slotColorButtonClicked()));//颜色选择按钮信号槽
    connect(noWaterrbBtn, SIGNAL(clicked()), this, SLOT(slotNoWaterrbBtnChecked()));
    connect(fontWaterrbBtn, SIGNAL(clicked()), this, SLOT(slotFontWaterrbBtnChecked()));


    //无授权，禁用按钮
    if(isLicense == false)
    {
        //裁切设置
        lblNULLCut->setEnabled(false);
        lblCut->setEnabled(false);
        parWidgetNoCut->setEnabled(false);
        parWidgetSingleCut->setEnabled(false);
        parWidgetMulCut->setEnabled(false);

        //水印设置
        lblNULLWM->setEnabled(false);
        lblWM->setEnabled(false);
        parWidgetNoWM->setEnabled(false);
        parWidgetFontWM->setEnabled(false);

        //文档处理设置
        lblNULLIE->setEnabled(false);
        lblIE->setEnabled(false);
        pnameLbl->setEnabled(false);
        parWidgetBB->setEnabled(false);
        parWidgetRepair->setEnabled(false);
    }

}

void ScanManagerWindow::slotNoWaterrbBtnChecked()
{
    if(currentDeviceIsCanUse == true)
    {
        scanBtn->setEnabled(true);
    }
     isNoWatermark = true;//是否选择了无水印
}

void ScanManagerWindow::slotFontWaterrbBtnChecked()
{
    QString txt = fontWaterText->toPlainText();
    if(txt.isEmpty())
    {
        scanBtn->setEnabled(false);
    }
    isNoWatermark = false;//是否选择了无水印
}

//图像格式和颜色模式UI
void ScanManagerWindow::showImgFormatAndTypeUI(bool isCamera,bool isLicense)
{
    DWidget *parWidgetBB = new DWidget ();
    parWidgetBB->setFixedHeight(48);
    if(isDarkType()==true)
    {
        GlobalHelper::setWidgetBackgroundColor(parWidgetBB,QColor(255,255,255,12),false);//容器设置背景色
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(parWidgetBB,QColor(249,249,249,255),false);//容器设置背景色
    }
    QHBoxLayout *parHLayoutBB = new QHBoxLayout ();
    DLabel  *pnameLbl= new DLabel ();
    pnameLbl->setFixedWidth(80);
    pnameLbl->setText(tr("Image"));//图片格式
    if(isDarkType() == true)
    {
        pnameLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;font-size:14px;color:rgb(192,198,212);");
    }
    else
    {
        pnameLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;font-size:14px;color:rgba(0,26,46,1);");
    }
    imgFormatCBB = new DComboBox ();
    imgFormatCBB->addItem("jpg");
    imgFormatCBB->addItem("bmp");
    imgFormatCBB->addItem("tif");
    if(!isCamera)
    {
        imgFormatCBB->addItem("png");
    }
    parHLayoutBB->addWidget(pnameLbl);
    parHLayoutBB->addWidget(imgFormatCBB);
    parWidgetBB->setLayout(parHLayoutBB);

    rtVLayout->addWidget(parWidgetBB,0,Qt::AlignTop);

    if(isCamera)
    {
        DWidget *parWidgetBB2 = new DWidget ();
        parWidgetBB2->setFixedHeight(48);
        if(isDarkType()==true)
        {
            GlobalHelper::setWidgetBackgroundColor(parWidgetBB2,QColor(255,255,255,12),false);//容器设置背景色
        }
        else
        {
            GlobalHelper::setWidgetBackgroundColor(parWidgetBB2,QColor(249,249,249,255),false);//容器设置背景色
        }
        QHBoxLayout *parHLayoutBB2 = new QHBoxLayout ();
        DLabel  *pnameLbl2= new DLabel ();
        pnameLbl2->setFixedWidth(80);
        pnameLbl2->setText(tr("Color mode"));//色彩模式
        /*if(isDarkType() == true)
        {
            pnameLbl2->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;font-size:14px;color:rgb(192,198,212);");
        }
        else
        {
            pnameLbl2->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;font-size:14px;color:rgba(0,26,46,1);");
        }*/

        imgTypeCBB = new DComboBox ();
        imgTypeCBB->addItem(tr("Colorful"));//彩色图
        imgTypeCBB->addItem(tr("Grey"));//灰度图
        imgTypeCBB->addItem(tr("Black and white"));//黑白图
        parHLayoutBB2->addWidget(pnameLbl2);
        parHLayoutBB2->addWidget(imgTypeCBB);
        parWidgetBB2->setLayout(parHLayoutBB2);

        rtVLayout->addWidget(parWidgetBB2,0,Qt::AlignTop);

        //无授权，禁用按钮
        if(isLicense == false)
        {
            pnameLbl2->setEnabled(false);
            imgTypeCBB->setEnabled(false);
        }
    }

}

//拍摄方式UI
void ScanManagerWindow::showShotTypeUI(bool isLicense)
{
    DLabel *lblNULLIE=new DLabel();//用一个空白的label，达到上边距距离
    rtVLayout->addWidget(lblNULLIE,0,Qt::AlignTop);
    DLabel *lblIE = new DLabel ();
    lblIE->setText(tr("Method"));//拍摄方式
    if(isDarkType() == true)
    {
        lblIE->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;color:rgb(192,198,212);font-size:17px");
    }
    else
    {
        lblIE->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;color:rgba(0,26,46,1);font-size:17px");
    }
    rtVLayout->addWidget(lblIE,0,Qt::AlignTop);

    DWidget *parWidgetBB = new DWidget ();
    parWidgetBB->setFixedHeight(48);
    if(isDarkType()==true)
    {
        GlobalHelper::setWidgetBackgroundColor(parWidgetBB,QColor(255,255,255,12),false);//容器设置背景色
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(parWidgetBB,QColor(249,249,249,255),false);//容器设置背景色
    }
    QHBoxLayout *parHLayoutBB = new QHBoxLayout ();
    shotTypeManualrbBtn = new DRadioButton (nullptr);
    shotTypeManualrbBtn->setText(tr("Manual scan"));//手动拍摄
    shotTypeManualrbBtn->setChecked(true);
    parHLayoutBB->addWidget(shotTypeManualrbBtn);
    parWidgetBB->setLayout(parHLayoutBB);
    rtVLayout->addWidget(parWidgetBB,0,Qt::AlignTop);

    DWidget *parWidgetBB2 = new DWidget ();
    parWidgetBB2->setFixedHeight(48);
    if(isDarkType()==true)
    {
        GlobalHelper::setWidgetBackgroundColor(parWidgetBB2,QColor(255,255,255,12),false);//容器设置背景色
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(parWidgetBB2,QColor(249,249,249,255),false);//容器设置背景色
    }
    QHBoxLayout *parHLayoutBB2 = new QHBoxLayout ();
    shotTypeTimerrbBtn = new DRadioButton (nullptr);
    shotTypeTimerrbBtn->setText(tr("Set timer"));//定时
    parHLayoutBB2->addWidget(shotTypeTimerrbBtn);
    shotTypeTimerCBB = new DComboBox ();
    shotTypeTimerCBB->addItem("5");
    shotTypeTimerCBB->addItem("8");
    shotTypeTimerCBB->addItem("10");
    shotTypeTimerCBB->setFixedWidth(80);
    parHLayoutBB2->addWidget(shotTypeTimerCBB);
    DLabel  *pnameLbl= new DLabel ();
    pnameLbl->setFixedWidth(80);
    pnameLbl->setText(tr("s"));//秒拍摄    
    parHLayoutBB2->addWidget(pnameLbl);
    parHLayoutBB2->addStretch();

    parWidgetBB2->setLayout(parHLayoutBB2);
    rtVLayout->addWidget(parWidgetBB2,0,Qt::AlignTop);

    /*
    QWidget *parWidgetAuto = new QWidget (rtWidget);
    parWidgetAuto->setFixedHeight(48);
    GlobalHelper::setWidgetBackgroundColor(parWidgetAuto,QColor(249,249,249,255));//容器设置背景
    QHBoxLayout *parHLayoutAuto = new QHBoxLayout ();
    shotTypeAutoCaptureBtn = new DRadioButton (nullptr);
    shotTypeAutoCaptureBtn->setText(tr("自动拍摄"));
   // shotTypeAutoCaptureBtn->setChecked(true);
    parHLayoutAuto->addWidget(shotTypeAutoCaptureBtn);
    parWidgetAuto->setLayout(parHLayoutAuto);
    rtVLayout->addWidget(parWidgetAuto,0,Qt::AlignTop);
   */

    //单选按钮加入组，互斥效果
    QButtonGroup *btnGroup=new QButtonGroup ();
    btnGroup->addButton(shotTypeManualrbBtn);
    btnGroup->addButton(shotTypeTimerrbBtn);
   // btnGroup->addButton(shotTypeAutoCaptureBtn);
    btnGroup->setExclusive(true);

    //无授权，禁用按钮
    if(isLicense == false)
    {
        lblIE->setEnabled(false);
        shotTypeManualrbBtn->setEnabled(false);
        shotTypeTimerrbBtn->setEnabled(false);
        shotTypeTimerCBB->setEnabled(false);
        pnameLbl->setEnabled(false);
    }

}

//无设备UI
void ScanManagerWindow::slotNoScannerUI()
{
    isNoDevice = true;//是否无设备
    mainSLayout->setCurrentIndex(0);
}

//开始按钮
void ScanManagerWindow::slotScanButtonClicked()
{
    isScanClose = true;
    GlobalHelper::writeSettingValue("set","imgPreNameIndex","0");//扫描文件的名称编号置0
    qDebug()<<"当前设备类型（0=扫描仪，1=拍摄仪）："<<nCurrentDeviceType<<",当前设备下标:"<<nCurrentDeviceIndex;
    if(nCurrentDeviceType == 0)//扫描仪
    {
        getImgEditPar();//从UI上获取图像处理参数
        //if(scanWindow == NULL)
        {
            scanWindow = new ScanWindow (nCurrentDeviceIndex,scannerChoiseParMap,this);
        }
        scanWindow->setAttribute(Qt::WA_ShowModal,true);//模态窗口
        scanWindow->show();
        QDesktopWidget * desktop = QApplication::desktop();
        int currentScreen = desktop->screenNumber(this);//获取程序所在屏幕是第几个屏幕
        //int screenCount = desktop->screenCount();//获取所有屏幕的个数
        int nw = 0;
        int nh = 0;
        for(int i=0;i<currentScreen;i++)
        {
            QRect rectTmp = desktop->screenGeometry(i);//获取屏幕的尺寸
            nw += rectTmp.width();
            nh += rectTmp.height();
        }
        QRect rect = desktop->screenGeometry(currentScreen);//获取程序所在屏幕的尺寸
        scanWindow->move((rect.width()-scanWindow->width())/2+nw,(rect.height()-scanWindow->height())/2);
        /*
        //屏幕中间显示
        scanWindow->move ((QApplication::desktop()->width() - scanWindow->width())/2,
                          (QApplication::desktop()->height() - scanWindow->height())/2);
        */
        this->close();//本窗口关闭
        //拍摄仪窗口的关闭信号与主窗口的搜索更新设备连接
        MainWindow *mainW = (MainWindow*)this->parent();
        connect(scanWindow,SIGNAL(signalSearchDevice()),mainW,SLOT(openCameraThread()));
    }
    else if(nCurrentDeviceType == 1)//拍摄仪
    {
        getImgEditPar();//从UI上获取图像处理参数
        //if(cameraWindow == NULL)
        {
            cameraWindow = new CameraWindow (nCurrentDeviceIndex,cameraChoiseParMap,this) ;
        }
        cameraWindow->setAttribute(Qt::WA_ShowModal,true);//模态窗口
        cameraWindow->show();

        QDesktopWidget * desktop = QApplication::desktop();
        int currentScreen = desktop->screenNumber(this);//获取程序所在屏幕是第几个屏幕
        //int screenCount = desktop->screenCount();//获取所有屏幕的个数
        int nw = 0;
        int nh = 0;
        for(int i=0;i<currentScreen;i++)
        {
            QRect rectTmp = desktop->screenGeometry(i);//获取屏幕的尺寸
            nw += rectTmp.width();
            nh += rectTmp.height();
        }
        QRect rect = desktop->screenGeometry(currentScreen);//获取程序所在屏幕的尺寸
        cameraWindow->move((rect.width()-cameraWindow->width())/2+nw,(rect.height()-cameraWindow->height())/2);
        /*
        cameraWindow->move ((QApplication::desktop()->width() - cameraWindow->width())/2,
                          (QApplication::desktop()->height() - cameraWindow->height())/2);
        */
        this->close();//本窗口关闭

        //拍摄仪窗口的关闭信号与主窗口的刷新数据槽连接
        MainWindow *mainW = (MainWindow*)this->parent();
        connect(cameraWindow,SIGNAL(signalWindowClosed()),mainW,SLOT(slotRefreshData()));

    }

}

//设备列表点击事件,获取指定设备的参数
void ScanManagerWindow::slotDevListPressed(const QModelIndex)
{
    //清空参数布局里所有控件
    QLayoutItem *item;
    while((item = rtVLayout->takeAt(0)) != 0)
    {
        if(item->widget())
        {
            item->widget()->setParent(NULL);
        }
        delete item;
    }

    //获取点击的项数据
    QModelIndexList selectItems = devListView->selectionModel()->selectedIndexes();
    foreach(QModelIndex mIndex,selectItems)
    {
        QVariant devData = (QVariant)(mIndex.data(Qt::UserRole+1));
        DeviceItemData d = devData.value<DeviceItemData>();

        currentDeviceModel = d.devModel;
        bool isContainsGroup = DeviceInfoHelper::isContainsGroup(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset");
        qDebug()<<currentDeviceModel<<"是否包含imgset，"<<isContainsGroup;

        QString iniFilePath = DeviceInfoHelper::getDeviceInfoFilePath(d.devModel);
        qDebug()<<"device par ini file:"<<iniFilePath;
        QFileInfo file(iniFilePath);
        if(file.exists())
        {
            //扫描仪
            if(d.devType == 0)
            {
                //scannerParMap.clear();
                //scannerChoiseParMap.clear();
                QStringList parList = DeviceInfoHelper::readValue(iniFilePath,"baseorder","order").split(";");
                if(parList.size() >= 4)
                {
                    //调整参数显示顺序 emily
                    if(QString::compare(parList.at(1),"resolution")==0&&QString::compare(parList.at(2),"source")==0)
                    {
                       QString tt= parList.at(0);
                       QString tt1 = parList.at(2);
                       QString tt2 = parList.at(1);
                       QString tt3 = parList.at(3);
                       parList.clear();
                       parList<<tt<<tt1<<tt2<<tt3;
                    }
                }

                for(int i=0;i<parList.size();i++)
                {
                    QString parName = parList.at(i);
                    if(!parName.isEmpty())
                    {
                        QStringList parValueList = DeviceInfoHelper::readValue(iniFilePath,"base",parName).split(";");
                        parValueList.removeAll(QString(""));//清除空字符项

                        QString parIndex = DeviceInfoHelper::readValue(iniFilePath,"baseorderindex",parName);

                        QString titleName="";
                        if(i==0)
                            titleName=tr("General");//扫描设置
                        showDeviceParUI(titleName,parName,parIndex,1,parValueList,false,false);//UI显示参数

                        scannerParMap.insert(parIndex.toInt(),parValueList);//参数加入集合
                        //选中参数集合默认加入第一个参数
                        if(parValueList.size()>0)
                        {

                           //记录扫描仪参数
                           if(isContainsGroup == false)
                           {
                                scannerChoiseParMap.insert(parIndex.toInt(),parValueList.at(0));
                                DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),
                                                        "imgset",
                                                        parName,
                                                        parValueList.at(0));
                           }

                        }
                    }
                }
                showImgFormatAndTypeUI();//图片格式UI
                showImgEditParUI();//图像算法UI
            }
            //拍摄仪
            else if(d.devType == 1)
            {
                qDebug()<<"拍摄仪是否授权："<<d.devLicense;
                //cameraParMap.clear();
                //cameraChoiseParMap.clear();
                QString resolution =  DeviceInfoHelper::readValue(iniFilePath,"base","resolution");
                QString format =  DeviceInfoHelper::readValue(iniFilePath,"base","format");
                QStringList resolutionList = resolution.split(";");
                resolutionList.removeAll(QString(""));//清除空字符项
                QStringList formatList = format.split(";");
                formatList.removeAll(QString(""));//清除空字符项

                showDeviceParUI(tr("General"),"resolution","-1",1,resolutionList,false,false);//UI显示参数
                showDeviceParUI("","format","-1",1,formatList,false,false);//UI显示参数

                cameraParMap.insert("resolution",resolutionList);//参数加入集合
                cameraParMap.insert("format",formatList);
                if(resolutionList.size()>=2)
                {
                    cameraChoiseParMap.insert("resolution",resolutionList.at(1));//选中参数集合默认加入第一个参数
                }
                else
                {
                    cameraChoiseParMap.insert("resolution","");
                }
                cameraChoiseParMap.insert("format",QString::number(0));

                bool devIsLicense = true;
                if(d.devLicense != 1)
                {
                    devIsLicense = false;
                }
                showImgFormatAndTypeUI(true,devIsLicense);//图片格式UI
                showShotTypeUI(devIsLicense);//拍摄方式UI
                showImgEditParUI(devIsLicense);//图像算法UI
            }
        }

        nCurrentDeviceType = d.devType;//当前设备类型，0=拍摄仪，1=扫描仪
        nCurrentDeviceIndex = d.devIndex;//当前设备下标
        if(d.devStatus != tr("Idle"))//非空闲状态
        {
            scanBtn->setEnabled(false);
            currentDeviceIsCanUse = false;
        }
        else
        {
            scanBtn->setEnabled(true);
            currentDeviceIsCanUse = true;
        }

        //记录默认设备
        DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceListInfoFilePath(),
                                     "default",
                                     "name",
                                     d.devModel);
        DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceListInfoFilePath(),
                                     "default",
                                     "config",
                                     DeviceInfoHelper::getDeviceInfoFilePath(d.devModel));
        DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceListInfoFilePath(),
                                     "default",
                                     "index",
                                     QString::number(nCurrentDeviceIndex));
        DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceListInfoFilePath(),
                                     "default",
                                     "type",
                                     QString::number(nCurrentDeviceType));

        break;
    }

    setUILastSetting();//设置lastsetting
}

//下拉框选项改变
void ScanManagerWindow::slotComboBoxCurrentIndexChanged(const int index)
{
    DComboBox *cb = qobject_cast<DComboBox*>(sender());//找到有哪一个控件发出的信号
    QString keyName = cb->objectName();//key名
    qDebug()<<"当前设备类型（0=扫描仪，1=拍摄仪）："<<nCurrentDeviceType<<",下拉框名称:"<<keyName<<",选中下标"<<index;

    QStringList valueList ;//value值
    if(nCurrentDeviceType == 0)//扫描仪
    {
        //所有参数集合是否包含此参数名
        if(scannerParMap.contains(cb->objectName().toInt()))
        {
            valueList = scannerParMap[keyName.toInt()];
        }
        else
        {
            return;
        }
        //选中参数集合是否包含此参数名
        if(scannerChoiseParMap.contains(cb->objectName().toInt()))
        {
            scannerChoiseParMap[cb->objectName().toInt()] = valueList.at(index);//修改值
        }
        else
        {
            scannerChoiseParMap.insert(keyName.toInt(),valueList.at(index));//加入值
        }
    }
    else if(nCurrentDeviceType == 1)//拍摄仪
    {
        //所有参数集合是否包含此参数名
        if(cameraParMap.contains(cb->objectName()))
        {
            valueList = cameraParMap[keyName];
        }
        else
        {
            return;
        }
        //选中参数集合是否包含此参数名
        if(cameraChoiseParMap.contains(cb->objectName()))
        {
            if(cb->objectName() == "format")//format记录选中的下标
            {
                cameraChoiseParMap[cb->objectName()] = QString::number(index);//修改值
            }
            else
            {
                cameraChoiseParMap[cb->objectName()] = valueList.at(index);//修改值
            }
        }
        else
        {
            if(cb->objectName() == "format")//format记录选中的下标
            {
                cameraChoiseParMap.insert(keyName,QString::number(index));//加入值
            }
            else
            {
                cameraChoiseParMap.insert(keyName,valueList.at(index));//加入值
            }
        }
    }

    //修改配置文件中的参数
    //找到参数名
    QString parName="";
    QStringList keys = DeviceInfoHelper::getAllKeys(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"baseorderindex");
    for(int i=0;i<keys.size();i++)
    {
        QString key = keys[i];
        QString value = DeviceInfoHelper::readValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"baseorderindex",key);
        qDebug()<<"key："<<key<<",value:"<<value;
        if(keyName == value)
        {
            parName = key;
            break;
        }
    }
    QString parValue = valueList.at(index);//参数值
    if(parName.isEmpty())
    {
        parName = keyName;
    }
    qDebug()<<"切换参数名："<<parName<<",参数值:"<<parValue;
    DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset",parName,parValue);

}

//水印文本框文本改变事件
void ScanManagerWindow::slotFontWaterTextChanged()
{
    if(isNoWatermark == false)//是否选择了无水印)
    {
        QString txt = fontWaterText->toPlainText();
        if(txt.isEmpty())
        {
            scanBtn->setEnabled(false);
        }
        else
        {
            if(currentDeviceIsCanUse == true)
            {
                scanBtn->setEnabled(true);
            }
        }
    }
    else
    {
        if(currentDeviceIsCanUse == true)
        {
            scanBtn->setEnabled(true);
        }
    }
}

//颜色按钮点击
void ScanManagerWindow::slotColorButtonClicked()
{
    QColor color=DColorDialog::getColor(Qt::black);
    QPixmap pixmap(colorBtn->width(),colorBtn->height());
    QPainter painter(&pixmap);
    painter.fillRect(QRect(0,0,colorBtn->width(),colorBtn->height()),color);
    QIcon btnicon(pixmap);
    colorBtn->setIcon(btnicon);

    int cred = QString::number(color.red(),10).toInt();
    int cgreen = QString::number(color.green(),10).toInt();
    int cblue = QString::number(color.blue(),10).toInt();
    int calpha = QString::number(color.alpha(),10).toInt();
    GlobalHelper::writeSettingValue("imgEdit","waterMarkColor_r",QString::number(cred));
    GlobalHelper::writeSettingValue("imgEdit","waterMarkColor_g",QString::number(cgreen));
    GlobalHelper::writeSettingValue("imgEdit","waterMarkColor_b",QString::number(cblue));
    GlobalHelper::writeSettingValue("imgEdit","waterMarkColor_a",QString::number(calpha));


    DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","waterMarkColor_r",QString::number(cred));
    DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","waterMarkColor_g",QString::number(cgreen));
    DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","waterMarkColor_b",QString::number(cblue));
    DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","waterMarkColor_a",QString::number(calpha));

}

//获取图像处理参数
void ScanManagerWindow::getImgEditPar()
{
    QString currentDeviceIniPath = DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel);
    //单图裁剪单选按钮
    if(singleCutrbBtn->isChecked())
    {
        //GlobalHelper::writeSettingValue("imgEdit","isCut","1");
        DeviceInfoHelper::writeValue(currentDeviceIniPath,"imgset","isCut","1");
    }
    //不裁剪单选按钮
    if(noCutrbBtn->isChecked())
    {
        //GlobalHelper::writeSettingValue("imgEdit","isCut","0");
        DeviceInfoHelper::writeValue(currentDeviceIniPath,"imgset","isCut","0");
    }
    //多图裁剪单选按钮
    if(mulCutrbBtn->isChecked())
    {
        //GlobalHelper::writeSettingValue("imgEdit","isCut","2");
        DeviceInfoHelper::writeValue(currentDeviceIniPath,"imgset","isCut","2");
    }
    //无水印单选按钮
    if(noWaterrbBtn->isChecked())
    {
        //GlobalHelper::writeSettingValue("imgEdit","isWaterMark","0");
        DeviceInfoHelper::writeValue(currentDeviceIniPath,"imgset","isWaterMark","0");
    }
    //文字水印单选按钮
    if(fontWaterrbBtn->isChecked())
    {
        //GlobalHelper::writeSettingValue("imgEdit","isWaterMark","1");
        DeviceInfoHelper::writeValue(currentDeviceIniPath,"imgset","isWaterMark","1");
    }
    //水印文本
    //GlobalHelper::writeSettingValue("imgEdit","waterMarkText",fontWaterText->toPlainText());
    DeviceInfoHelper::writeValue(currentDeviceIniPath,"imgset","waterMarkText",fontWaterText->toPlainText());

    //文档类型下拉框(0=原始文档，1=文档优化，2=彩色优化，3=红印文档优化，4=反色，5=滤红)
    int nDocType = docTypeCBB->currentIndex();
    //GlobalHelper::writeSettingValue("imgEdit","docType",QString::number(nDocType));
    DeviceInfoHelper::writeValue(currentDeviceIniPath,"imgset","docType",QString::number(nDocType));

    /*
    //去噪复选框
    if(filterDenoisingCKB->isChecked())
    {
        GlobalHelper::writeSettingValue("imgEdit","isFilterDenoising","0");
    }
    else
    {
        GlobalHelper::writeSettingValue("imgEdit","isFilterDenoising","1");
    }
    */

    //缺角修复复选框
    if(repairCKB->isChecked())
    {
        //GlobalHelper::writeSettingValue("imgEdit","isRepair","0");
        DeviceInfoHelper::writeValue(currentDeviceIniPath,"imgset","isRepair","0");
    }
    else
    {
        //GlobalHelper::writeSettingValue("imgEdit","isRepair","1");
        DeviceInfoHelper::writeValue(currentDeviceIniPath,"imgset","isRepair","1");
    }

    //文件类型
    //GlobalHelper::writeSettingValue("imgEdit","imgFormat",imgFormatCBB->currentText());
    DeviceInfoHelper::writeValue(currentDeviceIniPath,"imgset","imgFormat",imgFormatCBB->currentText());

    //拍摄仪
    if(nCurrentDeviceType == 1)
    {
        //色彩模式（彩色，灰度，黑白）
        //GlobalHelper::writeSettingValue("imgEdit","imgType",QString::number(imgTypeCBB->currentIndex()));
        DeviceInfoHelper::writeValue(currentDeviceIniPath,"imgset","imgType",QString::number(imgTypeCBB->currentIndex()));

        //手动拍摄单选按钮
        if(shotTypeManualrbBtn->isChecked())
        {
            //GlobalHelper::writeSettingValue("imgEdit","shotType","0");
            DeviceInfoHelper::writeValue(currentDeviceIniPath,"imgset","shotType","0");
        }

        //定时拍摄单选按钮
        if(shotTypeTimerrbBtn->isChecked())
        {
            //GlobalHelper::writeSettingValue("imgEdit","shotType","1");
            //GlobalHelper::writeSettingValue("imgEdit","shotTime",shotTypeTimerCBB->currentText());//定时拍摄时间下拉框
            DeviceInfoHelper::writeValue(currentDeviceIniPath,"imgset","shotType","1");
            DeviceInfoHelper::writeValue(currentDeviceIniPath,"imgset","shotTime",shotTypeTimerCBB->currentText());
         }
    }

}

//设置UI的LastSetting
void ScanManagerWindow::setUILastSetting()
{
    //裁切
    QString isCut = DeviceInfoHelper::readValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","isCut");
    if(isCut.toInt() == 0)
        noCutrbBtn->setChecked(true);
    else if(isCut.toInt() == 2)
        mulCutrbBtn->setChecked(true);
    else
        singleCutrbBtn->setChecked(true);

    //水印
    QString isWaterMark = DeviceInfoHelper::readValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","isWaterMark");
    if(isWaterMark.toInt() == 1)
        fontWaterrbBtn->setChecked(true);
    else
        noWaterrbBtn->setChecked(true);

    //水印文本
    QString waterMarkText = DeviceInfoHelper::readValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","waterMarkText");
    if(waterMarkText.isEmpty())
    {
        fontWaterText->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"));
    }
    else
    {
        fontWaterText->setText(waterMarkText);
    }

    //文档类型下拉框(0=原始文档，1=文档优化，2=彩色优化，3=红印文档优化，4=反色，5=滤红)
    QString nDocType = DeviceInfoHelper::readValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","docType");
    docTypeCBB->setCurrentIndex(nDocType.toInt());

    //缺角修复
    QString isRepair = DeviceInfoHelper::readValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","isRepair");
    if(isRepair.isEmpty() || isRepair.toInt() == 1)
        repairCKB->setChecked(false);
    else
        repairCKB->setChecked(true);

    //文件类型
    QString imgFormat = DeviceInfoHelper::readValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","imgFormat");
    imgFormatCBB->setCurrentText(imgFormat);

    //拍摄仪
    if(nCurrentDeviceType == 1)
    {
        //色彩模式（彩色，灰度，黑白）
        QString nImgType = DeviceInfoHelper::readValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","imgType");
        imgTypeCBB->setCurrentIndex(nImgType.toInt());

        QString shotType = DeviceInfoHelper::readValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","shotType");
        if(shotType.toInt() == 1)
            shotTypeTimerrbBtn->setChecked(true);//定时拍摄单选按钮
        else
            shotTypeManualrbBtn->setChecked(true);//手动拍摄单选按钮

        QString shotTime = DeviceInfoHelper::readValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","shotTime");
        shotTypeTimerCBB->setCurrentText(shotTime);//定时拍摄时间下拉框
    }

}


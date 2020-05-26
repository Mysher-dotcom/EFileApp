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
#include "globalhelper.h"
#include <QStandardPaths>
#include "mainwindow.h"
#include <DColorDialog>
#include <DCheckBox>
#include <QButtonGroup>
#include "camscansdk.h"
#include "cmimage.h"
#include "MImage.h"
#include <QDateTime>
#include <DDialog>


ScanManagerWindow::ScanManagerWindow(QWidget *parent) :
    DMainWindow(parent),
    ui(new Ui::ScanManagerWindow)
{
    ui->setupUi(this);

    isInitStart = true;//是否初始化，用于获取设备参数后结束线程的判断
    isHaveScanner = false;//是否拥有扫描仪设备
    isHaveCamera = false;//是否拥有拍摄仪设备

    initUI();

    openScannerThread();

}

//开始SANE线程
void ScanManagerWindow::openScannerThread()
{
    qDebug()<<"start...";
    //参数可以是任何值，线程对象的参数用于获取指定设备的参数，在获取设备信息中用不到
    getScannerInfoThread = new GetScannerInfoThread (-1);
    subSANEThread = new QThread();
    getScannerInfoThread->moveToThread(subSANEThread);

    connect(subSANEThread,SIGNAL(started()),getScannerInfoThread,SLOT(slotStartThread()));//开启线程槽函数
    connect(subSANEThread,SIGNAL(finished()),getScannerInfoThread,SLOT(deleteLater()));//终止线程时要调用deleteLater槽函数
    connect(subSANEThread,SIGNAL(finished()),this,SLOT(slotFinishThread()));

    //子线程的信号与本窗口的槽连接
    //无设备信号槽,关闭SANE线程
    connect(getScannerInfoThread,SIGNAL(signalNoScanner()),this,SLOT(slotNoScannerUI()));

    //设备信息信号槽
    connect(getScannerInfoThread,SIGNAL(signalScannerInfo(QVariant, const QString &)),
            this,SLOT(slotScannerInfoUI(QVariant, const QString &)),
            Qt::QueuedConnection);

    //设备参数信号槽
    //connect(getScannerInfoThread,SIGNAL(signalScannerParInfo(QVariant, const QString &)),
    //        this,SLOT(slotScannerParInfoUI(QVariant,const QString &)),
    //        Qt::QueuedConnection);

    subSANEThread->start();//开启多线程槽函数

}

//停止SANE线程
void ScanManagerWindow::closeScannerThread()
{
    qDebug()<<tr("关闭SANE线程");
    if(subSANEThread->isRunning())
    {
        getScannerInfoThread->closeThread();//关闭线程槽函数
        subSANEThread->quit();//退出事件循环
        subSANEThread->wait();//释放线程槽函数资源
    }
}

//SANE线程停止触发
void ScanManagerWindow::slotFinishThread()
{
    qDebug()<<tr("SANE线程停止,线程状态：")<<subSANEThread->isRunning();
    getScannerInfoThread = NULL;
    delete getScannerInfoThread;
    subSANEThread = NULL;
    delete subSANEThread;

    if(isInitStart == true)
    {
        qDebug()<<tr("SANE线程停止，开启Camera线程");
        openCameraThread();
    }
}

//开启拍摄仪线程
void ScanManagerWindow::openCameraThread()
{
    //参数可以是任何值，线程对象的参数用于获取指定设备的参数，在获取设备信息中用不到
    getCameraInfoThread = new GetCameraInfoThread (-1);
    subCameraThread = new QThread();
    getCameraInfoThread->moveToThread(subCameraThread);

    connect(subCameraThread,SIGNAL(started()),getCameraInfoThread,SLOT(slotStartCameraThread()));//开启线程槽函数
    connect(subCameraThread,SIGNAL(finished()),getCameraInfoThread,SLOT(deleteLater()));//终止线程时要调用deleteLater槽函数

    //子线程的信号与本窗口的槽连接
    //无设备信号槽
    connect(getCameraInfoThread,SIGNAL(signalNoCamera()),this,SLOT(slotNoCameraUI()));

    //设备信息信号槽
    connect(getCameraInfoThread,SIGNAL(signalCameraInfo(QVariant, const QString &)),
            this,SLOT(slotCameraInfoUI(QVariant,const QString &)),
            Qt::QueuedConnection);

    //设备参数信号槽
    //connect(getCameraInfoThread,SIGNAL(signalCameraParInfo(QVariant, const QString &)),
    //        this,SLOT(slotCameraParInfoUI(QVariant,const QString &)),
    //        Qt::QueuedConnection);

    //拍摄仪发生错误
    connect(getCameraInfoThread,SIGNAL(signalCameraError(QString)),
            this,SLOT(slotCameraError(QString)));

    subCameraThread->start();//开启多线程槽函数
}

//关闭拍摄仪线程
void ScanManagerWindow::closeCameraThread()
{
    qDebug()<<tr("关闭Camera线程");
    if(subCameraThread->isRunning())
    {
        //getCameraInfoThread->closeThread();//关闭线程槽函数
        subCameraThread->quit();//退出事件循环
        subCameraThread->wait();//释放线程槽函数资源
    }
    qDebug()<<tr("Camera线程停止,线程状态：")<<subCameraThread->isRunning();
    getCameraInfoThread = NULL;
    delete getCameraInfoThread;
    subCameraThread = NULL;
    delete subCameraThread;

    if(isInitStart == true)
    {
        isInitStart = false;
        int nDeviceType = -1;
        if(isHaveScanner == true)
        {
            nDeviceType = 1;
        }
        else if(isHaveCamera == true)
        {
            nDeviceType = 0;
        }
        //没有扫描仪并且没有拍摄仪
        if(nDeviceType == -1)
        {
            return;
        }
        getDevicePar(nDeviceType,0);// 获取指定设备参数:0=拍摄仪，1=扫描仪;设备下标index
        nCurrentDeviceType = nDeviceType;//当前设备类型，0=拍摄仪，1=扫描仪
        nCurrentDeviceIndex = 0;//当前设备下标

        //选中第0项
        QModelIndex index = proxyModel->index(0,0);
        devListView->setCurrentIndex(index);
    }
}

ScanManagerWindow::~ScanManagerWindow()
{
    delete ui;
}

//初始化界面
void ScanManagerWindow::initUI()
{
    //*****窗口设置*******
    this->resize(810,520);//窗口初始尺寸
    this->setMinimumSize(QSize(640,320));//窗口最小尺寸
    this->titlebar()->setTitle("");//标题栏文字设为空
    this->titlebar()->setMenuVisible(false);//隐藏标题栏
    setWindowIcon(QIcon(":/img/logo/logo-16.svg"));// 状态栏图标
    this->titlebar()->setIcon(QIcon(":/img/logo/logo-16.svg"));//标题栏图标
    this->setWindowTitle("设备管理");//开始菜单栏上鼠标悬浮在窗口上显示的名称

    //******窗口布局设置******
    //布局关系：窗口的容器Widget -> 主布局Layout -> 容器Widget -> 容器布局Layout -> 控件
    mainLayout = new QHBoxLayout();//主布局
    lWidget=new QWidget();//左侧容器
    rWidget=new QWidget();//右侧容器
    rtWidget=new QWidget ();//右侧-上部容器
    rbWidget=new QWidget ();//右侧-下部容器
    lWidget->setFixedWidth(300);//左侧容器固定宽度300px
    rbWidget->setFixedHeight(60);//右侧-下部容器固定高度60px
    lVLayout = new QVBoxLayout () ;//左侧容器布局
    rVLayout = new QVBoxLayout () ;//右侧容器布局
    rtVLayout = new QVBoxLayout () ;//右侧-上部容器布局
    rbHLayout = new QHBoxLayout () ;//右侧-下部容器布局
    lTipWidget=new QWidget();//左侧提示容器
    lTipHLayout= new QHBoxLayout ();//左侧提示布局

    //右侧-上部容器布局的内容间隙1px
    rtVLayout->setSpacing(1);

    //左侧容器设置背景白色
    lWidget->setGeometry(0, 0, 300, 100);
    QPalette pal(lWidget->palette());
    pal.setColor(QPalette::Background, Qt::white);
    lWidget->setAutoFillBackground(true);
    lWidget->setPalette(pal);

    //右侧容器设置背景白色
    rWidget->setGeometry(0, 0, 300, 100);
    QPalette palr(rWidget->palette());
    palr.setColor(QPalette::Background, Qt::white);
    rWidget->setAutoFillBackground(true);
    rWidget->setPalette(palr);

    //控件
    QLabel *ltLbl=new QLabel ();
    ltLbl->setText("扫描仪设备");
    ltLbl->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;color:rgba(0,26,46,1);font-size:17px");
    lSpinner = new DSpinner();//左侧提示动画
    lSpinner->setFixedSize(QSize(28,28));
    lSpinner->show();
    lSpinner->start();
    lLbl=new QLabel ();
    lLbl->setText("正在检测扫描设备...");
    lLbl->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;color:rgba(85,85,85,0.4);font-size:17px");
    rtipWidget=new QWidget ();
    rtipWidget->setFixedSize(QSize(450,80));
    rtipLayout = new QVBoxLayout ();
    rtNLbl=new QLabel();//右上空白占位提示信息
    rtLbl=new QLabel ();
    rtLbl->setText("正在检测");
    rtLbl->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;color:rgba(85,85,85,0.4);font-size:17px");
    rbLbl=new QLabel ();
    rbLbl->setText("若长时间没有反应，请更换USB接口，重启设备或电脑尝试。");
    rbLbl->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;color:rgba(85,85,85,0.4);font-size:14px");
    rtipLayout->addWidget(rtLbl,1,Qt::AlignCenter);
    rtipLayout->addWidget(rbLbl,1,Qt::AlignCenter);
    rtipWidget->setLayout(rtipLayout);

    scanBtn=new QPushButton ();//扫描按钮
    scanBtn->setText("开始");
    scanBtn->setFixedSize(QSize(240,36));

    //左侧提示
    lTipHLayout->addWidget(lSpinner,0,Qt::AlignCenter);//左侧提示动画
    lTipHLayout->addWidget(lLbl,1,Qt::AlignCenter);//左侧提示文字
    lTipWidget->setLayout(lTipHLayout);

    //控件加入布局中
    lVLayout->addWidget(ltLbl,0,Qt::AlignTop);//左侧顶部文本
    lVLayout->addWidget(lTipWidget,1,Qt::AlignHCenter);//左侧
    rtVLayout->addWidget(rtNLbl);//右侧-上部加一个空白的label，用于和左侧的“扫描仪设备”提示保持一致，否则右侧的提示信息比左侧提示信息下移一些
    rtVLayout->addWidget(rtipWidget,1,Qt::AlignCenter);//右侧-上部
    rbHLayout->addWidget(scanBtn);//右侧-下部

    //容器加入布局
    lWidget->setLayout(lVLayout);//左侧
    rWidget->setLayout(rVLayout);//右侧
    rtWidget->setLayout(rtVLayout);//右侧-上部
    rbWidget->setLayout(rbHLayout);//右侧-下部

    //滚动条，右侧上部，参数可能过多，需要滚动条
    DScrollArea *rtScrollArea = new DScrollArea (this);
    rtScrollArea->setWidget(rtWidget);
    rtScrollArea->setWidgetResizable(true);

    //右侧布局加入右侧-上部容器 右侧-下部容器
    rVLayout->addWidget(rtScrollArea);//(rtWidget);//
    //rVLayout->addWidget(rbWidget);//窗口初始化SANE时，不加入右侧下部的容器

    //主布局加入容器
    mainLayout->addWidget(lWidget);
    mainLayout->addWidget(rWidget);

    //窗口加入主布局
    ui->centralwidget->setLayout(mainLayout);

    //扫描按钮信号槽
    connect(scanBtn, SIGNAL(clicked()), this, SLOT(slotScanButtonClicked()));

    //去除右侧容器布局的外间隙
    rVLayout->setMargin(0);
    rVLayout->setSpacing(0);

}

//显示设备列表UI
void ScanManagerWindow::showDeviceListUI()
{
    //窗口的左侧布局加入设备列表容器
    devListView = new DListView();
    devListView->setSelectionMode(QAbstractItemView::SingleSelection);
    devListView->setViewMode(QListView::IconMode);
    devListView->setResizeMode(QListView::Adjust);
    pModel = new QStandardItemModel ();
    DeviceItemDelegate *pItemDelegate=new DeviceItemDelegate (this);
    devListView->setItemDelegate(pItemDelegate);
    proxyModel = new QSortFilterProxyModel (devListView);
    proxyModel->setSourceModel(pModel);
    devListView->setModel(proxyModel);

    lVLayout->addWidget(devListView);

    lVLayout->removeWidget(lTipWidget);//移除提示信息文字和动画
    delete lTipWidget;//移除控件，一定要delete，不然还会显示在UI上

    //加设备Item
    //addDeviceItem(1,"扫描仪","UNIS F3135","空闲");
    //addDeviceItem(0,"拍摄仪","SD544","空闲");


    //设备列表点击信号槽
    connect(devListView,SIGNAL(pressed(const QModelIndex)),this,SLOT(slotDevListPressed(const QModelIndex)));

}

/*
 * 新增设备Item
 * type=设备类型(0=拍摄仪，1=扫描仪)
 * type=设备名称
 * model=设备型号
 * status=设备状态(空闲或者忙碌)
 * index=设备下标
 */
void ScanManagerWindow::addDeviceItem(int type,QString name,QString model,QString status,int index)
{
    QStandardItem *pItem = new QStandardItem ();
    DeviceItemData itemData;
    itemData.devType = type;
    itemData.devName = name;
    itemData.devModel = model;
    itemData.devStatus = status;
    itemData.devIndex = index;
    pItem->setData(QVariant::fromValue(itemData),Qt::UserRole+1);
    pModel->appendRow(pItem);

}

/*
 * 获取指定设备参数
 * devType=设备类型，0=拍摄仪，1=扫描仪
 * devIndex=设备下标index
 */
void ScanManagerWindow::getDevicePar(int devType,int devIndex)
{
    if(devType == 0)//拍摄仪
    {
        getCameraInfoThread = new GetCameraInfoThread (devIndex);
        subCameraThread = new QThread();
        getCameraInfoThread->moveToThread(subCameraThread);
        connect(subCameraThread,SIGNAL(started()),getCameraInfoThread,SLOT(slotGetCameraPar()));//开启线程槽函数
        connect(subCameraThread,SIGNAL(finished()),getCameraInfoThread,SLOT(deleteLater()));//终止线程时要调用deleteLater槽函数

        //子线程的信号与本窗口的槽连接
        //无设备信号槽
        connect(getCameraInfoThread,SIGNAL(signalNoCamera()),this,SLOT(slotNoCameraUI()));

        //设备参数信号槽
        connect(getCameraInfoThread,SIGNAL(signalCameraParInfo(QVariant, const QString &)),
                this,SLOT(slotCameraParInfoUI(QVariant,const QString &)),
                Qt::QueuedConnection);

        subCameraThread->start();//开启多线程槽函数

    }
    else if(devType == 1)//扫描仪
    {
        /*
        //清空参数集合
        QMap<int,QStringList>::iterator it;
        for(it = scannerParMap.begin();it!=scannerParMap.end();it++)
        {
            if((it.key()%2)==0)
                scannerParMap.erase(it);
        }
        //清空选中参数集合
        QMap<int,QString>::iterator it2;
        for(it2 = scannerChoiseParMap.begin();it2!=scannerChoiseParMap.end();it2++)
        {
            if((it2.key()%2)==0)
                scannerChoiseParMap.erase(it2);
        }

        */

        getScannerInfoThread = new GetScannerInfoThread (devIndex);
        subSANEThread = new QThread();
        getScannerInfoThread->moveToThread(subSANEThread);

        connect(subSANEThread,SIGNAL(started()),getScannerInfoThread,SLOT(slotGetDevicePar()));//开启线程槽函数
        connect(subSANEThread,SIGNAL(finished()),getScannerInfoThread,SLOT(deleteLater()));//终止线程时要调用deleteLater槽函数
        connect(subSANEThread,SIGNAL(finished()),this,SLOT(slotFinishThread()));

        //子线程的信号与本窗口的槽连接
        //无设备信号槽,关闭SANE线程
        connect(getScannerInfoThread,SIGNAL(signalNoScanner()),this,SLOT(slotNoScannerUI()));

        //设备参数信号槽
        connect(getScannerInfoThread,SIGNAL(signalScannerParInfo(QVariant, const QString &)),
                this,SLOT(slotScannerParInfoUI(QVariant,const QString &)),
                Qt::QueuedConnection);

        subSANEThread->start();//开启多线程槽函数
    }

    rtVLayout->addStretch();//右上布局在加入所有参数后，加入弹簧
}

//拍摄仪发生错误
void ScanManagerWindow::slotCameraError(QString msg)
{
    DDialog *dialog = new DDialog ();
    dialog->setIcon(QIcon(":/img/dialogWarnIcon.svg"));
    dialog->setTitle("警告");
    dialog->setMessage(msg);
    dialog->addButton("确定",true);
    dialog->exec();

    slotNoCameraUI();//发生错误，结束线程
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
        QLabel *lbl = new QLabel ();
        lbl->setText(titleName);//("扫描设置");
        lbl->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;color:rgba(0,26,46,1);font-size:17px");
        if(isAddTitleTopMargin == true)
        {
            //用一个空白的label，达到上边距距离
            QLabel *tmplbl=new QLabel();
            rtVLayout->addWidget(tmplbl,0,Qt::AlignTop);
        }
        rtVLayout->addWidget(lbl,0,Qt::AlignTop);
    }
    QWidget *parWidget = new QWidget (rtWidget);//每一个参数行的容器
    parWidget->setFixedHeight(48);
    //容器设置背景
    parWidget->setGeometry(0, 0, 300, 100);
    QPalette pal(parWidget->palette());
    pal.setColor(QPalette::Background, QColor(249,249,249,255));
    parWidget->setAutoFillBackground(true);
    parWidget->setPalette(pal);

    QHBoxLayout *parHLayout = new QHBoxLayout ();//每一个参数行的布局
    QLabel  *pnameLbl= new QLabel ();//参数名称，显示配置文件中的翻译文本
    pnameLbl->setFixedWidth(80);
    pnameLbl->setText(GlobalHelper::readSettingValue("lan",parName));
    pnameLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgba(65,77,104,1);font-size:14px");
    parHLayout->addWidget(pnameLbl);

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
        ptypeCbb->setObjectName(parIndex);//一定要设置名称，否则无法获取到具体的控件,以参数下标命名
        for(QString pvalue : parValue)
        {
            QString tmpParValue = GlobalHelper::readSettingValue("lan",pvalue);
            if(tmpParValue.isNull() || tmpParValue.isEmpty())
            {
                tmpParValue = pvalue;
            }
            ptypeCbb->addItem(tmpParValue);
        }
        parHLayout->addWidget(ptypeCbb);
        parHLayout->setStretchFactor(ptypeCbb,1);//设置下拉框的拉伸系数，达到铺满布局效果

        //连接信号槽
        connect(ptypeCbb,SIGNAL(currentIndexChanged(const int)),this,SLOT(slotComboBoxCurrentIndexChanged(const int)));
    }
    parWidget->setLayout(parHLayout);//参数行的容器加入布局
    rtVLayout->addWidget(parWidget,0,Qt::AlignTop);//右上局部加入参数行容器
    //if(isLastPar == true)
    //{
        //rtVLayout->addStretch();//右上布局在加入所有参数后，加入弹簧
    //}
}

//图像处理参数UI
void ScanManagerWindow::showImgEditParUI()
{
    QLabel *lblNULLCut=new QLabel();//用一个空白的label，达到上边距距离
    QLabel *lblCut = new QLabel ();
    lblCut->setText("裁剪");
    lblCut->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;color:rgba(0,26,46,1);font-size:17px");

    QWidget *parWidgetNoCut = new QWidget (rtWidget);//每一个参数行的容器
    parWidgetNoCut->setFixedHeight(48);
    GlobalHelper::setWidgetBackgroundColor(parWidgetNoCut,QColor(249,249,249,255));//容器设置背景
    QHBoxLayout *parHLayoutNoCut = new QHBoxLayout ();
    noCutrbBtn=new DRadioButton (nullptr);
    noCutrbBtn->setText("不裁剪");
    noCutrbBtn->setChecked(true);//选中
    parHLayoutNoCut->addWidget(noCutrbBtn);
    parWidgetNoCut->setLayout(parHLayoutNoCut);//不裁切参数行的容器加入布局

    QWidget *parWidgetSingleCut = new QWidget (rtWidget);
    parWidgetSingleCut->setFixedHeight(48);
    GlobalHelper::setWidgetBackgroundColor(parWidgetSingleCut,QColor(249,249,249,255));//容器设置背景
    QHBoxLayout *parHLayoutSingleCut = new QHBoxLayout ();
    singleCutrbBtn=new DRadioButton (nullptr);
    singleCutrbBtn->setText("单图裁剪");
    parHLayoutSingleCut->addWidget(singleCutrbBtn);
    parWidgetSingleCut->setLayout(parHLayoutSingleCut);

    QWidget *parWidgetMulCut = new QWidget (rtWidget);
    parWidgetMulCut->setFixedHeight(48);
    GlobalHelper::setWidgetBackgroundColor(parWidgetMulCut,QColor(249,249,249,255));//容器设置背景
    QHBoxLayout *parHLayoutMulCut = new QHBoxLayout ();
    mulCutrbBtn=new DRadioButton (nullptr);
    mulCutrbBtn->setText("多图裁剪");
    parHLayoutMulCut->addWidget(mulCutrbBtn);
    parWidgetMulCut->setLayout(parHLayoutMulCut);

    //单选按钮加入组，互斥效果
    QButtonGroup *btnGroup=new QButtonGroup ();
    btnGroup->addButton(noCutrbBtn);
    btnGroup->addButton(singleCutrbBtn);
    btnGroup->addButton(mulCutrbBtn);
    btnGroup->setExclusive(true);

    QLabel *lblNULLWM=new QLabel(); //用一个空白的label，达到上边距距离
    QLabel *lblWM = new QLabel ();
    lblWM->setText("水印设置");
    lblWM->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;color:rgba(0,26,46,1);font-size:17px");
    QWidget *parWidgetNoWM = new QWidget (rtWidget);
    parWidgetNoWM->setFixedHeight(48);
    GlobalHelper::setWidgetBackgroundColor(parWidgetNoWM,QColor(249,249,249,255));//容器设置背景
    QHBoxLayout *parHLayoutNoWM = new QHBoxLayout ();
    noWaterrbBtn=new DRadioButton (nullptr);
    noWaterrbBtn->setText("无水印");
    noWaterrbBtn->setChecked(true);
    parHLayoutNoWM->addWidget(noWaterrbBtn);
    parWidgetNoWM->setLayout(parHLayoutNoWM);

    QWidget *parWidgetFontWM = new QWidget (rtWidget);
    parWidgetFontWM->setFixedHeight(48);
    GlobalHelper::setWidgetBackgroundColor(parWidgetFontWM,QColor(249,249,249,255));//容器设置背景
    QHBoxLayout *parHLayoutFontWM = new QHBoxLayout ();
    fontWaterrbBtn=new DRadioButton (nullptr);
    fontWaterrbBtn->setText("文字水印");
    fontWaterText = new DTextEdit();
    QString strWaterMarkText = GlobalHelper::readSettingValue("imgEdit","waterMarkText");
    if(strWaterMarkText.isEmpty())
    {
        fontWaterText->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"));
    }
    else
    {
        fontWaterText->setText(strWaterMarkText);
    }
    colorBtn = new DIconButton (nullptr);
    colorBtn->setIcon(QIcon(":/img/watermarkColor.svg"));
    colorBtn->setFixedSize(QSize(32,32));
    parHLayoutFontWM->addWidget(fontWaterrbBtn);
    parHLayoutFontWM->addWidget(fontWaterText);
    parHLayoutFontWM->addWidget(colorBtn);
    parWidgetFontWM->setLayout(parHLayoutFontWM);
    //单选按钮加入组，互斥效果
    QButtonGroup *btnGroup2=new QButtonGroup ();
    btnGroup2->addButton(noWaterrbBtn);
    btnGroup2->addButton(fontWaterrbBtn);
    btnGroup2->setExclusive(true);

    QLabel *lblNULLIE=new QLabel();//用一个空白的label，达到上边距距离
    QLabel *lblIE = new QLabel ();
    lblIE->setText("图像处理");
    lblIE->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;color:rgba(0,26,46,1);font-size:17px");
    QWidget *parWidgetBB = new QWidget (rtWidget);
    parWidgetBB->setFixedHeight(48);
    GlobalHelper::setWidgetBackgroundColor(parWidgetBB,QColor(249,249,249,255));//容器设置背景

    QHBoxLayout *parHLayoutBB = new QHBoxLayout ();
    QLabel  *pnameLbl= new QLabel ();
    pnameLbl->setFixedWidth(80);
    pnameLbl->setText("图像类型");
    pnameLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgba(65,77,104,1);font-size:14px");

    //文档类型下拉框
    docTypeCBB = new DComboBox ();
    docTypeCBB->addItem("原始文档");
    docTypeCBB->addItem("文档优化");
    docTypeCBB->addItem("彩色优化");
    docTypeCBB->addItem("红印文档优化");
    docTypeCBB->addItem("反色");
    //docTypeCBB->addItem("滤红");
    parHLayoutBB->addWidget(pnameLbl);
    parHLayoutBB->addWidget(docTypeCBB);
    parWidgetBB->setLayout(parHLayoutBB);

    QWidget *parWidgetFD = new QWidget (rtWidget);
    parWidgetFD->setFixedHeight(48);
    GlobalHelper::setWidgetBackgroundColor(parWidgetFD,QColor(249,249,249,255));//容器设置背景
    QHBoxLayout *parHLayoutFD = new QHBoxLayout ();
    filterDenoisingCKB=new DCheckBox (nullptr);
    filterDenoisingCKB->setText("去噪");
    parHLayoutFD->addWidget(filterDenoisingCKB);
    parWidgetFD->setLayout(parHLayoutFD);

    QWidget *parWidgetRepair = new QWidget (rtWidget);
    parWidgetRepair->setFixedHeight(48);
    GlobalHelper::setWidgetBackgroundColor(parWidgetRepair,QColor(249,249,249,255));//容器设置背景
    QHBoxLayout *parHLayoutRepair = new QHBoxLayout ();
    repairCKB=new DCheckBox (nullptr);
    repairCKB->setText("缺角修复");
    parHLayoutRepair->addWidget(repairCKB);
    parWidgetRepair->setLayout(parHLayoutRepair);

    //裁切设置
    rtVLayout->addWidget(lblNULLCut,0,Qt::AlignTop);
    rtVLayout->addWidget(lblCut,0,Qt::AlignTop);
    rtVLayout->addWidget(parWidgetNoCut,0,Qt::AlignTop);
    rtVLayout->addWidget(parWidgetSingleCut,0,Qt::AlignTop);
    //rtVLayout->addWidget(parWidgetMulCut,0,Qt::AlignTop);

    //水印设置
    rtVLayout->addWidget(lblNULLWM,0,Qt::AlignTop);
    rtVLayout->addWidget(lblWM,0,Qt::AlignTop);
    rtVLayout->addWidget(parWidgetNoWM,0,Qt::AlignTop);
    rtVLayout->addWidget(parWidgetFontWM,0,Qt::AlignTop);

    //文档处理设置
    rtVLayout->addWidget(lblNULLIE,0,Qt::AlignTop);
    rtVLayout->addWidget(lblIE,0,Qt::AlignTop);
    rtVLayout->addWidget(parWidgetBB,0,Qt::AlignTop);
    //rtVLayout->addWidget(parWidgetFD,0,Qt::AlignTop);
    rtVLayout->addWidget(parWidgetRepair,0,Qt::AlignTop);

    connect(colorBtn, SIGNAL(clicked()), this, SLOT(slotColorButtonClicked()));//颜色选择按钮信号槽
}

//图像格式和颜色模式UI
void ScanManagerWindow::showImgFormatAndTypeUI(bool isCamera)
{
    QWidget *parWidgetBB = new QWidget (rtWidget);
    parWidgetBB->setFixedHeight(48);
    GlobalHelper::setWidgetBackgroundColor(parWidgetBB,QColor(249,249,249,255));//容器设置背景
    QHBoxLayout *parHLayoutBB = new QHBoxLayout ();
    QLabel  *pnameLbl= new QLabel ();
    pnameLbl->setFixedWidth(80);
    pnameLbl->setText("图片格式");
    pnameLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgba(65,77,104,1);font-size:14px");

    imgFormatCBB = new DComboBox ();
    imgFormatCBB->addItem("jpg");
    imgFormatCBB->addItem("bmp");
    imgFormatCBB->addItem("tif");
    imgFormatCBB->addItem("png");
    parHLayoutBB->addWidget(pnameLbl);
    parHLayoutBB->addWidget(imgFormatCBB);
    parWidgetBB->setLayout(parHLayoutBB);

    rtVLayout->addWidget(parWidgetBB,0,Qt::AlignTop);

    if(isCamera)
    {
        QWidget *parWidgetBB2 = new QWidget (rtWidget);
        parWidgetBB2->setFixedHeight(48);
        GlobalHelper::setWidgetBackgroundColor(parWidgetBB2,QColor(249,249,249,255));//容器设置背景
        QHBoxLayout *parHLayoutBB2 = new QHBoxLayout ();
        QLabel  *pnameLbl2= new QLabel ();
        pnameLbl2->setFixedWidth(80);
        pnameLbl2->setText("色彩模式");
        pnameLbl2->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgba(65,77,104,1);font-size:14px");

        imgTypeCBB = new DComboBox ();
        imgTypeCBB->addItem("彩色图");
        imgTypeCBB->addItem("灰度图");
        imgTypeCBB->addItem("黑白图");
        parHLayoutBB2->addWidget(pnameLbl2);
        parHLayoutBB2->addWidget(imgTypeCBB);
        parWidgetBB2->setLayout(parHLayoutBB2);

        rtVLayout->addWidget(parWidgetBB2,0,Qt::AlignTop);
    }

}

//拍摄方式UI
void ScanManagerWindow::showShotTypeUI()
{
    QLabel *lblNULLIE=new QLabel();//用一个空白的label，达到上边距距离
    rtVLayout->addWidget(lblNULLIE,0,Qt::AlignTop);
    QLabel *lblIE = new QLabel ();
    lblIE->setText("拍摄方式");
    lblIE->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;color:rgba(0,26,46,1);font-size:17px");
    rtVLayout->addWidget(lblIE,0,Qt::AlignTop);

    QWidget *parWidgetBB = new QWidget (rtWidget);
    parWidgetBB->setFixedHeight(48);
    GlobalHelper::setWidgetBackgroundColor(parWidgetBB,QColor(249,249,249,255));//容器设置背景
    QHBoxLayout *parHLayoutBB = new QHBoxLayout ();
    shotTypeManualrbBtn = new DRadioButton (nullptr);
    shotTypeManualrbBtn->setText("手动拍摄");
    shotTypeManualrbBtn->setChecked(true);
    parHLayoutBB->addWidget(shotTypeManualrbBtn);
    parWidgetBB->setLayout(parHLayoutBB);
    rtVLayout->addWidget(parWidgetBB,0,Qt::AlignTop);

    QWidget *parWidgetBB2 = new QWidget (rtWidget);
    parWidgetBB2->setFixedHeight(48);
    GlobalHelper::setWidgetBackgroundColor(parWidgetBB2,QColor(249,249,249,255));//容器设置背景
    QHBoxLayout *parHLayoutBB2 = new QHBoxLayout ();
    shotTypeTimerrbBtn = new DRadioButton (nullptr);
    shotTypeTimerrbBtn->setText("定时");
    parHLayoutBB2->addWidget(shotTypeTimerrbBtn);
    shotTypeTimerCBB = new DComboBox ();
    shotTypeTimerCBB->addItem("5");
    shotTypeTimerCBB->addItem("8");
    shotTypeTimerCBB->addItem("10");
    shotTypeTimerCBB->setFixedWidth(80);
    parHLayoutBB2->addWidget(shotTypeTimerCBB);
    QLabel  *pnameLbl= new QLabel ();
    pnameLbl->setFixedWidth(80);
    pnameLbl->setText("秒拍摄");
    pnameLbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgba(65,77,104,1);font-size:14px");
    parHLayoutBB2->addWidget(pnameLbl);
    parHLayoutBB2->addStretch();

    parWidgetBB2->setLayout(parHLayoutBB2);
    rtVLayout->addWidget(parWidgetBB2,0,Qt::AlignTop);


    //单选按钮加入组，互斥效果
    QButtonGroup *btnGroup=new QButtonGroup ();
    btnGroup->addButton(shotTypeManualrbBtn);
    btnGroup->addButton(shotTypeTimerrbBtn);
    btnGroup->setExclusive(true);

}

//无SANE设备UI
void ScanManagerWindow::slotNoScannerUI()
{
    isHaveScanner = false;
    closeScannerThread();//停止SANE线程
}

//无拍摄仪UI
void ScanManagerWindow::slotNoCameraUI()
{
    if(isHaveScanner == false)//无扫描仪
    {
        lLbl->setText("未找到扫描设备");
        rtLbl->setText("没有找到连接到您计算机的扫描设备");

        rtipLayout->removeWidget(rbLbl);//移除提示信息label
        delete rbLbl;//移除控件，一定要delete，不然还会显示在UI上

        lSpinner->stop();
        rtipLayout->removeWidget(lSpinner);//移除提示信息动画
        delete lSpinner;
    }
    closeCameraThread();//停止拍摄仪线程
}

/*
 * SANE设备信息,由线程通知此函数，数据显示至UI
 * qv 设备参数键值对集合
 * int=设备类型，0=拍摄仪，1=扫描仪
 * QStringList=设备信息的值集合
 * 如：<1,"扫描仪,UNIS F3135,空闲">
 */
void ScanManagerWindow::slotScannerInfoUI(QVariant qv, const QString &str)
{
    //设备信息数据
    QMap<int ,QStringList> devInfoMap = qv.value<QMap<int ,QStringList>>();//还原为原来的数据结构类型
    if(devInfoMap.size() <= 0)
    {
        //slotNoDeviceUI();//无设备，显示无设备UI
        closeScannerThread();
        return;
    }

    isHaveScanner = true;

    //加入设备列表UI
    showDeviceListUI();

    //清理提示信息控件
    rVLayout->addWidget(rbWidget);//加入右侧下部的容器,开始按钮
    rtVLayout->removeWidget(rtNLbl);//移除右上空白占位提示信息label
    delete rtNLbl;
    rtVLayout->removeWidget(rtipWidget);//移除右侧-上部提示信息label
    delete rtipWidget;

    //循环加入设备Item
    QMap<int,QStringList>::iterator itDevInfo;
    for(itDevInfo = devInfoMap.begin();itDevInfo!=devInfoMap.end();itDevInfo++)
    {
        QStringList tmpQSList = itDevInfo.value();//参数值
        qDebug()<<"线程回传的设备信息数据,key:"<<itDevInfo.key()<<",value:"<<itDevInfo.value();
        if(tmpQSList.size()==4)
        {
            /*
             * 新增设备Item
             * type=设备类型(0=拍摄仪，1=扫描仪)
             * type=设备名称
             * model=设备型号
             * status=设备状态(空闲或者忙碌)
             * index=设备下标
             */
            addDeviceItem(1,tmpQSList.at(0),tmpQSList.at(1),tmpQSList.at(2),tmpQSList.at(3).toInt());
        }
    }
    closeScannerThread();

}

/*
 * SANE设备参数信息(信号槽不可以传复杂类型的参数，需要转化成QVariant作为参数)
 * parInfo 设备参数键值对集合
 * int=参数的下标
 * QStringList=参数的值集合
 * 如：<1,"Color24,Gary8,Lineart">
 */
void ScanManagerWindow::slotScannerParInfoUI(QVariant qv,const QString &str)
{
    //设备参数数据
    int i=0;
    QMap<int ,QMap<QString ,QStringList>> devParsMap = qv.value<QMap<int ,QMap<QString ,QStringList>>>();//还原为原来的数据结构类型
    QMap<int ,QMap<QString ,QStringList>>::iterator itDevParInfo;
    for(itDevParInfo = devParsMap.begin();itDevParInfo!=devParsMap.end();itDevParInfo++)
    {
        /*
         * 设备参数UI <0,("Color24", "Gray8", "Lineart")>
         * titleName=抬头名称(如：扫描设置),
         * parName=参数名称(如：分辨率)，
         * parIndex=参数下标，从设备中读取到的顺序
         * parType=参数类型(0=文本框，1=下拉框))
         * parValue=参数值（如：200DPI<<300DPI）
         * isAddTitleTopMargin=抬头是否需要加入上边距
         * isLastPar=是否为最后一个参数,布局需要加一个addStretch来控制参数行居上
         */

        //参数值 如：<色彩模式,"Color24,Gary8,Lineart">
        QMap<QString ,QStringList> tmpValueMap = itDevParInfo.value();
        QMap<QString ,QStringList>::iterator itTmpDevParInfo;
        for(itTmpDevParInfo = tmpValueMap.begin();itTmpDevParInfo!=tmpValueMap.end();itTmpDevParInfo++)
        {
            //读取配置文件，判断参数是否显示
            int nParIsShow = GlobalHelper::readSettingValue("show",itTmpDevParInfo.key()).toInt();
            if(nParIsShow == 1)
            {
                continue;
            }

            if(i==0)
            {
                showDeviceParUI("扫描设置",itTmpDevParInfo.key(),QString::number(itDevParInfo.key()),1,itTmpDevParInfo.value(),false,false);//UI显示参数
            }
            else
            {
                showDeviceParUI("",itTmpDevParInfo.key(),QString::number(itDevParInfo.key()),1,itTmpDevParInfo.value(),false,false);//UI显示参数
            }

            scannerParMap.insert(itDevParInfo.key(),itTmpDevParInfo.value());//参数加入集合
            if(itTmpDevParInfo.value().size()>0)
            {
                scannerChoiseParMap.insert(itDevParInfo.key(),itTmpDevParInfo.value().at(0));//选中参数集合默认加入第一个参数
            }
        }
        i++;
        qDebug()<<"thread back scanner par info,key:"<<itDevParInfo.key()<<",value:"<<itDevParInfo.value();

    }

    showImgFormatAndTypeUI(false);//图片格式UI
    showImgEditParUI();//图像算法UI
    closeScannerThread();//停止SANE线程
}

//拍摄仪信息
void ScanManagerWindow::slotCameraInfoUI(QVariant qv, const QString &str)
{
    //设备信息数据
    QMap<int ,QStringList> cameraInfoMap = qv.value<QMap<int ,QStringList>>();//还原为原来的数据结构类型
    if(cameraInfoMap.size() <= 0 && isHaveScanner == false)
    {
        //无设备，显示无设备UI
        slotNoScannerUI();
        closeCameraThread();
        return;
    }
    if(isHaveScanner == false)//无扫描仪
    {
        isHaveCamera = true;
        //加入设备列表UI
        showDeviceListUI();

        //清理提示信息控件
        rVLayout->addWidget(rbWidget);//加入右侧下部的容器,开始按钮
        rtVLayout->removeWidget(rtNLbl);//移除右上空白占位提示信息label
        delete rtNLbl;
        rtVLayout->removeWidget(rtipWidget);//移除右侧-上部提示信息label
        delete rtipWidget;
    }

    //循环加入设备Item
    QMap<int,QStringList>::iterator itCameraInfo;
    for(itCameraInfo = cameraInfoMap.begin();itCameraInfo!=cameraInfoMap.end();itCameraInfo++)
    {
        //如：<0,"拍摄仪,SD544,空闲,设备下标">
        QStringList tmpQSList = itCameraInfo.value();//参数值
        qDebug()<<"Camera线程回传的设备信息数据,key:"<<itCameraInfo.key()<<",value:"<<itCameraInfo.value();
        if(tmpQSList.size()==4)
        {
            /*
             * 新增设备Item
             * type=设备类型(0=拍摄仪，1=扫描仪)
             * type=设备名称
             * model=设备型号
             * status=设备状态(空闲或者忙碌)
             * index=设备下标
             */
            addDeviceItem(0,tmpQSList.at(0),tmpQSList.at(1),tmpQSList.at(2),tmpQSList.at(3).toInt());
        }
    }

    closeCameraThread();//停止拍摄仪线程

}

//拍摄仪参数信息
void ScanManagerWindow::slotCameraParInfoUI(QVariant qv,const QString &str)
{
    //设备参数数据
    int i=0;
    QMap<int ,QMap<QString ,QStringList>> devParsMap = qv.value<QMap<int ,QMap<QString ,QStringList>>>();//还原为原来的数据结构类型
    QMap<int ,QMap<QString ,QStringList>>::iterator itDevParInfo;
    for(itDevParInfo = devParsMap.begin();itDevParInfo!=devParsMap.end();itDevParInfo++)
    {
        /*
         * 设备参数UI <色彩模式,("Color24", "Gray8", "Lineart")>
         * titleName=抬头名称(如：扫描设置),
         * parName=参数名称(如：分辨率)，
         * parIndex=参数下标，从设备中读取到的顺序
         * parType=参数类型(0=文本框，1=下拉框))
         * parValue=参数值（如：200DPI<<300DPI）
         * isAddTitleTopMargin=抬头是否需要加入上边距
         * isLastPar=是否为最后一个参数,布局需要加一个addStretch来控制参数行居上
         */
        QMap<QString ,QStringList> tmpValueMap = itDevParInfo.value();
        QMap<QString ,QStringList>::iterator itTmpDevParInfo;
        for(itTmpDevParInfo = tmpValueMap.begin();itTmpDevParInfo!=tmpValueMap.end();itTmpDevParInfo++)
        {
            if(i==0)
            {
                showDeviceParUI("扫描设置",itTmpDevParInfo.key(),itTmpDevParInfo.key(),1,itTmpDevParInfo.value(),false,false);//UI显示参数
            }
            else
            {
                showDeviceParUI("",itTmpDevParInfo.key(),itTmpDevParInfo.key(),1,itTmpDevParInfo.value(),false,false);//UI显示参数
            }

            cameraParMap.insert(itTmpDevParInfo.key(),itTmpDevParInfo.value());//参数加入集合
            if(itTmpDevParInfo.value().size()>0)//选中参数集合默认加入第一个参数
            {
                if(itTmpDevParInfo.key() == "format_c")//format记录选中的下标
                {
                    cameraChoiseParMap.insert(itTmpDevParInfo.key(),QString::number(0));
                }
                else
                {
                    cameraChoiseParMap.insert(itTmpDevParInfo.key(),itTmpDevParInfo.value().at(0));
                }
                if(itTmpDevParInfo.key() == "resolution_c")//默认分辨率
                {
                    //选中参数集合是否包含此默认分辨率
                    if(cameraChoiseParMap.contains("DefaultResolution") == false)
                    {
                        cameraChoiseParMap.insert("DefaultResolution",itTmpDevParInfo.value().at(0));
                    }
                }
                //cameraChoiseParMap.insert(itTmpDevParInfo.key(),itTmpDevParInfo.value().at(0));
            }
        }

        i++;
        qDebug()<<"thread back camera par info,key:"<<itDevParInfo.key()<<",value:"<<itDevParInfo.value();

    }

    showImgFormatAndTypeUI(true);//图片格式UI
    showShotTypeUI();//拍摄方式UI
    showImgEditParUI();//图像算法UI
    closeCameraThread();

}

//开始按钮
void ScanManagerWindow::slotScanButtonClicked()
{
    GlobalHelper::writeSettingValue("set","imgPreNameIndex","0");//扫描文件的名称编号置0
    qDebug()<<"当前设备类型（0=拍摄仪，1=扫描仪）："<<nCurrentDeviceType<<",当前设备下标:"<<nCurrentDeviceIndex;
    if(nCurrentDeviceType == 1)//扫描仪
    {
        getImgEditPar();//从UI上获取图像处理参数
        //if(scanWindow == NULL)
        {
            scanWindow = new ScanWindow (scannerChoiseParMap,this);
        }
        scanWindow->setAttribute(Qt::WA_ShowModal,true);//模态窗口
        scanWindow->show();
        //屏幕中间显示
        scanWindow->move ((QApplication::desktop()->width() - scanWindow->width())/2,
                          (QApplication::desktop()->height() - scanWindow->height())/2);
        this->close();//本窗口关闭
    }
    else if(nCurrentDeviceType == 0)//拍摄仪
    {
        getImgEditPar();//从UI上获取图像处理参数
        //if(cameraWindow == NULL)
        {
            cameraWindow = new CameraWindow (nCurrentDeviceIndex,cameraChoiseParMap,this) ;
        }
        cameraWindow->setAttribute(Qt::WA_ShowModal,true);//模态窗口
        cameraWindow->show();
        cameraWindow->move ((QApplication::desktop()->width() - cameraWindow->width())/2,
                          (QApplication::desktop()->height() - cameraWindow->height())/2);
        this->close();//本窗口关闭

        //拍摄仪窗口的关闭信号与主窗口的刷新数据槽连接
        MainWindow *mainW = (MainWindow*)this->parent();
        connect(cameraWindow,SIGNAL(signalWindowClosed()),mainW,SLOT(slotRefreshData()));

    }

}

//设备列表点击事件
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
        qDebug()<<"device type:"<<d.devType<<"device name:"<<d.devName<<",device index:"<<d.devIndex;

        nCurrentDeviceType = d.devType;//当前设备类型，0=拍摄仪，1=扫描仪
        nCurrentDeviceIndex = d.devIndex;//当前设备下标
        getDevicePar(d.devType,d.devIndex);//获取设备参数，并显示在UI

        break;
    }
}

//下拉框选项改变
void ScanManagerWindow::slotComboBoxCurrentIndexChanged(const int index)
{
    DComboBox *cb = qobject_cast<DComboBox*>(sender());//找到有哪一个控件发出的信号
    QString keyName = cb->objectName();//key名
    qDebug()<<"当前设备类型（0=拍摄仪，1=扫描仪）："<<nCurrentDeviceType<<",下拉框名称:"<<keyName<<",选中下标"<<index;

    QStringList valueList ;//value值
    if(nCurrentDeviceType == 1)//扫描仪
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
    else if(nCurrentDeviceType == 0)//拍摄仪
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
            if(cb->objectName() == "format_c")//format记录选中的下标
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
            if(cb->objectName() == "format_c")//format记录选中的下标
            {
                cameraChoiseParMap.insert(keyName,QString::number(index));//加入值
            }
            else
            {
                cameraChoiseParMap.insert(keyName,valueList.at(index));//加入值
            }
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
}

//获取图像处理参数
void ScanManagerWindow::getImgEditPar()
{
    //单图裁剪单选按钮
    if(singleCutrbBtn->isChecked())
    {
        GlobalHelper::writeSettingValue("imgEdit","isCut","0");
    }
    //不裁剪单选按钮
    if(noCutrbBtn->isChecked())
    {
        GlobalHelper::writeSettingValue("imgEdit","isCut","1");
    }
    //多图裁剪单选按钮
    if(mulCutrbBtn->isChecked())
    {
        GlobalHelper::writeSettingValue("imgEdit","isCut","2");
    }
    //无水印单选按钮
    if(noWaterrbBtn->isChecked())
    {
        GlobalHelper::writeSettingValue("imgEdit","isWaterMark","1");
    }
    //文字水印单选按钮
    if(fontWaterrbBtn->isChecked())
    {
        GlobalHelper::writeSettingValue("imgEdit","isWaterMark","0");
    }
    //水印文本
    GlobalHelper::writeSettingValue("imgEdit","waterMarkText",fontWaterText->toPlainText());

    //文档类型下拉框(0=原始文档，1=文档优化，2=彩色优化，3=红印文档优化，4=反色，5=滤红)
    int nDocType = docTypeCBB->currentIndex();
    GlobalHelper::writeSettingValue("imgEdit","docType",QString::number(nDocType));

    //去噪复选框
    if(filterDenoisingCKB->isChecked())
    {
        GlobalHelper::writeSettingValue("imgEdit","isFilterDenoising","0");
    }
    else
    {
        GlobalHelper::writeSettingValue("imgEdit","isFilterDenoising","1");
    }

    //缺角修复复选框
    if(repairCKB->isChecked())
    {
        GlobalHelper::writeSettingValue("imgEdit","isRepair","0");
    }
    else
    {
        GlobalHelper::writeSettingValue("imgEdit","isRepair","1");
    }

    //文件类型
    GlobalHelper::writeSettingValue("imgEdit","imgFormat",imgFormatCBB->currentText());

    //拍摄仪
    if(nCurrentDeviceType == 0)
    {
        //色彩模式（彩色，灰度，黑白）
        GlobalHelper::writeSettingValue("imgEdit","imgType",QString::number(imgTypeCBB->currentIndex()));

        //手动拍摄单选按钮
        if(shotTypeManualrbBtn->isChecked())
        {
            GlobalHelper::writeSettingValue("imgEdit","shotType","0");
        }

        //定时拍摄单选按钮
        if(shotTypeTimerrbBtn->isChecked())
        {
            GlobalHelper::writeSettingValue("imgEdit","shotType","1");
            //定时拍摄时间下拉框
            GlobalHelper::writeSettingValue("imgEdit","shotTime",shotTypeTimerCBB->currentText());
        }
    }

}


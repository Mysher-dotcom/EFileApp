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


ScanManagerWindow::ScanManagerWindow(QWidget *parent) :
    DMainWindow(parent),
    ui(new Ui::ScanManagerWindow)
{
    ui->setupUi(this);

    isShowDeviceList = false;
    initUI();

    //线程执行中，不加载设备列表
    if( GlobalHelper::getDeviceInfoIsOver == true)
    {
        qDebug()<<"加载设备列表";
        slotGetDeviceList();
    }
    else
    {
        qDebug()<<"后台正在加载设备参数，等待...";
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

    showDeviceListUI();
    for(int i=0;i<devList.length();i++)
    {
        QString strStatus = "未连接";
        if(devList.at(i).status == 0)
        {
            strStatus = "空闲";
        }
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
    rVLayout->addWidget(rbWidget);//加入右侧下部的容器,开始按钮

    if(isShowDeviceList == false)
    {
        //清理提示信息控件
        rtVLayout->removeWidget(rtNLbl);//移除右上空白占位提示信息label
        delete rtNLbl;
        rtVLayout->removeWidget(rtipWidget);//移除右侧-上部提示信息label
        delete rtipWidget;
    }

}

//显示设备列表控件UI
void ScanManagerWindow::showDeviceListUI()
{
    if(isShowDeviceList == true)
    {
        return;
    }
    //窗口的左侧布局加入设备列表容器
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
    lVLayout->addWidget(devListView);
    lVLayout->removeWidget(lTipWidget);//移除提示信息文字和动画
    delete lTipWidget;//移除控件，一定要delete，不然还会显示在UI上
    //设备列表点击信号槽
    connect(devListView,SIGNAL(pressed(const QModelIndex)),this,SLOT(slotDevListPressed(const QModelIndex)));

    isShowDeviceList = true;
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
            QString tmpParValue = GlobalHelper::readSettingValue("lan",parValue.at(i));
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

    /*
    QWidget *parWidgetImage = new QWidget (rtWidget);
    parWidgetImage->setFixedHeight(48);
    GlobalHelper::setWidgetBackgroundColor(parWidgetImage,QColor(249,249,249,255));//容器设置背景
    QHBoxLayout *parHLayoutImage = new QHBoxLayout ();
    imageWaterBtn=new DRadioButton (nullptr);
    imageWaterBtn->setText("图片水印");
    parHLayoutImage->addWidget(imageWaterBtn);
    parWidgetImage->setLayout(parHLayoutImage);
    */


    //单选按钮加入组，互斥效果
    QButtonGroup *btnGroup2=new QButtonGroup ();
    btnGroup2->addButton(noWaterrbBtn);
    btnGroup2->addButton(fontWaterrbBtn);
   // btnGroup2->addButton(imageWaterBtn);
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

    /*
    QWidget *parWidgetFD = new QWidget (rtWidget);
    parWidgetFD->setFixedHeight(48);
    GlobalHelper::setWidgetBackgroundColor(parWidgetFD,QColor(249,249,249,255));//容器设置背景
    QHBoxLayout *parHLayoutFD = new QHBoxLayout ();
    filterDenoisingCKB=new DCheckBox (nullptr);
    filterDenoisingCKB->setText("去噪");
    parHLayoutFD->addWidget(filterDenoisingCKB);
    parWidgetFD->setLayout(parHLayoutFD);
    */

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

    connect(colorBtn, SIGNAL(clicked()), this, SLOT(slotColorButtonClicked()));//颜色选择按钮信号槽

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
        parWidgetBB->setEnabled(false);
        parWidgetRepair->setEnabled(false);
    }

}

//图像格式和颜色模式UI
void ScanManagerWindow::showImgFormatAndTypeUI(bool isCamera,bool isLicense)
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

    /*
    QWidget *parWidgetAuto = new QWidget (rtWidget);
    parWidgetAuto->setFixedHeight(48);
    GlobalHelper::setWidgetBackgroundColor(parWidgetAuto,QColor(249,249,249,255));//容器设置背景
    QHBoxLayout *parHLayoutAuto = new QHBoxLayout ();
    shotTypeAutoCaptureBtn = new DRadioButton (nullptr);
    shotTypeAutoCaptureBtn->setText("自动拍摄");
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
    lLbl->setText("未找到扫描设备");
    rtLbl->setText("没有找到连接到您计算机的扫描设备");
    rtipLayout->removeWidget(rbLbl);//移除提示信息label
    delete rbLbl;//移除控件，一定要delete，不然还会显示在UI上
    lSpinner->stop();
    rtipLayout->removeWidget(lSpinner);//移除提示信息动画
    delete lSpinner;
}

//开始按钮
void ScanManagerWindow::slotScanButtonClicked()
{
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
        //屏幕中间显示
        scanWindow->move ((QApplication::desktop()->width() - scanWindow->width())/2,
                          (QApplication::desktop()->height() - scanWindow->height())/2);
        this->close();//本窗口关闭
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
        cameraWindow->move ((QApplication::desktop()->width() - cameraWindow->width())/2,
                          (QApplication::desktop()->height() - cameraWindow->height())/2);
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
                        if(i==0) titleName="扫描设置";
                        showDeviceParUI(titleName,parName,parIndex,1,parValueList,false,false);//UI显示参数

                        scannerParMap.insert(parIndex.toInt(),parValueList);//参数加入集合
                        if(parValueList.size()>0)
                        {
                            //scannerChoiseParMap.insert(parIndex.toInt(),parValueList.at(0));//选中参数集合默认加入第一个参数
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

                showDeviceParUI("扫描设置","resolution","-1",1,resolutionList,false,false);//UI显示参数
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

        //记录默认设备
        DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceListInfoFilePath(),
                                     "default",
                                     "name",
                                     d.devModel);
        DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceListInfoFilePath(),
                                     "default",
                                     "config",
                                     DeviceInfoHelper::getDeviceInfoFilePath(d.devModel));

        break;
    }
   // showImgEditParUI();//图像算法UI

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
    //单图裁剪单选按钮
    if(singleCutrbBtn->isChecked())
    {
        GlobalHelper::writeSettingValue("imgEdit","isCut","1");
        DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","isCut","1");
    }
    //不裁剪单选按钮
    if(noCutrbBtn->isChecked())
    {
        GlobalHelper::writeSettingValue("imgEdit","isCut","0");
        DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","isCut","0");
    }
    //多图裁剪单选按钮
    if(mulCutrbBtn->isChecked())
    {
        GlobalHelper::writeSettingValue("imgEdit","isCut","2");
        DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","isCut","2");
    }
    //无水印单选按钮
    if(noWaterrbBtn->isChecked())
    {
        GlobalHelper::writeSettingValue("imgEdit","isWaterMark","1");
        DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","isWaterMark","1");
    }
    //文字水印单选按钮
    if(fontWaterrbBtn->isChecked())
    {
        GlobalHelper::writeSettingValue("imgEdit","isWaterMark","0");
        DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","isWaterMark","0");
    }
    //水印文本
    GlobalHelper::writeSettingValue("imgEdit","waterMarkText",fontWaterText->toPlainText());
    DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","waterMarkText",fontWaterText->toPlainText());

    //文档类型下拉框(0=原始文档，1=文档优化，2=彩色优化，3=红印文档优化，4=反色，5=滤红)
    int nDocType = docTypeCBB->currentIndex();
    GlobalHelper::writeSettingValue("imgEdit","docType",QString::number(nDocType));
    DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","docType",QString::number(nDocType));

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
        GlobalHelper::writeSettingValue("imgEdit","isRepair","0");
        DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","isRepair","0");
    }
    else
    {
        GlobalHelper::writeSettingValue("imgEdit","isRepair","1");
        DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","isRepair","1");
    }

    //文件类型
    GlobalHelper::writeSettingValue("imgEdit","imgFormat",imgFormatCBB->currentText());
    DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","imgFormat",imgFormatCBB->currentText());

    //拍摄仪
    if(nCurrentDeviceType == 1)
    {
        //色彩模式（彩色，灰度，黑白）
        GlobalHelper::writeSettingValue("imgEdit","imgType",QString::number(imgTypeCBB->currentIndex()));
        DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","imgType",QString::number(imgTypeCBB->currentIndex()));

        //手动拍摄单选按钮
        if(shotTypeManualrbBtn->isChecked())
        {
            GlobalHelper::writeSettingValue("imgEdit","shotType","0");
            DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","shotType","0");
        }

        //定时拍摄单选按钮
        if(shotTypeTimerrbBtn->isChecked())
        {
            GlobalHelper::writeSettingValue("imgEdit","shotType","1");
            GlobalHelper::writeSettingValue("imgEdit","shotTime",shotTypeTimerCBB->currentText());//定时拍摄时间下拉框
            DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","shotType","1");
            DeviceInfoHelper::writeValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","shotTime",shotTypeTimerCBB->currentText());
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
        noWaterrbBtn->setChecked(true);
    else
        fontWaterrbBtn->setChecked(true);

    //水印文本
    QString waterMarkText = DeviceInfoHelper::readValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","waterMarkText");
    fontWaterText->setText(waterMarkText);

    //文档类型下拉框(0=原始文档，1=文档优化，2=彩色优化，3=红印文档优化，4=反色，5=滤红)
    QString nDocType = DeviceInfoHelper::readValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","docType");
    docTypeCBB->setCurrentIndex(nDocType.toInt());

    //缺角修复
    QString isRepair = DeviceInfoHelper::readValue(DeviceInfoHelper::getDeviceInfoFilePath(currentDeviceModel),"imgset","isRepair");
    if(isRepair.toInt() == 0)
        repairCKB->setChecked(true);
    else
        repairCKB->setChecked(false);

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


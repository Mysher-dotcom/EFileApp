#include "setwindow.h"
#include "ui_setwindow.h"
#include <DColorDialog>
#include <QHBoxLayout>
#include <DFileDialog>
#include "helper/globalhelper.h"
#include <DLabel>

SetWindow::SetWindow(QWidget *parent) :
    DMainWindow(parent),
    ui(new Ui::SetWindow)
{
    ui->setupUi(this);
    initUI();
}

SetWindow::~SetWindow()
{
    delete ui;
}

void SetWindow::closeEvent(QCloseEvent *event)
{
    emit signalWindowClosed();//窗口关闭信号
}

//当前系统是否为深色主题
bool SetWindow::isDarkType()
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

void SetWindow::initUI()
{
    int nDZDA= GlobalHelper::readSettingValue("set","dzda").toInt();//是否显示分类显示开关
    if(nDZDA == 0)
    {
        this->resize(460,380);
    }
    else
    {
        this->resize(460,320);
    }
    //*窗口基本属性设置*
    //this->resize(460,320);//窗口初始尺寸 460,380
    this->titlebar()->setTitle("");//标题栏文字设为空
    setWindowIcon(QIcon(":/img/logo/logo-16.svg"));// 状态栏图标
    this->titlebar()->setIcon(QIcon(":/img/logo/logo-16.svg"));//标题栏图标
    this->titlebar()->setMenuVisible(false);//菜单按钮隐藏
    this->setWindowFlags(this->windowFlags()&~Qt::WindowMinMaxButtonsHint);//最小化、最大化按钮隐藏
    this->setFixedSize(this->width(),this->height());//禁止拖动改变窗口尺寸
    this->setWindowTitle(tr("Scan Assistant"));//开始菜单栏上鼠标悬浮在窗口上显示的名称

    //*窗口控件设置*
    QVBoxLayout *winLayout=new QVBoxLayout();//窗口的布局，最外层
    DWidget *mainWidget=new DWidget ();
    if(isDarkType() == true)
    {
        //深色主题
        //mainWidget->setStyleSheet(".DWidget#mainWidget{background:rgba(255,255,255,12);border-radius:8px;}");
        GlobalHelper::setWidgetBackgroundColor(mainWidget,QColor(255,255,255,12),false);
    }
    else
    {
        //mainWidget->setStyleSheet("DWidget#mainWidget{background:rgba(255,255,255,255);border-radius:8px;}");
        //GlobalHelper::setWidgetBackgroundColor(mainWidget,QColor(255,255,255,255),false);
        mainWidget->setGeometry(0, 0, 300, 100);  //主容器设置背景白色
        QPalette palMain(mainWidget->palette());
        palMain.setColor(QPalette::Background, Qt::white);
        mainWidget->setAutoFillBackground(true);
        mainWidget->setPalette(palMain);
    }

    QVBoxLayout *mainVLayout=new QVBoxLayout ();//主容器布局

    DLabel *lbl = new DLabel ();
    lbl->setText(tr("General Settings"));//扫描管理设置
    mainVLayout->addWidget(lbl,0,Qt::AlignTop);
    if(isDarkType() == true)
    {
        //深色主题
        lbl->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;color:rgb(192,198,212);font-size:17px");
    }
    else
    {
        lbl->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;color:rgba(0,26,46,1);font-size:17px");
    }

    DWidget *parWidget = new DWidget ();
    if(isDarkType() == true)
    {
        //深色主题
        GlobalHelper::setWidgetBackgroundColor(parWidget,QColor(255,255,255,12));
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(parWidget,QColor(249,249,249,255));
    }

    QHBoxLayout *parHLayout = new QHBoxLayout ();
    DLabel  *parTitleLbl= new DLabel ();
    parTitleLbl->setText(tr("Location"));//缓存位置
    setParTitleLabelStyle(parTitleLbl,80);
    parHLayout->addWidget(parTitleLbl);
    savePathText = new DTextEdit ();
    savePathText->setFixedHeight(30);
    savePathText->setText(GlobalHelper::readSettingValue("set","savepath"));//读取配置文件
    parHLayout->addWidget(savePathText);
    selectSavePathBtn = new DSuggestButton ();
    selectSavePathBtn->setFixedHeight(30);
    selectSavePathBtn->setText("...");
    parHLayout->addWidget(selectSavePathBtn);
    parWidget->setLayout(parHLayout);//参数行的容器加入布局

    DWidget *parWidget2 = new DWidget ();
    if(isDarkType() == true)
    {
        //深色主题
        GlobalHelper::setWidgetBackgroundColor(parWidget2,QColor(255,255,255,12));
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(parWidget2,QColor(249,249,249,255));
    }
    QHBoxLayout *parHLayout2 = new QHBoxLayout ();
    DLabel *parTitleLbl2= new DLabel ();
    parTitleLbl2->setText(tr("Notify me when overwriting"));//覆盖时警告
    setParTitleLabelStyle(parTitleLbl2,200);
    parHLayout2->addWidget(parTitleLbl2);
    parHLayout2->addStretch();
    fileOverWarnSBtn = new DSwitchButton ();
    parHLayout2->addWidget(fileOverWarnSBtn);
    parWidget2->setLayout(parHLayout2);//参数行的容器加入布局

    DWidget *parWidget3 = new DWidget ();
    if(isDarkType() == true)
    {
        //深色主题
        GlobalHelper::setWidgetBackgroundColor(parWidget3,QColor(255,255,255,12));
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(parWidget3,QColor(249,249,249,255));
    }
    QHBoxLayout *parHLayout3 = new QHBoxLayout ();
    DLabel *parTitleLbl3= new DLabel ();
    parTitleLbl3->setText(tr("Notify me when no scanners found"));//无扫描仪时警告
    setParTitleLabelStyle(parTitleLbl3,200);
    parHLayout3->addWidget(parTitleLbl3);
    parHLayout3->addStretch();
    noDeviceWarnSBtn = new DSwitchButton ();
    parHLayout3->addWidget(noDeviceWarnSBtn);
    parWidget3->setLayout(parHLayout3);//参数行的容器加入布局

    DWidget *parWidget4 = new DWidget ();
    if(isDarkType() == true)
    {
        //深色主题
        GlobalHelper::setWidgetBackgroundColor(parWidget4,QColor(255,255,255,12));
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(parWidget4,QColor(249,249,249,255));
    }
    QHBoxLayout *parHLayout4 = new QHBoxLayout ();
    DLabel *parTitleLbl4= new DLabel ();
    parTitleLbl4->setText(tr("Play a sound when completed"));//完成后提示音
    setParTitleLabelStyle(parTitleLbl4,200);
    parHLayout4->addWidget(parTitleLbl4);
    parHLayout4->addStretch();
    finishBeepSBtn = new DSwitchButton ();
    parHLayout4->addWidget(finishBeepSBtn);
    parWidget4->setLayout(parHLayout4);//参数行的容器加入布局

    DWidget *parWidget5 = new DWidget ();
    if(isDarkType() == true)
    {
        //深色主题
        GlobalHelper::setWidgetBackgroundColor(parWidget5,QColor(255,255,255,12));
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(parWidget5,QColor(249,249,249,255));
    }
    QHBoxLayout *parHLayout5 = new QHBoxLayout ();
    DLabel *parTitleLbl5= new DLabel ();
    parTitleLbl5->setText(tr("Exit when completed"));//完成后退出
    setParTitleLabelStyle(parTitleLbl5,200);
    parHLayout5->addWidget(parTitleLbl5);
    parHLayout5->addStretch();
    finishExitSBtn = new DSwitchButton ();
    parHLayout5->addWidget(finishExitSBtn);
    parWidget5->setLayout(parHLayout5);//参数行的容器加入布局

    DWidget *parWidget6 = new DWidget ();
    if(isDarkType() == true)
    {
        //深色主题
        GlobalHelper::setWidgetBackgroundColor(parWidget6,QColor(255,255,255,12));
    }
    else
    {
        GlobalHelper::setWidgetBackgroundColor(parWidget6,QColor(249,249,249,255));
    }
    QHBoxLayout *parHLayout6 = new QHBoxLayout ();
    DLabel *parTitleLbl6= new DLabel ();
    parTitleLbl6->setText("条码识别分类");
    setParTitleLabelStyle(parTitleLbl6,200);
    parHLayout6->addWidget(parTitleLbl6);
    parHLayout6->addStretch();
    classificationSBtn = new DSwitchButton ();
    parHLayout6->addWidget(classificationSBtn);
    parWidget6->setLayout(parHLayout6);//参数行的容器加入布局

    mainVLayout->addWidget(parWidget,0,Qt::AlignTop);
    mainVLayout->addWidget(parWidget2,0,Qt::AlignTop);
    //mainVLayout->addWidget(parWidget3,0,Qt::AlignTop);//无扫描仪时警告
    mainVLayout->addWidget(parWidget4,0,Qt::AlignTop);
    mainVLayout->addWidget(parWidget5,0,Qt::AlignTop);
    if(nDZDA == 0)
    {
        mainVLayout->addWidget(parWidget6,0,Qt::AlignTop);
    }

    mainVLayout->addStretch();
    mainWidget->setLayout(mainVLayout);
    winLayout->addWidget(mainWidget);
    ui->centralwidget->setLayout(winLayout);

    //读取配置文件,设置UI
    int nfileover= GlobalHelper::readSettingValue("set","fileover").toInt();
    int nnodevice= GlobalHelper::readSettingValue("set","nodevice").toInt();
    int nfinishbeep= GlobalHelper::readSettingValue("set","finishbeep").toInt();
    int nfinishexit= GlobalHelper::readSettingValue("set","finishexit").toInt();
    int nClassification= GlobalHelper::readSettingValue("set","classification").toInt();

    if(nfileover == 0)
        fileOverWarnSBtn->setChecked(true);
    else
        fileOverWarnSBtn->setChecked(false);

    if(nnodevice == 0)
        noDeviceWarnSBtn->setChecked(true);
    else
        noDeviceWarnSBtn->setChecked(false);

    if(nfinishbeep == 0)
        finishBeepSBtn->setChecked(true);
    else
        finishBeepSBtn->setChecked(false);

    if(nfinishexit == 0)
        finishExitSBtn->setChecked(true);
    else
        finishExitSBtn->setChecked(false);



    if(nClassification == 0)
    {
        classificationSBtn->setChecked(true);
    }
    else
    {
        classificationSBtn->setChecked(false);
    }

    connect(selectSavePathBtn, SIGNAL(clicked()), this, SLOT(onBtnSelectSavePathClicked()));
    connect(fileOverWarnSBtn, SIGNAL(checkedChanged(bool)), this, SLOT(onSBtnFileOverCheckedChanged(bool)));
    connect(noDeviceWarnSBtn, SIGNAL(checkedChanged(bool)), this, SLOT(onSBtnNoDeviceCheckedChanged(bool)));
    connect(finishBeepSBtn, SIGNAL(checkedChanged(bool)), this, SLOT(onSBtnFinishBeepCheckedChanged(bool)));
    connect(finishExitSBtn, SIGNAL(checkedChanged(bool)), this, SLOT(onSBtnFinishExitCheckedChanged(bool)));
    connect(classificationSBtn, SIGNAL(checkedChanged(bool)), this, SLOT(onSBtnClassificationCheckedChanged(bool)));

}

//参数名称label设置样式
void SetWindow::setParTitleLabelStyle(QLabel *lbl,int w)
{
    lbl->setFixedWidth(w);

    if(isDarkType() == true)
    {
        //深色主题
        lbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgb(192,198,212);font-size:14px");
    }
    else
    {
        lbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgba(65,77,104,1);font-size:14px");
    }
}

void SetWindow::onBtnColorClicked()
{

    QColor color=DColorDialog::getColor(Qt::black);
    //int cred = QString::number(color.red(),10).toInt();
    //int cgreen = QString::number(color.green(),10).toInt();
    //int cblue = QString::number(color.blue(),10).toInt();
    //int calpha = QString::number(color.alpha(),10).toInt();
    QPixmap pixmap(btnColor->width(),btnColor->height());
    QPainter painter(&pixmap);
    painter.fillRect(QRect(0,0,btnColor->width(),btnColor->height()),color);
    QIcon btnicon(pixmap);
    btnColor->setIcon(btnicon);

}

//选择缓存路径按钮
void SetWindow::onBtnSelectSavePathClicked()
{
    QString oldFolder = savePathText->toPlainText() ;
    QString folder = DFileDialog::getExistingDirectory(this,tr("Select a location"));
    if(folder.isNull() || folder.isEmpty())
    {
        folder = oldFolder;
    }
    savePathText->setText(folder);
    GlobalHelper::writeSettingValue("set","savepath",folder);//修改配置文件键值
}

//覆盖时警告开关改变
void SetWindow::onSBtnFileOverCheckedChanged(bool ck)
{
    QString keyValue ;
    if(ck==true)
        keyValue = "0";
    else
        keyValue = "1";
    GlobalHelper::writeSettingValue("set","fileover",keyValue);//修改配置文件键值
}

//无设备警告开关改变
void SetWindow::onSBtnNoDeviceCheckedChanged(bool ck)
{
    QString keyValue ;
    if(ck==true)
        keyValue = "0";
    else
        keyValue = "1";
    GlobalHelper::writeSettingValue("set","nodevice",keyValue);//修改配置文件键值
}

//完成时提示音开关改变
void SetWindow::onSBtnFinishBeepCheckedChanged(bool ck)
{
    QString keyValue ;
    if(ck==true)
        keyValue = "0";
    else
        keyValue = "1";
    GlobalHelper::writeSettingValue("set","finishbeep",keyValue);//修改配置文件键值
}

//完成时退出开关改变
void SetWindow::onSBtnFinishExitCheckedChanged(bool ck)
{
    QString keyValue ;
    if(ck==true)
        keyValue = "0";
    else
        keyValue = "1";
    GlobalHelper::writeSettingValue("set","finishexit",keyValue);//修改配置文件键值
}

//电子档案布局开关改变
void SetWindow::onSBtnClassificationCheckedChanged(bool ck)
{
    QString keyValue ;
    if(ck==true)
        keyValue = "0";
    else
        keyValue = "1";
    GlobalHelper::writeSettingValue("set","classification",keyValue);//修改配置文件键值

}


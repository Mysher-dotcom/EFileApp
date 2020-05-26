#include "setwindow.h"
#include "ui_setwindow.h"
#include <DColorDialog>
#include <QHBoxLayout>
#include <DFileDialog>
#include "globalhelper.h"

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


void SetWindow::initUI()
{
    //*窗口基本属性设置*
    this->resize(460,320);//窗口初始尺寸 460,380
    this->titlebar()->setTitle("");//标题栏文字设为空
    setWindowIcon(QIcon(":/img/logo/logo-16.svg"));// 状态栏图标
    this->titlebar()->setIcon(QIcon(":/img/logo/logo-16.svg"));//标题栏图标
    this->titlebar()->setMenuVisible(false);//菜单按钮隐藏
    this->setWindowFlags(this->windowFlags()&~Qt::WindowMinMaxButtonsHint);//最小化、最大化按钮隐藏
    this->setFixedSize(this->width(),this->height());//禁止拖动改变窗口尺寸
    this->setWindowTitle("设置");//开始菜单栏上鼠标悬浮在窗口上显示的名称

    //*窗口控件设置*
    QVBoxLayout *winLayout=new QVBoxLayout();//窗口的布局，最外层
    QWidget *mainWidget=new QWidget ();
    mainWidget->setGeometry(0, 0, 300, 100);  //主容器设置背景白色
    QPalette palMain(mainWidget->palette());
    palMain.setColor(QPalette::Background, Qt::white);
    mainWidget->setAutoFillBackground(true);
    mainWidget->setPalette(palMain);

    QVBoxLayout *mainVLayout=new QVBoxLayout ();//主容器布局

    QLabel *lbl = new QLabel ();
    lbl->setText("扫描管理设置");
    lbl->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;color:rgba(0,26,46,1);font-size:17px");
    mainVLayout->addWidget(lbl,0,Qt::AlignTop);

    QWidget *parWidget = new QWidget ();
    GlobalHelper::setWidgetBackgroundColor(parWidget,QColor(249,249,249,255));//容器设置背景
    QHBoxLayout *parHLayout = new QHBoxLayout ();
    QLabel  *parTitleLbl= new QLabel ();
    parTitleLbl->setText("缓存位置");
    setParTitleLabelStyle(parTitleLbl,80);
    parHLayout->addWidget(parTitleLbl);
    savePathText = new DTextEdit ();
    savePathText->setText(GlobalHelper::readSettingValue("set","savepath"));//读取配置文件
    parHLayout->addWidget(savePathText);
    selectSavePathBtn = new DSuggestButton ();
    selectSavePathBtn->setText("...");
    parHLayout->addWidget(selectSavePathBtn);
    parWidget->setLayout(parHLayout);//参数行的容器加入布局

    QWidget *parWidget2 = new QWidget ();
    GlobalHelper::setWidgetBackgroundColor(parWidget2,QColor(249,249,249,255));//容器设置背景
    QHBoxLayout *parHLayout2 = new QHBoxLayout ();
    QLabel *parTitleLbl2= new QLabel ();
    parTitleLbl2->setText("覆盖时警告");
    setParTitleLabelStyle(parTitleLbl2,140);
    parHLayout2->addWidget(parTitleLbl2);
    parHLayout2->addStretch();
    fileOverWarnSBtn = new DSwitchButton ();
    parHLayout2->addWidget(fileOverWarnSBtn);
    parWidget2->setLayout(parHLayout2);//参数行的容器加入布局

    QWidget *parWidget3 = new QWidget ();
    GlobalHelper::setWidgetBackgroundColor(parWidget3,QColor(249,249,249,255));//容器设置背景
    QHBoxLayout *parHLayout3 = new QHBoxLayout ();
    QLabel *parTitleLbl3= new QLabel ();
    parTitleLbl3->setText("无扫描仪时警告");
    setParTitleLabelStyle(parTitleLbl3,140);
    parHLayout3->addWidget(parTitleLbl3);
    parHLayout3->addStretch();
    noDeviceWarnSBtn = new DSwitchButton ();
    parHLayout3->addWidget(noDeviceWarnSBtn);
    parWidget3->setLayout(parHLayout3);//参数行的容器加入布局

    QWidget *parWidget4 = new QWidget ();
    GlobalHelper::setWidgetBackgroundColor(parWidget4,QColor(249,249,249,255));//容器设置背景
    QHBoxLayout *parHLayout4 = new QHBoxLayout ();
    QLabel *parTitleLbl4= new QLabel ();
    parTitleLbl4->setText("完成后提示音");
    setParTitleLabelStyle(parTitleLbl4,140);
    parHLayout4->addWidget(parTitleLbl4);
    parHLayout4->addStretch();
    finishBeepSBtn = new DSwitchButton ();
    parHLayout4->addWidget(finishBeepSBtn);
    parWidget4->setLayout(parHLayout4);//参数行的容器加入布局

    QWidget *parWidget5 = new QWidget ();
    GlobalHelper::setWidgetBackgroundColor(parWidget5,QColor(249,249,249,255));//容器设置背景
    QHBoxLayout *parHLayout5 = new QHBoxLayout ();
    QLabel *parTitleLbl5= new QLabel ();
    parTitleLbl5->setText("完成后退出");
    setParTitleLabelStyle(parTitleLbl5,140);
    parHLayout5->addWidget(parTitleLbl5);
    parHLayout5->addStretch();
    finishExitSBtn = new DSwitchButton ();
    parHLayout5->addWidget(finishExitSBtn);
    parWidget5->setLayout(parHLayout5);//参数行的容器加入布局

    mainVLayout->addWidget(parWidget,0,Qt::AlignTop);
    mainVLayout->addWidget(parWidget2,0,Qt::AlignTop);
    //mainVLayout->addWidget(parWidget3,0,Qt::AlignTop);//无扫描仪时警告
    mainVLayout->addWidget(parWidget4,0,Qt::AlignTop);
    mainVLayout->addWidget(parWidget5,0,Qt::AlignTop);
    mainVLayout->addStretch();
    mainWidget->setLayout(mainVLayout);
    winLayout->addWidget(mainWidget);
    ui->centralwidget->setLayout(winLayout);

    //读取配置文件,设置UI
    int nfileover= GlobalHelper::readSettingValue("set","fileover").toInt();
    int nnodevice= GlobalHelper::readSettingValue("set","nodevice").toInt();
    int nfinishbeep= GlobalHelper::readSettingValue("set","finishbeep").toInt();
    int nfinishexit= GlobalHelper::readSettingValue("set","finishexit").toInt();
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


    /*
    btnGetPar = new QPushButton ();
    btnGetPar->setText("获取参数");
    btnGetPar->setFixedSize(QSize(100,30));

    rbColor = new DRadioButton(nullptr);
    ckbColor = new DCheckBox (nullptr);
    txtColor = new DTextEdit (nullptr);
    btnColor = new DIconButton (nullptr);
    txtColor->setFixedHeight(30);
    btnColor->setFixedSize(QSize(30,30));

    QHBoxLayout *hlayout = new QHBoxLayout ();
    hlayout->addWidget(rbColor);
    hlayout->addWidget(ckbColor);
    hlayout->addWidget(txtColor);
    hlayout->addWidget(btnColor);
    ui->centralwidget->setLayout(hlayout);


    connect(btnColor, SIGNAL(clicked()), this, SLOT(onBtnColorClicked()));
*/
    connect(selectSavePathBtn, SIGNAL(clicked()), this, SLOT(onBtnSelectSavePathClicked()));
    connect(fileOverWarnSBtn, SIGNAL(checkedChanged(bool)), this, SLOT(onSBtnFileOverCheckedChanged(bool)));
    connect(noDeviceWarnSBtn, SIGNAL(checkedChanged(bool)), this, SLOT(onSBtnNoDeviceCheckedChanged(bool)));
    connect(finishBeepSBtn, SIGNAL(checkedChanged(bool)), this, SLOT(onSBtnFinishBeepCheckedChanged(bool)));
    connect(finishExitSBtn, SIGNAL(checkedChanged(bool)), this, SLOT(onSBtnFinishExitCheckedChanged(bool)));

}

//参数名称label设置样式
void SetWindow::setParTitleLabelStyle(QLabel *lbl,int w)
{
    lbl->setFixedWidth(w);
    lbl->setStyleSheet("font-family:SourceHanSansSC-Medium,sourceHanSansSC;font-weight:500;color:rgba(65,77,104,1);font-size:14px");
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
    QString folder = DFileDialog::getExistingDirectory(this,"请选择缓存位置");
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

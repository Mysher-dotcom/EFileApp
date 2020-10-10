#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QHBoxLayout>
#include "DTitlebar"
#include "DIconButton"
#include "dlabel.h"
#include "dstyle.h"
#include "dtextedit.h"
#include "dbuttonbox.h"
#include <QDesktopWidget>
#include "sanehelper/sanehelper.h"
#include <DDialog>
#include <QFile>
#include <qdir.h>
#include "helper/globalhelper.h"
#include <QDebug>
#include <QColorDialog>
#include "setwindow.h"
#include <QMenu>
#include <QStandardPaths>
#include <DTableView>
#include <QHeaderView>
#include <QFileIconProvider>
#include <QDateTime>
#include <qmath.h>
#include <QDesktopServices>
#include <DFileDialog>
#include <DDialogButtonBox>
#include <QtPrintSupport/qprinter.h>
#include <QtPrintSupport/QPrintDialog>
#include <QTextDocument>
#include <QTextCodec>
#include <QToolButton>
#include <QImageReader>
#include "helper/classificationhelper.h"
#include "helper/deviceinfohelper.h"
#include <QProcess>
#include <QPrintPreviewDialog>
#include <QPrintPreviewWidget>
#include "hpdfoperation.h"
#include <string>
#include <iostream>
#include "cmimage.h"

using namespace std;

#define BUTTON_HEIGHT 30        // 功能按钮高度（扫描 图像设置 文字设置 导出）
#define BUTTON_WIDTH 30         // 功能按钮宽度（扫描 图像设置 文字设置 导出）

MainWindow::MainWindow(QWidget *parent) :
    DMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //匹配版本号，用于是否删除配置文件
    GlobalHelper::softVersion = "5.1.0.4-13";
    GlobalHelper::checkVersion();
    //检查配置文件是否存在，不存在就创建，并写入初始值
    QFileInfo settingFile(GlobalHelper::getSettingFilePath());
    if(settingFile.exists()==false)
    {
        GlobalHelper::writeSettingFile();
    }
    WebHelper::writeWebDefault();

    initUI();
    initConnection();
    refreshData();
    showTreeViewModel(GlobalHelper::getScanFolder() + "/bacode");

    /*
    //清空设备信息配置文件
    QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/EFile_config/device";
    QDir dir(docPath);
    dir.removeRecursively();
    */

    deviceCount = 0;
    scannerDeviceCount = 0;
    openCameraThread();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//当前系统是否为深色主题
bool MainWindow::isDarkType()
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


void MainWindow::closeEvent(QCloseEvent *event)
{
    //DeviceInfoHelper::setAllDeviceIsNoCollec();//设置所有设备为未连接状态
}

//初始化界面
void MainWindow::initUI()
{
    this->resize(960,750);//窗口初始尺寸
    this->setMinimumSize(QSize(960,750));//窗口最小尺寸
    this->titlebar()->setTitle("");//标题栏文字设为空
    setWindowIcon(QIcon(":/img/logo/logo-16.svg"));// 状态栏图标
    this->titlebar()->setIcon(QIcon(":/img/logo/logo-16.svg"));//标题栏图标
    this->setWindowTitle(tr("Scan Assistant"));//开始菜单栏上鼠标悬浮在窗口上显示的名称

    //扫描按钮
    pbtnScan = new DIconButton (nullptr);
    pbtnScan->setIcon(QIcon(":/img/title/scan_new.svg"));//图标
    pbtnScan->setFixedSize(QSize(50, 50));//按钮尺寸
    pbtnScan->setIconSize(QSize(25,25));//图标尺寸
    pbtnScan->setToolTip(tr("Scan"));
    pbtnScan->raise();//置顶
    pbtnScan->setStyleSheet("border-radius:25px; background-color:#0081FF; ");

    //图片编辑按钮
    pbtnPicEdit = new DIconButton (nullptr);
    pbtnPicEdit->setIcon(QIcon(":/img/title/picEdit.svg"));
    pbtnPicEdit->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    pbtnPicEdit->setIconSize(QSize(16,16));
    pbtnPicEdit->setVisible(false);

    //文字编辑按钮
    pbtnFontEdit = new DIconButton (nullptr);
    pbtnFontEdit->setIcon(QIcon(":/img/title/font.svg"));
    pbtnFontEdit->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    pbtnFontEdit->setIconSize(QSize(16,16));
    pbtnFontEdit->setVisible(false);

    //导出按钮
    pbtnOutput = new QPushButton (nullptr);
    pbtnOutput->setFixedSize(QSize(BUTTON_WIDTH * 1.5, BUTTON_HEIGHT));
    //pbtnOutput->setIcon(QIcon(":/img/title/output.svg"));
    //pbtnOutput->setIconSize(QSize(16,16));
    outputMenu = new DMenu(pbtnOutput);//导出按钮菜单
    outputMenu->addAction(tr("Export"), this, SLOT(slotTableViewMenuOutputFile()));
   // outputMenu->addAction(tr("Print"), this, SLOT(slotTableViewMenuPrintFile()));
    outputMenu->addAction(tr("Attach to mail"), this, SLOT(slotTableViewMenuEmailFile()));
    pbtnOutput->setMenu(outputMenu);
    QHBoxLayout *outputHLayout = new QHBoxLayout ();//图标居左显示
    QLabel *lbl = new QLabel ();
    QIcon icon(":/img/title/output.svg");
    lbl->setPixmap(icon.pixmap(QSize(16,16)));
    outputHLayout->addWidget(lbl);
    outputHLayout->addStretch();
    pbtnOutput->setLayout(outputHLayout);
    pbtnOutput->setVisible(false);//隐藏

    //搜索框
    pSearchEdit = new DSearchEdit (this->titlebar()) ;
    pSearchEdit->setMinimumSize(350,35);

    //缩略图模式按钮
    pbtnIconLayout = new DIconButton (nullptr);
    pbtnIconLayout->setIcon(QIcon(":/img/title/iconLayout.svg"));
    pbtnIconLayout->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    pbtnIconLayout->setStyleSheet("background:transparent;");
    pbtnIconLayout->setIconSize(QSize(12,12));
    pbtnIconLayout->setCheckable(true);//开启可选中模式状态

    //列表模式按钮
    pbtnListLayout = new DIconButton (nullptr);
    pbtnListLayout->setIcon(QIcon(":/img/title/listLayout.svg"));
    pbtnListLayout->setStyleSheet("background:transparent;");
    pbtnListLayout->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    pbtnListLayout->setIconSize(QSize(12,12));
    pbtnListLayout->setCheckable(true);//开启可选中模式状态

    //产生互斥效果
    QButtonGroup *g=new QButtonGroup ();
    g->setExclusive(true);
    g->addButton(pbtnIconLayout);
    g->addButton(pbtnListLayout);

    //上传文件按钮
    uploadBtn = new QPushButton();
    uploadBtn->setIcon(QIcon(":/img/upload.svg"));
    uploadBtn->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    uploadBtn->setIconSize(QSize(16,16));
    uploadBtn->setVisible(false);

    //this->titlebar()->addWidget(pbtnScan,Qt::AlignLeft);
    this->titlebar()->addWidget(pbtnPicEdit,Qt::AlignLeft);
    this->titlebar()->addWidget(pbtnFontEdit,Qt::AlignLeft);
    this->titlebar()->addWidget(pbtnOutput,Qt::AlignLeft);
    this->titlebar()->addWidget(uploadBtn,Qt::AlignLeft);//上传文件按钮
    this->titlebar()->addWidget(pSearchEdit,Qt::AlignCenter);
    this->titlebar()->addWidget(pbtnIconLayout,Qt::AlignRight);
    this->titlebar()->addWidget(pbtnListLayout,Qt::AlignRight);

    QMenu *m = new QMenu () ;
    m->addAction(tr("Settings"), this, SLOT(slotMenuSetButtonClicked()));
    this->titlebar()->setMenu(m);

    //主布局，分左右结构;左侧为tree，分类显示时设置宽度>0;右侧为缩略图、详细信息列表
    mainHLayout = new QHBoxLayout();
    //左侧tree
    treeView = new DTreeView();
    treeView->setFixedWidth(0);
    treeViewModel = new QStandardItemModel();
    treeView->setModel(treeViewModel);
    treeViewModel->setHorizontalHeaderLabels(QStringList()<<"档案信息");
    treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);//双击Item屏蔽可编辑

    //右侧
    DWidget *rightAllWidget = new DWidget () ;//右侧容器
    QVBoxLayout *rightAllLayout = new QVBoxLayout();//右侧布局
    rightAllWidget->setLayout(rightAllLayout);

    renameWidget = new  DWidget();//重命名控件
    renameWidget->setFixedHeight(0);
    //GlobalHelper::setWidgetBackgroundColor(renameWidget,QColor(255,255,255,178),false);
    QHBoxLayout *renameHLayout = new QHBoxLayout();
    renameWidget->setLayout(renameHLayout);
    DLabel *fileNameTipLabel = new DLabel ();
    fileNameTipLabel->setText(tr("File name"));
    fileNameTipLabel->setStyleSheet("font-size: 14px;font-family: SourceHanSansSC, SourceHanSansSC-Normal;font-weight: Normal;text-align: left;");//color: #414D68;
    fileNameTipLabel->setFixedWidth(66);
    fileNameText = new DLineEdit();//文件名文本框
    //fileNameText->setPlaceholderText(tr("Required"));
    fileNameText->setFixedSize(QSize(140,36));
    //fileNameText->setMaxLength(255);//设置最大长度
    DLabel *fileNameNoTipLabel = new DLabel ();
    fileNameNoTipLabel->setText(tr("Start at"));
    fileNameNoTipLabel->setStyleSheet("font-size: 14px;font-family: SourceHanSansSC, SourceHanSansSC-Normal;font-weight: Normal;text-align: left;");//color: #414D68;
    fileNameNoTipLabel->setFixedWidth(42);
    fileNameNoText = new DLineEdit();//文件名编号文本框,
    fileNameNoText->setText("1");
    fileNameNoText->setFixedSize(QSize(120,36));
    //fileNameNoText->setValidator(new QIntValidator(1,1000,this));//只能输入数字1-1000

    DLabel *renameTipLabel = new DLabel ();
    renameTipLabel->setText(tr("Tips: Sort by selected file order"));
    renameTipLabel->setStyleSheet("font-size: 12px;font-family: SourceHanSansSC, SourceHanSansSC-Normal;font-weight: Normal;text-align: left;color: #526a7f;");
    cancelRenameBtn = new QPushButton();//取消重命名按钮
    cancelRenameBtn->setText(tr("Cancel"));
    cancelRenameBtn->setFixedSize(QSize(73,36));
    renameBtn = new QPushButton();//重命名按钮
    renameBtn->setText(tr("Rename"));
    renameBtn->setFixedSize(QSize(101,36));
    renameBtn->setEnabled(false);
    renameHLayout->addWidget(fileNameTipLabel,0,Qt::AlignLeft);
    renameHLayout->addWidget(fileNameText,0,Qt::AlignLeft);
    renameHLayout->addWidget(fileNameNoTipLabel,0,Qt::AlignLeft);
    renameHLayout->addWidget(fileNameNoText,0,Qt::AlignLeft);
    renameHLayout->addWidget(renameTipLabel,0,Qt::AlignLeft);
    renameHLayout->addStretch();
    renameHLayout->addWidget(cancelRenameBtn);
    renameHLayout->addWidget(renameBtn);

    DWidget *rightListWidget = new DWidget () ;//右侧图像列表容器
    winStackedLayout = new QStackedLayout();//右侧图像列表布局
    rightListWidget->setLayout(winStackedLayout);

    rightListBtnWidget = new DWidget();//右侧图像列表+扫描按钮容器
    QGridLayout *gridLayout = new QGridLayout();//右侧图像列表+扫描按钮布局
    gridLayout->addWidget(rightListWidget, 0, 0, 1, 1);
    gridLayout->addWidget(pbtnScan, 0, 0, 1, 1,Qt::AlignBottom | Qt::AlignCenter);
    rightListBtnWidget->setLayout(gridLayout);

    rightAllLayout->addWidget(renameWidget);
    rightAllLayout->addWidget(rightListBtnWidget);

    mainHLayout->addWidget(treeView);
    mainHLayout->addWidget(rightAllWidget);
    ui->centralWidget->setLayout(mainHLayout);

    showMaskWidgetUI();
    showNoFileUI();
    showFileListUI();
    showFileTableUI();

    //去除布局的间隙
    mainHLayout->setMargin(0);
    mainHLayout->setSpacing(0);
    gridLayout->setMargin(10);
    gridLayout->setSpacing(0);
    winStackedLayout->setMargin(0);
    winStackedLayout->setSpacing(0);
    rightAllLayout->setMargin(0);

}

//信号槽的绑定
void MainWindow::initConnection()
{
    connect(pbtnScan, SIGNAL(clicked()), this, SLOT(slotScanButtonClicked()));//扫描按钮
    connect(pbtnPicEdit, SIGNAL(clicked()), this, SLOT(slotPicEditButtonClicked()));//图片编辑按钮
    connect(pbtnFontEdit, SIGNAL(clicked()), this, SLOT(slotFontEditButtonClicked()));//文字编辑按钮
    connect(pbtnOutput, SIGNAL(clicked()), this, SLOT(slotOutputButtonClicked()));//导出按钮
    connect(pbtnIconLayout, SIGNAL(clicked()), this, SLOT(slotIconLayoutButtonClicked()));//缩略图显示
    connect(pbtnListLayout, SIGNAL(clicked()), this, SLOT(slotListLayoutButtonClicked()));//列表显示
    connect(pSearchEdit,SIGNAL(textChanged(QString)),this,SLOT(slotSearchTextChange(QString)));//搜索框文本改变信号槽
    connect(treeView,SIGNAL(clicked ( const QModelIndex)),this,SLOT(slotTreeViewClick(const QModelIndex)));//树节点单击
    connect(uploadBtn,SIGNAL(clicked()),this,SLOT(slotUploadBtnClick()));//上传按钮

    connect(cancelRenameBtn,SIGNAL(clicked()),this,SLOT(slotCancelRenameBtnClicked()));//取消重命名按钮
    connect(renameBtn,SIGNAL(clicked()),this,SLOT(slotRenameBtnClicked()));//重命名按钮
    connect(fileNameText,SIGNAL(textChanged(const QString &)),this,SLOT(slotFileNameTextChanged(const QString &)));//文件名文本框

}

//更新UI
void MainWindow::refreshData(bool isTreeClick,QString folderPath)
{
    QString folderPathTmp = GlobalHelper::getScanFolder();
    int isDZDA = GlobalHelper::readSettingValue("set","dzda").toInt();
    if(isDZDA == 0)//是否支持分类
    {
        int isClassification = GlobalHelper::readSettingValue("set","classification").toInt();//是否显示分类
        if(isClassification == 0)
        {
            uploadBtn->setVisible(true);
            showTreeViewUI(true);

            //检查分类配置文件是否存在，不存在就创建，并写入初始值
            QFileInfo settingFile(ClassificationHelper::getClassificationFilePath());
            if(settingFile.exists()==false)
            {
                ClassificationHelper::writeClassficationFileDefault();
            }

            writeClassShowFile();
            folderPathTmp = GlobalHelper::getScanFolder() + "/bacode";
            //showTreeViewModel(folderPathTmp);
        }
        else
        {
            uploadBtn->setVisible(false);
            showTreeViewUI(false);
        }
    }
    else
    {
        uploadBtn->setVisible(false);
    }

    QStringList list ;
    if(isTreeClick)
    {
        //if(QFile::exists(folderPath))
        {
            list = getFileByFolder(folderPath);
        }
    }
    else
    {
       list = getFileByFolder(folderPathTmp);
    }
    //QStringList list = getFileByFolder(GlobalHelper::getScanFolder());
    if(list.size() <= 0)
    {
        winStackedLayout->setCurrentIndex(0);//无图UI
    }
    else
    {
        int showModel = GlobalHelper::readSettingValue("set","iconList").toInt();//读取配置文件
        if(showModel == 0)
        {
            winStackedLayout->setCurrentIndex(1); //缩略图模式
            pbtnIconLayout->setDown(true);
            pbtnListLayout->setDown(false);
        }
        else
        {
            winStackedLayout->setCurrentIndex(2); //文件信息模式
            setFileTableSize();//设置文件信息列表尺寸
            pbtnIconLayout->setDown(false);
            pbtnListLayout->setDown(true);
        }

        int fileListViewItemCount = fileListView->count();
        int fileTableViewItemCount = tableViewModel->rowCount();//fileTableView->model()->rowCount() ;
        for(int i=0;i<fileListViewItemCount;i++)
        {
            fileListView->removeItem(0);
        }
        for(int j=0;j<fileTableViewItemCount;j++)
        {
            tableViewModel->removeRow(0);
        }

        //加入文件Item
        for(QString str : list)
        {
           addItem(str);
        }
    }

}

//写文件分类配置文件,主窗根据此配置文件显示树
bool MainWindow::writeClassShowFile()
{
    //没有图像
    if(ClassificationHelper::captureFilePath.length() <= 0)
    {
        return false;
    }
    maskWidget->show();
    maskSpinner->start();
    maskLabel->setText("处理中，请稍等...");
    startRecognizeThread();
    return true;
}

//开启识别线程
void MainWindow::startRecognizeThread()
{
    recognizeThread = new RecognizeThread ();
    _recognizeThread = new QThread();
    recognizeThread->moveToThread(_recognizeThread);

    connect(_recognizeThread,SIGNAL(started()),recognizeThread,SLOT(startRecognize()));//开启线程槽函数
    connect(_recognizeThread,SIGNAL(finished()),recognizeThread,SLOT(deleteLater()));//终止线程时要调用deleteLater槽函数
    connect(recognizeThread,SIGNAL(signalSendCode(QString,QString,QStringList )),this,SLOT(slotGetCodeInfo(QString,QString,QStringList)));//获取条码信息(图片路径，条码号，条码名称)
    connect(recognizeThread,SIGNAL(signalOver()),this,SLOT(slotRecognizeOver()));//识别结束
    _recognizeThread->start();//开启多线程
}

//关闭识别线程
void MainWindow::closeRecognizeThread()
{
    if(_recognizeThread->isRunning())
    {
        _recognizeThread->quit();//退出事件循环
        _recognizeThread->wait();//释放线程槽函数资源
    }
    qDebug()<<"关闭识别线程,线程状态："<<_recognizeThread->isRunning();
}

//识别结束
void MainWindow::slotRecognizeOver()
{
    qDebug()<<"signal Recognize Over";
    ClassificationHelper::captureFilePath.clear();
    closeRecognizeThread();
    treeViewModel->clear();
    treeViewModel->setHorizontalHeaderLabels(QStringList()<<"档案信息");
    showTreeViewModel(GlobalHelper::getScanFolder() + "/bacode");
    maskWidget->hide();
    maskSpinner->stop();
}

//获取条码信息(图片路径，条码号，条码信息)
void MainWindow::slotGetCodeInfo(QString filePath,QString code,QStringList codeInfo)
{
    //"/home/viisan/Desktop/ViisanScanPic/000001.jpg" , "10004" , ("张三", "入职体检")
    qDebug()<<"code back:"<<filePath<<","<<code<<","<<codeInfo;

    if(!QFile::exists(filePath)) return;
    QString codeTmp = code;
    QStringList codeInfoTmp = codeInfo;
    //如果是无条码，就用上一次识别的结果
    if(code.isEmpty() || codeInfo.length() <= 0 )
    {
        if(!preCode.isEmpty())
            codeTmp = preCode;

        if(preCodeInfo.length() > 0)
            codeInfoTmp = preCodeInfo;
    }
    else
    {
        preCode = code;
        preCodeInfo = codeInfo;
    }

    QString newFolder =  ClassificationHelper::createFolderByCodeInfo(GlobalHelper::getScanFolder(),codeInfoTmp);
    QString newFilePath = ClassificationHelper::renameFile(filePath,newFolder,codeTmp);
    qDebug()<<"new path:"<<newFilePath;
}

//展示Tree节点
void MainWindow::showTreeViewModel(QString folderPath,QStandardItem* item)
{
    QDir dir(folderPath);
    QStringList list;
    QStringList::Iterator iter;
    list = dir.entryList(QDir::Dirs, QDir::Name);
    for(iter=list.begin(); iter!=list.end(); ++iter)
    {
        if( "." == *iter || ".." == *iter ) continue;

        QStandardItem *treeParentNode = new QStandardItem ();
        treeParentNode->setIcon(QIcon(":/img/treeIcon.svg"));
        treeParentNode->setText(*iter);
        treeParentNode->setToolTip(folderPath + "/" + *iter);
        if(item == NULL)
        {
            treeViewModel->appendRow(treeParentNode);
        }
        else
        {
            item->appendRow(treeParentNode);
        }
        showTreeViewModel(folderPath+"/"+*iter,treeParentNode);
    }
}

//树节点单击
void MainWindow::slotTreeViewClick(const QModelIndex &index)
{
    QList<QVariant> list = treeViewModel->itemData(index).values();//选中节点的数据集合 0=显示的字符，1=图标，2=路径
    QString nodeText  = treeViewModel->itemData(index).values()[0].toString();
    QString folderPath  = treeViewModel->itemData(index).values()[list.length()-1].toString();

    qDebug()<<"double click:"<<nodeText<<","<<folderPath;
    refreshData(true,folderPath);
}

//遮罩窗口
void MainWindow::showMaskWidgetUI()
{
    maskWidget = new  QWidget(this);
    QString str("QWidget{background-color:rgba(0,0,0,0.5);}");//0.5代表透明度
    maskWidget->setStyleSheet(str);
    maskWidget->setGeometry(0, 0, this->width(), this->height());//遮罩窗口位置

    QVBoxLayout *vLayout = new QVBoxLayout();
    maskSpinner = new DSpinner ();//遮罩窗口进度动画
    maskSpinner->setFixedSize(QSize(28,36));
    maskLabel = new QLabel();//遮罩窗口显示文本
    maskLabel->setText("处理中，请稍等...");
    maskLabel->setStyleSheet("color:white;background:transparent;");

    vLayout->addStretch();
    vLayout->addWidget(maskSpinner, 0,Qt::AlignHCenter);
    vLayout->addWidget(maskLabel, 0, Qt::AlignHCenter);
    vLayout->addStretch();
    maskWidget->setLayout(vLayout);
    maskWidget->hide();
}

//上传按钮单击
void MainWindow::slotUploadBtnClick()
{
    maskWidget->show();
    maskSpinner->start();
    maskLabel->setText("上传中，请稍等...");

    QStringList list;
    //list<<"/home/viisan//Desktop/ViisanScanPic/bacode/李四/中共党员需有自传材料/10191002.jpg";

    QStringList folderList ;
    getAllFolder(GlobalHelper::getScanFolder() + "/bacode",folderList);
    for(int i=0;i<folderList.length();i++)
    {
        QStringList l = getFileByFolder(folderList.at(i));
        list.append(l);
    }


    uploadFile(list);
}

//上传文件
void MainWindow::uploadFile(QStringList list)
{
    uploadThread = new UploadFileThread ();
    _uploadThread = new QThread();
    uploadThread->moveToThread(_uploadThread);

    uploadThread->setFileList(list);

    connect(_uploadThread,SIGNAL(started()),uploadThread,SLOT(startUpload()));//开启线程槽函数
    connect(_uploadThread,SIGNAL(finished()),uploadThread,SLOT(deleteLater()));//终止线程时要调用deleteLater槽函数

    connect(uploadThread,SIGNAL(signalSingleUploadOver(QString,bool)),this,SLOT(slotSingleUploadOver(QString,bool)));//单个文件上传完成
    connect(uploadThread,SIGNAL(signalOver()),this,SLOT(slotUploadOver()));//上传结束
    _uploadThread->start();//开启线程
}

//单个文件上传完成
void MainWindow::slotSingleUploadOver(QString filepath,bool uploadResult)
{
    qDebug()<<"single upload:"<<filepath<<","<<uploadResult;
}

//上传结束
void MainWindow::slotUploadOver()
{
    qDebug()<<"upload over";
    if(_uploadThread->isRunning())
    {
        _uploadThread->quit();//退出事件循环
        _uploadThread->wait();//释放线程槽函数资源
    }
    maskWidget->hide();
    maskSpinner->stop();
    qDebug()<<"关闭上传线程,线程状态："<<_uploadThread->isRunning();
}


//开启拍摄仪线程
void MainWindow::openCameraThread()
{
    DeviceInfoHelper::setAllDeviceIsNoCollec();//设置所有设备为未连接状态,设备下标置成999

    QStringList list;
    //参数可以是任何值，线程对象的参数用于获取指定设备的参数，在获取设备信息中用不到
    getCameraInfoThread = new GetCameraInfoThread (list);
    _getCameraInfoThread = new QThread();
    getCameraInfoThread->moveToThread(_getCameraInfoThread);

    connect(_getCameraInfoThread,SIGNAL(started()),getCameraInfoThread,SLOT(slotStartCameraThread()));//开启线程槽函数
    connect(_getCameraInfoThread,SIGNAL(finished()),getCameraInfoThread,SLOT(deleteLater()));//终止线程时要调用deleteLater槽函数
    connect(getCameraInfoThread,SIGNAL(signalNoCamera()),this,SLOT(slotNoCamera()));//无设备信号槽
    connect(getCameraInfoThread,SIGNAL(signalCameraInfo(QVariant, const QString &)),this,SLOT(slotCameraInfo(QVariant,const QString &)),Qt::QueuedConnection);//设备信息信号槽
    _getCameraInfoThread->start();//开启线程
}

//无拍摄仪设备
void MainWindow::slotNoCamera()
{
    qDebug()<<"无拍摄仪设备,关闭拍摄仪线程";
    deviceCount = 0;
    slotCloseCameraThread();
}

//记录拍摄仪设备信息
void MainWindow::slotCameraInfo(QVariant qv, const QString &str)
{
    QMap<int ,QStringList> cameraInfoMap = qv.value<QMap<int ,QStringList>>();//还原为原来的数据结构类型
    if(cameraInfoMap.size() <= 0)
    {
        //无设备
        deviceCount = 0;
        slotCloseCameraThread();
        return;
    }
    //循环记录设备信息
    QString devListFilePath = DeviceInfoHelper::getDeviceListInfoFilePath();
    QMap<int,QStringList>::iterator itCameraInfo;
    for(itCameraInfo = cameraInfoMap.begin();itCameraInfo!=cameraInfoMap.end();itCameraInfo++)
    {
        //记录设备信息,key: 0 ,value: ("拍摄仪 1", "Document Scanner", "空闲", "0","1")
        QStringList tmpQSList = itCameraInfo.value();//参数值
        qDebug()<<"Camera线程回传的设备信息数据,key:"<<itCameraInfo.key()<<",value:"<<itCameraInfo.value();
        if(tmpQSList.size() >= 5)
        {
            QString devGroupName = DeviceInfoHelper::getGroupNameByDeviceName(devListFilePath,tmpQSList.at(1));//获取连接电脑的设备名称(设备类型:Document Scanner)在配置文件中的组名
            qDebug()<<tmpQSList.at(1)<<" 设备组名："<<devGroupName;
            if(devGroupName.isEmpty() == true)
            {
                devGroupName = "device"+tmpQSList.at(3);
            }
            DeviceInfoHelper::writeValue(devListFilePath,devGroupName,"type","1");//类型，0=扫描仪，1=拍摄仪
            DeviceInfoHelper::writeValue(devListFilePath,devGroupName,"index",tmpQSList.at(3));//设备下标
            DeviceInfoHelper::writeValue(devListFilePath,devGroupName,"name",tmpQSList.at(0));//设备名称
            DeviceInfoHelper::writeValue(devListFilePath,devGroupName,"model",tmpQSList.at(1));//设备类型
            DeviceInfoHelper::writeValue(devListFilePath,devGroupName,"status",tmpQSList.at(2));//设备状态，0=空闲
            DeviceInfoHelper::writeValue(devListFilePath,devGroupName,"config", DeviceInfoHelper::getDeviceInfoFilePath(tmpQSList.at(1)));//设备具体配置文件路径
            DeviceInfoHelper::writeValue(devListFilePath,devGroupName,"license",tmpQSList.at(4));//设备是否授权
            deviceCount++;
        }
    }
    slotCloseCameraThread();//停止拍摄仪线程

}

//关闭拍摄仪线程
void MainWindow::slotCloseCameraThread()
{
    qDebug()<<"关闭Camera线程";
    if(_getCameraInfoThread->isRunning())
    {
        _getCameraInfoThread->quit();//退出事件循环
        _getCameraInfoThread->wait();//释放线程槽函数资源
    }
    qDebug()<<"Camera线程停止,线程状态："<<_getCameraInfoThread->isRunning();
    openScannerThread();//开启SANE线程
}

//开启SANE线程
void MainWindow::openScannerThread()
{
    QStringList list;
    //参数可以是任何值，线程对象的参数用于获取指定设备的参数，在获取设备信息中用不到
    getScannerInfoThread = new GetScannerInfoThread (list);
    _getScannerInfoThread = new QThread();
    getScannerInfoThread->moveToThread(_getScannerInfoThread);

    connect(_getScannerInfoThread,SIGNAL(started()),getScannerInfoThread,SLOT(slotStartThread()));//开启线程槽函数
    connect(_getScannerInfoThread,SIGNAL(finished()),getScannerInfoThread,SLOT(deleteLater()));//终止线程时要调用deleteLater槽函数
    connect(getScannerInfoThread,SIGNAL(signalOver()),this,SLOT(slotCloseScannerThread()));//关闭SANE线程
    connect(getScannerInfoThread,SIGNAL(signalNoScanner()),this,SLOT(slotNoScanner()));//无扫描仪设备
    connect(getScannerInfoThread,SIGNAL(signalScannerInfo(QVariant, const QString &)),this,SLOT(slotScannerInfo(QVariant, const QString &)),Qt::QueuedConnection); //设备信息信号槽
    _getScannerInfoThread->start();//开启多线程槽函数

}

//无扫描仪设备
void MainWindow::slotNoScanner()
{
    qDebug()<<"无扫描仪设备,关闭SANE线程";
    scannerDeviceCount = 0;
    slotCloseScannerThread();
}

//记录扫描仪设备信息
void MainWindow::slotScannerInfo(QVariant qv, const QString &str)
{
    QMap<int ,QStringList> scannerInfoMap = qv.value<QMap<int ,QStringList>>();//还原为原来的数据结构类型
    if(scannerInfoMap.size() <= 0)
    {
        //无设备
        scannerDeviceCount = 0;
        slotCloseScannerThread();
        return;
    }
    //循环记录设备信息
    QString devListFilePath = DeviceInfoHelper::getDeviceListInfoFilePath();
    QMap<int,QStringList>::iterator itInfo;
    for(itInfo = scannerInfoMap.begin();itInfo!=scannerInfoMap.end();itInfo++)
    {
        //记录设备信息,key: 0 ,value: ("拍摄仪 1", "Document Scanner", "空闲", "0")
        QStringList tmpQSList = itInfo.value();//参数值
        qDebug()<<"sane线程回传的设备信息数据,key:"<<itInfo.key()<<",value:"<<itInfo.value();
        if(tmpQSList.size()>=4)
        {
            QString groupName = DeviceInfoHelper::getGroupNameByDeviceName(devListFilePath,tmpQSList.at(1));//获取连接电脑的设备名称(设备类型:Document Scanner)在配置文件中的组名
            qDebug()<<tmpQSList.at(1)<<" 设备组名："<<groupName;
            if(groupName.isEmpty() == true)
            {
                int groupNameIndex = DeviceInfoHelper::getGroupCount(devListFilePath) ;
                if(DeviceInfoHelper::isContainsGroup(devListFilePath,"default"))
                {
                    groupName = "device" + QString::number(groupNameIndex - 1);
                }
                else
                {
                    groupName = "device" + QString::number(groupNameIndex);
                }
            }
            DeviceInfoHelper::writeValue(devListFilePath,groupName,"type","0");//类型，0=扫描仪，1=拍摄仪
            DeviceInfoHelper::writeValue(devListFilePath,groupName,"index",tmpQSList.at(3));//设备下标
            DeviceInfoHelper::writeValue(devListFilePath,groupName,"name",tmpQSList.at(0));//设备名称
            DeviceInfoHelper::writeValue(devListFilePath,groupName,"model",tmpQSList.at(1));//设备类型
            DeviceInfoHelper::writeValue(devListFilePath,groupName,"status",tmpQSList.at(2));//设备状态，0=空闲
            DeviceInfoHelper::writeValue(devListFilePath,groupName,"config", DeviceInfoHelper::getDeviceInfoFilePath(tmpQSList.at(1)));//设备具体配置文件路径
            scannerDeviceCount++;
        }
    }

    slotCloseScannerThread();//停止线程
}

//关闭SANE线程
void MainWindow::slotCloseScannerThread()
{
    GlobalHelper::getDeviceInfoIsOver = true;
    qDebug()<<"关闭SANE线程";
    if(_getScannerInfoThread->isRunning())
    {
        _getScannerInfoThread->quit();//退出事件循环
        _getScannerInfoThread->wait();//释放线程槽函数资源
    }
    qDebug()<<"SANE线程停止,线程状态："<<_getScannerInfoThread->isRunning();
    getDevicePar();
}

//获取设备参数
void MainWindow::getDevicePar()
{
    //无拍摄仪，不获取参数
    if(deviceCount <= 0 && scannerDeviceCount <= 0)
    {
        slotDeviceParOver();
        return;
    }
    QList<DeviceInfoData> devList = DeviceInfoHelper::getDeviceListInfo();//从配置文件中获取所有记录的设备
    if(devList.length()<=0)
    {
        slotDeviceParOver();
        return;
    }
    QStringList cameraDevIndexList,scannerDevIndexList;
    for(int i=0;i<devList.length();i++)
    {
        int devType = devList.at(i).type;
        int devIndex = devList.at(i).index;
        int devStatus = devList.at(i).status;//设备状态，0=空闲，1=占用，2=未连接
        qDebug()<<"dev type:"<<devType<<",dev index:"<<devIndex<<",dev status:"<<devStatus;
        if(devStatus != 0)//未连接的设备不取参数
        {
            continue;
        }
        if(devType == 0)//扫描仪
        {
            scannerDevIndexList<<QString::number(devIndex);
        }
        else
        {
           cameraDevIndexList<<QString::number(devIndex);
        }
    }

    //获取拍摄仪参数
    if(cameraDevIndexList.length() > 0 )
    {
        getCameraInfoThread = new GetCameraInfoThread (cameraDevIndexList);
        _getCameraInfoThread = new QThread();
        getCameraInfoThread->moveToThread(_getCameraInfoThread);
        connect(_getCameraInfoThread,SIGNAL(started()),getCameraInfoThread,SLOT(slotGetCameraPar()));//开启线程槽函数
        connect(_getCameraInfoThread,SIGNAL(finished()),getCameraInfoThread,SLOT(deleteLater()));//终止线程时要调用deleteLater槽函数
        connect(getCameraInfoThread,SIGNAL(signalCameraParInfo(QVariant, const QString &)),this,SLOT(slotCameraParInfo(QVariant,const QString &)),Qt::QueuedConnection);//设备参数信号槽
        if(scannerDevIndexList.length() <= 0)//如果没有扫描仪，获取拍摄仪参数后就直接退出
        {
            connect(getCameraInfoThread,SIGNAL(signalParOver()), this,SLOT(slotDeviceParOver()));
        }
        else
        {
            connect(getCameraInfoThread,SIGNAL(signalParOver()), this,SLOT(slotCameraParOver()));
        }
        _getCameraInfoThread->start();//开启多线程
        GlobalHelper::getDeviceInfoIsOver = false;
    }

    //获取扫描仪参数
    if(scannerDevIndexList.length() > 0)
    {
        getScannerInfoThread = new GetScannerInfoThread (scannerDevIndexList);
        _getScannerInfoThread = new QThread();
        getScannerInfoThread->moveToThread(_getScannerInfoThread);
        connect(_getScannerInfoThread,SIGNAL(started()),getScannerInfoThread,SLOT(slotGetDevicePar()));//开启线程槽函数
        connect(_getScannerInfoThread,SIGNAL(finished()),getScannerInfoThread,SLOT(deleteLater()));//终止线程时要调用deleteLater槽函数
        connect(getScannerInfoThread,SIGNAL(signalScannerParInfo(QVariant, const QString &)), this,SLOT(slotScannerParInfo(QVariant,const QString &)),Qt::QueuedConnection);//设备参数信号槽
        connect(getScannerInfoThread,SIGNAL(signalParOver()), this,SLOT(slotScannerParOver()));//设备参数停止线程
        _getScannerInfoThread->start();//开启多线程
        GlobalHelper::getDeviceInfoIsOver = false;
    }
}

//记录拍摄仪设备参数信息
void MainWindow::slotCameraParInfo(QVariant qv,const QString &str)
{
    QString filePath="";
    QList<DeviceInfoData> devList = DeviceInfoHelper::getDeviceListInfo();
    for(int i=0;i<devList.length();i++)
    {
        int devType = devList.at(i).type;
        int devIndex = devList.at(i).index;
        int devStatus = devList.at(i).status;//设备状态，0=空闲，1=占用，2=未连接
        if(devStatus != 0)//未连接的设备不取参数
        {
            continue;
        }

        if(devType == 1 && devIndex == str.toInt())
        {
            filePath = devList.at(i).config;
            break;
        }
    }

    //设备参数数据
    QMap<int ,QMap<QString ,QStringList>> devParsMap = qv.value<QMap<int ,QMap<QString ,QStringList>>>();//还原为原来的数据结构类型
    QMap<int ,QMap<QString ,QStringList>>::iterator itDevParInfo;
    for(itDevParInfo = devParsMap.begin();itDevParInfo!=devParsMap.end();itDevParInfo++)
    {
        QMap<QString ,QStringList> tmpValueMap = itDevParInfo.value();
        QMap<QString ,QStringList>::iterator itTmpDevParInfo;
        for(itTmpDevParInfo = tmpValueMap.begin();itTmpDevParInfo!=tmpValueMap.end();itTmpDevParInfo++)
        {
            qDebug()<<"扫描设置"<<itTmpDevParInfo.key()<<"-"<<itTmpDevParInfo.value();

            qDebug()<<"ini file path:"<<filePath;
            QString strValue;
            QStringList strList = itTmpDevParInfo.value();
            for(int i=0;i<strList.size();i++)
            {
                strValue += ";" + strList.at(i);
            }
            DeviceInfoHelper::writeValue(filePath,"base",itTmpDevParInfo.key(),strValue);
            //int groupNameIndex = tmpQSList.at(3).toInt() ;
            //groupNameIndex = groupNameIndex + deviceCount;
            //QString groupName = QString("device%1").arg(QString::number(groupNameIndex));
            //DeviceInfoHelper::writeValue(devListFilePath,groupName,"type","0");//类型，0=扫描仪，1=拍摄仪

        }

        qDebug()<<"thread back camera par info,key:"<<itDevParInfo.key()<<",value:"<<itDevParInfo.value();

    }

}

//记录扫描仪设备参数信息
void MainWindow::slotScannerParInfo(QVariant qv,const QString &str)
{
    QString filePath="";
    QList<DeviceInfoData> devList = DeviceInfoHelper::getDeviceListInfo();
    for(int i=0;i<devList.length();i++)
    {
        int devType = devList.at(i).type;
        int devIndex = devList.at(i).index;
        int devStatus = devList.at(i).status;//设备状态，0=空闲，1=占用，2=未连接
        if(devStatus != 0)//未连接的设备不取参数
        {
            continue;
        }

        if(devType == 0 && devIndex == str.toInt())
        {
            filePath = devList.at(i).config;
            break;
        }
    }

    qDebug()<<"scanner ini file path:"<<filePath;
    QString parOrder;//记录参数顺序，在下参扫描的时候用到
    //设备参数数据
    QMap<int ,QMap<QString ,QStringList>> devParsMap = qv.value<QMap<int ,QMap<QString ,QStringList>>>();//还原为原来的数据结构类型
    QMap<int ,QMap<QString ,QStringList>>::iterator itDevParInfo;
    for(itDevParInfo = devParsMap.begin();itDevParInfo!=devParsMap.end();itDevParInfo++)
    {
        int tempIndex = itDevParInfo.key();
        qDebug()<<"参数下标:"<<tempIndex;

        //参数值 如：<色彩模式,"Color24,Gary8,Lineart">
        QMap<QString ,QStringList> tmpValueMap = itDevParInfo.value();
        QMap<QString ,QStringList>::iterator itTmpDevParInfo;
        for(itTmpDevParInfo = tmpValueMap.begin();itTmpDevParInfo!=tmpValueMap.end();itTmpDevParInfo++)
        {
            //读取配置文件，判断参数是否显示
            int nParIsShow = GlobalHelper::readSettingValue("show",itTmpDevParInfo.key()).toInt();
            if(nParIsShow == 1)
            {
                if(QString::compare(itTmpDevParInfo.value().at(0).right(1),"1")==0)
                {
                    itTmpDevParInfo.value().clear();
                    itTmpDevParInfo.value()<<QString::number(200)<<QString::number(300)<<QString::number(600);
                }
                QString strValue;
                QStringList strList = itTmpDevParInfo.value();
                for(int i=0;i<strList.size();i++)
                {
                    if(QString::compare(strList.at(i),"Error Diffusion") == 0 )
                       continue;
                    if(QString::compare(strList.at(i),"ATEII") == 0 )
                       continue;
                    strValue += ";" + strList.at(i);

                }
                DeviceInfoHelper::writeValue(filePath,"base",itTmpDevParInfo.key(),strValue);
                parOrder += itTmpDevParInfo.key() + ";";

                //记录参数顺序，在下参扫描的时候用到
                DeviceInfoHelper::writeValue(filePath,"baseorderindex",itTmpDevParInfo.key(),QString::number(tempIndex));
            }
        }

        qDebug()<<"thread back scanner par info,key:"<<itDevParInfo.key()<<",value:"<<itDevParInfo.value();

    }

    //记录参数顺序，在下参扫描的时候用到
    DeviceInfoHelper::writeValue(filePath,"baseorder","order",parOrder);
}


//获取拍摄仪设备参数结束
void MainWindow::slotCameraParOver()
{
    if(_getCameraInfoThread->isRunning())
    {
        _getCameraInfoThread->quit();//退出事件循环
        _getCameraInfoThread->wait();//释放线程槽函数资源
    }
    qDebug()<<"par Camera线程停止,线程状态："<<_getCameraInfoThread->isRunning();
}

//获取扫描仪设备参数结束
void MainWindow::slotScannerParOver()
{
    if(_getScannerInfoThread->isRunning())
    {
        _getScannerInfoThread->quit();//退出事件循环
        _getScannerInfoThread->wait();//释放线程槽函数资源
    }
    qDebug()<<"par SANE线程停止,线程状态："<<_getScannerInfoThread->isRunning();
    slotDeviceParOver();
}

//设备参数停止线程,发送信号通知刷新UI
void MainWindow::slotDeviceParOver()
{
    if(_getCameraInfoThread->isRunning())
    {
        _getCameraInfoThread->quit();//退出事件循环
        _getCameraInfoThread->wait();//释放线程槽函数资源
    }
    qDebug()<<"par Camera线程停止,线程状态："<<_getCameraInfoThread->isRunning();
    if(_getScannerInfoThread->isRunning())
    {
        _getScannerInfoThread->quit();//退出事件循环
        _getScannerInfoThread->wait();//释放线程槽函数资源
    }
    qDebug()<<"par SANE线程停止,线程状态："<<_getScannerInfoThread->isRunning();
    qDebug()<<"设备参数停止线程,发送信号通知刷新UI";
    GlobalHelper::getDeviceInfoIsOver = true;
    emit signalThreadOver();
}

//无文件UI
void MainWindow::showNoFileUI()
{
    tipWidget = new DWidget ();//提示容器
    pVLayout = new QVBoxLayout () ;//提示布局
    pNoPicTip1 = new DLabel () ;//无图提示1
    pNoPicTip2 = new DLabel () ;//无图提示2
    pNoPicTip1->setText(tr("No scanned images"));//暂无扫描的文件
    //pNoPicTip1->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;color:rgba(85,85,85,0.4);font-size:17px");
    pNoPicTip1->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;font-size:17px");
    pNoPicTip2->setText(tr("Click Scan button to start"));//可点击扫描按钮添加扫描文件
    //pNoPicTip2->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;color:rgba(85,85,85,0.4);font-size:14px");
    pNoPicTip2->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;font-size:14px");
    pNoPicTip1->setAlignment(Qt::AlignCenter);
    pNoPicTip2->setAlignment(Qt::AlignCenter);
    pVLayout->addStretch();
    pVLayout->addWidget(pNoPicTip1);//扫描中错误信号
    pVLayout->addWidget(pNoPicTip2);
    pVLayout->addStretch();
    tipWidget->setLayout(pVLayout);

    winStackedLayout->addWidget(tipWidget);
}

//显示缩略图列表
void MainWindow::showFileListUI()
{
    fileListView = new DListView();
    fileListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    fileListView->setViewMode(QListView::IconMode);
    fileListView->setResizeMode(QListView::Adjust);

    listViewModel = new QStandardItemModel ();
    PicItemDelegate *pItemDelegate=new PicItemDelegate (fileListView);
    fileListView->setItemDelegate(pItemDelegate);
    listViewProxyModel = new QSortFilterProxyModel (fileListView);
    listViewProxyModel->setSourceModel(listViewModel);
    listViewProxyModel->setFilterRole(Qt::UserRole);
    listViewProxyModel->setDynamicSortFilter(true);
    fileListView->setModel(listViewProxyModel);
    fileListView->setContextMenuPolicy(Qt::CustomContextMenu);//可弹出右键菜单
    fileListView->setEditTriggers(QAbstractItemView::NoEditTriggers);//双击Item屏蔽可编辑
    fileListView->setMovement(QListView::Static);//禁止拖拽Item

    //右键菜单
    listViewMenu = new DMenu(fileListView);
    listViewMenu->addAction(tr("Open"), this, SLOT(slotTableViewMenuOpenFile()));
    listViewMenu->addAction(tr("Edit"), this, SLOT(slotTableViewMenuEditFile()));
    listViewMenu->addAction(tr("Export"), this, SLOT(slotTableViewMenuOutputFile()));
    listViewMenu->addAction(tr("Rename"), this, SLOT(slotTableViewMenuRenameFile()));
    listViewMenu->addSeparator();
    listViewMenu->addAction(tr("Display in file manager"), this, SLOT(slotTableViewMenuOpenFolder()));
    listViewMenu->addAction(tr("Combine into PDF"), this, SLOT(slotTableViewMenuOutputPDFFile()));
    //listViewMenu->addAction(tr("Print"), this, SLOT(slotTableViewMenuPrintFile()));
    listViewMenu->addAction(tr("Attach to mail"), this, SLOT(slotTableViewMenuEmailFile()));
    listViewMenu->addSeparator();
    listViewMenu->addAction(tr("Delete"), this, SLOT(slotTableViewMenuDelFile()));

    //右键菜单响应
    connect(fileListView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotListViewContextMenu(QPoint)));

    //图像列表点击事件
    //connect(imgList,SIGNAL(pressed(const QModelIndex)),this,SLOT(imgListPressed(const QModelIndex)));
    //列表项双击
    connect(fileListView,SIGNAL(doubleClicked(const QModelIndex)),this,SLOT(slotListDoubleClicked(const QModelIndex)));

    winStackedLayout->addWidget(fileListView);
}

//显示文件信息列表
void MainWindow::showFileTableUI()
{
    fileTableView = new DTableView ();
    tableViewModel = new QStandardItemModel();
    tableViewProxyModel = new QSortFilterProxyModel(fileTableView);
    //fileTableView->setModel(tableViewModel);
    tableViewProxyModel->setSourceModel(tableViewModel);
    tableViewProxyModel->setFilterKeyColumn(0);//检索第一列
    tableViewProxyModel->setDynamicSortFilter(true);
    fileTableView->setModel(tableViewProxyModel);

    // model 初始化
    tableViewModel->setColumnCount(4);
    tableViewModel->setHeaderData(0, Qt::Horizontal, tr("Name"));
    tableViewModel->setHeaderData(1, Qt::Horizontal, tr("Time Modified"));
    tableViewModel->setHeaderData(2, Qt::Horizontal, tr("Type"));
    tableViewModel->setHeaderData(3, Qt::Horizontal, tr("Size"));

    //设置列宽
    setFileTableSize();

    // tableview 初始化设置
    fileTableView->setSelectionBehavior(QAbstractItemView::SelectRows); // 选中整行
    fileTableView->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);//可多选
    fileTableView->setEditTriggers(QTableView::EditTrigger::NoEditTriggers);//禁止编辑
    fileTableView->setShowGrid(false);//隐藏背景网格线
    fileTableView->setSortingEnabled(true);//排序
    fileTableView->setAlternatingRowColors(false);//隔行背景变色
    fileTableView->verticalHeader()->setVisible(false); //隐藏列表头
    fileTableView->setContextMenuPolicy(Qt::CustomContextMenu);//可弹出右键菜单
    fileTableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);//标题文字显示居左

    //右键菜单
    tableViewMenu = new DMenu(fileTableView);
    tableViewMenu->addAction(tr("Open"), this, SLOT(slotTableViewMenuOpenFile()));
    tableViewMenu->addAction(tr("Edit"), this, SLOT(slotTableViewMenuEditFile()));
    tableViewMenu->addAction(tr("Export"), this, SLOT(slotTableViewMenuOutputFile()));
    tableViewMenu->addAction(tr("Rename"), this, SLOT(slotTableViewMenuRenameFile()));
    tableViewMenu->addSeparator();
    tableViewMenu->addAction(tr("Display in file manager"), this, SLOT(slotTableViewMenuOpenFolder()));
    tableViewMenu->addAction(tr("Combine into PDF"), this, SLOT(slotTableViewMenuOutputPDFFile()));
    //tableViewMenu->addAction(tr("Print"), this, SLOT(slotTableViewMenuPrintFile()));
    tableViewMenu->addAction(tr("Attach to mail"), this, SLOT(slotTableViewMenuEmailFile()));
    tableViewMenu->addSeparator();
    tableViewMenu->addAction(tr("Delete"), this, SLOT(slotTableViewMenuDelFile()));

    //右键菜单响应
    connect(fileTableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotTableViewContextMenu(QPoint)));

    //列表项双击
    connect(fileTableView,SIGNAL(doubleClicked(const QModelIndex)),this,SLOT(slotListDoubleClicked(const QModelIndex)));
    winStackedLayout->addWidget(fileTableView);
}

//显示电子档案模式
void MainWindow::showTreeViewUI(bool isShow)
{
    if(isShow == true)
    {
        treeView->setFixedWidth(300);
    }
    else
    {
        treeView->setFixedWidth(0);
    }

}

//设置文件信息列表尺寸
void MainWindow::setFileTableSize()
{
    int winWidth = this->width();
    int isDZDA = GlobalHelper::readSettingValue("set","dzda").toInt();
    if(isDZDA == 0)//是否支持分类
    {
        int isClassification = GlobalHelper::readSettingValue("set","classification").toInt();//是否显示分类
        if(isClassification == 0)
        {
            winWidth = winWidth - 300;
        }
    }
    //列表根据窗口尺寸改变
    fileTableView->setFixedWidth(winWidth);
    fileTableView->setColumnWidth(0,winWidth/5*1.5);
    fileTableView->setColumnWidth(1,winWidth/5*1.5);
    fileTableView->setColumnWidth(2,winWidth/5);
    fileTableView->setColumnWidth(3,winWidth/5);
}

//增加Item
void MainWindow::addItem(QString path)
{
    QFileInfo *fi = new QFileInfo(path);

    //***缩略图列表***
    QStandardItem *pItem = new QStandardItem ();
    PicListItemData itemData;
    itemData.picPath =path;
    itemData.picName =fi->fileName();
    pItem->setData(fi->fileName(),Qt::UserRole);//存储文件名用于检索
    pItem->setData(QVariant::fromValue(itemData),Qt::UserRole+1);//显示的数据
    listViewModel->appendRow(pItem);
    pItem->setToolTip(fi->fileName());//悬浮在Item上，显示文件名

    //***文件信息列表***
    // 获取系统图标、文件类型、最后修改时间
    QFileIconProvider provider;
    QIcon icon = provider.icon(*fi);//系统图标
    QString updateTime=fi->metadataChangeTime().toString("yyyy-MM-dd hh:mm:ss");//最后修改时间
    QString strType = provider.type(*fi);//文件类型

    int rowCount = tableViewModel->rowCount();//fileTableView->model()->rowCount() ;//获取列表行数
    //加入列表中
    QStandardItem *pTableItem = new QStandardItem ();
    pTableItem->setData(fi->fileName(),Qt::UserRole+2);//存储文件名用于检索
    tableViewModel->setItem(rowCount, 0, new QStandardItem(icon,fi->fileName()));//图标+文件名
    tableViewModel->setItem(rowCount, 1, new QStandardItem(updateTime));//最后修改时间
    tableViewModel->setItem(rowCount, 2, new QStandardItem(strType));//文件类型
    tableViewModel->setItem(rowCount, 3, new QStandardItem(readableFilesize(fi->size())));//文件尺寸

}

//获取文件夹包含子文件夹下所有的文件夹
void MainWindow::getAllFolder(QString folderPath,QStringList &folderList)
{
    QDir dir(folderPath);
    QStringList::Iterator iter;
    QStringList list = dir.entryList(QDir::Dirs, QDir::Name);
    for(iter=list.begin(); iter!=list.end(); ++iter)
    {
        if( "." == *iter || ".." == *iter ) continue;
        folderList<<folderPath + "/" + *iter;
        getAllFolder(folderPath+"/"+*iter,folderList);
    }
    return;
}

//从文件夹获取所有文件
QStringList MainWindow::getFileByFolder(QString folderPath)
{
    QStringList picList;
    QDir dir(folderPath);
    QStringList filters;
    filters << "*.jpg"<<"*.jpeg"<<"*.png"<< "*.pnm"<< "*.ppm";//设置过滤类型,允许的类型
    //dir.setNameFilters(filters);//设置文件名的过滤
    dir.setFilter(QDir::Files|QDir::Hidden|QDir::NoSymLinks);
    dir.setSorting(QDir::Time);
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        QString name = fileInfo.fileName();
        QString path = fileInfo.absoluteFilePath();
        picList << path;
    }
    return picList;
}

//kb单位转换
QString MainWindow::readableFilesize( quint64 filesize)
{
    QStringList units;
    units << "B" << "KB" << "MB" << "GB" << "TB" << "PB";
    double mod  = 1024.0;
    double size = filesize;
    //qDebug() << size;
    int i = 0;
    long rest = 0;
    while (size >= mod && i < units.count()-1 )
    {
        rest= (long)size % (long)mod ;
        size /= mod;
        i++;
    }
    QString szResult = QString::number(floor(size));
    if( rest > 0)
    {
       szResult += QString(".") + QString::number(rest).left(2);
    }
    szResult += units[i];
    return  szResult;
}

//获取列表选中的文件
QStringList MainWindow::getListSelectedFile()
{
    QStringList list;

    int model = winStackedLayout->currentIndex();
    if(model == 1)//缩略图
    {
        QModelIndexList selectItems = fileListView->selectionModel()->selectedIndexes();
        foreach(QModelIndex index , selectItems)
        {
            QVariant v = index.data(Qt::UserRole+1);
            PicListItemData d = v.value<PicListItemData>();
            list<<d.picPath;
        }
    }
    else if(model == 2)//信息列表
    {
        QItemSelectionModel *selections = fileTableView->selectionModel();
        QModelIndexList selectedsList = selections->selectedRows();
        for (int i = 0; i < selectedsList.count(); i++)
        {
            int n = selectedsList.at(i).row();
            QModelIndex index = fileTableView->model()->index(n,0);
            QString name = tableViewProxyModel->data(index).toString();
            list<<QString("%1/%2").arg(GlobalHelper::getScanFolder()).arg(name);
        }
    }

    return list;
}

//刷新数据槽
void MainWindow::slotRefreshData()
{
    refreshData();
}

//窗口尺寸改变
void MainWindow::resizeEvent(QResizeEvent *event)
{
    setFileTableSize();
    maskWidget->setGeometry(0, 0, this->width(), this->height());//遮罩窗口位置
}

//***************按钮 槽*********************************

//缩略图列表右键菜单响应函数
void MainWindow::slotListViewContextMenu(QPoint pos)
{
    auto index = fileListView->indexAt(pos);
    if (index.isValid())
    {
        listViewMenu->exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
    }
}

//信息列表右键菜单响应函数
void MainWindow::slotTableViewContextMenu(QPoint pos)
{
    auto index = fileTableView->indexAt(pos);
    if (index.isValid())
    {
        tableViewMenu->exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
    }
}

//搜索框文本改变
void MainWindow::slotSearchTextChange(QString str)
{
    int model = winStackedLayout->currentIndex();
    if(model == 1)//缩略图
    {
        if(listViewProxyModel)
        {
            listViewProxyModel->setFilterFixedString(str);
        }
    }
    else if(model == 2)//信息列表
    {
        if(tableViewProxyModel)
        {
            tableViewProxyModel->setFilterFixedString(str);
        }
    }
}

//扫描按钮
void MainWindow::slotScanButtonClicked()
{
   if(_getCameraInfoThread->isRunning() || _getScannerInfoThread->isRunning())
   {
        GlobalHelper::getDeviceInfoIsOver = false;
        qDebug()<<"thread is running";
   }

    smWindow = new ScanManagerWindow (this);
    smWindow->setAttribute(Qt::WA_ShowModal,true);//模态窗口
    smWindow->show();
    smWindow->move ((QApplication::desktop()->width() - smWindow->width())/2,
                    (QApplication::desktop()->height() - smWindow->height())/2);//屏幕中间显示

    connect(this, SIGNAL(signalThreadOver()), smWindow, SLOT(slotGetDeviceList()));
    connect(smWindow, SIGNAL(signalSearchDevice()), this, SLOT(openCameraThread()));


}

//图片编辑按钮
void MainWindow::slotPicEditButtonClicked()
{
    DDialog *dialog = new DDialog ();
    dialog->setIcon(QIcon(":/img/dialogWarnIcon.svg"));
    dialog->setTitle("通知");
    dialog->setMessage("此功能开发中...");
    dialog->addButton("确定",true);
    dialog->exec();

}

//文字按钮
void MainWindow::slotFontEditButtonClicked()
{
    DDialog *dialog = new DDialog ();
    dialog->setIcon(QIcon(":/img/dialogWarnIcon.svg"));
    dialog->setTitle("通知");
    dialog->setMessage("此功能开发中...");
    dialog->addButton("确定",true);
    dialog->exec();
}

//导出按钮
void MainWindow::slotOutputButtonClicked()
{
    DDialog *dialog = new DDialog ();
    dialog->setIcon(QIcon(":/img/dialogWarnIcon.svg"));
    dialog->setTitle("通知");
    dialog->setMessage("此功能开发中...");
    dialog->addButton("确定",true);
    dialog->exec();
}

//缩略图显示
void MainWindow::slotIconLayoutButtonClicked()
{
    pSearchEdit->clear();
    GlobalHelper::writeSettingValue("set","iconList","0");//修改配置文件键值
    pbtnIconLayout->setDown(true);
    pbtnListLayout->setDown(false);
    renameWidget->setFixedHeight(0);
    rightListBtnWidget->setEnabled(true);//右侧图像列表+扫描按钮容器
    refreshData();
}

//列表显示
void MainWindow::slotListLayoutButtonClicked()
{
    pSearchEdit->clear();
    GlobalHelper::writeSettingValue("set","iconList","1");//修改配置文件键值
    pbtnIconLayout->setDown(false);
    pbtnListLayout->setDown(true);
    renameWidget->setFixedHeight(0);
    rightListBtnWidget->setEnabled(true);//右侧图像列表+扫描按钮容器
    refreshData();
}

//设置菜单
void MainWindow::slotMenuSetButtonClicked()
{
    SetWindow *sw = new  SetWindow (this);
    //sw->setAttribute(Qt::WA_ShowModal,true);//模态窗口
    sw->show();
    //屏幕中间显示
    sw->move ((QApplication::desktop()->width() - sw->width())/2,
                    (QApplication::desktop()->height() - sw->height())/2);

    //设置窗口的关闭信号与本窗口的刷新数据槽连接
    connect(sw,SIGNAL(signalWindowClosed()),this,SLOT(slotRefreshData()));

}


//************右键菜单***********************************
//右键打开
void MainWindow::slotTableViewMenuOpenFile()
{
    QStringList list = getListSelectedFile();
    if(list.size() <= 0)
    {
        return;
    }
    for(int i=0;i<list.size();i++)
    {
        //QDesktopServices::openUrl(QUrl(list.at(i),QUrl::TolerantMode));//包含中文，无法打开
        QDesktopServices::openUrl(QUrl::fromLocalFile(list.at(i)));
    }
}

//右键编辑
void MainWindow::slotTableViewMenuEditFile()
{
    QStringList list = getListSelectedFile();
    DrawInterface *m_draw = new DrawInterface("com.deepin.Draw",
                                              "/com/deepin/Draw",
                                              QDBusConnection::sessionBus(),this);
    QList<QString> picList;
    for(int i=0;i<list.count();i++)
    {
        picList.append(list.at(i));
    }
    qDebug()<<"图像编辑，数量："<<picList.size();
    m_draw->openFiles(picList);

/*
    QProcess *process = new QProcess(this);
    process->startDetached("deepin-draw",QStringList(list.at(0)));
    delete process;
*/
}

//右键导出
void MainWindow::slotTableViewMenuOutputFile()
{
    QStringList list = getListSelectedFile();
    if(list.size() <= 0)
    {
        return;
    }
    QString folder = DFileDialog::getExistingDirectory(this,tr("Select a location"));
    if(folder.isNull() == false || folder.isEmpty() == false)
    {
        for(int i=0;i<list.size();i++)
        {
            QFileInfo fi(list.at(i));
            QString fname = fi.fileName();
            QString newPath = QString("%1/%2").arg(folder).arg(fname);
            QFile::copy(list.at(i),newPath);
        }
    }
}

//右键重命名
void MainWindow::slotTableViewMenuRenameFile()
{
    QStringList list = getListSelectedFile();
    if(list.size() <= 0) return;

    /*if(list.size() == 1)//单个文件
    {
        int model = winStackedLayout->currentIndex();
        if(model == 1)//缩略图
        {
            QModelIndexList selectItems = fileListView->selectionModel()->selectedIndexes();
            if(selectItems.size() > 0)
            {
                fileListView->edit(selectItems.at(0));
                //列表item改变（重命名）
                connect(listViewModel,SIGNAL(itemChanged(QStandardItem *)),this,SLOT(slotListItemChanged(QStandardItem *)));
                fileListView->setEditTriggers(QListView::AnyKeyPressed);


            }
        }
        else if(model == 2)//信息列表
        {

        }
    }
    else*///if(list.size() > 1)//多文件
    {
        renameWidget->setFixedHeight(56);
        rightListBtnWidget->setEnabled(false);//右侧图像列表+扫描按钮容器
    }
}


//列表item改变（重命名）
void MainWindow::slotListItemChanged(QStandardItem *item)
{
    QVariant itemData= item->data();
    PicListItemData picData = itemData.value<PicListItemData>();//还原为原来的数据结构类型;
    qDebug()<<"*************rename:"<<picData.picName<<","<<picData.picPath;
}


//在文件管理器中打开
void MainWindow::slotTableViewMenuOpenFolder()
{
    //图像文件夹路径
    QString imgFolderPath =  GlobalHelper::getScanFolder() + "/";
    //QDesktopServices::openUrl(QUrl::fromEncoded(qba));//GlobalHelper::getScanFolder()));
    QDesktopServices::openUrl(QUrl::fromLocalFile(imgFolderPath));

}

//右键合并PDF
void MainWindow::slotTableViewMenuOutputPDFFile()
{
    QStringList list = getListSelectedFile();
    if(list.size() <= 0)
    {
        return;
    }
    //限制200个
    int i=200;
    while(list.size()>200)
    {
        list.removeAt(i);
    }

    progressBarWindow = new ProgressBarWindow (1,list.size(),tr("Combining into PDF"),this);
    progressBarWindow->setAttribute(Qt::WA_ShowModal,true);//模态窗口
    progressBarWindow->show();
    progressBarWindow->move ((QApplication::desktop()->width() - progressBarWindow->width())/2,
                            (QApplication::desktop()->height() - progressBarWindow->height())/2);
    connect(progressBarWindow,SIGNAL(signalWindowClosed()), this,SLOT(slotGetProgressBarCloseSignal()));

    mergeThread = new MergePDFThread ();
    _mergeThread = new QThread();
    mergeThread->moveToThread(_mergeThread);
    mergeThread->setFileList(list);
    connect(_mergeThread,SIGNAL(started()),mergeThread,SLOT(startMerge()));//开启线程槽函数
    connect(_mergeThread,SIGNAL(finished()),mergeThread,SLOT(deleteLater()));//终止线程时要调用deleteLater槽函数
    connect(mergeThread,SIGNAL(signalSingleFileMergeOver(QString ,int)), this,SLOT(slotSingleFileMergeOver(QString ,int)),Qt::QueuedConnection);
    connect(mergeThread,SIGNAL(signalOver()), this,SLOT(slotMergeOver()));
    _mergeThread->start();//开启多线程

}

//单个文件执行合并PDF结束
void MainWindow::slotSingleFileMergeOver(QString filePath,int fileIndex)
{
    if(progressBarWindow != NULL)
        progressBarWindow->setProgressBarValue(fileIndex+1);
}

//合并PDF结束
void MainWindow::slotMergeOver()
{
    if(progressBarWindow != NULL)
    {
        progressBarWindow->close();
        progressBarWindow = NULL;
    }

    qDebug()<<"1合并线程停止,线程状态："<<_mergeThread->isRunning();
    if(_mergeThread->isRunning())
    {
        _mergeThread->quit();//退出事件循环
        _mergeThread->wait();//释放线程槽函数资源
    }

    qDebug()<<"合并线程停止,线程状态："<<_mergeThread->isRunning();
    refreshData();
}

//获取进度条窗口关闭信号
void MainWindow::slotGetProgressBarCloseSignal()
{
    slotMergeOver();
}


//右键打印
void MainWindow::slotTableViewMenuPrintFile()
{
    DDialog *dialog = new DDialog ();
    dialog->setIcon(QIcon(":/img/dialogWarnIcon.svg"));
    dialog->setTitle("通知");
    dialog->setMessage("打印功能开发中...");
    dialog->addButton("确定",true);
    dialog->exec();
    return;

    QStringList list = getListSelectedFile();
    if(list.size() <= 0)
    {
        return;
    }
    for(int i=0;i<list.size();i++)
    {
        QPrinter printer;
        QPrintDialog printerDialog(&printer,this);
        if(printerDialog.exec())
        {
            QImage img(list.at(i));
            if(img.isNull() == true)
                return;
            QPainter painter(&printer);// 创建一个QPainter对象，并指定绘图设备为一个QPainter对象
            QRect rect =painter.viewport();// 获得QPainter对象的视图矩形区域
            QSize size = img.size(); // 获得图像的大小
            /* 按照图形的比例大小重新设置视图矩形区域 */
            size.scale(rect.size(),Qt::KeepAspectRatio);
            painter.setViewport(rect.x(),rect.y(),size.width(),size.height());
            painter.setWindow(img.rect());// 设置QPainter窗口大小为图像的大小
            painter.drawImage(0,0,img); // 打印图像
         }
    }
}

//右键邮件
void MainWindow::slotTableViewMenuEmailFile()
{
    QStringList list = getListSelectedFile();
    if(list.size() <= 0)
    {
        return;
    }
    QString mail = QString("%1/%2").arg("mailto:?subject=扫描管理&body=扫描管理软件&attachment=").arg(list.at(0));
    QUrl url(mail,QUrl::TolerantMode);
    QDesktopServices::openUrl(url);
}

//右键删除
void MainWindow::slotTableViewMenuDelFile()
{
    QStringList list = getListSelectedFile();
    if(list.size() <= 0)
    {
        return;
    }
    DDialog *dialog = new DDialog ();
    dialog->setIcon(QIcon(":/img/dialogWarnIcon.svg"));
    dialog->setMessage(tr("Are you sure you want to delete the file(s) permanently?"));
    dialog->addButton(tr("Cancel"),false,DDialog::ButtonType::ButtonNormal);
    dialog->addButton(tr("Delete"),true,DDialog::ButtonType::ButtonWarning);
    if(dialog->exec()==DDialog::Accepted)//用户点击了确定按钮
    {
        for(int i=0;i<list.size();i++)
        {
            QFileInfo fileInfo(list.at(i));
            if(fileInfo.exists())
            {
                QFile::remove(list.at(i));
            }
        }
        refreshData();
    }
    delete dialog;
}

//列表项双击
void MainWindow::slotListDoubleClicked(const QModelIndex index)
{
    int model = winStackedLayout->currentIndex();
    if(model == 1)//缩略图
    {
        QVariant v = index.data(Qt::UserRole+1);
        PicListItemData d = v.value<PicListItemData>();
        QDesktopServices::openUrl(QUrl::fromLocalFile(d.picPath));
    }
    else if(model == 2)//信息列表
    {
        QString name = tableViewProxyModel->data(index).toString();
        QDesktopServices::openUrl(QUrl::fromLocalFile(QString("%1/%2").arg(GlobalHelper::getScanFolder()).arg(name)));
    }
}

//取消重命名按钮
void MainWindow::slotCancelRenameBtnClicked()
{
    renameWidget->setFixedHeight(0);
    rightListBtnWidget->setEnabled(true);//右侧图像列表+扫描按钮容器
}

//重命名按钮
void MainWindow::slotRenameBtnClicked()
{
    renameWidget->setFixedHeight(0);
    rightListBtnWidget->setEnabled(true);//右侧图像列表+扫描按钮容器

    QStringList list = getListSelectedFile();
    if(list.size() <= 0)
    {
        return;
    }

    QString newNameText = fileNameText->text().trimmed();
    int newNameNo = fileNameNoText->text().trimmed().toInt();
    for(int i=0;i<list.size();i++)
    {
       if(QFile::exists(list.at(i)))
       {
           QFileInfo fi(list.at(i));
           QString folderPath = fi.absoluteDir().path();//文件父目录
           QString suffix = fi.suffix();//文件后缀名
           QString newName = folderPath + "/" + newNameText + QString::number(newNameNo) + "." + suffix;
           while(QFileInfo(newName).exists() == true)//遇到同名，编号追加1
           {
               newNameNo++;
               newName = folderPath + "/" + newNameText + QString::number(newNameNo) + "." + suffix;
           }
           bool result = QFile::rename(list.at(i),newName);
           qDebug()<<"rename result:"<<result<<",old:"<<list.at(i)<<",new:"<<newName;
       }
       newNameNo++;
    }
    refreshData();
}

//文件名文本框文本改变事件
void MainWindow::slotFileNameTextChanged(const QString & newText )
{
    int maxLength = 250; // 最大字符数
    int trueLength = 83; //中文最大字符数
    QString newName = fileNameText->text().trimmed();
    int length = newName.toLocal8Bit().length();//newName.count();
    int tmpLength = newName.length();
    if(tmpLength != 0 && length != 0 )
    {
        trueLength = maxLength * tmpLength / length;
    }
    qDebug()<<"rename's length,"<<length;
    if(length > maxLength)
    {
        //newName = newName.toLocal8Bit().remove(maxLength,length - maxLength);
        newName = newName.remove(trueLength,tmpLength - trueLength);
        qDebug()<<"rename's newName,"<<newName;
        fileNameText->setText(newName);
    }
    //qDebug()<<"rename new name:"<<newName;

    //newName = newName .replace("\n", "");//无法包括\\,\t, \n 单独处理
    //newName = newName.replace("\t", "");
    //newName = newName.replace("\\","");
    //newName = newName.simplified(); //去掉前后空格
    QRegExp exp("[</\:>*?|]");//去掉特殊字符
    newName = newName.replace(exp, "");
    //  \x00-\x1f是不可见的控制字符
    //QRegExp exp1("[\x01-0x1f]");
    //newName = newName.replace(exp1, "");
    // \x7f是delete，也不可见，所以替换掉
    //QRegExp exp2("[\x7f]");
    //newName = newName.replace(exp2, "");
   // qDebug()<<"rename's new name:"<<newName;

    fileNameText->setText(newName);

    if(newName.isEmpty() || newName.length() <= 0)
    {
        renameBtn->setEnabled(false);
    }
    else
    {
        renameBtn->setEnabled(true);
    }
}


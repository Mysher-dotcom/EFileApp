#include "scanwindow.h"
#include "ui_scanwindow.h"

#include <qdir.h>
#include <QListWidget>
#include <QGraphicsDropShadowEffect>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <dbaseline.h>
#include <QDesktopWidget>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDebug>
#include <helper/globalhelper.h>
#include <DDialog>
#include <QThread>
#include <QList>
#include <DLabel>
#include <QStringListModel>
#include "listview/listviewitemdata.h"
#include "listview/picitemdelegate.h"
#include "mainwindow.h"
#include <DSpinner>
#include "helper/globalhelper.h"
#include <QDateTime>
#include <QDesktopServices>
#include <QMutex>
#include "helper/classificationhelper.h"
#include "helper/deviceinfohelper.h"
#include "cpng.h"
ScanWindow::ScanWindow(int devIndex,QMap<int ,QString> map,QWidget *parent) :
    DMainWindow(parent),
    ui(new Ui::ScanWindow)
{
    ui->setupUi(this);

    initUI();

    deviceIndex = devIndex;
    parMap = map;

    //清理扫描临时文件夹
    GlobalHelper::deleteDirectory(GlobalHelper::getScanTempFoler());

    startScanThread();
}

//窗口关闭
void ScanWindow::closeEvent(QCloseEvent *event)
{
    MainWindow *mainW = (MainWindow*)this->parent()->parent();
    mainW->refreshData();
}

ScanWindow::~ScanWindow()
{
    qDebug()<<"~ScanWindow()";
    delete ui;
}

//初始化界面
void ScanWindow::initUI()
{
    //*****窗口设置*******
    this->resize(840,480);
    this->titlebar()->setTitle("");//标题栏文字设为空
    this->titlebar()->setMenuVisible(false);//隐藏标题栏
    setWindowIcon(QIcon(":/img/logo/logo-16.svg"));// 状态栏图标
    this->titlebar()->setIcon(QIcon(":/img/logo/logo-16.svg"));//标题栏图标
    this->setWindowTitle(tr("Scan Assistant"));//开始菜单栏上鼠标悬浮在窗口上显示的名称

    //******窗口布局设置******
    //布局关系：窗口的容器Widget -> 主布局Layout -> 容器Widget -> 容器布局Layout -> 控件
    winLayout = new QHBoxLayout();//窗口容器布局

    tipWidget = new QWidget () ;//扫描中图标提示容器
    pVLayout = new QVBoxLayout ()  ;//扫描中图标提示布局
    pScanningIconTip = new DSpinner ();//扫描中图标提示
    pScanningFontTip = new QLabel () ;//扫描中文字提示
    pScanningIconTip->setFixedSize(QSize(28,28));
    pScanningIconTip->show();
    pScanningIconTip->start();
    pScanningFontTip->setText(tr("Scanning..."));
    pScanningFontTip->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;color:rgba(85,85,85,0.4);font-size:17px");
    pVLayout->addStretch();
    pVLayout->addWidget(pScanningIconTip,0,Qt::AlignHCenter);
    pVLayout->addWidget(pScanningFontTip,0,Qt::AlignHCenter);
    pVLayout->addStretch();
    tipWidget->setLayout(pVLayout);

    winLayout->addWidget(tipWidget);
    ui->centralwidget->setLayout(winLayout);

}

//开始扫描
void ScanWindow::startScanThread()
{
    doScanThread = new DoScanThread (deviceIndex,parMap);
    subThread = new QThread();
    doScanThread->moveToThread(subThread);

    connect(subThread,SIGNAL(started()),doScanThread,SLOT(startScanSlot()));//开启线程槽函数
    connect(subThread,SIGNAL(finished()),doScanThread,SLOT(deleteLater()));//终止线程时要调用deleteLater槽函数
    connect(subThread,SIGNAL(finished()),this,SLOT(slotFinishThread()));

    //添加图像Item，线程的信号与本窗口的槽关联
    connect(DoScanThread::g_doScanThread,SIGNAL(signalAddPic(QString)),this,SLOT(addItem(QString)));

    //扫描中错误信号
    connect(DoScanThread::g_doScanThread,SIGNAL(signalScanError(QString)),this,SLOT(slotScanError(QString)));

    //扫描结束
    connect(DoScanThread::g_doScanThread,SIGNAL(signalScanOver()),this,SLOT(closeThread()));

    //扫描保存图像
    connect(DoScanThread::g_doScanThread,SIGNAL(signalScanSaveImage(char*,int,int,int,int,int)),this,SLOT(slotScanSaveImage(char*,int,int,int,int,int)));

    subThread->start();//开启多线程

}

//关闭线程
void ScanWindow::closeThread()
{
    if(subThread->isRunning())
    {
        doScanThread->closeThread();//关闭线程槽函数
        subThread->quit();//退出事件循环
        subThread->wait();//释放线程槽函数资源
    }
}

//线程停止触发
void ScanWindow::slotFinishThread()
{
    qDebug()<<"关闭扫描线程,线程状态："<<subThread->isRunning();

    GlobalHelper::playSound("");//播放音效

    //完成后是否直接退出保存
    int nIsExit = GlobalHelper::readSettingValue("set","finishexit").toInt();
    if(nIsExit == 0)
    {
        MainWindow *mainW = (MainWindow*)this->parent()->parent();
        mainW->refreshData();
        this->close();
    }
    changeScanBtnStyle(false);
}

//成功扫描UI，出现图像列表和底部扫描按钮
void ScanWindow::successScanUI()
{
    //移除扫描中图片提示
    winLayout->removeWidget(tipWidget);
    delete tipWidget;

    mainWidget=new QWidget();//主容器
    tWidget=new QWidget ();//上部容器
    bWidget=new QWidget ();//下部容器
    bWidget->setFixedHeight(50);//下部容器固定高度50px
    mainVLayout = new QVBoxLayout () ;//容器布局
    tVLayout = new QVBoxLayout () ;//上部容器布局
    bHLayout = new QHBoxLayout () ;//下部容器布局

    scanBtn=new QPushButton ();//扫描按钮
    scanBtn->setStyleSheet("background:transparent;border:none;");
    scanBtn->setFixedSize(QSize(240,36));
    QHBoxLayout *btnLayout = new QHBoxLayout ();
    scanBtnSpinner = new DSpinner ();
    scanBtnSpinner->setFixedSize(QSize(28,36));
    scanBtnText = new DLabel ();
    scanBtnText->setText(tr("Scan"));//扫描
    scanBtnText->setFixedHeight(36);
    btnLayout->addStretch();
    btnLayout->addWidget(scanBtnSpinner);
    btnLayout->addWidget(scanBtnText);
    btnLayout->addStretch();
   // changeScanBtnStyle(false);
    scanBtn->setLayout(btnLayout);
    btnLayout->setMargin(0);
    btnLayout->setSpacing(0);

    //控件加入布局中
    bHLayout->addWidget(scanBtn);//下部

    //容器加入布局
    mainWidget->setLayout(mainVLayout);//主
    tWidget->setLayout(tVLayout);//上部
    bWidget->setLayout(bHLayout);//下部

    //主布局加入右侧-上部容器 右侧-下部容器
    mainVLayout->addWidget(tWidget,Qt::AlignTop);
    mainVLayout->addWidget(bWidget,Qt::AlignBottom);

    //主布局加入容器
    winLayout->addWidget(mainWidget);

    //窗口加入主布局
    ui->centralwidget->setLayout(winLayout);

    //去除容器布局的外间隙
    tVLayout->setMargin(0);
    tVLayout->setSpacing(0);
    bHLayout->setMargin(0);
    bHLayout->setSpacing(0);
    mainVLayout->setMargin(0);
    mainVLayout->setSpacing(0);
    winLayout->setMargin(0);
    winLayout->setSpacing(0);

    connect(scanBtn, SIGNAL(clicked()), this, SLOT(onBtnScanClicked()));//扫描按钮信号槽

}

//扫描按钮
void ScanWindow::onBtnScanClicked()
{
    changeScanBtnStyle(true);
    startScanThread();
}

//拷贝文件
void ScanWindow::copyFile(QString oldFilePath, QString newFilePath)
{
    QFile oldFile(oldFilePath);
    QFile newFile(newFilePath);
    int nIsFileOver= GlobalHelper::readSettingValue("set","fileover").toInt();
    if(oldFile.exists() == true)
    {
        if(newFile.exists() == true)//如果目标文件已存在
        {
            if(nIsFileOver == 0)//并且提示开关已打开
            {
                QFileInfo *fi = new QFileInfo(oldFilePath);
                DDialog *dialog = new DDialog ();
                dialog->setIcon(QIcon(":/img/dialogWarnIcon.svg"));
                dialog->setMessage(fi->fileName()+tr("The file already exists. Do you want to replace it?"));
                dialog->addButton(tr("Cancel"),false,DDialog::ButtonType::ButtonNormal);
                dialog->addButton(tr("Replace"),true,DDialog::ButtonType::ButtonWarning);
                if(dialog->exec()==DDialog::Accepted)//用户点击了覆盖按钮
                {
                    newFile.remove();//删除已存在的文件
                    oldFile.copy(newFilePath);
                }
                else//用户点击了取消按钮
                {
                    newFilePath =  getCopyFileName(newFilePath);
                    oldFile.copy(newFilePath);
                }
                delete dialog;
                delete fi;
            }
            else//提示开关未打开
            {
                newFilePath =  getCopyFileName(newFilePath);
                oldFile.copy(newFilePath);
            }
        }
        else//新文件目录不存在重名文件
        {
           oldFile.copy(newFilePath);
        }
    }
    else
    {
        qDebug()<<oldFilePath<<",文件不存在";
    }
}

//获取拷贝文件名，用于重名文件覆盖
QString ScanWindow::getCopyFileName(QString filePath)
{
    int index = 0;
    QFileInfo *fi = new QFileInfo(filePath);
    QFile newFile(filePath);
    QString fileSuffix = fi->suffix().trimmed();
    QString fileName = fi->fileName();
    fileName = fileName.mid(0,fileName.indexOf("."));
    while(newFile.exists() == true)
    {
       if(index == 0)
       {
           filePath = GlobalHelper::getScanFolder()+"/"+fileName+"("+tr("copy")+")."+fileSuffix;
       }
       else
       {
           filePath = GlobalHelper::getScanFolder()+"/"+fileName+"("+tr("copy")+QString::number(index)+")."+fileSuffix;
       }
       qDebug()<<filePath;
       newFile.setFileName(filePath);
       index++;
    }
    delete  fi;
    return filePath;
}

//列表项双击
void ScanWindow::slotListDoubleClicked(const QModelIndex index)
{
    QVariant v = index.data(Qt::UserRole+1);
    PicListItemData d = v.value<PicListItemData>();
    QDesktopServices::openUrl(QUrl::fromLocalFile(d.picPath));
}

//显示图片列表到UI
void ScanWindow::showImageList()
{
    imgList = new DListView();
    //imgList->setSelectionMode(QAbstractItemView::MultiSelection);
    imgList->setViewMode(QListView::IconMode);
    imgList->setResizeMode(QListView::Adjust);
    imgList->setEditTriggers(QAbstractItemView::NoEditTriggers);//双击Item屏蔽可编辑
    imgList->setMovement(QListView::Static);//禁止拖拽Item

    pModel = new QStandardItemModel ();
    for(QString tmp : fileNames)
    {
       addItem(tmp);
    }
    PicItemDelegate *pItemDelegate=new PicItemDelegate (this);
    imgList->setItemDelegate(pItemDelegate);
    //imgList->setModel(pModel);
    proxyModel = new QSortFilterProxyModel (imgList);
    proxyModel->setSourceModel(pModel);
    imgList->setModel(proxyModel);

    connect(imgList,SIGNAL(doubleClicked(const QModelIndex)),this,SLOT(slotListDoubleClicked(const QModelIndex)));//列表项双击

    tVLayout->addWidget(imgList);

}

//增加图像Item到列表
void ScanWindow::addItem(QString path)
{
    qDebug()<<"增加图像Item到列表:"<<path;
    if(listViewIsShow == false)
    {
        successScanUI();
        showImageList();
        listViewIsShow = true;
    }
    QStandardItem *pItem = new QStandardItem ();
    PicListItemData itemData;
    itemData.picPath =path;
    QFileInfo *fi = new QFileInfo(path);
    itemData.picName =fi->fileName();
    pItem->setData(QVariant::fromValue(itemData),Qt::UserRole+1);
    pModel->appendRow(pItem);
    fileNames << path;
    imgList->scrollToBottom();//自动滚动到最下面
    pItem->setToolTip(fi->fileName());//悬浮在Item上，显示文件名

}

//修改扫描按钮样式，true=扫描中样式，false=普通样式
void ScanWindow::changeScanBtnStyle(bool isScanning)
{
    if(listViewIsShow == false)
    {
        successScanUI();
        showImageList();
        listViewIsShow = true;
    }
    if(isScanning == true)
    {
        scanBtnText->setText(tr("Scanning..."));
        scanBtn->setStyleSheet("background:transparent;border:none;");
        scanBtnSpinner->show();
        scanBtnSpinner->start();
    }
    else
    {
        scanBtnText->setText(tr("Scan"));
        scanBtn->setStyleSheet("");
        scanBtnSpinner->hide();
        scanBtnSpinner->stop();
    }
}

//扫描中错误信号
void ScanWindow::slotScanError(QString msg)
{
    DDialog *dialog = new DDialog ();
    dialog->setIcon(QIcon(":/img/dialogWarnIcon.svg"));
    dialog->setTitle(tr("Scan failed"));
    dialog->setMessage(msg);
    dialog->addButton(tr("Confirm"),true);
    dialog->exec();

    if(listViewIsShow == false)
    {
        successScanUI();
        showImageList();
        listViewIsShow = true;
    }
    changeScanBtnStyle(false);//修改扫描按钮样式，true=扫描中样式，false=普通样式
    closeThread();//发生错误，结束扫描线程
}


QMutex MutexBuffLock;
CJpeg m_jpg_scan;//CJPEG对象
int ScanWindow::WriteFile(unsigned char* srcData,int len,FILE * destfb)
{
    int write_length = 0;
    write_length = fwrite(srcData,sizeof(char),len,destfb);
    usleep(100);
    return write_length;
}
void ScanWindow::slotScanSaveImage(char* data,int nSize,int w,int h,int nBpp,int nDPI)
{
    QString defaultDeviceModelFilePath =  DeviceInfoHelper::readValue(DeviceInfoHelper::getDeviceListInfoFilePath(),
                                             "default",
                                             "config");
    //图像文件夹路径
    QString imgFolderPath =  GlobalHelper::getScanFolder() + "/";
    char *folderPath;
    QByteArray qba = imgFolderPath.toLatin1();
    folderPath = qba.data();

    //编号
    int imgIndex = GlobalHelper::readSettingValue("set","imgPreNameIndex").toInt();
    imgIndex++;
    QString strImgIndex = QString("%1").arg(imgIndex,6,10,QLatin1Char('0'));
    QString strImgIndexDate = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");//日期编号
    char *cStrImgIndex;
    QByteArray qbaIndex = strImgIndexDate.toLatin1();//strImgIndex.toLatin1();
    cStrImgIndex = qbaIndex.data();
    GlobalHelper::writeSettingValue("set","imgPreNameIndex",QString::number(imgIndex));//扫描文件的名称编号

    //后缀名
    //QString sImgSuffix= GlobalHelper::readSettingValue("imgEdit","imgFormat").toLower();
    QString sImgSuffix= DeviceInfoHelper::readValue(defaultDeviceModelFilePath,"imgset","imgFormat").toLower();
    if(sImgSuffix.isNull() || sImgSuffix.isEmpty())
    {
        sImgSuffix = "jpg";
    }
    char *imgSuffix;
    QByteArray qbaimgSuffix = sImgSuffix.toLatin1();
    imgSuffix = qbaimgSuffix.data();

    //图像存储路径
    char path[256]={0};
    char part_path[256]={0};
    sprintf (path, "%s%s.%s", folderPath,cStrImgIndex,imgSuffix);
    strcpy (part_path, path);

    qDebug()<<"scanwindow存图路径:"<<part_path;
    int color_type = 2;
    //内存段转成MImage
    mcvInit();
    int height = h;
    int width = w;
    int channel  = nBpp ;
    if(channel==3)
    {
        color_type=2;
    }
    else
        color_type=0;
    MImage * src = mcvCreateImageFromArray(width,height,channel,data,false);
    qDebug("1\n");

    delete data;
    data = NULL;  

    //图像处理顺序：裁切-文档优化-黑白图-打水印
    //是否裁切
    MImage* srcCut = NULL;
    int nIsCut= DeviceInfoHelper::readValue(defaultDeviceModelFilePath,"imgset","isCut").toInt();
    if(nIsCut != 0)
    {
        float fAngle = 0;
        MPoint mp4[4];
        //if(mcvDetectRect_scanner(src,fAngle,mp4,MRect(0,0,0,0)))
        if(mcvDetectRect(src,fAngle,mp4,MRect(0,0,0,0)))
        {
            MRectR rectR;
            if(fAngle < -45)
            {
                fAngle += 90;
            }
            rectR.m_fAngle = fAngle;
            for(int i = 0;i < 4;i++)
            {
                rectR.m_pt[i].x= mp4[i].x;
                rectR.m_pt[i].y = mp4[i].y;
                if(rectR.m_fAngle == 0)
                {
                    rectR.m_pt[i].x = rectR.m_pt[i].x < src->width?rectR.m_pt[i].x:src->width;
                    rectR.m_pt[i].y = rectR.m_pt[i].y < src->height?rectR.m_pt[i].y:src->height;
                }
            }
            srcCut = mcvCutR(src,rectR);
            if(src)
            {
                mcvReleaseImage1(src);
                src = NULL;
            }
        }
    }
    else {
        srcCut = mcvClone(src);
        if(src)
        {
            mcvReleaseImage1(src);
            src = NULL;
        }
    }
    //文档类型下拉框(0=原始文档，1=文档优化，2=彩色优化，3=红印文档优化，4=反色，5=滤红)
    //int nDocType = GlobalHelper::readSettingValue("imgEdit","docType").toInt();
    int nDocType= DeviceInfoHelper::readValue(defaultDeviceModelFilePath,"imgset","docType").toInt();
    switch (nDocType)
    {
        case 1:
            mcvBrightBalance(srcCut);//文档优化
            break;
       /* case 2:
            src = mcvColorBlance(srcCut);//彩色优化
            break;*/
        case 3:
            mcvEnhancement3In1(srcCut,2);//红印文档优化(Linux 红蓝参数颠倒一下)
            break;
        case 4:
            mcvInverse(srcCut);//反色
            break;
        case 5:
            mcvColorDropout(srcCut,240);//滤红(Linux 红蓝参数颠倒一下)
            break;
    }
    if(nDocType==2)
    {
        src = mcvColorBlance(srcCut);
        if(srcCut)
        {
            mcvReleaseImage1(srcCut);
            srcCut = NULL;
        }
        srcCut = mcvClone(src);
        if(src)
        {
            mcvReleaseImage1(src);
            src = NULL;
        }
    }
    //缺角修复
    //int nIsRepair = GlobalHelper::readSettingValue("imgEdit","isRepair").toInt();
    int nIsRepair= DeviceInfoHelper::readValue(defaultDeviceModelFilePath,"imgset","isRepair").toInt();
    if(nIsRepair == 0)
    {
        mcvFillBorder(srcCut);
    }

    //去噪
   // int nIsNoise = GlobalHelper::readSettingValue("imgEdit","isFilterDenoising").toInt();
   // if(nIsNoise == 0)
    {
   //     mcvNoise(srcCut,0);
    }
    //黑白图，二值化
    MImage* srcThreshold = NULL;
    //int nIsLineart = GlobalHelper::readSettingValue("imgEdit","nIsLineart").toInt();
    QString strIsLineart = DeviceInfoHelper::readValue(defaultDeviceModelFilePath,"imgset","nIsLineart");
    if(strIsLineart.isEmpty() || strIsLineart.toInt() == 1)
    {
        srcThreshold = mcvClone(srcCut);
        if(srcCut)
        {
            mcvReleaseImage1(srcCut);
            srcCut = NULL;
        }
    }
    else
    {
        srcThreshold = mcvAdaptiveThreshold(srcCut);
        if(srcCut)
        {
            mcvReleaseImage1(srcCut);
            srcCut = NULL;
        }
    }
    //水印
    MImage* srcWater = NULL;
    //int nIsWaterMark= GlobalHelper::readSettingValue("imgEdit","isWaterMark").toInt();
    int nIsWaterMark= DeviceInfoHelper::readValue(defaultDeviceModelFilePath,"imgset","isWaterMark").toInt();
    if(nIsWaterMark==1)
    {
        //QString strWaterMarkText = GlobalHelper::readSettingValue("imgEdit","waterMarkText");
        QString strWaterMarkText= DeviceInfoHelper::readValue(defaultDeviceModelFilePath,"imgset","waterMarkText");
        if(strWaterMarkText.isEmpty())
        {
            strWaterMarkText = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz");
        }
        char *cWaterMarkText;
        QByteArray qbaWaterMarkText= strWaterMarkText.toUtf8();
        cWaterMarkText = qbaWaterMarkText.data();
        //long color_r = GlobalHelper::readSettingValue("imgEdit","waterMarkColor_r").toLong();
        //long color_g = GlobalHelper::readSettingValue("imgEdit","waterMarkColor_g").toLong();
        //long color_b = GlobalHelper::readSettingValue("imgEdit","waterMarkColor_b").toLong();
        long color_r = DeviceInfoHelper::readValue(defaultDeviceModelFilePath,"imgset","waterMarkColor_r").toLong();
        long color_g = DeviceInfoHelper::readValue(defaultDeviceModelFilePath,"imgset","waterMarkColor_g").toLong();
        long color_b = DeviceInfoHelper::readValue(defaultDeviceModelFilePath,"imgset","waterMarkColor_b").toLong();
        //src = mcvWaterMark2(src,cWaterMarkText,"./simhei.ttf",color_r,color_g,color_b,0,0);
        //水印颜色 R B颠倒，字号给0为自适应
        //获取路径
        QString str = QCoreApplication::applicationDirPath();
        QString strFontPath = QString("%1/%2").arg(str).arg("simhei.ttf");
        qDebug()<<"watermark font file's path:"<<strFontPath;
        qDebug()<<"QApplication::font():"<<QApplication::font().toString();
        //QFontDatabase::addApplicationFont(QApplication::font().toString());
        //QDesktopServices::openUrl(QUrl::fromLocalFile(strFontPath));
        srcWater = mcvWaterMark2(srcThreshold,cWaterMarkText,strFontPath.toLocal8Bit().data(),0,color_b,color_g,color_r,0,0);
        if(srcThreshold)
        {
            mcvReleaseImage1(srcThreshold);
            srcThreshold = NULL;
        }
    }
    else
    {
        srcWater = mcvClone(srcThreshold);
        if(srcThreshold)
        {
            mcvReleaseImage1(srcThreshold);
            srcThreshold = NULL;
        }
    }
    char* tmp = substrend(part_path,2);
    if(strcmp(tmp,"jpg")==NULL)
    {
        free(tmp);
        tmp = NULL;
        uchar* dst = mcvGetImageData(srcWater);
        JPEGInfo jpgInfo;
        memset(&jpgInfo,0,sizeof(jpgInfo));
        jpgInfo.xResolution.b = 1000;
        jpgInfo.xResolution.a = nDPI*jpgInfo.xResolution.b;
        jpgInfo.yResolution.b = 1000;
        jpgInfo.yResolution.a =nDPI*jpgInfo.yResolution.b;
        jpgInfo.compression = 50;
        qDebug("saveImageToJpeg width is %d,height is %d,channel is %d\n",srcWater->width,srcWater->height,srcWater->channel * 8);
        long lret = m_jpg_scan.saveImageToJpeg(dst,srcWater->width,srcWater->height,srcWater->channel * 8,part_path,jpgInfo);
        dst = NULL;
    }
    else if(strcmp(tmp,"png")==NULL)
    {
        unsigned char* dst = mcvGetImageData(srcWater);
        CPNG ggpng;
        pic_data outimage;
        outimage.width = srcWater->width;
        outimage.height = srcWater->height;
        if(color_type==2)
        {
            outimage.rgba = new unsigned char[outimage.width*outimage.height*3];
            memset(outimage.rgba,0,outimage.width*outimage.height*3);
            memcpy(outimage.rgba,dst,outimage.width*outimage.height*3);
        }
        else
        {
            outimage.rgba = new unsigned char[outimage.width*outimage.height];
            memset(outimage.rgba,0,outimage.width*outimage.height);
            memcpy(outimage.rgba,dst,outimage.width*outimage.height);
        }
        outimage.color_type=color_type;
        outimage.bit_depth = 8;
        outimage.alpha_flag = 0;
        ggpng.write_png_file(part_path,&outimage);
        //delete outimage.rgba;
    }
    else
    {
        mcvSaveImage(part_path,srcWater,200,200);
    }
    mcvReleaseImage(&srcWater);
    srcWater = NULL;

    addItem(part_path);

}
char* ScanWindow::substrend(char * str, int n)
 {
     char * substr = (char*) malloc (n+1);
     int length = strlen(str);
     if(n >= length)//若截自取长度大百于字符度串长度，则问直接截取全部答字符串
     {
     strcpy(substr, str);
     return substr;
     }
     int k = 0;
     for(int i = strlen(str) - n - 1; i < strlen(str); i++)
     {
     substr[k++] = str[i];
     }
     substr[k] = '\0';
     return substr;
 }

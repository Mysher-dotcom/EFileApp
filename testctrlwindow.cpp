#include "testctrlwindow.h"
#include "ui_testctrlwindow.h"

#include <QHBoxLayout>
#include <DFloatingButton>
#include <dfilechooseredit.h>
#include <dswitchlineexpand.h>
#include <DTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QFileIconProvider>
#include <QDateTime>
#include <qmath.h>

#include "errortype.h"
#include "camscansdk.h"
#include "cmimage.h"
#include "MImage.h"
#include "Jpeg.h"


int imgIndexTest;
CJpeg m_jpgTest;//CJPEG对象
TestCtrlWindow *tCtrlWindow;


TestCtrlWindow::TestCtrlWindow(QWidget *parent) :
    DMainWindow(parent),
    ui(new Ui::TestCtrlWindow)
{
    tCtrlWindow = this;
    ui->setupUi(this);
    initUI();
    initConnection();
}

TestCtrlWindow::~TestCtrlWindow()
{
    delete ui;
}

void TestCtrlWindow::initUI()
{
    this->resize(500,520);//窗口初始尺寸
    QVBoxLayout *mainVLayout = new QVBoxLayout ();
    QHBoxLayout *topHLayout = new QHBoxLayout ();
    QHBoxLayout *centerHLayout = new QHBoxLayout ();
    QHBoxLayout *bottomHLayout = new QHBoxLayout ();
    QWidget *mainWidget = new QWidget ();
    QWidget *topWidget = new QWidget ();
    QWidget *centerWidget = new QWidget ();
    QWidget *bottomWidget = new QWidget ();

    initSANEBtn = new QPushButton ();
    initSANEBtn->setText("1-init");
    initSANEBtn->setFixedSize(QSize(100,30));

    getParBtn = new QPushButton ();
    getParBtn->setText("2-get par");
    getParBtn->setFixedSize(QSize(100,30));

    scanBtn = new QPushButton ();
    scanBtn->setText("3-scan");
    scanBtn->setFixedSize(QSize(100,30));

    colorCb = new QComboBox ();
    scanmodeCb = new QComboBox ();
    dpiCb = new QComboBox ();

    QString parColor1 = "Color24";
    QString parColor2 = "Gray8";
    QString parColor3 = "Lineart";
    colorCb->addItems(QStringList()<<parColor1<<parColor2<<parColor3);

    QString parMode1 = "ADF_FRONT";
    QString parMode2 = "ADF_DUPLEX";
    QString parMode3 = "FLAT_FRONT";
    scanmodeCb->addItems(QStringList()<<parMode1<<parMode2<<parMode3);

    QString dpi1 = "200";
    QString dpi2 = "300";
    QString dpi3 = "600";
    dpiCb->addItems(QStringList()<<dpi1<<dpi2<<dpi3);


    logText = new QTextEdit ();
    logText->setFixedWidth(this->width());
    logText->setFixedHeight(500);

    topHLayout->addWidget(initSANEBtn);
    topHLayout->addWidget(getParBtn);
    topHLayout->addWidget(scanBtn);

    centerHLayout->addWidget(colorCb);
    centerHLayout->addWidget(scanmodeCb);
    centerHLayout->addWidget(dpiCb);

    bottomHLayout->addWidget(logText);

    mainVLayout->addWidget(topWidget,0,Qt::AlignTop);
    mainVLayout->addWidget(centerWidget,0,Qt::AlignTop);
    mainVLayout->addWidget(bottomWidget);
    mainVLayout->addStretch();

    mainWidget->setLayout(mainVLayout);
    topWidget->setLayout(topHLayout);
    centerWidget->setLayout(centerHLayout);
    bottomWidget->setLayout(bottomHLayout);

    ui->centralwidget->setLayout(mainVLayout);
}

void TestCtrlWindow::initConnection()
{
    connect(initSANEBtn, SIGNAL(clicked()), this, SLOT(slotInitSANE()));
    connect(getParBtn, SIGNAL(clicked()), this, SLOT(slotGetPar()));
    connect(scanBtn, SIGNAL(clicked()), this, SLOT(slotScan()));
}



//扫描过程中存图回调
int TestCtrlWindow::doScanReceiveCB(uchar*data,int size,int w,int h,int nBpp,int nDPI)
{
    //图像文件夹路径
    QString imgFolderPath = "/tmp/";
    char *folderPath;
    QByteArray qba = imgFolderPath.toLatin1();
    folderPath = qba.data();

    //编号
    imgIndexTest++;
    QString strImgIndex = QString("%1").arg(imgIndexTest,6,10,QLatin1Char('0'));
    char *cStrImgIndex;
    QByteArray qbaIndex = strImgIndex.toLatin1();
    cStrImgIndex = qbaIndex.data();

    //图像存储路径
    char path[256]={0};
    char part_path[256]={0};
    sprintf (path, "%s%s.jpg", folderPath,cStrImgIndex);
    strcpy (part_path, path);


    //内存段转成MImage
    mcvInit();
    int height = h;
    int width = w;
    int channel  = nBpp;
    char* buffer =(char*)malloc(sizeof(char)*height*width*channel);
    memcpy(buffer,data,height*width*channel);
    MImage * src = mcvCreateImageFromArray(width,height,channel,buffer,false);
    free(buffer);

    //mcvSaveImage(part_path,src,nDPI,nDPI);//CmImage存图
    //CJPEG存图
    uchar* dst = mcvGetImageData(src);
    JPEGInfo jpgInfo;
    memset(&jpgInfo,0,sizeof(jpgInfo));
    jpgInfo.xResolution.b = 1000;
    jpgInfo.xResolution.a = 200*jpgInfo.xResolution.b;
    jpgInfo.yResolution.b = 1000;
    jpgInfo.yResolution.a =200*jpgInfo.yResolution.b;
    jpgInfo.compression = 50;
    //long lret = m_jpgTest.saveImageToJpeg(dst,src->width,src->height,src->channel*8,part_path,jpgInfo);

    mcvReleaseImage1(src);

    tCtrlWindow->writeMsg("扫描图像路径:"+tr(part_path));

    return 0;

}

//扫描过程中错误信息回调
int TestCtrlWindow::doScanStatuCB(int nStatus)
{
    QString msg="未知错误";
    if(nStatus == STATUS_NO_DOCS)
    {
        msg="请正确放入纸张！";
    }
    else if(nStatus = STATUS_DEVICE_BUSY)
    {
        msg="扫描过程中卡纸！";
    }
    else
    {
        msg="未知错误,"+nStatus;
    }
    tCtrlWindow->writeMsg("扫描错误:"+msg);
    return  0;
}



void TestCtrlWindow::slotInitSANE()
{
    Init();
    writeMsg("初始化SANE完成");

    long nDevCount = -1;
    long nResult = GetDevCount(nDevCount);
    writeMsg("获取设备数量完成，共 " + QString::number(nDevCount) + " 台");

    char szDevKanas[256] = {0},szDevVendor[256] = {0},szDevModel[256]={0},szDevType[256]={0};
    for (int i = 0;i < nDevCount;i++)
    {
         //数据示例：
         //szDevKanas = kanas:Kanas
         //szDevVendor = Uniscan
         //szDevModel = F3135
         //szDevType = ADF scanner
        GetDevName(i,szDevKanas,szDevVendor,szDevModel,szDevType);
        writeMsg("获取设备信息完成，名称:" + tr(szDevKanas) + ",品牌:" + tr(szDevVendor) +
                                ",型号:" + tr(szDevModel) + ",类型:" + tr(szDevType));
    }

}

void TestCtrlWindow::slotGetPar()
{
    int nResult = OpenDev(0);
    if(nResult != 0)
    {
        writeMsg("打开设备失败");
        return;
    }

    long nCapCount = -1;
    nResult = GetCapCount(0,nCapCount);
    if(nResult != 0)
    {
        writeMsg("获取设备参数失败");
        return;
    }

    if(nResult == 0 && nCapCount > 0)
    {
        writeMsg("参数类型:0=预览 ;1=范围类型 最大值 最小值等；2=long ；3=char*");
        //暂时nCapCount-1，获取到11个参数，会崩溃，提示数组越界 vector 20200310
        for(int j=0;j<nCapCount-1;j++)
        {
            char szCapName[256]={0};//参数名
            long nType = -1;//参数类型
            void* szdata = NULL;//参数数据
            GetCapInfo(0,j,szCapName,nType,&szdata);
            writeMsg("参数名:" + tr(szCapName) + ",参数类型:" + QString::number(nType));

            long nParValueCount = -1;//参数值数量
            long longMin=-1,longMax=-1,longQuant=-1;//范围值
            long longParValue = 0;//long型参数值
            char stringParValue[256]={0};//string型参数值
            //nType 0=预览 1=范围类型 最大值 最小值等；2=long ；3=char*
            switch (nType)
            {
                case 0:

                break;
                case 1:
                    AnalysisRangeCapData(nType,szdata,longMin,longMax,longQuant);

                    //writeMsg("范围型参数值：最小：" + QString::number(longMin) +
                    //         ",最大：:" + QString::number(longMax)+
                    //         ",最佳：:" + QString::number(longQuant));
                break;
                case 2:
                    AnalysisWordCapDataCount(nType,szdata,nParValueCount);
                    for (int i = 0;i <nParValueCount;i++)
                    {
                        AnalysisWordCapData(i,longParValue);
                        writeMsg("int型参数值："+QString::number(longParValue));
                    }
                break;
                case 3:
                    AnalysisStringCapDataCount(nType,szdata,nParValueCount);
                    for (int i = 0;i <nParValueCount;i++)
                    {
                        AnalysisStringCapData(i,stringParValue);
                        QString d=  QLatin1String(stringParValue);
                        writeMsg("string型参数值："+d);
                    }
                break;
            }//end  switch (nType)
        }//end  for(int j=0;j<nCapCount-1;j++)
    }//end if(nResult == 0 && nCapCount > 0)

}

void TestCtrlWindow::slotScan()
{
    QString c = colorCb->currentText();
    QByteArray bac = c.toLatin1();

    QString d = scanmodeCb->currentText();
    QByteArray bad = d.toLatin1();

    QString p = dpiCb->currentText();
    QByteArray bap = p.toLatin1();

    SetCapCtrl(0,bac.data());
    SetCapCtrl(1,bad.data());
    SetCapCtrl(2,bap.data());

    Scan(0,doScanReceiveCB,doScanStatuCB);//扫描

    writeMsg("扫描完成");
}

void TestCtrlWindow::writeMsg(QString msg)
{
    logText->append(msg);
}

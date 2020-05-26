#include "camerawindow.h"
#include "ui_camerawindow.h"
#include "globalhelper.h"
#include <QGraphicsDropShadowEffect>
#include "camcapturesdk.h"
#include <QDebug>
#include <QString>
#include <QImage>
#include <QPainter>
#include <QMutex>
#include <QDir>
#include "listview/listviewitemdata.h"
#include "listview/deviceitemdelegate.h"
#include <QWheelEvent>
#include <DDialog>
#include <QDesktopServices>


QImage *g_image = NULL;
QMutex MutexDataBuffLock;
CameraWindow *g_CameraWindow = NULL;

//QMatrix matrix;

CameraWindow::CameraWindow(int devIndex,QMap<QString ,QString> map,QWidget *parent) :
    DMainWindow(parent),
    ui(new Ui::CameraWindow)
{
    ui->setupUi(this);

    g_CameraWindow = this;
    nDeviceIndex = devIndex;
    parMap = map;
    nVideoRotateAngle = 0;
    nVideoScaleSize = 1;
    isCut = false;
    int nIsCutTmp = GlobalHelper::readSettingValue("imgEdit","isCut").toInt();
    if(nIsCutTmp != 1)
    {
        isCut = true;
    }
    nShotType = GlobalHelper::readSettingValue("imgEdit","shotType").toInt();//拍摄类型（0=手动，1=定时拍）
    nShotTime = GlobalHelper::readSettingValue("imgEdit","shotTime").toInt();//定时拍秒

    initUI();
    initConnection();
    initSDK();

    isStartShotTimer = false;
    timerWidget->setVisible(false);//隐藏定时拍
    saveBtn->setEnabled(false);//禁用保存按钮
    scanBtn->setEnabled(false);//禁用扫描按钮
    toolBarWidget->setEnabled(false);//禁用工具栏

    //清理扫描临时文件夹
    GlobalHelper::deleteDirectory(GlobalHelper::getScanTempFoler());
}

CameraWindow::~CameraWindow()
{
    delete ui;
}

//初始化界面
void CameraWindow::initUI()
{
    //*窗口基本属性设置*
    this->resize(840,480);//窗口初始尺寸
    this->setMinimumSize(840,480);//窗口最小尺寸
    this->titlebar()->setTitle("");//标题栏文字设为空
    setWindowIcon(QIcon(":/img/logo/logo-16.svg"));// 状态栏图标
    this->titlebar()->setIcon(QIcon(":/img/logo/logo-16.svg"));//标题栏图标
    this->titlebar()->setMenuVisible(false);//菜单按钮隐藏
    this->setWindowTitle("拍摄仪");//开始菜单栏上鼠标悬浮在窗口上显示的名称

    //*窗口布局设置*
    winVLayout = new QVBoxLayout();//窗口布局
    mainVLayout = new QHBoxLayout();//主布局
    leftVLayout = new QVBoxLayout();//左侧布局
    videoHLayout = new QHBoxLayout ();//视频布局
    leftBottomVLayout = new QVBoxLayout();//左侧下部布局
    rightVLayout = new QVBoxLayout();//右侧布局
    rightTopVLayout = new QVBoxLayout();//右侧上部布局
    rightBottomVLayout = new QVBoxLayout();//右侧下部布局
    toolBarHLayout = new QHBoxLayout();//工具栏布局
    mainWidget = new QWidget () ;//主容器
    leftWidget = new QWidget () ;//左侧容器
    videoWidget = new QWidget ();//视频容器
    leftBottomWidget = new QWidget () ;//左侧下部容器
    rightWidget = new QWidget () ;//右侧容器
    rightTopWidget = new QWidget () ;//右侧上部容器
    rightBottomWidget = new QWidget () ;//右侧下部容器
    toolBarWidget = new QWidget () ;//工具栏容器
    scanBtn = new QPushButton();//扫描按钮
    oneTOneBtn = new DIconButton(nullptr);//1:1按钮
    zoomInBtn = new DIconButton(nullptr);//放大按钮
    zoomOutBtn = new DIconButton(nullptr);//缩小按钮
    rotateLeftBtn = new DIconButton(nullptr);//左旋按钮
    rotateRightBtn = new DIconButton(nullptr);//右旋按钮
    cutBtn = new DIconButton(nullptr);//裁切按钮
    imgListView = new DListView () ;//图像列表
    saveBtn = new QPushButton();//保存按钮

    oneTOneBtn->setIcon(QIcon(":/img/camera/1t1.svg"));
    oneTOneBtn->setFixedSize(QSize(50, 50));
    oneTOneBtn->setIconSize(QSize(20,20));
    oneTOneBtn->setToolTip("1:1显示");
    oneTOneBtn->setStyleSheet("background:white;border-radius:8px;");

    zoomInBtn->setIcon(QIcon(":/img/camera/zoomin.svg"));
    zoomInBtn->setFixedSize(QSize(50, 50));
    zoomInBtn->setIconSize(QSize(20,20));
    zoomInBtn->setToolTip("放大");
    zoomInBtn->setStyleSheet("background:white;border-radius:8px;");

    zoomOutBtn->setIcon(QIcon(":/img/camera/zoomout.svg"));
    zoomOutBtn->setFixedSize(QSize(50, 50));
    zoomOutBtn->setIconSize(QSize(20,20));
    zoomOutBtn->setToolTip("缩小");
    zoomOutBtn->setStyleSheet("background:white;border-radius:8px;");

    rotateLeftBtn->setIcon(QIcon(":/img/camera/rotateleft.svg"));
    rotateLeftBtn->setFixedSize(QSize(50, 50));
    rotateLeftBtn->setIconSize(QSize(20,20));
    rotateLeftBtn->setToolTip("左旋");
    rotateLeftBtn->setStyleSheet("background:white;border-radius:8px;");

    rotateRightBtn->setIcon(QIcon(":/img/camera/rotateright.svg"));
    rotateRightBtn->setFixedSize(QSize(50, 50));
    rotateRightBtn->setIconSize(QSize(20,20));
    rotateRightBtn->setToolTip("右旋");
    rotateRightBtn->setStyleSheet("background:white;border-radius:8px;");

    cutBtn->setIcon(QIcon(":/img/camera/cut.svg"));
    cutBtn->setFixedSize(QSize(50, 50));
    cutBtn->setIconSize(QSize(20,20));
    cutBtn->setToolTip("裁切");
    cutBtn->setStyleSheet("background:white;border-radius:8px;");

    //videoWidget->setMinimumSize(QSize(600,380));
    toolBarWidget->setFixedSize(QSize(430,70));//工具栏容器尺寸 430×70
    scanBtn->setText("扫描");
    scanBtn->setFixedSize(QSize(240,36));
    saveBtn->setText("保存");
    saveBtn->setFixedSize(QSize(180,36));

    rightWidget->setFixedWidth(220);//右侧容器固定220px宽
    //rightWidget->setStyleSheet("background:white;");
    QColor bgColor(255,255,255,255);
    //rightWidget->setGeometry(0, 0, 300, 100);
    QPalette palr(rightWidget->palette());
    palr.setColor(QPalette::Background, bgColor);
    rightWidget->setAutoFillBackground(true);
    rightWidget->setPalette(palr);

    QColor bgColor2(247,247,247,255);
    //leftBottomWidget->setGeometry(0, 0, 300, 100);
    QPalette pal2(leftBottomWidget->palette());
    pal2.setColor(QPalette::Background, bgColor2);
    leftBottomWidget->setAutoFillBackground(true);
    leftBottomWidget->setPalette(pal2);

    leftBottomWidget->setFixedHeight(140);//左侧下部容器一定要设置高度，否则视频容器不会铺满整个左侧上部

    //工具栏背景色和边框阴影
    toolBarWidget->setStyleSheet("background:rgba(247,247,247,255);border-radius:18px;");
    QColor shadowColor(0,0,0,25);
    QGraphicsDropShadowEffect *toolBarShadow = new QGraphicsDropShadowEffect (this);
    toolBarShadow->setOffset(-1,5);
    toolBarShadow->setColor(shadowColor);
    toolBarShadow->setBlurRadius(18);
    toolBarWidget->setGraphicsEffect(toolBarShadow);

    QHBoxLayout *timerHLayout = new QHBoxLayout () ;//定时拍布局
    timerWidget = new QWidget () ;//定时拍容器
    timerIconLabel = new QLabel();//定时拍图标
    timerIconLabel->setPixmap(QPixmap(":/img/camera/shottimer.svg"));
    timerLabel = new QLabel();//定时拍秒
    timerLabel->setText(QString(" %1 秒").arg(QString::number(nShotTime)));
    timerHLayout->addStretch();
    timerHLayout->addWidget(timerIconLabel);
    timerHLayout->addWidget(timerLabel);
    timerHLayout->addStretch();
    timerWidget->setLayout(timerHLayout);
    //定时拍背景色和边框
    timerWidget->setStyleSheet("background:rgba(247,247,247,255);border-radius:18px;");
    timerWidget->setFixedSize(100,50);

    videoHLayout->addWidget(timerWidget);

    //图像列表
    imgListView->setSelectionMode(QAbstractItemView::MultiSelection);
    imgListView->setViewMode(QListView::IconMode);
    imgListView->setResizeMode(QListView::Adjust);
    listViewModel = new QStandardItemModel ();
    PicItemDelegate *pItemDelegate = new PicItemDelegate (this);
    imgListView->setItemDelegate(pItemDelegate);
    listViewProxyModel = new QSortFilterProxyModel (imgListView);
    listViewProxyModel->setSourceModel(listViewModel);
    imgListView->setModel(listViewProxyModel);
    imgListView->setEditTriggers(QAbstractItemView::NoEditTriggers);//双击Item屏蔽可编辑
    imgListView->setMovement(QListView::Static);//禁止拖拽Item

    //布局加入控件
    toolBarHLayout->addWidget(oneTOneBtn);
    toolBarHLayout->addWidget(zoomInBtn);
    toolBarHLayout->addWidget(zoomOutBtn);
    toolBarHLayout->addStretch();
    toolBarHLayout->addWidget(rotateLeftBtn);
    toolBarHLayout->addWidget(rotateRightBtn);
    toolBarHLayout->addStretch();
    toolBarHLayout->addWidget(cutBtn);
    leftBottomVLayout->addWidget(toolBarWidget,0,Qt::AlignBottom | Qt::AlignHCenter);
    leftBottomVLayout->addWidget(scanBtn,0,Qt::AlignBottom | Qt::AlignHCenter);
    //previewImageFrame = new QFrame () ;
    //videoHLayout->addWidget(previewImageFrame);
    rightTopVLayout->addWidget(imgListView);
    rightBottomVLayout->addWidget(saveBtn);

    //布局加入子容器
    leftVLayout->addWidget(videoWidget);//,0,Qt::AlignBottom | Qt::AlignHCenter);
    leftVLayout->addWidget(leftBottomWidget);//,0,Qt::AlignBottom | Qt::AlignHCenter);
    rightVLayout->addWidget(rightTopWidget);
    rightVLayout->addWidget(rightBottomWidget,0,Qt::AlignBottom | Qt::AlignHCenter);
    mainVLayout->addWidget(leftWidget);
    mainVLayout->addWidget(rightWidget);
    winVLayout->addWidget(mainWidget);

    //容器设置布局
    toolBarWidget->setLayout(toolBarHLayout);
    leftWidget->setLayout(leftVLayout);
    videoWidget->setLayout(videoHLayout);
    leftBottomWidget->setLayout(leftBottomVLayout);
    rightWidget->setLayout(rightVLayout);
    rightTopWidget->setLayout(rightTopVLayout);
    rightBottomWidget->setLayout(rightBottomVLayout);
    mainWidget->setLayout(mainVLayout);
    ui->centralwidget->setLayout(winVLayout);

    winVLayout->setAlignment(Qt::AlignBottom);

    //容器布局的内外间隙
    leftVLayout->setMargin(0);
    leftBottomVLayout->setSpacing(17);
    rightVLayout->setMargin(0);
    rightVLayout->setSpacing(0);
    mainVLayout->setMargin(0);
    mainVLayout->setSpacing(0);
    winVLayout->setMargin(0);
    winVLayout->setSpacing(0);

    //videoWidget->setStyleSheet("background:blue;");
    //previewImageFrame->setStyleSheet("background:red");
    //leftWidget->setStyleSheet("background:blue;");
    //leftBottomWidget->setStyleSheet("background:yellow;");
}

//信号槽的绑定
void CameraWindow::initConnection()
{
    connect(scanBtn, SIGNAL(clicked()), this, SLOT(slotScanBtnClicked()));//扫描按钮
    connect(oneTOneBtn, SIGNAL(clicked()), this, SLOT(slotOneTOneBtnClicked()));//1:1按钮
    connect(zoomInBtn, SIGNAL(clicked()), this, SLOT(slotZoomInBtnClicked()));//放大按钮
    connect(zoomOutBtn, SIGNAL(clicked()), this, SLOT(slotZoomOutBtnClicked()));//缩小按钮
    connect(rotateLeftBtn, SIGNAL(clicked()), this, SLOT(slotRotateLeftBtnClicked()));//左旋按钮
    connect(rotateRightBtn, SIGNAL(clicked()), this, SLOT(slotRotateRightBtnClicked()));//右旋按钮
    connect(cutBtn, SIGNAL(clicked()), this, SLOT(slotCutBtnClicked()));//裁切按钮
    connect(saveBtn,SIGNAL(clicked()),this,SLOT(slotSaveBtnClicked()));//保存按钮
    connect(imgListView,SIGNAL(pressed(const QModelIndex)),this,SLOT(slotImgListViewPressed(const QModelIndex)));//图像列表点击事件
    connect(imgListView,SIGNAL(doubleClicked(const QModelIndex)),this,SLOT(slotListDoubleClicked(const QModelIndex)));//列表项双击
}

//增加图像Item到列表
void CameraWindow::addItem(QString path)
{
    QStandardItem *pItem = new QStandardItem ();
    PicListItemData itemData;
    itemData.picPath =path;
    QFileInfo *fi = new QFileInfo(path);
    itemData.picName =fi->fileName();
    pItem->setData(QVariant::fromValue(itemData),Qt::UserRole+1);
    listViewModel->appendRow(pItem);
    fileNameList<<path;
    imgListView->scrollToBottom();//自动滚动到最下面
}

//初始化SDK
void CameraWindow::initSDK()
{
    //从传过来的参数中，解析参数值
    QMap<QString,QString>::iterator it;
    for(it = parMap.begin();it!=parMap.end();it++)
    {
        qDebug()<<"camera's par key:"<<it.key()<<",value:"<<it.value();

        if(it.key() == "format_c")
        {
            formatIndex = it.value().toInt();
        }
        if(it.key() == "resolution_c")
        {
            QStringList resolutionList = it.value().split("x");
            if(resolutionList.count() >= 2)
            {
                videoWidth = resolutionList.at(0).toInt();
                videoHeight = resolutionList.at(1).toInt();
            }
        }
    }

    //如果没有分辨率就使用默认分辨率
    if(videoWidth <= 0 || videoHeight <= 0)
    {
       QStringList defaultResolutionList = parMap["DefaultResolution"].split("x") ;
       if(defaultResolutionList.count() >= 2)
       {
           videoWidth = defaultResolutionList.at(0).toInt();
           videoHeight = defaultResolutionList.at(1).toInt();
       }
    }

    Cam_SetCameraFormat(nDeviceIndex,formatIndex);
    Cam_SetCameraResolution(nDeviceIndex,videoWidth,videoHeight);
    qDebug()<<"formatIndex:"<<formatIndex<<",videoWidth:"<<videoWidth<<",videoHeight:"<<videoHeight;
    Cam_CameraCaptureStart(nDeviceIndex,ReceiveImage);
    Cam_SetAutoCrop(isCut);

}

//视频绘制回调
int CameraWindow::ReceiveImage(void *data, int size,int w,int h,int nFormatType)
{
    if(g_image == NULL || g_image->width() != w || g_image->height() != h)
    {
        if(g_image)
        {
            delete g_image;
            g_image = NULL;
        }
        g_image = new QImage(w,h,QImage::Format_RGB888);
    }

    if(data!=NULL)
    {
        MutexDataBuffLock.lock();
        unsigned char * srcBuf = (unsigned char * )data;
        MutexDataBuffLock.unlock();
        if(srcBuf)
        {
            for (int i = 0 ; i < h ; i++)
            {
               for (int j = 0 ; j < w ; j++)
               {
                   g_image->setPixel(j,i,qRgb(* (srcBuf + i*w * 3 + j * 3),* (srcBuf + i*w * 3 + j * 3 + 1),* (srcBuf + i*w * 3 + j * 3 + 2)));
               }
            }
        }
    }

    if(g_CameraWindow)
    {
       g_CameraWindow->update();
    }
    return 0;
}

//窗口渲染
void CameraWindow::paintEvent(QPaintEvent *event)
{
    QPainter painterDraw(this);
    QRect rc;
    if(g_image)
    {
        //根据容器显示区域尺寸和视频实际尺寸，计算视频显示尺寸
        double nVideoWidth = g_image->width();
        double nVideoHeight = g_image->height();
        double nWidgetWidth = videoWidget->width();
        double nWidgetHeight = videoWidget->height();
        double videoWHProportion = nVideoWidth / nVideoHeight;//视频的宽高比例
        if(nVideoRotateAngle == 90 ||  nVideoRotateAngle == 270 )
        {
            videoWHProportion = nVideoHeight / nVideoWidth;//视频的宽高比例
        }

        //视频显示左上角位置
        double nVideoX = 0;
        double nVideoY = videoWidget->pos().ry()+50;
        //qDebug()<<"v:"<<videoWidget->pos().ry()<<",p:"<<previewImageFrame->pos().ry();

        //以容器尺寸为显示边界，按比例缩放视频尺寸
        //容器宽度大于高度，以容器的高度为准，按比例缩放视频尺寸
        if(nWidgetWidth >= nWidgetHeight)
        {
            nVideoHeight = nWidgetHeight + nVideoScaleSize;
            nVideoWidth = nVideoHeight * videoWHProportion ;
        }
        //容器高度大于宽度，以容器的宽度为准，按比例缩放视频尺寸
        else
        {
            nVideoWidth = nWidgetWidth + nVideoScaleSize;
            nVideoHeight = nVideoWidth / videoWHProportion ;
        }

        nVideoX = (nWidgetWidth - nVideoWidth) / 2;
        nVideoY = (nWidgetHeight - nVideoHeight) / 2 + 50;

        rc = QRect(nVideoX, nVideoY, nVideoWidth, nVideoHeight);//视频绘制区域

        //painterDraw.scale(nVideoScaleSize,nVideoScaleSize);//缩放
        QMatrix matrix;
        matrix.rotate(nVideoRotateAngle);//旋转
        //matrix.scale(nVideoScaleSize,nVideoScaleSize);//此缩放是控制原视频的尺寸,显示的控件不会缩放
        //painterDraw.translate(nVideoX + nMoveX,nVideoY + nMoveY);//平移
        *g_image = g_image->transformed(matrix,Qt::FastTransformation);
        QPixmap pix = QPixmap::fromImage(*g_image);
        painterDraw.drawPixmap(rc,pix,QRect(0,0,g_image->width(),g_image->height()));

        /*
        qDebug()<<"视频容器尺寸："<<nWidgetWidth<<":"<<nWidgetHeight
                <<",视频原尺寸："<<g_image->width()<<":"<<g_image->height()
                <<",视频显示尺寸："<<nVideoWidth<<":"<<nVideoHeight
                <<",视频尺寸宽高比："<<videoWHProportion
                <<",视频缩放比："<<nVideoScaleSize
                <<",视频坐标："<<nVideoX<<""<<nVideoY;
        */
         bool isEnableScanBtn = scanBtn->isEnabled();
         if(isEnableScanBtn == false)
         {
             scanBtn->setEnabled(true);//启用扫描按钮
         }

         bool isEnabletoolBar = toolBarWidget->isEnabled();
         if(isEnabletoolBar == false)
         {
             toolBarWidget->setEnabled(true);//启用工具栏
         }

         if(nShotType == 1 && isStartShotTimer == false)
         {
             scanBtn->setText("停止");
             timerWidget->setVisible(true);//显示定时拍
             slotStartShotTime();
             isStartShotTimer = true;
         }
    }
}

//窗口关闭,停止视频
void CameraWindow::closeEvent(QCloseEvent *event)
{
    emit signalWindowClosed();//窗口关闭信号
    Cam_CameraCaptureStop();
    if(nShotType == 1)
    {
        if(shotTimer != NULL)
        {
             if(shotTimer->isActive() == true)
             {
                 shotTimer->stop();
                 shotTimer = NULL;
             }
        }
    }
}

//扫描按钮槽
void CameraWindow::slotScanBtnClicked()
{
    if(scanBtn->text() == "停止" && nShotType == 1 )
    {
        if(shotTimer != NULL && shotTimer->isActive() == true)
        {
            shotTimer->stop();
            shotTimer = NULL;
        }
        scanBtn->setText("扫描");
        timerWidget->setVisible(false);//隐藏定时拍
        return;
    }
    else if(scanBtn->text() == "扫描" && nShotType == 1)
    {
        if(shotTimer != NULL)
        {
            shotTimer->stop();
            shotTimer = NULL;
        }
        slotStartShotTime();
        scanBtn->setText("停止");
        timerWidget->setVisible(true);//显示定时拍
        return;
    }

    shot();//拍摄图像
}

//拍摄图像
void CameraWindow::shot()
{
    //图像文件夹路径
    QString imgFolderPath = GlobalHelper::getScanTempFoler();
    char *folderPath;
    QByteArray qba = imgFolderPath.toLatin1();
    folderPath = qba.data();

    //编号
    int imgIndex = GlobalHelper::readSettingValue("set","imgPreNameIndex").toInt();
    imgIndex++;
    QString strImgIndex = QString("%1").arg(imgIndex,6,10,QLatin1Char('0'));//编号补齐6位，如：000001
    char *cStrImgIndex;
    QByteArray qbaIndex = strImgIndex.toLatin1();
    cStrImgIndex = qbaIndex.data();
    GlobalHelper::writeSettingValue("set","imgPreNameIndex",QString::number(imgIndex));//扫描文件的名称编号

    //后缀名
    QString sImgSuffix= GlobalHelper::readSettingValue("imgEdit","imgFormat").toLower();
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
    qDebug()<<"camera存图路径:"<<part_path;

    //图像处理参数
    bool bIsWater = false;//水印
    if(GlobalHelper::readSettingValue("imgEdit","isWaterMark").toInt() == 0)
    {
        bIsWater = true;
    }
    int nR = GlobalHelper::readSettingValue("imgEdit","waterMarkColor_r").toInt();
    int nG = GlobalHelper::readSettingValue("imgEdit","waterMarkColor_g").toInt();
    int nB = GlobalHelper::readSettingValue("imgEdit","waterMarkColor_b").toInt();
    if(sImgSuffix.contains("jpg") == false)
    {
         nR = GlobalHelper::readSettingValue("imgEdit","waterMarkColor_b").toInt();
         nB = GlobalHelper::readSettingValue("imgEdit","waterMarkColor_r").toInt();
    }

    QString szWaterContent = GlobalHelper::readSettingValue("imgEdit","waterMarkText");
    //文档类型(0=原始文档，1=文档优化，2=彩色优化，3=红印文档优化，4=反色，5=滤红)
    int nDocType = GlobalHelper::readSettingValue("imgEdit","docType").toInt();
    bool bIsTextEn = false;//文档优化
    bool bColorBlance = false;//彩色优化
    bool bRedEn = false;//红印文档
    bool bInverse = false;//反色
    bool bColorDropOut = false;//滤红
    switch (nDocType)
    {
        case 1:
            bIsTextEn = true;
            break;
        case 2:
            bColorBlance = true;
            break;
        case 3:
            bRedEn = true;
            break;
        case 4:
            bInverse = true;
            break;
        case 5:
            bColorDropOut = true;
            break;
    }

    bool bNoise = false;//去噪
    if(GlobalHelper::readSettingValue("imgEdit","isFilterDenoising").toInt() == 0)
    {
        bNoise = true;
    }
    bool bFillBorder = false;//缺角修复
    if(GlobalHelper::readSettingValue("imgEdit","isRepair").toInt() == 0)
    {
        bFillBorder = true;
    }

    //图片的颜色模式
    int nImgColorType = GlobalHelper::readSettingValue("imgEdit","imgType").toInt();

    ImageParam imgparam;
    memset(&imgparam,0,sizeof(imgparam));
    imgparam.bIsAutoCrop = isCut;//自动裁切（调用多图裁切的算法）
    imgparam.nRotate = nVideoRotateAngle;//旋转角度
    imgparam.nColorType = nImgColorType; //颜色类型（彩色、灰度、黑白）
    imgparam.bIsWater = bIsWater;//水印
    imgparam.nR= nR;//水印颜色
    imgparam.nG= nG;
    imgparam.nB= nB;
    imgparam.nFont = 0;//水印字体
    string tmp = szWaterContent.toStdString();//水印内容
    imgparam.szWaterContent = (char*)tmp.c_str();
    imgparam.bIsTextEn = bIsTextEn;//文档优化
    imgparam.bColorBlance = bColorBlance;//彩色优化
    imgparam.bRedEn = bRedEn;//红印文档
    imgparam.bInverse = bInverse;//反色
    imgparam.bColorDropOut = bColorDropOut;//滤红
    imgparam.bNoise = bNoise;//去噪
    imgparam.bFillBorder = bFillBorder;//缺角修复

    Cam_CameraCaptureFile(0,part_path,imgparam);

    addItem(part_path);
}

//1:1按钮
void CameraWindow::slotOneTOneBtnClicked()
{
    nVideoScaleSize = g_image->width();
}

//放大按钮槽
void CameraWindow::slotZoomInBtnClicked()
{
    nVideoScaleSize += 50;
    if(nVideoScaleSize >= VIDEO_MAX_SIZE)
    {
        nVideoScaleSize = VIDEO_MAX_SIZE;
    }
}

//缩小按钮槽
void CameraWindow::slotZoomOutBtnClicked()
{
    nVideoScaleSize -= 50;
    if(nVideoScaleSize <= VIDEO_MIN_SIZE)
    {
        nVideoScaleSize = VIDEO_MIN_SIZE;
    }
}

//左旋按钮槽
void CameraWindow::slotRotateLeftBtnClicked()
{
    nVideoRotateAngle = nVideoRotateAngle - 90;
    if(nVideoRotateAngle < 0)
    {
        nVideoRotateAngle = 270;
    }
}

//右旋按钮槽
void CameraWindow::slotRotateRightBtnClicked()
{
    nVideoRotateAngle = nVideoRotateAngle + 90;
    if(nVideoRotateAngle >= 360)
    {
        nVideoRotateAngle = 0;
    }
}

//裁切按钮槽
void CameraWindow::slotCutBtnClicked()
{
    if(isCut == true)
    {
        isCut = false;
    }
    else
    {
        isCut = true;
    }
    Cam_SetAutoCrop(isCut);
}

//保存按钮槽
void CameraWindow::slotSaveBtnClicked()
{
    QModelIndexList selectItems = imgListView->selectionModel()->selectedIndexes();
    //目标文件夹不存在就创建
    QDir newFolder;
    if(newFolder.exists(GlobalHelper::getScanFolder()) == false)
    {
        newFolder.mkpath(GlobalHelper::getScanFolder());
    }
    foreach(QModelIndex mIndex,selectItems)
    {
        QString oldFilePath = fileNameList.value(mIndex.row());
        QFileInfo *fi = new QFileInfo(oldFilePath);
        QString newFilePath = GlobalHelper::getScanFolder()+"/"+fi->fileName();
        copyFile(oldFilePath,newFilePath);
        delete fi;
    }

    this->close();
}

//拷贝文件
void CameraWindow::copyFile(QString oldFilePath, QString newFilePath)
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
                dialog->setMessage(fi->fileName()+" 文件已存在，您要覆盖文件吗？");
                dialog->addButton("取消",false,DDialog::ButtonType::ButtonNormal);
                dialog->addButton("覆盖",true,DDialog::ButtonType::ButtonWarning);
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
QString CameraWindow::getCopyFileName(QString filePath)
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
           filePath = GlobalHelper::getScanFolder()+"/"+fileName+"(副本)."+fileSuffix;
       }
       else
       {
           filePath = GlobalHelper::getScanFolder()+"/"+fileName+"(副本"+QString::number(index)+")."+fileSuffix;
       }
       qDebug()<<filePath;
       newFile.setFileName(filePath);
       index++;
    }
    delete  fi;
    return filePath;
}

//图片列表点击
void CameraWindow::slotImgListViewPressed(const QModelIndex selectIndex)
{
    QModelIndexList selectItems = imgListView->selectionModel()->selectedIndexes();
    if(selectItems.count() > 0)
    {
        saveBtn->setEnabled(true);//启用保存按钮
    }
    else
    {
        saveBtn->setEnabled(false);//禁用保存按钮
    }
}

//列表项双击
void CameraWindow::slotListDoubleClicked(const QModelIndex index)
{
    QVariant v = index.data(Qt::UserRole+1);
    PicListItemData d = v.value<PicListItemData>();
    QDesktopServices::openUrl(QUrl::fromLocalFile(d.picPath));
}

void CameraWindow::slotStartShotTime()
{
    nShotTime = GlobalHelper::readSettingValue("imgEdit","shotTime").toInt();
    shotTimer = new QTimer ();
    connect(shotTimer,SIGNAL(timeout()),this,SLOT(slotShotTimerUpdate()));
    shotTimer->start(1000);
}

void CameraWindow::timerEvent(QTimerEvent *event)
{
    qDebug()<<"定时拍超时...";
}

void CameraWindow::slotShotTimerUpdate()
{
    if(nShotTime <= 0)
    {
        shot();
        nShotTime = GlobalHelper::readSettingValue("imgEdit","shotTime").toInt();
    }
    timerLabel->setText(QString(" %1 秒").arg(QString::number(nShotTime)));
    nShotTime--;
}

//鼠标滑轮事件(禁用)
void CameraWindow::wheelEvent(QWheelEvent *event)
{
    /*
    nVideoScaleSize += event->delta();
    if(nVideoScaleSize >= VIDEO_MAX_SIZE)
    {
        nVideoScaleSize = VIDEO_MAX_SIZE;
    }
    if(nVideoScaleSize <= VIDEO_MIN_SIZE)
    {
        nVideoScaleSize = VIDEO_MIN_SIZE;
    }
    qDebug()<<"mouse wheel:"<<event->delta()<<",video size:"<<nVideoScaleSize;
    */
}


/*
void CameraWindow::mousePressEvent(QMouseEvent *event)
{
    mousePressedPoint = event->pos();
    //Ctrl + 鼠标左键
    if(QApplication::keyboardModifiers() == Qt::ControlModifier)
    {
       if(event->button() == Qt::LeftButton)
       {
           qDebug()<<"Ctrl + 鼠标左键";
           isMousePressed = true;
           return;
       }
    }
}

void CameraWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(isMousePressed == false)
    {
        qDebug()<<"移动窗口";
        QPoint pos = event->pos();
        int moveX = pos.x() - mousePressedPoint.x();
        int moveY = pos.y() - mousePressedPoint.y();
        nMoveX += moveX;
        nMoveY += moveY;
        this->move(nMoveX,nMoveY);
        mousePressedPoint = pos;
    }
    else
    {
        //Ctrl + 鼠标左键,移动视频
        qDebug()<<"移动视频";
        QPoint pos = event->pos();
        int moveX = pos.x() - mousePressedPoint.x();
        int moveY = pos.y() - mousePressedPoint.y();

        nMoveX += moveX;
        nMoveY += moveY;

        mousePressedPoint = pos;
    }
}

void CameraWindow::mouseReleaseEvent(QMouseEvent *event)
{
    isMousePressed = false;
    //nMoveX = 0;
    //nMoveY = 0;
}
*/
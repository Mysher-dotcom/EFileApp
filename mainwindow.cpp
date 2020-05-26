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
#include "globalhelper.h"
#include <QDebug>
#include <QColorDialog>
#include "setwindow.h"
#include <QMenu>
#include <QStandardPaths>
#include "testctrlwindow.h"
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


#define BUTTON_HEIGHT 30        // 功能按钮高度（扫描 图像设置 文字设置 导出）
#define BUTTON_WIDTH 30         // 功能按钮宽度（扫描 图像设置 文字设置 导出）


MainWindow::MainWindow(QWidget *parent) :
    DMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //检查配置文件是否存在，不存在就创建，并写入初始值
    QFileInfo settingFile(GlobalHelper::getSettingFilePath());
    if(settingFile.exists()==false)
    {
        GlobalHelper::writeSettingFile();
    }
    initUI();
    initConnection();
    refreshData();

}

MainWindow::~MainWindow()
{
    delete ui;
}

//初始化界面
void MainWindow::initUI()
{
    this->resize(810,520);//窗口初始尺寸
    this->setMinimumSize(QSize(640,320));//窗口最小尺寸
    this->titlebar()->setTitle("");//标题栏文字设为空
    setWindowIcon(QIcon(":/img/logo/logo-16.svg"));// 状态栏图标
    this->titlebar()->setIcon(QIcon(":/img/logo/logo-16.svg"));//标题栏图标
    this->setWindowTitle("扫描管理");//开始菜单栏上鼠标悬浮在窗口上显示的名称

    /*
    //主容器设置背景白色
    ui->centralWidget->setGeometry(0, 0, 300, 100);
    QPalette pal(ui->centralWidget->palette());
    pal.setColor(QPalette::Background, Qt::white);
    ui->centralWidget->setAutoFillBackground(true);
    ui->centralWidget->setPalette(pal);
    */

    //扫描按钮
    pbtnScan = new DIconButton (nullptr);
    pbtnScan->setIcon(QIcon(":/img/title/scan.svg"));//图标
    pbtnScan->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));//按钮尺寸
    pbtnScan->setIconSize(QSize(16,16));//图标尺寸
    pbtnScan->setToolTip("扫描");

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
    outputMenu->addAction(QStringLiteral("导出"), this, SLOT(slotTableViewMenuOutputFile()));
    //outputMenu->addAction(QStringLiteral("打印"), this, SLOT(slotTableViewMenuPrintFile()));
    outputMenu->addAction(QStringLiteral("添加到“邮件”"), this, SLOT(slotTableViewMenuEmailFile()));
    pbtnOutput->setMenu(outputMenu);
    QHBoxLayout *outputHLayout = new QHBoxLayout ();//图标居左显示
    QLabel *lbl = new QLabel ();
    QIcon icon(":/img/title/output.svg");
    lbl->setPixmap(icon.pixmap(QSize(16,16)));
    outputHLayout->addWidget(lbl);
    outputHLayout->addStretch();
    pbtnOutput->setLayout(outputHLayout);

    //搜索框
    pSearchEdit = new DSearchEdit (this->titlebar()) ;

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
    pbtnListLayout->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    pbtnListLayout->setStyleSheet("background:transparent;");
    pbtnListLayout->setIconSize(QSize(12,12));
    pbtnListLayout->setCheckable(true);//开启可选中模式状态

    //产生互斥效果
    QButtonGroup *g=new QButtonGroup ();
    g->setExclusive(true);
    g->addButton(pbtnIconLayout);
    g->addButton(pbtnListLayout);

    pbtnIconLayout->setAutoExclusive(true);
    pbtnListLayout->setAutoExclusive(true);
    pbtnIconLayout->setChecked(true);

    this->titlebar()->addWidget(pbtnScan,Qt::AlignLeft);
    this->titlebar()->addWidget(pbtnPicEdit,Qt::AlignLeft);
    this->titlebar()->addWidget(pbtnFontEdit,Qt::AlignLeft);
    this->titlebar()->addWidget(pbtnOutput,Qt::AlignLeft);
    this->titlebar()->addWidget(pSearchEdit,Qt::AlignCenter);
    this->titlebar()->addWidget(pbtnIconLayout,Qt::AlignRight);
    this->titlebar()->addWidget(pbtnListLayout,Qt::AlignRight);

    QMenu *m = new QMenu () ;
    m->addAction(QStringLiteral("设置"), this, SLOT(slotMenuSetButtonClicked()));
    this->titlebar()->setMenu(m);

    winStackedLayout = new QStackedLayout();
    ui->centralWidget->setLayout(winStackedLayout);

    showNoFileUI();
    showFileListUI();
    showFileTableUI();

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
}

//更新UI
void MainWindow::refreshData()
{
    QStringList list = getFileByFolder(GlobalHelper::getScanFolder());
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
        }
        else
        {           
            winStackedLayout->setCurrentIndex(2); //文件信息模式
        }

        int fileListViewItemCount = fileListView->count();
        int fileTableViewItemCount = tableViewModel->rowCount();//fileTableView->model()->rowCount() ;
        for(int i=0;i<fileListViewItemCount;i++)
        {
            fileListView->removeItem(0);
        }
        for(int j=0;j<fileTableViewItemCount;j++)
        {
            //fileTableView->model()->removeRow(0);
            tableViewModel->removeRow(0);
        }

        //加入文件Item
        for(QString str : list)
        {
           addItem(str);
        }
    }
}

//无文件UI
void MainWindow::showNoFileUI()
{
    tipWidget = new QWidget ();//提示容器
    pVLayout = new QVBoxLayout () ;//提示布局
    pNoPicTip1 = new DLabel () ;//无图提示1
    pNoPicTip2 = new DLabel () ;//无图提示2
    pNoPicTip1->setText("暂无扫描的文件");
    pNoPicTip1->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;color:rgba(85,85,85,0.4);font-size:17px");
    pNoPicTip2->setText("可点击扫描按钮添加扫描文件");
    pNoPicTip2->setStyleSheet("font-family:SourceHanSansSC-Bold,sourceHanSansSC;font-weight:bold;color:rgba(85,85,85,0.4);font-size:14px");
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
    PicItemDelegate *pItemDelegate=new PicItemDelegate (this);
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
    listViewMenu->addAction(QStringLiteral("打开"), this, SLOT(slotTableViewMenuOpenFile()));
    //listViewMenu->addAction(QStringLiteral("编辑"), this, SLOT(slotTableViewMenuEditFile()));
    listViewMenu->addAction(QStringLiteral("导出"), this, SLOT(slotTableViewMenuOutputFile()));
    listViewMenu->addSeparator();
    //listViewMenu->addAction(QStringLiteral("打印"), this, SLOT(slotTableViewMenuPrintFile()));
    listViewMenu->addAction(QStringLiteral("添加到“邮件”"), this, SLOT(slotTableViewMenuEmailFile()));
    listViewMenu->addSeparator();
    listViewMenu->addAction(QStringLiteral("删除"), this, SLOT(slotTableViewMenuDelFile()));

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
    tableViewModel->setHeaderData(0, Qt::Horizontal, tr("名称"));
    tableViewModel->setHeaderData(1, Qt::Horizontal, tr("修改时间"));
    tableViewModel->setHeaderData(2, Qt::Horizontal, tr("类型"));
    tableViewModel->setHeaderData(3, Qt::Horizontal, tr("大小"));

    //设置列宽
    fileTableView->setColumnWidth(0,(this->width()-300)/2);
    fileTableView->setColumnWidth(1,(this->width()-300)/2);
    fileTableView->setColumnWidth(2,150);
    fileTableView->setColumnWidth(3,150);

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
    tableViewMenu->addAction(QStringLiteral("打开"), this, SLOT(slotTableViewMenuOpenFile()));
    //tableViewMenu->addAction(QStringLiteral("编辑"), this, SLOT(slotTableViewMenuEditFile()));
    tableViewMenu->addAction(QStringLiteral("导出"), this, SLOT(slotTableViewMenuOutputFile()));
    tableViewMenu->addSeparator();
    //tableViewMenu->addAction(QStringLiteral("打印"), this, SLOT(slotTableViewMenuPrintFile()));
    tableViewMenu->addAction(QStringLiteral("添加到“邮件”"), this, SLOT(slotTableViewMenuEmailFile()));
    tableViewMenu->addSeparator();
    tableViewMenu->addAction(QStringLiteral("删除"), this, SLOT(slotTableViewMenuDelFile()));

    //右键菜单响应
    connect(fileTableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotTableViewContextMenu(QPoint)));

    //列表项双击
    connect(fileTableView,SIGNAL(doubleClicked(const QModelIndex)),this,SLOT(slotListDoubleClicked(const QModelIndex)));
    winStackedLayout->addWidget(fileTableView);
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
    pItem->setToolTip(fi->fileName());

    //***文件信息列表***
    // 获取系统图标、文件类型、最后修改时间
    QFileIconProvider provider;
    QIcon icon = provider.icon(*fi);//系统图标
    QDateTime updateTime=fi->metadataChangeTime();//最后修改时间
    QString strType = provider.type(*fi);//文件类型

    int rowCount = tableViewModel->rowCount();//fileTableView->model()->rowCount() ;//获取列表行数
    //加入列表中
    QStandardItem *pTableItem = new QStandardItem ();
    pTableItem->setData(fi->fileName(),Qt::UserRole+2);//存储文件名用于检索
    tableViewModel->setItem(rowCount, 0, new QStandardItem(icon,fi->fileName()));//图标+文件名
    tableViewModel->setItem(rowCount, 1, new QStandardItem(updateTime.toString()));//最后修改时间
    tableViewModel->setItem(rowCount, 2, new QStandardItem(strType));//文件类型
    tableViewModel->setItem(rowCount, 3, new QStandardItem(readableFilesize(fi->size())));//文件尺寸

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
    //列表根据窗口尺寸改变
    fileTableView->setFixedWidth(this->width());
    fileTableView->setColumnWidth(0,(this->width()-300)/2);
    fileTableView->setColumnWidth(1,(this->width()-300)/2);
    fileTableView->setColumnWidth(2,150);
    fileTableView->setColumnWidth(3,150);
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
    smWindow = new ScanManagerWindow (this);
    smWindow->setAttribute(Qt::WA_ShowModal,true);//模态窗口
    smWindow->show();
    //屏幕中间显示
    smWindow->move ((QApplication::desktop()->width() - smWindow->width())/2,
                    (QApplication::desktop()->height() - smWindow->height())/2);


}

//图片编辑按钮
void MainWindow::slotPicEditButtonClicked()
{
    //TestCtrlWindow *tcw = new TestCtrlWindow (this);
    //tcw->show();
    //return;

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
    //pbtnIconLayout->setChecked(true);
    //pbtnListLayout->setChecked(false);
    GlobalHelper::writeSettingValue("set","iconList","0");//修改配置文件键值

    refreshData();

}

//列表显示
void MainWindow::slotListLayoutButtonClicked()
{
    pSearchEdit->clear();
    //pbtnIconLayout->setChecked(false);
    //pbtnListLayout->setChecked(true);
    GlobalHelper::writeSettingValue("set","iconList","1");//修改配置文件键值

    refreshData();
}

//设置菜单
void MainWindow::slotMenuSetButtonClicked()
{
    SetWindow *sw = new  SetWindow (this);
    sw->setAttribute(Qt::WA_ShowModal,true);//模态窗口
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
    DDialog *dialog = new DDialog ();
    dialog->setIcon(QIcon(":/img/dialogWarnIcon.svg"));
    dialog->setTitle("通知");
    dialog->setMessage("图像编辑功能开发中...");
    dialog->addButton("确定",true);
    dialog->exec();
}

//右键导出
void MainWindow::slotTableViewMenuOutputFile()
{
    QStringList list = getListSelectedFile();
    if(list.size() <= 0)
    {
        return;
    }
    QString folder = DFileDialog::getExistingDirectory(this,"请选择导出位置");
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
    dialog->setMessage("您确定要彻底删除文件吗？");
    dialog->addButton("取消",false,DDialog::ButtonType::ButtonNormal);
    dialog->addButton("删除",true,DDialog::ButtonType::ButtonWarning);
    if(dialog->exec()==DDialog::Accepted)//用户点击了确定按钮
    {
        for(int i=0;i<list.size();i++)
        {
            QFile::remove(list.at(i));
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


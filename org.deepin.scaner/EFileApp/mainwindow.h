#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "scanmanagerwindow.h"

#include <DMainWindow>
#include "DTitlebar"
#include "DIconButton"
#include "dlabel.h"
#include "dstyle.h"
#include "dtextedit.h"
#include <dsearchedit.h>
#include "listview/picitemdelegate.h"
#include "listview/listviewitemdata.h"
#include <QButtonGroup>
#include <DTableView>
#include <QStackedLayout>
#include <DMenu>
#include <DComboBox>
#include <DTreeView>
#include "thread/recognizethread.h"
#include <QThread>
#include <DSpinner>
#include "thread/uploadfilethread.h"
#include "helper/webhelper.h"
#include "thread/getscannerinfothread.h"
#include "thread/getcamerainfothread.h"
#include "Jpeg.h"
DWIDGET_USE_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public DMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void refreshData(bool isTreeClick = false, QString folderPath = nullptr);//更新文件列表
    void resizeEvent(QResizeEvent *event);//窗口尺寸改变

private:
    Ui::MainWindow *ui;

    //函数
    void initUI();//初始化界面
    void initConnection();//按钮信号槽连接
    void showNoFileUI();//显示无文件UI
    void showFileListUI();//显示缩略图列表
    void showFileTableUI();//显示文件信息列表
    void showTreeViewUI(bool);//显示电子档案模式
    void addItem(QString path);//增加文件Item
    void getAllFolder(QString folderPath,QStringList &list);//获取文件夹包含子文件夹下所有的文件夹
    QStringList getFileByFolder(QString folderPath);//从文件夹获取所有文件
    QString readableFilesize( quint64 filesize);//kb单位转换
    QStringList getListSelectedFile();//获取列表选中的文件
    void setFileTableSize();//设置文件信息列表尺寸
    bool writeClassShowFile();//写文件分类配置文件,主窗根据此配置文件显示树
    void startRecognizeThread();//开启识别线程
    void closeRecognizeThread();//关闭识别线程
    void showTreeViewModel(QString folderPath,QStandardItem* item = NULL);//展示Tree节点
    void showMaskWidgetUI();//遮罩窗口
    void uploadFile(QStringList list);//上传文件
    void openCameraThread();//开启拍摄仪线程
    void openScannerThread();//开启SANE线程
    void getDevicePar();//获取设备参数


    //变量
    QString preCode;//上一次的条码
    QStringList preCodeInfo;//上一次的条码信息
    RecognizeThread *recognizeThread;//识别线程
    QThread *_recognizeThread;//识别线程
    UploadFileThread *uploadThread;//上传线程
    QThread *_uploadThread;//上传线程

    int deviceCount;//设备数量
    int scannerDeviceCount;//扫描仪设备数量
    GetScannerInfoThread *getScannerInfoThread;//获取扫描仪信息
    QThread *_getScannerInfoThread;//SANE扫描仪线程
    GetCameraInfoThread *getCameraInfoThread;//获取拍摄仪信息
    QThread *_getCameraInfoThread;//拍摄仪线程

    CJpeg m_jpg;//CJPEG对象


    //控件
    ScanManagerWindow *smWindow;//扫描管理窗口
    DIconButton *pbtnScan;//扫描按钮
    DIconButton *pbtnPicEdit ;//图片编辑按钮
    DIconButton *pbtnFontEdit ;//文字编辑按钮
    QPushButton *pbtnOutput ;//导出按钮
    DSearchEdit *pSearchEdit;//搜索框
    DIconButton *pbtnIconLayout ; //缩略图模式按钮
    DIconButton *pbtnListLayout ;//列表模式按钮
    QHBoxLayout *mainHLayout;//主布局，分左右结构;左侧为tree，分类显示时设置宽度>0;右侧为缩略图、详细信息列表
    DTreeView *treeView;//左侧tree
    QStandardItemModel *treeViewModel;//左侧tree Item数据
    QWidget *rightWidget;//右侧容器
    QStackedLayout *winStackedLayout;//右侧容器布局
    QWidget *tipWidget;//提示容器
    QVBoxLayout *pVLayout;//提示布局
    DLabel *pNoPicTip1;//无图提示1
    DLabel *pNoPicTip2;//无图提示2
    DListView *fileListView;//缩略图列表控件
    QStandardItemModel *listViewModel;//缩略图Item数据
    QSortFilterProxyModel *listViewProxyModel;//代理数据，用于缩略图列表的筛选和排序
    DTableView *fileTableView;//详细信息列表控件
    QStandardItemModel *tableViewModel;//详细信息Item数据
    QSortFilterProxyModel *tableViewProxyModel;//代理数据，用于详细信息列表的筛选和排序
    DMenu *listViewMenu;//缩略图列表右键菜单
    DMenu *tableViewMenu;//详细信息列表右键菜单
    DMenu *outputMenu;//导出按钮菜单
    QWidget *maskWidget;//遮罩窗口
    DSpinner *maskSpinner;//遮罩窗口进度动画
    QLabel *maskLabel;//遮罩窗口显示文本
    QPushButton *uploadBtn;//上传文件按钮

signals:
    //信号
    void signalThreadOver();//線程結束信号

private slots:
    //槽
    void slotScanButtonClicked();//扫描按钮
    void slotPicEditButtonClicked();//图片编辑按钮
    void slotFontEditButtonClicked();//文字编辑按钮
    void slotOutputButtonClicked();//导出按钮
    void slotIconLayoutButtonClicked();//缩略图显示
    void slotListLayoutButtonClicked();//列表显示
    void slotMenuSetButtonClicked();//设置菜单
    void slotSearchTextChange(QString);//搜索框文本改变
    void slotListViewContextMenu(QPoint pos);//缩略图列表右键菜单响应函数
    void slotTableViewContextMenu(QPoint pos);//信息列表右键菜单响应函数
    void slotTableViewMenuOpenFile();//右键打开
    void slotTableViewMenuEditFile();//右键编辑
    void slotTableViewMenuOutputFile();//右键导出
    void slotTableViewMenuOutputPDFFile();//右键合并PDF
    void slotTableViewMenuPrintFile();//右键打印
    void slotTableViewMenuEmailFile();//右键邮件
    void slotTableViewMenuDelFile();//右键删除
    void slotRefreshData();//刷新数据
    void slotListDoubleClicked(const QModelIndex);//列表项双击
    void slotGetCodeInfo(QString filePath,QString code,QStringList codeInfo);//获取条码信息(图片路径，条码号，条码信息)
    void slotRecognizeOver();//识别结束
    void slotTreeViewClick(const QModelIndex &index);//树节点单击
    void slotUploadBtnClick();//上传按钮
    void slotSingleUploadOver(QString filepath,bool uploadResult);//单个文件上传完成
    void slotUploadOver();//上传结束
    void slotCameraInfo(QVariant qv, const QString &str);//记录拍摄仪设备信息
    void slotScannerInfo(QVariant qv, const QString &str);//记录扫描仪设备信息
    void slotCameraParInfo(QVariant qv,  const QString &str);//记录拍摄仪设备参数信息
    void slotScannerParInfo(QVariant qv, const QString &str);//记录扫描仪设备参数信息
    void slotCloseCameraThread();//关闭拍摄仪线程
    void slotCloseScannerThread();//关闭SANE线程    
    void slotNoScanner();//无扫描仪设备
    void slotDeviceParOver();//设备参数停止线程

};

#endif // MAINWINDOW_H

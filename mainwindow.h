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

    void refreshData();//更新文件列表
    void resizeEvent(QResizeEvent *event);//窗口尺寸改变

private:
    Ui::MainWindow *ui;

    //函数
    void initUI();//初始化界面
    void initConnection();//按钮信号槽连接
    void showNoFileUI();//显示无文件UI
    void showFileListUI();//显示缩略图列表
    void showFileTableUI();//显示文件信息列表
    void addItem(QString path);//增加文件Item
    QStringList getFileByFolder(QString folderPath);//从文件夹获取所有文件
    QString readableFilesize( quint64 filesize);//kb单位转换
    QStringList getListSelectedFile();//获取列表选中的文件

    //控件
    ScanManagerWindow *smWindow;//扫描管理窗口
    DIconButton *pbtnScan;//扫描按钮
    DIconButton *pbtnPicEdit ;//图片编辑按钮
    DIconButton *pbtnFontEdit ;//文字编辑按钮
    QPushButton *pbtnOutput ;//导出按钮
    DSearchEdit *pSearchEdit;//搜索框
    DIconButton *pbtnIconLayout ; //缩略图模式按钮
    DIconButton *pbtnListLayout ;//列表模式按钮
    QStackedLayout *winStackedLayout;//窗口容器布局
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

signals:
    //信号

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
    void slotTableViewMenuPrintFile();//右键打印
    void slotTableViewMenuEmailFile();//右键邮件
    void slotTableViewMenuDelFile();//右键删除
    void slotRefreshData();//刷新数据
    void slotListDoubleClicked(const QModelIndex);//列表项双击


};

#endif // MAINWINDOW_H

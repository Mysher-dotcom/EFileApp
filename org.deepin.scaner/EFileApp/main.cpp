#include "mainwindow.h"
#include <QApplication>
#include <DApplication>
#include <DWidgetUtil>

int main(int argc, char *argv[])
{
    //QApplication a(argc, argv);

    DApplication::loadDXcbPlugin();  //让bar处在标题栏中
    DApplication a(argc, argv);

    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
    a.loadTranslator();
    a.setOrganizationName("扫描管理");
    a.setApplicationVersion(DApplication::buildVersion("5.1.0.3-2"));
   // a.setApplicationAcknowledgementPage("http://www.viisan.com");
    a.setProductIcon(QIcon(":/img/logo/logo-16.svg"));
    a.setProductName("扫描管理");
    a.setApplicationName("扫描管理"); //只有在这儿修改窗口标题才有效
    a.setApplicationDescription("扫描管理是一款便捷的扫描工具，支持多款扫描设备。");

    MainWindow w;
    w.show();

    Dtk::Widget::moveToCenter(&w);//让打开时界面显示在正中

    return a.exec();
}

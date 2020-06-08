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
    a.setOrganizationName("viisan");
    a.setApplicationVersion(DApplication::buildVersion("1.0.0"));
    a.setApplicationAcknowledgementPage("http://www.viisan.com");
    a.setProductIcon(QIcon(":/img/logo/logo-16.svg"));
    a.setProductName("Viisan Scan Manager");
    a.setApplicationName("Viisan Scan Manager"); //只有在这儿修改窗口标题才有效

    MainWindow w;
    w.show();

    Dtk::Widget::moveToCenter(&w);//让打开时界面显示在正中

    return a.exec();
}

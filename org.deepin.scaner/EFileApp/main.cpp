#include "mainwindow.h"
#include <QApplication>
#include <DApplication>
#include <DWidgetUtil>
#include "scanmanagerwindow.h"
#include <QTranslator>

int main(int argc, char *argv[])
{
    //QApplication a(argc, argv);

    DApplication::loadDXcbPlugin();  //让bar处在标题栏中
    DApplication a(argc, argv);

    /*
     * QLocale localLan = QLocale::system();
     * 中文：QLocale(Chinese, Simplified Han, China)
     */
    QTranslator translator;
    QLocale locale;
    if(locale.language() == QLocale::Chinese )
    {
       qDebug() << "中文系统";
       translator.load("zh_cn.qm");
    }
    else //if( locale.language() == QLocale::English )
    {
       qDebug() << "English system" ;
       translator.load("en_us.qm");
    }
    qApp->installTranslator(&translator); //安装翻译器


    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
    a.loadTranslator();
    a.setApplicationVersion(DApplication::buildVersion("5.1.0.4-2"));
    a.setProductIcon(QIcon(":/img/logo/logo-16.svg"));
    a.setOrganizationName(QObject::tr("Scan Assistant"));
    a.setProductName(QObject::tr("Scan Assistant"));
    a.setApplicationName(QObject::tr("Scan Assistant")); //只有在这儿修改窗口标题才有效
    a.setApplicationDescription(QObject::tr("Scan Assistant is a scanner management tool that supports a variety of scanning devices."));


    MainWindow w;
    //ScanManagerWindow w;
    w.show();

    Dtk::Widget::moveToCenter(&w);//让打开时界面显示在正中

    return a.exec();
}

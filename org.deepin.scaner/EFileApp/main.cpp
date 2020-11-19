#include "mainwindow.h"
#include <QApplication>
#include <DApplication>
#include <DWidgetUtil>
#include "scanmanagerwindow.h"
#include <QTranslator>
#include "helper/loggerhelper.h"
#include <QLocale>
#include "helper/globalhelper.h"

int main(int argc, char *argv[])
{
    //QApplication a(argc, argv);
    qInstallMessageHandler(myMsgOutput);//记录调试信息到文件中

    DApplication::loadDXcbPlugin();  //让bar处在标题栏中
    DApplication a(argc, argv);

    QString appPath = QCoreApplication::applicationDirPath();
    /*
     * QLocale localLan = QLocale::system();
     * 中文：QLocale(Chinese, Simplified Han, China)
     */
    QTranslator translator;
    QLocale locale;
    qDebug()<<"current system's language:"<<locale.language()<<",name:"<<locale.name();
    if(locale.name() == "zh_CN" )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_zh_CN.qm"));//中文（中华人民共和国）
    else if(locale.name() == "zh_HK" )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_zh_HK.qm"));//中文（香港特区）
    else if(locale.name() == "zh_TW" )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_zh_TW.qm"));//中文（台湾）
    else if(locale.name() == "en_us" )//else if(locale.language() == QLocale::English )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_en_US.qm"));//英文
    else if(locale.name() == "am_et" )//else if(locale.language() == QLocale::Armenian )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_am_ET.qm"));//亚美尼亚
    else if(locale.name() == "es_AR" )//else if(locale.language() == QLocale::Argentina )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_ar.qm"));//阿根廷
    else if(locale.language() == QLocale::Bulgarian )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_bg.qm"));//保加利亚
    else if(locale.name() == "en_CA" )//else if(locale.language() == QLocale::CanadianAboriginalScript)
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_ca.qm"));//加拿大
    else if(locale.language() == QLocale::Czech )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_cs.qm"));//捷克
    else if(locale.language() == QLocale::Danish )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_da.qm"));// 丹麦
    else if(locale.language() == QLocale::German )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_de.qm"));//德国
    else if(locale.language() == QLocale::Greek )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_el.qm"));//希腊
    else if(locale.language() == QLocale::Spanish )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_es.qm"));//西班牙
    else if(locale.language() == QLocale::Persian )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_fa.qm"));//波斯
    else if(locale.language() == QLocale::Finnish )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_fi.qm"));//芬兰
    else if(locale.language() == QLocale::French )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_fr.qm"));//法国
    else if(locale.language() == QLocale::Hindi )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_hi_IN.qm"));//印度
    else if(locale.language() == QLocale::Croatian )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_hr.qm"));//克罗地亚
    else if(locale.language() == QLocale::Hungarian )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_hu.qm"));//匈牙利
    else if(locale.language() == QLocale::Indonesian )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_id.qm"));//印度尼西亚
    else if(locale.language() == QLocale::Italian )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_it.qm"));//意大利
    else if(locale.language() == QLocale::Japanese )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_ja.qm"));//日本
    else if(locale.language() == QLocale::Korean )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_ko.qm"));//韩国
    else if(locale.language() == QLocale::Lithuanian )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_lt.qm"));//立陶宛
    else if(locale.name() == "ms" )//else if(locale.language() == QLocale::Montserrat )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_ms.qm"));//蒙特塞拉特岛
    else if(locale.language() == QLocale::Dutch )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_nl.qm"));//荷兰
    else if(locale.language() == QLocale::Polish )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_pl.qm"));//波兰
    else if(locale.language() == QLocale::Portuguese )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_pt.qm"));//葡萄牙语（葡萄牙）
    else if(locale.language() == QLocale::Portuguese )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_pt_BR.qm"));//葡萄牙语（巴西）
    else if(locale.language() == QLocale::Romanian )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_ro.qm"));//罗马尼亚
    else if(locale.language() == QLocale::Russian )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_ru.qm"));//俄罗斯
    else if(locale.language() == QLocale::Slovenian )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_sk.qm"));//斯洛伐克
    else if(locale.language() == QLocale::Albanian )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_sq.qm"));//阿尔巴尼亚语
    else if(locale.name() == "sr" )//else if(locale.language() == QLocale::Suriname )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_sr.qm"));//苏里南
    else if(locale.name() == "sv" )//else if(locale.language() == QLocale::ElSalvador )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_sv.qm"));//萨尔瓦多
    else if(locale.language() == QLocale::Turkish )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_tr.qm"));//土耳其
    else if(locale.name() == "ug" )//else if(locale.language() == QLocale::Uganda)
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_ug.qm"));//乌干达
    else if(locale.language() == QLocale::Ukrainian )
        translator.load(QString("%1/%2").arg(appPath).arg("translations/scan-assistantts_uk.qm"));//乌克兰

    qApp->installTranslator(&translator); //安装翻译器

    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
    a.loadTranslator();
    a.setApplicationVersion(DApplication::buildVersion(GlobalHelper::getVersion()));
    a.setProductIcon(QIcon(":/img/logo/logo-16.svg"));
    a.setOrganizationName(QObject::tr("Scan Assistant"));
    a.setProductName(QObject::tr("Scan Assistant"));
    a.setApplicationName(QObject::tr("Scan Assistant")); //只有在这儿修改窗口标题才有效
    a.setApplicationDescription(QObject::tr("Scan Assistant is a scanner management tool that supports a variety of scanning devices."));



    MainWindow w;
    w.show();

    Dtk::Widget::moveToCenter(&w);//让打开时界面显示在正中

    return a.exec();
}

/********************************************************************************
** Form generated from reading UI file 'scanmanagerwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCANMANAGERWINDOW_H
#define UI_SCANMANAGERWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ScanManagerWindow
{
public:
    QWidget *centralwidget;

    void setupUi(QMainWindow *ScanManagerWindow)
    {
        if (ScanManagerWindow->objectName().isEmpty())
            ScanManagerWindow->setObjectName(QStringLiteral("ScanManagerWindow"));
        ScanManagerWindow->resize(800, 600);
        centralwidget = new QWidget(ScanManagerWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        ScanManagerWindow->setCentralWidget(centralwidget);

        retranslateUi(ScanManagerWindow);

        QMetaObject::connectSlotsByName(ScanManagerWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ScanManagerWindow)
    {
        ScanManagerWindow->setWindowTitle(QApplication::translate("ScanManagerWindow", "MainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ScanManagerWindow: public Ui_ScanManagerWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCANMANAGERWINDOW_H

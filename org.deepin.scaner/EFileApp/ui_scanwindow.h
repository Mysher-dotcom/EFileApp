/********************************************************************************
** Form generated from reading UI file 'scanwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCANWINDOW_H
#define UI_SCANWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ScanWindow
{
public:
    QWidget *centralwidget;

    void setupUi(QMainWindow *ScanWindow)
    {
        if (ScanWindow->objectName().isEmpty())
            ScanWindow->setObjectName(QStringLiteral("ScanWindow"));
        ScanWindow->resize(800, 600);
        centralwidget = new QWidget(ScanWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        ScanWindow->setCentralWidget(centralwidget);

        retranslateUi(ScanWindow);

        QMetaObject::connectSlotsByName(ScanWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ScanWindow)
    {
        ScanWindow->setWindowTitle(QApplication::translate("ScanWindow", "MainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ScanWindow: public Ui_ScanWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCANWINDOW_H

/********************************************************************************
** Form generated from reading UI file 'testctrlwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TESTCTRLWINDOW_H
#define UI_TESTCTRLWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TestCtrlWindow
{
public:
    QWidget *centralwidget;

    void setupUi(QMainWindow *TestCtrlWindow)
    {
        if (TestCtrlWindow->objectName().isEmpty())
            TestCtrlWindow->setObjectName(QStringLiteral("TestCtrlWindow"));
        TestCtrlWindow->resize(800, 600);
        centralwidget = new QWidget(TestCtrlWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        TestCtrlWindow->setCentralWidget(centralwidget);

        retranslateUi(TestCtrlWindow);

        QMetaObject::connectSlotsByName(TestCtrlWindow);
    } // setupUi

    void retranslateUi(QMainWindow *TestCtrlWindow)
    {
        TestCtrlWindow->setWindowTitle(QApplication::translate("TestCtrlWindow", "MainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TestCtrlWindow: public Ui_TestCtrlWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TESTCTRLWINDOW_H

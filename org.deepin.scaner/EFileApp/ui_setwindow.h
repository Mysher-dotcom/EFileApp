/********************************************************************************
** Form generated from reading UI file 'setwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETWINDOW_H
#define UI_SETWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SetWindow
{
public:
    QWidget *centralwidget;

    void setupUi(QMainWindow *SetWindow)
    {
        if (SetWindow->objectName().isEmpty())
            SetWindow->setObjectName(QStringLiteral("SetWindow"));
        SetWindow->resize(800, 600);
        centralwidget = new QWidget(SetWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        SetWindow->setCentralWidget(centralwidget);

        retranslateUi(SetWindow);

        QMetaObject::connectSlotsByName(SetWindow);
    } // setupUi

    void retranslateUi(QMainWindow *SetWindow)
    {
        SetWindow->setWindowTitle(QApplication::translate("SetWindow", "MainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SetWindow: public Ui_SetWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETWINDOW_H

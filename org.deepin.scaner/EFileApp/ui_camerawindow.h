/********************************************************************************
** Form generated from reading UI file 'camerawindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CAMERAWINDOW_H
#define UI_CAMERAWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CameraWindow
{
public:
    QWidget *centralwidget;

    void setupUi(QMainWindow *CameraWindow)
    {
        if (CameraWindow->objectName().isEmpty())
            CameraWindow->setObjectName(QStringLiteral("CameraWindow"));
        CameraWindow->resize(800, 600);
        centralwidget = new QWidget(CameraWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        CameraWindow->setCentralWidget(centralwidget);

        retranslateUi(CameraWindow);

        QMetaObject::connectSlotsByName(CameraWindow);
    } // setupUi

    void retranslateUi(QMainWindow *CameraWindow)
    {
        CameraWindow->setWindowTitle(QApplication::translate("CameraWindow", "MainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CameraWindow: public Ui_CameraWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CAMERAWINDOW_H

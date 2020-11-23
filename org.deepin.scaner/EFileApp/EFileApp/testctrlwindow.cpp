#include "testctrlwindow.h"
#include "ui_testctrlwindow.h"

#include <QHBoxLayout>
#include <DFloatingButton>
#include <dfilechooseredit.h>
#include <dswitchlineexpand.h>
#include <DTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QFileIconProvider>
#include <QDateTime>
#include <qmath.h>

#include <QPrinter>
#include <QPrintPreviewDialog>



TestCtrlWindow::TestCtrlWindow(QWidget *parent) :
    DMainWindow(parent),
    ui(new Ui::TestCtrlWindow)
{
    ui->setupUi(this);
    initUI();
}

TestCtrlWindow::~TestCtrlWindow()
{
    delete ui;
}

void TestCtrlWindow::initUI()
{
    this->resize(500,520);//窗口初始尺寸

    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog m_printPreDlg;
    m_printPreDlg.exec();
}

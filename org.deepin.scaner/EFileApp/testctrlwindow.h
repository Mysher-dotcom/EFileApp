#ifndef TESTCTRLWINDOW_H
#define TESTCTRLWINDOW_H

#include <DMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QComboBox>

DWIDGET_USE_NAMESPACE

namespace Ui {
class TestCtrlWindow;
}

class TestCtrlWindow : public DMainWindow
{
    Q_OBJECT

public:
    explicit TestCtrlWindow(QWidget *parent = nullptr);
    ~TestCtrlWindow();

    void initUI();

private:
    Ui::TestCtrlWindow *ui;


private slots:

};

#endif // TESTCTRLWINDOW_H

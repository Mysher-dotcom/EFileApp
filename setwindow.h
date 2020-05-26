#ifndef SETWINDOW_H
#define SETWINDOW_H

#include <DMainWindow>
#include "DTitlebar"
#include <DIconButton>
#include <DCheckBox>
#include <DTextEdit>
#include <DRadioButton>
#include <QPushButton>
#include <DSuggestButton>
#include <DSwitchButton>
#include <QLabel>

DWIDGET_USE_NAMESPACE

namespace Ui {
class SetWindow;
}

class SetWindow : public DMainWindow
{
    Q_OBJECT

public:
    explicit SetWindow(QWidget *parent = nullptr);
    ~SetWindow();
    void closeEvent(QCloseEvent *event);

private:
    Ui::SetWindow *ui;

    DRadioButton *rbColor;
    DCheckBox *ckbColor;
    DTextEdit *txtColor;
    DIconButton *btnColor;
    QPushButton *btnGetPar;

    DTextEdit *savePathText;//缓存路径文本框
    DSuggestButton *selectSavePathBtn;//选择缓存路径按钮
    DSwitchButton *fileOverWarnSBtn;//覆盖时警告开关按钮
    DSwitchButton *noDeviceWarnSBtn;//无设备警告开关按钮
    DSwitchButton *finishBeepSBtn;//完成时提示音开关按钮
    DSwitchButton *finishExitSBtn;//完成时退出开关按钮

    void initUI();
    void setParTitleLabelStyle(QLabel*,int width);//参数名称label设置样式

signals:
    void signalWindowClosed();//窗口关闭信号

private slots:
    void onBtnColorClicked();//按钮点击
    void onBtnSelectSavePathClicked();//选择缓存路径
    void onSBtnFileOverCheckedChanged(bool);//覆盖时警告开关改变
    void onSBtnNoDeviceCheckedChanged(bool);//无设备警告开关改变
    void onSBtnFinishBeepCheckedChanged(bool);//完成时提示音开关改变
    void onSBtnFinishExitCheckedChanged(bool);//完成时退出开关改变

};

#endif // SETWINDOW_H

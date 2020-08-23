#ifndef PROGRESSBARWINDOW_H
#define PROGRESSBARWINDOW_H

#include <DMainWindow>
#include "DTitlebar"
#include <DProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

namespace Ui {
class ProgressBarWindow;
}

class ProgressBarWindow : public DMainWindow
{
    Q_OBJECT

public:
    explicit ProgressBarWindow(int minValue,int maxValue,QString tip,QWidget *parent = nullptr);
    ~ProgressBarWindow();
    void closeEvent(QCloseEvent *event);

    void initUI();
    void setProgressBarValue(int);

    QLabel *tipLabel;
    DProgressBar *progressBar;
    QPushButton *cancelButton;

    int minimum;
    int maximum;
    int value;
    QString tipText;


private:
    Ui::ProgressBarWindow *ui;

signals:
    void signalWindowClosed();//窗口关闭信号

private slots:
    void slotCancelClicked();
};

#endif // PROGRESSBARWINDOW_H

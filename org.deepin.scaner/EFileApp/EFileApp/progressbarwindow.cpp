#include "progressbarwindow.h"
#include "ui_progressbarwindow.h"

ProgressBarWindow::ProgressBarWindow(int minValue,int maxValue,QString tip,QWidget *parent) :
    DMainWindow(parent),
    ui(new Ui::ProgressBarWindow)
{
    ui->setupUi(this);
    minimum = minValue;
    maximum = maxValue;
    tipText = tip;
    initUI();
}

ProgressBarWindow::~ProgressBarWindow()
{
    delete ui;
}

void ProgressBarWindow::closeEvent(QCloseEvent *event)
{
    emit signalWindowClosed();//窗口关闭信号
}

void ProgressBarWindow::initUI()
{
    //*窗口基本属性设置*
    this->resize(422,202);//窗口初始尺寸 460,380
    this->titlebar()->setTitle("");//标题栏文字设为空
    setWindowIcon(QIcon(":/img/logo/logo-16.svg"));// 状态栏图标
    this->titlebar()->setIcon(QIcon(":/img/logo/logo-16.svg"));//标题栏图标
    this->titlebar()->setMenuVisible(false);//菜单按钮隐藏
    this->setWindowFlags(this->windowFlags()&~Qt::WindowMinMaxButtonsHint);//最小化、最大化按钮隐藏
    this->setFixedSize(this->width(),this->height());//禁止拖动改变窗口尺寸
    this->setWindowTitle(tr("Combining into PDF..."));//开始菜单栏上鼠标悬浮在窗口上显示的名称

    //*显示的控件*
    tipLabel = new QLabel();
    progressBar = new DProgressBar();
    cancelButton = new QPushButton();

    tipLabel->setText(QString("%1 ( %2 / %3 )").arg(tipText).arg(value).arg(maximum));
    progressBar->setFixedSize(QSize(400,10));
    progressBar->setOrientation(Qt::Horizontal);
    progressBar->setMinimum(minimum);
    progressBar->setMaximum(maximum);
    progressBar->setValue(minimum);
    cancelButton->setText(tr("Cancel"));
    cancelButton->setFixedSize(QSize(422,40));

    QVBoxLayout *layout = new QVBoxLayout ();
    layout->addStretch();
    layout->addStretch();
    layout->addWidget(tipLabel,0,Qt::AlignCenter);
    layout->addStretch();
    layout->addWidget(progressBar,0,Qt::AlignCenter);
    layout->addStretch();
    layout->addStretch();
    layout->addWidget(cancelButton,0, Qt::AlignCenter | Qt::AlignBottom);
    ui->centralwidget->setLayout(layout);

    //去除布局的间隙
    layout->setMargin(10);
    layout->setSpacing(0);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(slotCancelClicked()));

}


void ProgressBarWindow::setProgressBarValue(int value)
{
    value = value;
    if(progressBar != NULL)
    {
        progressBar->setValue(value);
        tipLabel->setText(QString("%1 ( %2 / %3 )").arg(tipText).arg(value).arg(maximum));
    }
}


void ProgressBarWindow::slotCancelClicked()
{
   this->close();
}

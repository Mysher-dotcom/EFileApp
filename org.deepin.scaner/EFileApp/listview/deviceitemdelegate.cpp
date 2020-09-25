#include "deviceitemdelegate.h"
#include <QPainter>
#include "listviewitemdata.h"
#include <QDebug>
#include <QFile>
#include <DGuiApplicationHelper>

//设备列表Item样式重构
DeviceItemDelegate::DeviceItemDelegate(QObject *parent) : QStyledItemDelegate (parent)
{

}

//当前系统是否为深色主题
bool DeviceItemDelegate::isDarkType() const
{
    DGuiApplicationHelper *guiAppHelp = DGuiApplicationHelper::instance();
    if(guiAppHelp->themeType() == DGuiApplicationHelper::ColorType::DarkType)
    {
        //深色主题
        return true;
    }
    else
    {
        //浅色主题
        return false;
    }
}

void DeviceItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.isValid())
    {
        painter->save();
        QVariant var = index.data(Qt::UserRole+1);
        DeviceItemData deviceItemData = var.value<DeviceItemData>();

        QStyleOptionViewItem viewOption(option);

        //Item内容区域
        QRectF rect;
        rect.setX(option.rect.x());
        rect.setY(option.rect.y());
        rect.setWidth(option.rect.width()-1);
        rect.setHeight(option.rect.height()-1);

        //圆角8px
        const qreal radius = 8;
        QPainterPath path;
        path.moveTo(rect.topRight() - QPointF(radius,0));
        path.lineTo(rect.topLeft() + QPointF(radius,0));
        path.quadTo(rect.topLeft(),rect.topLeft()+ QPointF(0,radius));
        path.lineTo(rect.bottomLeft() + QPointF(0,-radius));
        path.quadTo(rect.bottomLeft(),rect.bottomLeft()+ QPointF(radius,0));
        path.lineTo(rect.bottomRight() - QPointF(radius,0));
        path.quadTo(rect.bottomRight(),rect.bottomRight()+ QPointF(0,-radius));
        path.lineTo(rect.topRight()+ QPointF(0,radius));
        path.quadTo(rect.topRight(),rect.topRight()+ QPointF(-radius,-0));
        //painter->fillPath(path,QBrush(QColor(0,0,0,7)));//黑色0.03半透明透明背景
        if(isDarkType()==true)
        {
            painter->fillPath(path,QBrush(QColor(255,255,255,12)));
        }
        else
        {
            painter->fillPath(path,QBrush(QColor(0,0,0,7)));//黑色0.03半透明透明背景
        }

        //绘制图标、名称、型号、状态位置区域
        QRectF iconRect=QRectF(rect.left()+10, rect.top()+15, 60, 60);
        QRectF nameRect=QRectF(rect.left()+iconRect.width()+20,rect.top()+15, 200, 20);
        QRectF modelTitleRect = QRectF(rect.left()+iconRect.width()+20,rect.top()+nameRect.height()+20,60, 20);
        QRectF modelRect=QRectF(rect.left()+iconRect.width()+20+46,rect.top()+nameRect.height()+20,100, 20);
        QRectF statusTitleRect=QRectF(rect.left()+iconRect.width()+20,rect.top()+nameRect.height()+modelRect.height()+20,60, 20);
        QRectF statusRect=QRectF(rect.left()+iconRect.width()+20+46,rect.top()+nameRect.height()+modelRect.height()+20,100, 20);

        QPen namePen ;
        QPen modelTitlePen ;
        QPen modelPen ;
        QPen statusTitlePen ;
        QPen statusPen ;

        if(isDarkType()==true)
        {
            namePen = QColor(255,255,255,255);
            modelTitlePen =  QColor(255,255,255,255);
            modelPen =  QColor(255,255,255,255);
            statusTitlePen =  QColor(255,255,255,255);
            statusPen =  QColor(255,255,255,255);
        }
        else
        {
            namePen = QColor(65,77,104,255);
            modelTitlePen = QColor(82,106,127,255);
            modelPen = QColor(0,26,46,255);
            statusTitlePen = QColor(82,106,127,255);
            statusPen = QColor(0,26,46,255);
        }

        //绘制图标、名称、型号、状态
        //图标
        painter->setRenderHint(QPainter::Antialiasing);//反锯齿
        QString iconPath = ":/img/cameraIcon.svg";
        if(deviceItemData.devType == 0)
        {
            iconPath = ":/img/scannerIcon.svg";
        }
        painter->drawImage(iconRect,QImage(iconPath));

        //设备名
        painter->setFont(QFont("SourceHanSansSC-Medium,SourceHanSansSC",11,400));
        painter->setPen(namePen);
        painter->drawText(nameRect,deviceItemData.devName);

        //型号
        painter->setFont(QFont("SourceHanSansSC-Normal,SourceHanSansSC",10,300));
        painter->setPen(modelTitlePen);
        painter->drawText(modelTitleRect,deviceItemData.devModelTitle);//型号：tr("Model:")

        painter->setFont(QFont("SourceHanSansSC-Medium,SourceHanSansSC",10,300));
        painter->setPen(modelPen);
        painter->drawText(modelRect,deviceItemData.devModel);

        //状态
        painter->setFont(QFont("SourceHanSansSC-Normal,SourceHanSansSC",10,300));
        painter->setPen(statusTitlePen);
        painter->drawText(statusTitleRect,deviceItemData.devStatusTitle);//状态：tr("Status:")

        painter->setFont(QFont("SourceHanSansSC-Medium,SourceHanSansSC",10,300));
        painter->setPen(statusPen);
        painter->drawText(statusRect,deviceItemData.devStatus);


        //鼠标悬浮、选中样式
        //先判断选中状态，再判断鼠标悬浮状态，如果反之，鼠标悬浮选中项时，会改变样式
        if(option.state.testFlag(QStyle::State_Selected))
        {
            //内容区域背景色  painter->fillPath(path,QBrush(QColor(0,129,255)));
            if(isDarkType()==true)
            {
                painter->fillPath(path,QBrush(QColor(0,89,210)));
            }
            else
            {
                painter->fillPath(path,QBrush(QColor(0,129,255)));
            }

            QPen namePen = QColor(255,255,255,255);
            QPen modelTitlePen = QColor(255,255,255,255);
            QPen modelPen = QColor(255,255,255,255);
            QPen statusTitlePen = QColor(255,255,255,255);
            QPen statusPen = QColor(255,255,255,255);

            //绘制图标、名称、型号、状态
            //图标
            painter->setRenderHint(QPainter::Antialiasing);//反锯齿
            QString iconPath = ":/img/cameraIcon.svg";
            if(deviceItemData.devType == 0)
            {
                iconPath = ":/img/scannerIcon.svg";
            }
            painter->drawImage(iconRect,QImage(iconPath));

            //设备名
            painter->setFont(QFont("SourceHanSansSC-Medium,SourceHanSansSC",11,400));
            painter->setPen(namePen);
            painter->drawText(nameRect,deviceItemData.devName);

            //型号
            painter->setFont(QFont("SourceHanSansSC-Normal,SourceHanSansSC",10,300));
            painter->setPen(modelTitlePen);
            painter->drawText(modelTitleRect,deviceItemData.devModelTitle);//型号：tr("Model:")

            painter->setFont(QFont("SourceHanSansSC-Medium,SourceHanSansSC",10,300));
            painter->setPen(modelPen);
            painter->drawText(modelRect,deviceItemData.devModel);

            //状态
            painter->setFont(QFont("SourceHanSansSC-Normal,SourceHanSansSC",10,300));
            painter->setPen(statusTitlePen);
            painter->drawText(statusTitleRect,deviceItemData.devStatusTitle);//状态：tr("Status:")

            painter->setFont(QFont("SourceHanSansSC-Medium,SourceHanSansSC",10,300));
            painter->setPen(statusPen);
            painter->drawText(statusRect,deviceItemData.devStatus);


        }
        else if(option.state.testFlag(QStyle::State_MouseOver))
        {
            painter->fillPath(path,QBrush(QColor(0,0,0,10)));
        }

        painter->restore();
    }
}

QSize DeviceItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QSize(284,90);//288,90
}


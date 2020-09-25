#include "picitemdelegate.h"
#include <QPainter>
#include "listviewitemdata.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QPixmap>
#include <QMimeDatabase>
#include "camcapturesdk.h"
#include "helper/globalhelper.h"
#include <DListView>
#include <QLineEdit>
#include "cpng.h"
#include <DGuiApplicationHelper>

CPNG m_picpng;
//图像列表item样式重构
PicItemDelegate::PicItemDelegate(QObject *parent) : QStyledItemDelegate (parent)
{
    parentCtrl = parent;
}

//当前系统是否为深色主题
bool PicItemDelegate::isDarkType() const
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

void PicItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.isValid())
    {
        painter->save();
        QVariant var = index.data(Qt::UserRole+1);
        PicListItemData picItemData = var.value<PicListItemData>();

        QFile fileTmp(picItemData.picPath);
        if(fileTmp.exists() == false)
            return;

        QStyleOptionViewItem viewOption(option);

        //Item内容区域
        QRectF rect;
        rect.setX(option.rect.x());
        rect.setY(option.rect.y());
        rect.setWidth(option.rect.width()-1);
        rect.setHeight(option.rect.height()-1);

        //圆角18px
        const qreal radius = 18;
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
        painter->fillPath(path,QBrush(Qt::transparent));//透明背景

        //内容区域比Item区域小一圈 154*154,显示范围包含图片、图片名、右上角的勾
        QRectF rectContent;
        rectContent.setX(rect.x()+10);
        rectContent.setY(rect.y()+10);
        rectContent.setWidth(rect.width()-20);
        rectContent.setHeight(rect.height()-20);
        QPainterPath pathMain;
        pathMain.addRoundedRect(rectContent,18,18);//18px圆角
        if(isDarkType()==true)
        {
            painter->fillPath(pathMain,QBrush(QColor(255,255,255,12)));
        }
        else
        {
            painter->fillPath(pathMain,QBrush(Qt::white));//白色背景
        }

        //计算图像显示的宽高，最宽/最高 = 85px
        bool isJPG = false;
        QImage picIconImage;
        long w=85,h=85;//图像宽高
        int tmpW = w;//绘制区域宽高
        int tmpH = h;
        //说明载入的文件是图像
        QMimeDatabase db;
        QMimeType mime = db.mimeTypeForFile(picItemData.picPath);
        bool isPic =  mime.name().startsWith("image/");

        QFileInfo fileInfo(picItemData.picPath);
        QString fileSuffix = fileInfo.suffix().toLower();
        pic_data out;
        //JPG图像载入
        QImage *imgPreview = NULL;
        QFile *file = new QFile(picItemData.picPath);
        //QImage *pngImage = NULL;
        if(isPic == true && fileSuffix.contains("jpg"))
        {
            imgPreview=new QImage(3264,2448,QImage::Format_RGB888);
            isJPG = true;
            QByteArray pData;
            file->open(QIODevice::ReadOnly);
            pData = file->readAll();
            char* tmp = pData.data();
            unsigned char * srcBuf = (unsigned char * )tmp;
            unsigned char* dstBuf = NULL;
            Cam_readBufFromJpegBuf(srcBuf,file->size(),w,h,&dstBuf);
            for (int i = 0 ; i < h ; i++)
            {
                for (int j = 0 ; j < w ; j++)
                {
                    imgPreview->setPixel(j,i,qRgb(* (dstBuf + i*w * 3 + j * 3),* (dstBuf + i*w * 3 + j * 3 + 1),* (dstBuf + i*w * 3 + j * 3 + 2)));
                }
            }
            //删除SDK内的缓存
            delete dstBuf;
            dstBuf = NULL;

            tmpW = w;
            tmpH = h;
            if(tmpW >= tmpH)
            {
                tmpW = 85;
                tmpH = h * 85 / w;
            }
            else
            {
                tmpH = 85;
                tmpW = w * 85 / h;
            }
        }
        //非JPG图像载入
        else if(isPic == true &&  (fileSuffix.contains("ico")
                                   ||fileSuffix.contains("bmp")
                                   ||fileSuffix.contains("svg")
                                   ||fileSuffix.contains("tif")))
        {
            isJPG = false;
            QPixmap pngIcon(picItemData.picPath);
            picIconImage = pngIcon.toImage();
            //picIconImage.load(picItemData.picPath);
        }
        else if (isPic == true &&fileSuffix.contains("png"))
        {
            isJPG = true;
            m_picpng.decode_png(picItemData.picPath.toUtf8().data(),&out);
           // m_picpng.read_png(picItemData.picPath.toUtf8().data());
            qDebug("png width is %d,height is %d,bit_depth is %d,alpha_flag is %d,color type is %d\n",out.width,out.height,out.bit_depth,out.alpha_flag,out.color_type);
            if(out.color_type==2)
            {
                imgPreview = new QImage(out.rgba,out.width,out.height,QImage::Format_RGB888);
            }
            else
            {
                imgPreview = new QImage(out.rgba,out.width,out.height,QImage::Format_Grayscale8);
            }
            if(imgPreview==NULL)
            {
                qDebug("png qimage failed\n");
            }
            w=out.width;
            h=out.height;
        }

        /*
        else if (isPic == true &&fileSuffix.contains("png"))
        {
           isJPG = false;
           QFileInfo info;
           info.setFile(picItemData.picPath);
           // 获取图标
           QFileIconProvider provider;
           QIcon icon = provider.icon(info);
           //将QIcon转换成QImage
           QPixmap osIcon = icon.pixmap(QSize(tmpW,tmpH));
           picIconImage = osIcon.toImage();
        }
        */
        //载入的文件非图像，就加在系统文件图标
        else
        {
            isJPG = false;
            QFileInfo info;
            info.setFile(picItemData.picPath);
            // 获取图标
            QFileIconProvider provider;
            QIcon icon = provider.icon(info);
            //将QIcon转换成QImage
            QPixmap osIcon = icon.pixmap(QSize(tmpW,tmpH));
            picIconImage = osIcon.toImage();
        }

        //计算图像显示位置，Item尺寸：194*194，图像尺寸：100*h 或者 w*100
        //图像只在item的上部120px显示，Item显示图像区域尺寸：154*120
        int x = rect.left() + ((194 - tmpW) / 2);
        int y = rect.top() + ((160 - tmpH) / 2);

        //计算图像名称文字显示的位置
        QFont font("SourceHanSansSC-Normal,SourceHanSansSC",11,300);
        painter->setFont(font);
        QString fileName = GlobalHelper::setShowString(picItemData.picName,150);//图片名超长，就用...
        int fontWidth = painter->fontMetrics().width(fileName);//文字显示的宽度
        if(fontWidth >= 150)
        {
            fontWidth = 150;
        }
        int fontX = rect.left() + ((194 - fontWidth)) / 2;
        int fontY = rect.bottom() - 60;

        //绘制图片、图片名位置区域
        QRectF picRect=QRectF(x, y, tmpW,tmpH );
        QRectF nameRect=QRectF(fontX,fontY, 150, 25);
        QRectF iconRect=QRectF(rect.right()-30,rect.top()+5,28,28);//右上角勾图标

        //绘制图片、图片名
        painter->setRenderHint(QPainter::Antialiasing);//反锯齿
        //painter->drawImage(picRect,picIconImage);//QImage(picItemData.picPath));//贴图

        if(isJPG == true)
        {
           painter-> drawImage(picRect,*imgPreview,QRect(0,0,w,h));
        }
        else
        {
            painter->drawImage(picRect,picIconImage);
        }
        if(isDarkType()==true)
        {
            painter->setPen(QPen(QColor(255,255,255,255)));//字体颜色
        }
        else
        {
            painter->setPen(QPen(QColor("#526A7F")));//字体颜色
        }
        painter->drawText(nameRect,fileName);//图片名

        //鼠标悬浮、选中样式
        //先判断选中状态，再判断鼠标悬浮状态，如果反之，鼠标悬浮选中项时，会改变样式
        if(option.state.testFlag(QStyle::State_Selected))
        {
            painter->setRenderHint(QPainter::Antialiasing);

            if(isDarkType()==true)
            {
                painter->fillPath(pathMain,QBrush(QColor(255,255,255,20)));//内容区域背景色
            }
            else
            {
                painter->fillPath(pathMain,QBrush(QColor(0,0,0,51)));//内容区域背景色
            }
            painter->setRenderHint(QPainter::Antialiasing);
            painter->drawImage(iconRect,QImage(":/img/itemSelected.svg"));//右上角勾图标
        }
        else if(option.state.testFlag(QStyle::State_MouseOver))
        {
            if(isDarkType()==true)
            {
                painter->fillPath(pathMain,QBrush(QColor(255,255,255,12)));
            }
            else
            {
                painter->fillPath(pathMain,QBrush(QColor(0,0,0,5)));
            }
        }

        if(picItemData.isRename == true)
        {
            qDebug()<<"**************"<<parentCtrl->metaObject()->className();
            //确定父控件类型
            if (parentCtrl->metaObject()->className() == QStringLiteral("Dtk::Widget::DListView"))
            {
                DListView *tmpparentCtrl = qobject_cast<DListView*>(parentCtrl); //转为真实的类型
                QLineEdit *lineEdit = new QLineEdit (tmpparentCtrl);
                lineEdit->show();
                lineEdit->setFixedSize(QSize(100,30));
                lineEdit->move(QPoint(150, 25));
                lineEdit ->raise();   //在最上层显示
                lineEdit->setFocus();
             }
        }

        file->close();
        delete file;
        file = NULL;
        delete  imgPreview;
        imgPreview =NULL;
        if (isPic == true &&fileSuffix.contains("png"))
        {
            free(out.rgba);
            out.rgba=NULL;
        }

        painter->restore();
    }
}

QSize PicItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QSize(194,194);
}


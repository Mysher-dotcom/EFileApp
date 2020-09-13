#include "picitemdelegate.h"
#include <QPainter>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
//#include "datatype.h"
#include <QImageReader>
#include <QImage>
#include "camcapturesdk.h"
#include "helper/globalhelper.h"
#include "cmimage.h"
#include "listviewitemdata.h"
//图像列表item样式重构picItemData.picPath
PicItemDelegate::PicItemDelegate(QObject *parent) : QStyledItemDelegate (parent)
{

}
char* PicItemDelegate::substrend(char * str, int n)
{
    char * substr = (char*) malloc (n+1);
    int length = strlen(str);
    if(n >= length)//若截自取长度大百于字符度串长度，则问直接截取全部答字符串
    {
    strcpy(substr, str);
    return substr;
    }
    int k = 0;
    for(int i = strlen(str) - n - 1; i < strlen(str); i++)
    {
    substr[k++] = str[i];
    }
    substr[k] = '\0';
    return substr;
}
void PicItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.isValid())
    {
        painter->save();
        QVariant var = index.data(Qt::UserRole+1);
        PicListItemData picItemData = var.value<PicListItemData>();

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
        painter->fillPath(pathMain,QBrush(Qt::white));//白色背景

        //计算图像显示的宽高，最宽/最高 = 85px
        QFileInfo f(picItemData.picPath);
        if(f.exists())
        {
            char* tmpFile = substrend((char*)picItemData.picPath.toLatin1().data(),2);
            //判断jpg图
            if(strcmp(tmpFile,"jpg")==NULL){
                int nw = 3264;
                int nh = 2448;
               //QImage* img2 = new QImage(picItemData.picPath);
                QImage* img1 = new QImage(nw,nh,QImage::Format_RGB888);
                QByteArray pData;
                QFile *file=new QFile(picItemData.picPath);
                file->open(QIODevice::ReadOnly);
                pData=file->readAll();

                char* tmp = pData.data();
                unsigned char * srcBuf = (unsigned char * )tmp;
                unsigned char* dstBuf = NULL;
                long lWidth=200,lHeight=200;

                Cam_readBufFromJpegBuf(srcBuf,file->size(),lWidth,lHeight,&dstBuf);
                for (int i = 0 ; i < lHeight ; i++)
                    for (int j = 0 ; j < lWidth ; j++)
                    {
                        img1->setPixel(j,i,qRgb(* (dstBuf + i*lWidth * 3 + j * 3),* (dstBuf + i*lWidth * 3 + j * 3 + 1),* (dstBuf + i*lWidth * 3 + j * 3 + 2)));
                    }

                delete dstBuf;
                dstBuf = NULL;
                int w = lWidth;//img1->width();
                int h = lHeight;//img1->height();
                if(w >= h)
                {
                    w = 85;
                    h = lHeight * 100 / lWidth;
                }
                else
                {
                    h = 85;
                    w = lWidth * 100 / lHeight;
                }

                //计算图像显示位置，Item尺寸：194*194，图像尺寸：100*h 或者 w*100
                //图像只在item的上部120px显示，Item显示图像区域尺寸：154*120
                int x = rect.left() + ((160 - w) / 2);
                int y = rect.top() + ((160 - h) / 2);

                //计算图像名称文字显示的位置
                QFont font("SourceHanSansSC-Normal,SourceHanSansSC",12,300);
                painter->setFont(font);
                int fontWidth = painter->fontMetrics().width(picItemData.picName);//文字显示的宽度
                if(fontWidth >= 150)
                {
                    fontWidth = 150;
                }
                int fontX = rect.left() + ((194 - fontWidth)) / 2;
                int fontY = rect.bottom() - 40;


                //绘制图片、图片名位置区域
                QRectF picRect=QRectF(x, y, w, h);
                QRectF nameRect=QRectF(fontX,fontY, 150, 25);
                QRectF iconRect=QRectF(rect.right()-30,rect.top()+5,28,28);//右上角勾图标


                //绘制图片、图片名
                painter->setRenderHint(QPainter::Antialiasing);//反锯齿
                //painter->drawImage(picRect,QImage(picItemData.picPath));//贴图
                painter-> drawImage(picRect,*img1,QRect(0,0,lWidth,lHeight));
                //painter->drawImage(picRect,img1,picRect);
                painter->setPen(QPen(QColor("#526A7F")));//字体颜色
                painter->drawText(nameRect,picItemData.picName);//图片名
                file->close();
                delete file;
                file = NULL;
                delete  img1;
                img1 =NULL;
            }
              else {

                mcvInit();
                MImage* src = mcvLoadImage(picItemData.picPath.toUtf8().data(),0,0);
                int nWidht = src->width;
                int nHeight = src->height;
                uchar* srcDataBuf = mcvGetImageData(src);
                mcvReleaseImage(&src);
                QImage* img2 = new QImage(nWidht,nHeight,QImage::Format_RGB888);
                for (int i = 0 ; i < nHeight ; i++)
                    for (int j = 0 ; j < nWidht ; j++)
                    {
                        img2->setPixel(j,i,qRgb(* (srcDataBuf + i*nWidht * 3 + j * 3),* (srcDataBuf + i*nHeight * 3 + j * 3 + 1),* (srcDataBuf + i*nWidht * 3 + j * 3 + 2)));
                    }

               int w = nWidht;
               int h = nHeight;
               if(w >= h)
               {
                   w = 85;
                   h = nHeight * 100 / nWidht;
               }
               else
               {
                   h = 85;
                   w = nWidht * 100 / nHeight;
               }
               //计算图像显示位置，Item尺寸：194*194，图像尺寸：100*h 或者 w*100
               //图像只在item的上部120px显示，Item显示图像区域尺寸：154*120
               int x = rect.left() + ((160 - w) / 2);
               int y = rect.top() + ((160 - h) / 2);

               //计算图像名称文字显示的位置
               QFont font("SourceHanSansSC-Normal,SourceHanSansSC",12,300);
               painter->setFont(font);
               int fontWidth = painter->fontMetrics().width(picItemData.picName);//文字显示的宽度
               if(fontWidth >= 150)
               {
                   fontWidth = 150;
               }
               int fontX = rect.left() + ((194 - fontWidth)) / 2;
               int fontY = rect.bottom() - 40;


               //绘制图片、图片名位置区域
               QRectF picRect=QRectF(x, y, w, h);
               QRectF nameRect=QRectF(fontX,fontY, 150, 25);
               QRectF iconRect=QRectF(rect.right()-30,rect.top()+5,28,28);//右上角勾图标


               //绘制图片、图片名
               painter->setRenderHint(QPainter::Antialiasing);//反锯齿
               //painter->drawImage(picRect,QImage(picItemData.picPath));//贴图
               painter-> drawImage(picRect,*img2,QRect(0,0,nWidht,nHeight));
              //painter->drawImage(picRect,img2,picRect);
               painter->setPen(QPen(QColor("#526A7F")));//字体颜色
               painter->drawText(nameRect,picItemData.picName);//图片名
               //file->close();
              // delete file;
               //file = NULL;
               delete  img2;
               img2 =NULL;
            }
            //鼠标悬浮、选中样式
            //先判断选中状态，再判断鼠标悬浮状态，如果反之，鼠标悬浮选中项时，会改变样式
            if(option.state.testFlag(QStyle::State_Selected))
            {
                painter->setRenderHint(QPainter::Antialiasing);
                painter->fillPath(pathMain,QBrush(QColor(0,0,0,51)));//内容区域背景色
                painter->setRenderHint(QPainter::Antialiasing);
               // painter->drawImage(iconRect,QImage(":/img/itemSelected.svg"));//右上角勾图标
            }
            else if(option.state.testFlag(QStyle::State_MouseOver))
            {
                painter->fillPath(pathMain,QBrush(QColor(0,0,0,5)));
            }


        }


        painter->restore();
    }
}

QSize PicItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QSize(164,164);
}

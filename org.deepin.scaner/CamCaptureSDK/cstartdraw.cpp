#include "cstartdraw.h"
#include "cimagehelper.h"
extern CImageHelper g_ImageHelper;
extern CameraCpatureReciveCallback gCameraRecvFun;

CStartDraw::CStartDraw(QObject *parent) : QObject(parent)
{

}
void CStartDraw::handleVideoBuffer(void *data, int size,int w,int h)
{

   g_ImageHelper.DrawLine((char*)data,size,w,h,1);
   gCameraRecvFun((char*)data,size,w,h,0);
   if(data)
   {
       delete data;
       data = NULL;
   }


}

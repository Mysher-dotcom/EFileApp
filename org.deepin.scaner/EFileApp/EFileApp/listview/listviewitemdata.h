#ifndef LISTVIEWITEMDATA_H
#define LISTVIEWITEMDATA_H
#include <QMetaType>
#include <qstring.h>

//图像列表Item数据
typedef struct{
    QString picPath;//图像全路径
    QString picName;//图像名称
    bool isRename;//是否重命名
}PicListItemData;

Q_DECLARE_METATYPE(PicListItemData)



//设备信息列表Item数据
typedef struct{
    int devType; //设备类型，0=扫描仪,1=拍摄仪
    QString devName;//设备名称
    QString devModel;//设备型号
    QString devStatus;//设备状态
    int devIndex;//设备下标
    int devLicense;//是否有授权（0=普通设备，1=授权设备，2=设备没有usb权限）
    QString devModelTitle;//设备型号标题
    QString devStatusTitle;//设备状态标题
}DeviceItemData;

Q_DECLARE_METATYPE(DeviceItemData)

#endif // LISTVIEWITEMDATA_H

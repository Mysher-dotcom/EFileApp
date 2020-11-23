#ifndef ERRORTYPE_H
#define ERRORTYPE_H
typedef enum{  
    ERROR_GOOD=0,	//成功。
    STATUS_UNSUPPORTED,//不支持该操作。
    STATUS_CANCELLED,//操作被取消。
    STATUS_DEVICE_BUSY,//设备正忙--请稍后重试。
    STATUS_INVAL,//数据或参数无效。
    STATUS_EOF	,//没有更多可用数据（文件结束）。
    STATUS_JAMMED,//文档进纸器卡纸。
    STATUS_NO_DOCS,//文档进纸器中的文档不足。（无纸张）
    STATUS_COVER_OPEN,//扫描仪盖板已打开。
    STATUS_IO_ERROR,//设备I/O期间出错。
    STATUS_NO_MEM,//内存不足
    STATUS_ACCESS_DENIED, //对资源的访问已被拒绝。
    STATUS_INIT_FAILED, //初始化失败
    STATUS_OPEN_FAILED, //打开失败
    STATUS_CONNECT_FAILED
};


#endif // ERRORTYPE_H

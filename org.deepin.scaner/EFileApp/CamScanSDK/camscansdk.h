#ifndef CAMSCANSDK_H
#define CAMSCANSDK_H

#include "camscansdk_global.h"
#include "datatype.h"
extern "C"{
    /*
    **dec：扫描仪初始化
    **Return：0表示成功，其他返回值详见错误码
    */
    long Init();
    /*
    ** dec：获取扫描仪设备个数
    ** param：nDevCount-设备总数（out）
    ** reurn：0表示成功，其他返回值详见错误码
    */
    long GetDevCount(long& nDevCount);
    /*
    ** dec：获取扫描仪设备名称
    ** param：nDevIndex-设备号（in），szDevKanas-设备kanas（out），szDevVendor-设备vendor（out），szDevModel-szDevType-设备Dev（out），
    ** reurn：0表示成功，其他返回值详见错误码
    */
    long GetDevName(long nDevIndex,char* szDevKanas,char* szDevVendor,char* szDevModel,char* szDevType);
    /*
    ** dec：打开设备
    ** param：nDevIndex-设备号（in）
    ** reurn：0表示成功，其他返回值详见错误码
    */
    long OpenDev(long nDevIndex);
    /*
    ** dec：获取Cap能力总数
    ** param：nDevIndex-设备号（in） nCapCount-Cap总数（out）
    ** reurn：0表示成功，其他返回值详见错误码
    */
    long GetCapCount(long nDevIndex,long &nCapCount);
    /*
    ** dec：获取每个Cap对应的名称，类型以及数据
    ** param：nDevIndex-设备号（in） nCapIndex-Cap号（in）szCapName-Cap名称（out），nCapType-Cap类型（out），szData-Cap数据（out）
    ** reurn：0表示成功，其他返回值详见错误码
    */
    long GetCapInfo(long nDevIndex,long nCapIndex,char* szCapName,long &nCapType,void **szData);
    /*
    ** dec：解析Cap类型为String数据的总数
    ** param：nCapType-类型id（in） szData-Cap数据(这个数据是GetCapInfo参数szData的数据，in）nCount-数据总数（out）
    ** reurn：0表示成功，其他返回值详见错误码
    */
    long AnalysisStringCapDataCount(long nCapType,void* szData,long &nCount);
    /*
    ** dec：解析Cap类型为String数据名称
    ** param：nCapType-类型id（in） szData-数据名称(获取每个cap下面对应每个类型的数据（out））
    ** reurn：0表示成功，其他返回值详见错误码
    */
    long AnalysisStringCapData(long nCapType,char* szData);
    /*
    ** dec：解析Cap类型为int数据的总数
    ** param：nCapType-类型id（in） szData-Cap数据(这个数据是GetCapInfo参数szData的数据，in）nCapDataCount-数据总数（out）
    ** reurn：0表示成功，其他返回值详见错误码
    */
    long AnalysisWordCapDataCount(long nCapType,void* szData,long &nCapDataCount);
    /*
    ** dec：解析Cap类型为int数据名称
    ** param：nCapType-类型id（in） nCapData-数据名称(获取每个cap下面对应每个类型的数据（out））
    ** reurn：0表示成功，其他返回值详见错误码
    */
    long AnalysisWordCapData(long nCapType,long &nCapData);
    /*
    ** dec：解析Cap类型为range
    ** param：nCapType-类型id（in） szData-Cap数据(这个数据是GetCapInfo参数szData的数据，in）nMin-最小值（out），nMax-最大值（out），nQuant-quant值（out）
    ** reurn：0表示成功，其他返回值详见错误码
    */
    long AnalysisRangeCapData(long nCapType,void* szData,long &nMin,long &nMax,long &nQuant);
    /*
    ** dec：设置cap参数
    ** param：nCapIndex-Cap号（in），nCapType-Cap类型，szCapInfo-cap数据(in）
    ** reurn：0表示成功，其他返回值详见错误码
    */
    long SetCapCtrl(long nCapIndex,long nCapType,void* szCapInfo);
    /*
    ** dec：扫描
    ** param：nDevIndex-设备号，DevRecvFun-获取扫描图像的回调，ScanResultFun-扫描返回状态）
    ** reurn：0表示成功，其他返回值详见错误码
    */
    long Scan(long nDevIndex,DevScanReciveCallback DevRecvFun,ScanResultCallback ScanResultFun);
    /*
    ** dec：取消当前设备扫描
    ** param：nDevIndex-设备号
    ** reurn：0表示成功，其他返回值详见错误码
    */
    long CancelScan(long nDevIndex);
    /*
    ** dec：关闭扫描
    ** param：nDevIndex-设备号
    ** reurn：0表示成功，其他返回值详见错误码
    */
    long CloseDev(long nDevIndex);
}

#endif // CAMSCANSDK_H

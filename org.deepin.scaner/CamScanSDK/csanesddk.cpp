#include "csanesddk.h"
#include <QDebug>
#include "errortypescanner.h"
#include <QFile>

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <asm/types.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#include <errno.h>
#include <fcntl.h>
//#include <QFile>
#include <sys/ioctl.h>
#include <iostream>


#define STRIP_HEIGHT 256;
static SANE_Handle g_sane_handle;
static SANE_Byte *buffer;
static size_t buffer_size;


ScanResultCallback g_ScanResultFun;
static int gDPI = 0;
CSaneSDDK::CSaneSDDK()
{
    vecBuffer.clear();
    m_vecCapIntParam.clear();
    m_vecCapStringParam.clear();
    vecsaneOptionDescriptor.clear();
    m_vecCapType.clear();
    m_nInitStatus = -1;
    bIsFloat = false;
    bIsFirstScan = false;
    bIsUnis = false;
}
static void auth_callback(SANE_String_Const resource,
                          SANE_Char* username,SANE_Char* password)
{
}
/*
**dec：扫描仪初始化
**Return：0表示成功，其他返回值详见错误码
*/
long CSaneSDDK::Init()
{
    int nVersion_code = 0;
    SANE_Status nStatus = sane_init(&nVersion_code,auth_callback);
    qDebug("SANE version code:%d,nStatus is %d\n",nVersion_code,nStatus);
    return nStatus;

}
/*
** dec：获取扫描仪设备个数
** param：nDevCount-设备总数（out）
** reurn：0表示成功，其他返回值详见错误码
*/
long CSaneSDDK::GetDevCount(long& nDevCount){

    //const SANE_Device ** m_device_list;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    int nFont = tv.tv_sec;
    SANE_Status nsane_status = sane_get_devices(&device_list,SANE_FALSE);
    gettimeofday(&tv,NULL);
    int nLater = tv.tv_sec;
    int nResult = nLater - nFont;
    qDebug("运行时间 is %ds\n",nResult);
    int DevIndex = 0;
    for (int i = 0; device_list[i]; ++i)
    {
        DevIndex++;
        qDebug("device `%s' is a %s %s %s\n",device_list[i]->name, device_list[i]->vendor,device_list[i]->model, device_list[i]->type);
    }
    if(device_list[0]==NULL)
    {
        qDebug("scanner no connect\n");
        return STATUS_CONNECT_FAILED;
    }
    nDevCount = DevIndex;
    qDebug("sane_get_device status:%d nDevCount is %d\n",nsane_status,nDevCount);
    return nsane_status;
}
/*
** dec：获取扫描仪设备名称
** param：nDevIndex-设备号（in），szDevKanas-设备kanas（out），szDevVendor-设备vendor（out），szDevModel-szDevType-设备Dev（out），
** reurn：0表示成功，其他返回值详见错误码
*/
long CSaneSDDK::GetDevName(long nDevIndex,char* szDevKanas,char* szDevVendor,char* szDevModel,char* szDevType){

    strcpy(szDevKanas,device_list[nDevIndex]->name);
    strcpy(szDevVendor,device_list[nDevIndex]->vendor);
    strcpy(szDevModel,device_list[nDevIndex]->model);
    strcpy(szDevType,device_list[nDevIndex]->type);
    if(szDevType==NULL&&szDevKanas==NULL&&szDevModel==NULL&&szDevVendor==NULL)
    {
        return STATUS_INVAL;
    }
    qDebug("GetDevName szDevKanas is %s,szDevVendor is %s,szDEvModel is %s,szDevType is %s\n",szDevKanas,szDevVendor,szDevModel,szDevType);
    return SANE_STATUS_GOOD;
}
/*
** dec：打开设备
** param：nDevIndex-设备号（in）
** reurn：0表示成功，其他返回值详见错误码
*/
long CSaneSDDK::OpenDev(long nDevIndex){
    qDebug("Opendev start is %d\n",nDevIndex);
    sane_handle = new SANE_Handle();
    qDebug("new SANE_Handle\n");
    qDebug("open dev name is %s\n",device_list[nDevIndex]->name);
    SANE_Status sane_status = sane_open(device_list[nDevIndex]->name,&sane_handle);
    qDebug("opendev return is %d\n",sane_status);
    return sane_status;
}
/*
** dec：获取Cap能力总数
** param：nDevIndex-设备号（in） nCapCount-Cap总数（out）
** reurn：0表示成功，其他返回值详见错误码
*/
long CSaneSDDK::GetCapCount(long nDevIndex,long &nCapCount){
    qDebug("CSaneSDDK::GetCapCount start is %d\n",vecsaneOptionDescriptor.size());
    if(vecsaneOptionDescriptor.size()>0)
    {
        qDebug("清空之前的列表");
        vecsaneOptionDescriptor.clear();
        m_vecCapType.clear();

    }

    int nOptionIndex = 0;
    while(1)
    {
        nOptionIndex++;
        const SANE_Option_Descriptor * saneOptionDescriptor = sane_get_option_descriptor(sane_handle,nOptionIndex);
        if(saneOptionDescriptor)
        {
            SANE_String_Const stringConst = saneOptionDescriptor->name;

        }
        if(saneOptionDescriptor==NULL)
        {
            qDebug("get par[%d] is null,break\n",nOptionIndex);
            break;
        }

        vecsaneOptionDescriptor.push_back(saneOptionDescriptor);
    }
    nCapCount = vecsaneOptionDescriptor.size();
    qDebug("nCapCount is %d,%d\n",nCapCount,vecsaneOptionDescriptor.size());
    return SANE_STATUS_GOOD;
}
/*
** dec：获取每个Cap对应的名称，类型以及数据
** param：nDevIndex-设备号（in） nCapIndex-Cap号（in）szCapName-Cap名称（out），nCapType-Cap类型（out），szData-Cap数据（out）
** reurn：0表示成功，其他返回值详见错误码
*/
long CSaneSDDK::GetCapInfo(long nDevIndex,long nCapIndex,char* szCapName,long &nCapType,void **szData){
    SANE_String_Const stringConst = vecsaneOptionDescriptor.at(nCapIndex)->name;
    nCapType = vecsaneOptionDescriptor.at(nCapIndex)->constraint_type;
    m_vecCapType.push_back(nCapType);
    if(stringConst==NULL)
    {
      return SANE_STATUS_INVAL;
    }
     strcpy(szCapName,stringConst);

     if(nCapType==1)
     {
         SANE_Range * rangevalue = (SANE_Range*)vecsaneOptionDescriptor.at(nCapIndex)->constraint.range;
         *szData = rangevalue;
     }
     else if(nCapType==2)
     {
         SANE_Word * wordvalue = (SANE_Word*)vecsaneOptionDescriptor.at(nCapIndex)->constraint.range;
         *szData = wordvalue;     
     }
     else if(nCapType==3)
     {
         SANE_String * stringvalue = (SANE_String*)vecsaneOptionDescriptor.at(nCapIndex)->constraint.range;
         *szData = stringvalue;
     }

     return SANE_STATUS_GOOD;
}
/*
** dec：解析Cap类型为String数据的总数
** param：nCapType-类型id（in） szData-Cap数据(这个数据是GetCapInfo参数szData的数据，in）nCount-数据总数（out）
** reurn：0表示成功，其他返回值详见错误码
*/
long CSaneSDDK::AnalysisStringCapDataCount(long nCapType,void* szData,long &nCount){
    m_vecCapStringParam.clear();
    int nTmp = 0;
    SANE_String *pstringValue = (SANE_String*)szData;
    SANE_String tmp;
    do{
        tmp = (SANE_String)pstringValue[nTmp];
        qDebug("AnalysisStringCapDataCount is %s\n",tmp);
        if(tmp==0)
        {
            break;
        }
        m_vecCapStringParam.push_back(tmp);
        nTmp++;
    }
    while(tmp!=NULL);
    nCount = nTmp;
    qDebug("AnalysisStringCapDataCount is %d\n",nCount);
    return SANE_STATUS_GOOD;
}
/*
** dec：解析Cap类型为String数据名称
** param：nCapType-类型id（in） szData-数据名称(获取每个cap下面对应每个类型的数据（out））
** reurn：0表示成功，其他返回值详见错误码
*/
long CSaneSDDK::AnalysisStringCapData(long nCapType,char* szData){
    strcpy(szData,m_vecCapStringParam.at(nCapType).toLatin1().data());
    qDebug("AnalysisStringCapData is %s\n",szData);
    return SANE_STATUS_GOOD;
}
/*
** dec：解析Cap类型为int数据的总数
** param：nCapType-类型id（in） szData-Cap数据(这个数据是GetCapInfo参数szData的数据，in）nCapDataCount-数据总数（out）
** reurn：0表示成功，其他返回值详见错误码
*/
long CSaneSDDK::AnalysisWordCapDataCount(long nCapType,void* szData,long &nCapDataCount){
   m_vecCapIntParam.clear();
    SANE_Word *wordValue = (SANE_Word*)szData;
    int nwordSize = *wordValue;
    nCapDataCount = nwordSize;
    for (int constraintWordIndex=0;constraintWordIndex<nwordSize;constraintWordIndex++) {
        SANE_Int dpi = *(++wordValue);
        qDebug("AnalysisWordCapDataCount is %d\n",dpi);
        m_vecCapIntParam.push_back(dpi);
    }
     qDebug("AnalysisWordCapDataCount is %d\n",nCapDataCount);
    return  SANE_STATUS_GOOD;
}
/*
** dec：解析Cap类型为int数据名称
** param：nCapType-类型id（in） n&&szDevKanas==NULL&&szDevModel==NULL&&szDevVendor==NULLCapData-数据名称(获取每个cap下面对应每个类型的数据（out））
** reurn：0表示成功，其他返回值详见错误码
*/
long CSaneSDDK::AnalysisWordCapData(long nCapType,long &nCapData){
    nCapData = m_vecCapIntParam.at(nCapType);
     qDebug("AnalysisWordCapData is %d\n",nCapData);
    return SANE_STATUS_GOOD;
}
/*
** dec：解析Cap类型为range
** param：nCapType-类型id（in） szData-Cap数据(这个数据是GetCapInfo参数szData的数据，in）nMin-最小值（out），nMax-最大值（out），nQuant-quant值（out）
** reurn：0表示成功，其他返回值详见错误码
*/
long CSaneSDDK::AnalysisRangeCapData(long nCapType,void* szData,long &nMin,long &nMax,long &nQuant){
    SANE_Range *rangeValue = (SANE_Range*)szData;
    nMin = rangeValue->min;
    nMax = rangeValue->max;
    nQuant = rangeValue->quant;
    qDebug("AnalysisRangeCapData is %d,%d,%d\n",rangeValue->min,rangeValue->max,rangeValue->quant);
    return SANE_STATUS_GOOD;
}
/*g_saneSDK
** dec：设置cap参数
** param：nCapIndex-Cap号（in），szCapInfo-cap数据(in）
** reurn：0表示成功，其他返回值详见错误码
*/
long CSaneSDDK::SetCapCtrl(long nCapIndex,char* szCapInfo){
    SANE_Int info =0;
    int nCap = m_vecCapType.at(nCapIndex);
    nCapIndex =nCapIndex +1;
    SANE_Int ss = nCapIndex;
    SANE_Status nret;

    //if(nCap==1||nCap==2)
    if(strcmp(szCapInfo,"100")==NULL||strcmp(szCapInfo,"150")==NULL||strcmp(szCapInfo,"200")==NULL||
            strcmp(szCapInfo,"240")==NULL||strcmp(szCapInfo,"300")==NULL||strcmp(szCapInfo,"400")==NULL||
            strcmp(szCapInfo,"600")==NULL)
    {
       SANE_Int sDPI = (SANE_Int)atoi(szCapInfo);
        gDPI = atoi(szCapInfo);
        nret =  sane_control_option(sane_handle,ss,SANE_Action::SANE_ACTION_SET_VALUE,&sDPI,&info);
    }
    else
     {
        if(strcmp(szCapInfo,"FLAT_FRONT")==NULL||strcmp(szCapInfo,"Flatbed")==NULL)
        {
             bIsFloat = true;
        }
        else {
            bIsFloat = false;
        }
       /* if(strcmp(szCapInfo,"FLAT_FRONT")!=NULL||strcmp(szCapInfo,"ADF Front")!=NULL)
        {
            bIsFloat = false;
        }*/
        if(strcmp(szCapInfo,"Lineart")==NULL)
        {
           // qDebug("111111111111111111111111111\n");
            szCapInfo = "Gray8";
            SANE_String sScanMode = (SANE_String)szCapInfo;
            qDebug("ss is %dsScanMode is %s\n",ss,sScanMode);
            nret =  sane_control_option(sane_handle,ss,SANE_Action::SANE_ACTION_SET_VALUE,sScanMode,&info);
            if(nret==4)
            {
                szCapInfo = "Gray";
            }
            else {
                qDebug("sane_control_option is %d\n",nret);
                return nret;
            }
        }
        qDebug("mode is %s\n",szCapInfo);
        SANE_String sScanMode = (SANE_String)szCapInfo;
        qDebug("ss is %dsScanMode is %s\n",ss,sScanMode);
        nret =  sane_control_option(sane_handle,ss,SANE_Action::SANE_ACTION_SET_VALUE,sScanMode,&info);
    }
    qDebug("sane_control_option is %d\n",nret);
    return nret;
}
int g_nImageBufferCount = 0;
SANE_Status CSaneSDDK::scan_it()
{
    int i, len, first_frame = 1, offset = 0, must_buffer = 0, hundred_percent;
    SANE_Byte min = 0xff, max = 0;
    SANE_Parameters parm;
    SANE_Status status;
    SANE_Byte * bufferm  = NULL;
    static const char *format_name[] = {"gray", "RGB", "red", "green", "blue"};
    SANE_Word total_bytes = 0, expected_bytes;
    SANE_Int hang_over = -1;
    do
    {
        if (!first_frame)
        {
            status = sane_start (sane_handle);
            if (status != SANE_STATUS_GOOD)
            {
                //goto cleanup;
                break;
            }
        }

        status = sane_get_parameters (sane_handle, &parm);
        qDebug("sane_get_parameters is %d\n",parm.last_frame);
        bufferm = new SANE_Byte[parm.bytes_per_line*parm.lines];
        int nWidth= parm.pixels_per_line;
        int nBpp = parm.bytes_per_line/nWidth;
        int nHeight =parm.lines;
        if (status != SANE_STATUS_GOOD)
        {
          //goto cleanup;
            break;
        }
        while (1)
        {
            status = sane_read (sane_handle, buffer, buffer_size, &len);
            qDebug("sane_read is %d\n",status);
            if (status != SANE_STATUS_GOOD)
            {
                if (status != SANE_STATUS_EOF)
                {

                    return status;
                   // break;
                }
                qDebug("sane_read buf is end\n");
                //usleep(500);
                break;
                // goto cleanup;
            }
            memcpy(bufferm + offset,buffer,len);
            offset += len;
        }
        //vecBuffer.push_back((char*)bufferm);

        //qDebug("vecBuffer is %d\n",vecBuffer.size());
        first_frame = 0;
        g_nImageBufferCount++;
        qDebug("g_nImageBufferCount is %d offset is %d\n",g_nImageBufferCount,offset);
        /*FILE *ofp = NULL;
        if (NULL != (ofp = fopen ("/tmp/szFilePath.jpg", "w+")))
        {
            int write_length = 0;
            fwrite ((uchar*)bufferm, 1, 1, ofp);
            //write_length = fwrite((uchar*)bufferm,sizeof(char),len,ofp);
           // WriteFile((uchar*)bufferm,parm.bytes_per_line*parm.lines,ofp);
        }
        fclose(ofp);*/
        qDebug("bytes_per_line is %d,lines is %d,nBpp is %d pixels_per_line is %d\n",parm.bytes_per_line,parm.lines,nBpp,parm.pixels_per_line);
        if(g_DevRecvFun)
            g_DevRecvFun((uchar*)bufferm,parm.bytes_per_line*parm.lines,nWidth,nHeight,nBpp,gDPI);
    }while(!parm.last_frame);
//cleanup:
   // if (bufferm)
    //{
     //   delete bufferm;
      //  bufferm = NULL;
    //}

    return status;
    /*
        status = sane_get_parameters(g_sane_handle,&parm);
        qDebug("last_frame is %d\n",parm.last_frame);
        if(status !=SANE_STATUS_GOOD)
        {
            qDebug("sane_get_parameters return is %d\n",status);
            return SANE_STATUS_INVAL;
        }
        nWidth= parm.pixels_per_line;

        nHeight =parm.lines;

        SANE_Byte * bufferm = new SANE_Byte[parm.bytes_per_line*parm.lines];
        offset = 0;

    while(1)
    {
        status = sane_read(g_sane_handle,buffer,buffer_size,&len);
        if(status != SANE_STATUS_GOOD)
        {
                if(status != SANE_STATUS_EOF)
                    return status;
                break;

        }
        memcpy(bufferm + offset,buffer,len);
        offset += len;

    }
    qDebug("last_frame is %d\n",parm.last_frame);
    first_frame = 0;
    g_nImageBufferCount++;
    qDebug("g_nImageBufferCount is %d\n",g_nImageBufferCount);
    if(g_DevRecvFun)
         g_DevRecvFun(bufferm,parm.bytes_per_line*parm.lines,nWidth,nHeight,parm.bytes_per_line/nWidth,gDPI);
    delete bufferm;
     bufferm = NULL;

*/
   return status;
}
int dwProcessID = 0;
SANE_Status CSaneSDDK::do_scan()
{
    //vecBuffer.clear();
    SANE_Status status;
    bIsFirstScan = false;
    buffer_size = (32 * 1024);
    buffer = (SANE_Byte *) malloc(buffer_size);
    do{
        dwProcessID++;
        printf("Count is %d\n",dwProcessID);
        status = sane_start (sane_handle);

        if (status != SANE_STATUS_GOOD)
        {
            if(status == SANE_STATUS_NO_DOCS)
            {
                qDebug("sane_start return SANE_STATUS_NO_DOCS\n");
                if(!bIsFirstScan)
                {
                    g_ScanResultFun(SANE_STATUS_NO_DOCS);
                    status= SANE_STATUS_NO_DOCS;
                }
                else {
                    bIsFirstScan=false;
                    status= SANE_STATUS_GOOD;
                }
                break;
            }
            else if (status == SANE_STATUS_DEVICE_BUSY) {
                g_ScanResultFun(SANE_STATUS_DEVICE_BUSY);
                status = SANE_STATUS_DEVICE_BUSY;
                break;
            }
            else if(status == STATUS_IO_ERROR)
            {
                continue;
            }
            /*
            else {
                 qDebug("sane_start return is %d\n",status);
                 g_ScanResultFun(status);
                 break;
            }*/
            break;
        }
        //sleep(1);
       // status = sane_set_io_mode(sane_handle,SANE_TRUE);
        //qDebug("sane_set_io_mode status is %d\n",status);
        status = scan_it();

        switch (status)
        {
            case SANE_STATUS_GOOD:
            case SANE_STATUS_EOF:
                 {
                      status = SANE_STATUS_GOOD;
                  }
                  break;
            default:
                  break;
        }
        if(bIsFloat)
        {

            break;
        }
        bIsFirstScan = true;

    }while(1);
    if (SANE_STATUS_GOOD != status)
    {
        sane_cancel (sane_handle);
    }
    //if (buffer)
   // {
     //   free (buffer);
       // buffer = NULL;
    //}
    return status;
    /*
    //qDebug("do scan start\n");
    g_nImageBufferCount = 0;
    bIsFirstScan = false;
    SANE_Status status;
    buffer_size= (32*1024);
    buffer = (SANE_Byte*)malloc(buffer_size);
   do
    {
        g_nImageBufferCount++;
        qDebug("g_nImageBufferCount is %d\n",g_nImageBufferCount);
        status =sane_start(g_sane_handle);

        if(status !=SANE_STATUS_GOOD)
        {
            qDebug("sane_start return SANE_STATUS_GOOD\n");
            if(status == SANE_STATUS_NO_DOCS)
            {
                qDebug("sane_start return SANE_STATUS_NO_DOCS\n");
                if(!bIsFirstScan)
                {
                    g_ScanResultFun(SANE_STATUS_NO_DOCS);
                    status= SANE_STATUS_NO_DOCS;
                }

                break;
            }
            else if(status == SANE_STATUS_DEVICE_BUSY)
            {
                qDebug("sane_start return SANE_STATUS_DEVICE_BUSY\n");
                g_ScanResultFun(SANE_STATUS_DEVICE_BUSY);
                status= SANE_STATUS_DEVICE_BUSY;
                break;
            }
            break;
        }
        status = scan_it();

        switch (status) {
            case SANE_STATUS_GOOD:
            case SANE_STATUS_EOF:
                status = SANE_STATUS_GOOD;
                break;
        default:
            break;
        }

        if(bIsFloat)
        {

            break;
        }
        bIsFirstScan = true;
    //}
    }while(1);
    if(SANE_STATUS_GOOD != status)
    {
        //status = SANE_STATUS_GOOD;
        sane_cancel(g_sane_handle);
    }
    free(buffer);
   return status;
   */
}

/*
** dec：扫描
** param：nDevIndex-设备号，DevRecvFun-获取扫描图像的回调，ScanResultFun-扫描返回状态）
** reurn：0表示成功，其他返回值详见错误码
*/
long CSaneSDDK::Scan(long nDevIndex,DevScanReciveCallback DevRecvFun,ScanResultCallback ScanResultFun){
    g_sane_handle = sane_handle;
    g_DevRecvFun = DevRecvFun;
    g_ScanResultFun = ScanResultFun;
    return do_scan();
}
/*
** dec：取消当前设备扫描
** param：nDevIndex-设备号
** reurn：0表示成功，其他返回值详见错误码
*/
long CSaneSDDK::CancelScan(long nDevIndex)
{
    sane_cancel(sane_handle);
    return SANE_STATUS_GOOD;

}
/*
** dec：关闭扫描
** param：nDevIndex-设备号
** reurn：0表示成功，其他返回值详见错误码
*/
long CSaneSDDK::CloseDev(long nDevIndex){
    qDebug("CloseDev\n");
      sane_close(sane_handle);
      qDebug("CloseDev 1\n");
      //delete sane_handle;
      qDebug("CloseDev 2\n");
      //sane_handle = NULL;
      qDebug("CloseDev end\n");
    return SANE_STATUS_GOOD;
}
long CSaneSDDK::ExitDev()
{
    sane_exit();
     return SANE_STATUS_GOOD;
}

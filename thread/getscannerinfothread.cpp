#include "getscannerinfothread.h"
//#include <QThread>
#include <QDebug>
#include "camscansdk.h"

GetScannerInfoThread::GetScannerInfoThread(QStringList devIndexList,QObject *parent) : QObject(parent)
{
    _devIndexList = devIndexList;
    isStop = false;
}

void GetScannerInfoThread::closeThread()
{
    isStop = true;
}


/*
 * 通过sdk获取设备信息，将信息发送给主线程
 */
void GetScannerInfoThread::slotStartThread()
{
    if(isStop) return;
    Init();//初始化SANE

    long nDevCount = -1;
    long nResult = GetDevCount(nDevCount);//获取设备数量
    if(nResult != 0 && nDevCount <= 0)
    {
        //emit signalNoScanner();//无设备，发送信号给主线程，主线程绘制无设备的UI，并结束线程
        emit signalOver();//结束线程
        return;
    }
    else
    {
        //清空设备信息参数集合
        QMap<int ,QStringList>::iterator it;
        for(it = devInfoMap.begin();it!=devInfoMap.end();it++)
        {
            if((it.key()%2)==0)
                devInfoMap.erase(it);
        }

        //获取所有设备信息,以及第一个设备的参数信息，将数据发送给主线程
        char szDevKanas[256] = {0},szDevVendor[256] = {0},szDevModel[256]={0},szDevType[256]={0};
        for (int i = 0;i < nDevCount;i++)
        {
             //数据示例：
             //szDevKanas = kanas:Kanas
             //szDevVendor = Uniscan
             //szDevModel = F3135
             //szDevType = ADF scanner
            GetDevName(i,szDevKanas,szDevVendor,szDevModel,szDevType);
            qDebug("GetDevName is: %d,%s,%s,%s,%s",i,szDevKanas,szDevVendor,szDevModel,szDevType);

            //加设备Item
            char szNum[512]={0};
            strcpy(szNum,szDevVendor);
            strcat(szNum," ");
            strcat(szNum,szDevModel);

            //如：<1,"扫描仪,UNIS F3135,空闲,设备下标">
            QStringList devInfo;
            devInfo<<szDevType<<szNum<<"0"<<QString::number(i);
            devInfoMap.insert(i,devInfo);//参数加入集合

        }

        //需要传递的扫描仪数据
        QVariant qvDevInfoMap; //申明通用数据对象
        qvDevInfoMap.setValue(devInfoMap); //数据包装
        emit signalScannerInfo(qvDevInfoMap,"abc");//发送信号给主线程，主线程绘制设备信息和参数UI
    }
}

/*
 * 获取指定设备参数
 * devIndex=设备下标index
 */
void GetScannerInfoThread::slotGetDevicePar()
{
    for (int i=0;i<_devIndexList.size();i++)
    {
       int _scannerIndex = _devIndexList.at(i).toInt();
       int nResult = OpenDev(_scannerIndex);//打开设备
       if(nResult != 0)
       {
           emit signalNoScanner();//无设备，发送信号给主线程，主线程绘制无设备的UI，并结束线程
           return;
       }

        //清空参数集合
       /*
       QMap<int ,QMap<QString ,QStringList>>::iterator it;
       for(it = devParMap.begin();it!=devParMap.end();it++)
       {
           if((it.key()%2)==0)
                devParMap.erase(it);
       }
       */

        //获取设备参数数量
        long nCapCount = -1;
        long nDev = _scannerIndex;
        nResult = GetCapCount(nDev,nCapCount);
        if(nResult == 0 && nCapCount > 0)
        {
            //暂时nCapCount-1，获取到11个参数，会崩溃，提示数组越界 vector 20200310
            for(int j=0;j<nCapCount-1;j++)
            {
                QStringList parvalue ;//加入UI的参数值集合
                int nCtrlType = 1;//参数类型(0=文本框，1=下拉框))

                char szCapName[256]={0};//参数名
                long nType = -1;//参数类型
                void* szdata = NULL;//参数数据
                GetCapInfo(0,j,szCapName,nType,&szdata);
                qDebug("\n参数名:%s,参数类型:%d",szCapName,nType);

                long nParValueCount = -1;//参数值数量
                long longMin=-1,longMax=-1,longQuant=-1;//范围值
                long longParValue = 0;//long型参数值
                char stringParValue[256]={0};//string型参数值
                //nType 0=预览 1=范围类型 最大值 最小值等；2=long ；3=char*
                switch (nType)
                {
                    case 0:

                    break;
                    case 1:
                        AnalysisRangeCapData(nType,szdata,longMin,longMax,longQuant);
                        qDebug("范围型参数值：最小：%d，最大：%d,最佳：%d",longMin,longMax,longQuant);
                        parvalue<< QString::number(longMin)+","+QString::number(longMax)+","+QString::number(longQuant);
                        nCtrlType = 0;
                    break;
                    case 2:
                        AnalysisWordCapDataCount(nType,szdata,nParValueCount);
                        for (int i = 0;i <nParValueCount;i++)
                        {
                            AnalysisWordCapData(i,longParValue);
                            qDebug("int型参数值：%d",longParValue);
                            parvalue<< QString::number(longParValue);
                        }
                        nCtrlType = 1;
                    break;
                    case 3:
                        AnalysisStringCapDataCount(nType,szdata,nParValueCount);
                        for (int i = 0;i <nParValueCount;i++)
                        {
                            AnalysisStringCapData(i,stringParValue);
                            //QString d=  GlobalHelper::readSettingValue("lan",QLatin1String(stringParValue));
                            QString d=  QLatin1String(stringParValue);
                            parvalue<< d;
                            qDebug()<<"string型参数值："<<d;
                        }
                        nCtrlType = 1;
                    break;
                }//end  switch (nType)

                //设备参数UI
                QString title = "";
                bool isAddTitleTopMargin = false;
                bool isLastPar=false;
                if(j==0)
                {
                    title = "扫描设置";
                }
                else
                {
                    title="";
                }
                if(j>=nCapCount-1)
                {
                    isLastPar=true;
                }

                /*
                 * 设备参数UI,显示时要根据配置文件翻译一下
                 * titleName=抬头名称(如：扫描设置),
                 * parName=参数名称(如：分辨率)，
                 * parIndex=参数下标，从设备中读取到的顺序
                 * parType=参数类型(0=文本框，1=下拉框))
                 * parValue=参数值（如：200DPI<<300DPI）
                 * isAddTitleTopMargin=抬头是否需要加入上边距
                 * isLastPar=是否为最后一个参数,布局需要加一个addStretch来控制参数行居上
                 */
                QMap<QString,QStringList> tmpMap ;
                tmpMap.insert(szCapName,parvalue);
                devParMap.insert(j,tmpMap);//参数加入集合

            }//end  for(int j=0;j<nCapCount-1;j++)

            //需要传递的数据
            QVariant DataVar; //申明通用数据对象
            DataVar.setValue(devParMap); //数据包装
            emit signalScannerParInfo(DataVar,QString::number(_scannerIndex));//发送信号给主线程，主线程绘制设备信息和参数UI

        }//end if(nResult == 0 && nCapCount > 0)

    }//end for
    emit signalParOver();
}



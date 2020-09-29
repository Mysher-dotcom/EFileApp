
#include "Jpeg.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "jpeglib.h"
#include "jconfig.h"

#include <setjmp.h>
#include <QFile>
#define SOI 0xD8                              //图像开始
#define APP0 0xE0                             //JFIF应用数据块
#define APP1 0xE1                            //Exif数据块
#define BYTE unsigned char
CJpeg::CJpeg(void)
{
}


CJpeg::~CJpeg(void)
{
}

void CJpeg::compresss_JPEG(uchar * & pOutJpgImg,unsigned long & outSize, uchar *inRgbImg,int image_width,
    int image_height,int nchannels, int quality /*= 90*/)
{
    struct jpeg_compress_struct cinfoDecodec;
    struct jpeg_error_mgr jerr;
    uchar* inImageBuffer =  inRgbImg;
    //uchar* outbuffer = NULL;
    JSAMPROW row_pointer[1];
    int row_stride;

    inImageBuffer = inRgbImg;
    cinfoDecodec.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfoDecodec);
    jpeg_mem_dest (&cinfoDecodec, &pOutJpgImg, &outSize);
    cinfoDecodec.image_width = image_width;
    cinfoDecodec.image_height = image_height;

    cinfoDecodec.input_components = nchannels;
    if (cinfoDecodec.input_components == 3)
    {
        cinfoDecodec.in_color_space = JCS_RGB;
    }
    else if (cinfoDecodec.input_components == 1)
    {
        cinfoDecodec.in_color_space = JCS_GRAYSCALE;
    }
    jpeg_set_defaults(&cinfoDecodec);
    jpeg_set_quality(&cinfoDecodec, quality, (boolean)TRUE);
    jpeg_start_compress(&cinfoDecodec, (boolean)TRUE);
    row_stride = image_width * nchannels;
    while (cinfoDecodec.next_scanline < cinfoDecodec.image_height)
    {
        row_pointer[0] = & inImageBuffer[cinfoDecodec.next_scanline * row_stride];
        (void) jpeg_write_scanlines(&cinfoDecodec, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfoDecodec);

    //pOutJpgImg = new uchar[outSize];
    //memcpy(pOutJpgImg,outbuffer,outSize);

    //for(unsigned long i = 0; i < outSize; i++)
    //{
    //	pOutJpgImg[i] = outbuffer[i];
    //}
    jpeg_destroy_compress(&cinfoDecodec);

    //if(NULL != outbuffer)
    //{
    //	free(outbuffer);
    //	outbuffer = NULL;
    //}
}


void CJpeg::GetString(uchar * out,int size)
{
    memcpy(out,app1Buf+app1Index,size);
    app1Index+=size;
}
void CJpeg::GetString(uchar * out,int size,int offset)
{
    memcpy(out,app1Buf+app1Index+offset,size);
}
DWORD StringToDWORD(uchar * str,bool isLittleEndian)
{
    if(isLittleEndian)
        return ((BYTE)str[3]<<24)+((BYTE)str[2]<<16)+((BYTE)str[1]<<8)+(BYTE)str[0];
    else
        return ((BYTE)str[0]<<24)+((BYTE)str[1]<<16)+((BYTE)str[2]<<8)+(BYTE)str[3];
}
WORD StringToWORD(uchar * str,bool isLittleEndian)
{
    return isLittleEndian ? ((BYTE)str[1]<<8)+(BYTE)str[0] : ((BYTE)str[0]<<8)+(BYTE)str[1];
}



void CJpeg::ReadIFD(uchar * app1Buf,int offset,bool isLittleEndian)
{
    uchar ifddata[6] = {0};
    GetString(ifddata,2, offset);
    WORD numOfIFD = StringToWORD(ifddata,isLittleEndian);//ifd个数
    offset += 2;
    for(int i=0;i<numOfIFD;i++)
    {

        TIFDEntry * entry=new TIFDEntry;
        memset(entry,0,sizeof(TIFDEntry));
        uchar data[4];
        GetString(data,2,offset);
        entry->tag=StringToWORD(data,isLittleEndian);
        GetString(data,2,offset+2);
        entry->type=StringToWORD(data,isLittleEndian);
        GetString(data,4,offset+4);
        entry->size=StringToDWORD(data,isLittleEndian);
        GetString(data,4,offset+8);
        entry->value=StringToDWORD(data,isLittleEndian);
        this->entries.push_back(entry);
        offset += 12;
    }

    //GetString(ifddata,4, offset);
    //DWORD offsetOfNext=StringToDWORD(ifddata,isLittleEndian);
    //if(offsetOfNext!=0)
    //	ReadIFD(app1Buf,offset + offsetOfNext,isLittleEndian);
}

string CJpeg::GetEntryASCII(TIFDEntry * entry)
{
    string ret;
    if(entry->type!=2)
        return ret;
    uchar * des=new uchar[entry->size];
    GetString(des,entry->size,entry->value);
    ret=(char*)des;
    delete des;
    return ret;
}
string CJpeg::GetEntryUndefined(TIFDEntry * entry)
{
    string ret;
    if(entry->type!=7)
        return ret;
    uchar * des=new uchar[entry->size];
    if(entry->size>4)
    {
        GetString(des,entry->size,entry->value);
        ret=(char*)des;
    }
    delete des;
    return ret;
}
Sfraction CJpeg::GetEntrySfraction(TIFDEntry * entry)
{
    Sfraction ret;
    ret.a=0,ret.b=1;
    if(entry->type!=10)
        return ret;
    uchar data[4];
    GetString(data,4,entry->value);
    ret.a=(data[3]<<24)+(data[2]<<16)+(data[1]<<8)+data[0];
    GetString(data,4,entry->value+4);
    ret.b=(data[3]<<24)+(data[2]<<16)+(data[1]<<8)+data[0];
    return ret;
}
Ufraction CJpeg::GetEntryUfraction(TIFDEntry * entry)
{
    Ufraction ret;
    ret.a=0,ret.b=1;
    if(entry->type!=5)
        return ret;
    uchar data[4];
    GetString(data,4,entry->value);
    ret.a=StringToDWORD(data,isLittleEndian);
    GetString(data,4,entry->value+4);
    ret.b=StringToDWORD(data,isLittleEndian);
    return ret;
}

void CJpeg::AnalyseTIFD(TIFDEntry * entry,JPEGInfo &info)
{
    switch(entry->tag)
    {
    case 0x010E:                   // 图像说明
       info.description=GetEntryASCII(entry);
       break;
    case 0x010F:             // 制造厂商
       info.maker=GetEntryASCII(entry);
       break;
    case 0x0110:             // 型号
       info.model=GetEntryASCII(entry);
       break;
    case 0x011A:             // x分辩率
       info.xResolution=GetEntryUfraction(entry);
       break;
    case 0x011B:             // y分辩率
       info.yResolution=GetEntryUfraction(entry);
       break;
    case 0x0128:             // 分辩率单位
       info.resolutionUnit=(WORD)entry->value;
       break;
    case 0x0131:                   // 创建软件名称
       info.software=GetEntryASCII(entry);
       break;
    case 0x0132:             // 创建时间
       info.createTime=GetEntryASCII(entry);
       break;
    case 0x0213:             // YCbCr位置
       info.YCbCrPosition=(WORD)entry->value;
       break;
    case 0x8298:             // 版权信息
       info.copyright=GetEntryASCII(entry);
       break;
    case 0x8769:             // Exif末尾
       ReadIFD(app1Buf,entry->value,isLittleEndian);
       break;
    case 0x0103:             // 压缩信息
       info.compression=(WORD)entry->value;
       break;
    case 0x829A:             // 曝光时间
       info.exposureTime=GetEntryUfraction(entry);
       break;
    case 0x829D:             // F-值
       info.fNumber=GetEntryUfraction(entry);
       break;
    case 0x8822:             // 曝光设定
       info.exposureProgram=(WORD)entry->value;
       break;
    case 0x8827:             // ISO速率
       info.ISOSpeedRatings=(WORD)entry->value;
       break;
    case 0x9003:             // 拍摄时间
       info.orgTime=GetEntryASCII(entry);
       break;
    case 0x9004:             // 被软件修改的时间
       info.digTime=GetEntryASCII(entry);
       break;
    case 0x9102:             // 每像素压缩位数
       info.compressBit=GetEntryUfraction(entry);
       break;
    case 0x9201:             // 快门速度
       info.shutterSpeed=GetEntrySfraction(entry);
       break;
    case 0x9202:             // 光圈值
       info.aperture=GetEntryUfraction(entry);
       break;
    case 0x9204:             // 曝光补偿值
       info.exposureBias=GetEntrySfraction(entry);
       break;
    case 0x9205:             // 最大光圈
       info.maxAperture=GetEntryUfraction(entry);
       break;
    case 0x9207:             // 测光模式
       info.meteringMode=entry->value;
       break;
    case 0x9208:             // 光源
       info.lightSource=entry->value;
       break;
    case 0x9209:             // 闪光灯
       info.flash=entry->value;
       break;
    case 0x920a:             // 焦距
       info.focalLength=GetEntryUfraction(entry);
       break;
    /*
    case 0x927c:
       makerNote=GetEntryUndefined(entry);
       cout << makerNote << endl;
       break;
    */
    case 0xa001:             // 色彩空间
       info.colorSpace=(WORD)entry->value;
       break;
    case 0xa002:             // Exif宽度
       info.width=entry->value;
       break;
    case 0xa003:             // Exif高度
       info.height=entry->value;
       break;
    case 0xa215:             // 曝光指数
       info.exposureIndex=GetEntryUfraction(entry);
       break;
    case 0xa217:
       info.sensingMethod=entry->value;
       break;
    }
}


//CFile file1;
//file1.Open(L"D:\\1.jpg",CFile::modeRead,NULL,NULL);
//DWORD len=file1.GetLength();
//uchar*srcJPGBuff = new uchar[len + 1];
//srcJPGBuff[len]=0;
//file1.Read(srcJPGBuff,len);
//CJpeg jpeg;
//JPEGInfo jpegInfo;
//jpeg.getImageToJpeg(srcJPGBuff,jpegInfo);
//delete srcJPGBuff;
long CJpeg::getJPEGInfoFromImage(uchar * src,JPEGInfo &jpgInfo)
{
    int readIndex = 0;
    uchar * title = src;
    memset(&jpgInfo,0,sizeof(JPEGInfo));
    //判断是否是JPG
    if ((BYTE)title[readIndex]!=0xFF | (BYTE)title[readIndex + 1]!=SOI)
    {
        return -1;
    }
    //读取APP0
    readIndex += 2;
    if((BYTE)title[readIndex]==0xFF && (BYTE)title[readIndex + 1]==APP0)
    {
        jpgInfo.xResolution.b = 1000;
        jpgInfo.yResolution.b = 1000;
        WORD xDPI,yDPI;
        xDPI = StringToWORD(title+0xe,false);
        yDPI = StringToWORD(title+0x10,false);

        jpgInfo.xResolution.a = jpgInfo.xResolution.b * xDPI;
        jpgInfo.yResolution.a = jpgInfo.yResolution.b * yDPI;;

        readIndex += 2;
        int app0length=0;
        app0length=((BYTE)title[readIndex]<<8)+title[readIndex + 1];
        readIndex += app0length ;
    }
    return 0;
    //读取APP1
    if((BYTE)title[readIndex ++]!=0xFF | (BYTE)title[readIndex ++]!=APP1)
        return -2;//没有APP1信息

    int app1length = ((BYTE)title[readIndex]<<8)+title[readIndex + 1] - 2;
    readIndex += 2;
    app1Buf = new uchar[app1length];
    memset(app1Buf,0,app1length);
    memcpy(app1Buf,(uchar*)title + readIndex,app1length);
    app1Index = 0;
    uchar data[10] = {0};
    GetString(data,6);							//"exif"00
    GetString(data,8,0);
    if(data[0]=='I' && data[1]=='I')            //II形式
        isLittleEndian=true;
    else                                        //MM形式
        isLittleEndian=false;
    //data[2] data[3] Flag(0x2A)				 //fixed
    DWORD offset = StringToDWORD(data + 4,isLittleEndian); //第一个IFD的偏移量data[4] - data[7]
    ReadIFD(app1Buf,offset,isLittleEndian );

    while(entries.size()!=0)
    {
        TIFDEntry *entry=entries[entries.size()-1];
        entries.pop_back();
        AnalyseTIFD(entry,jpgInfo);
        delete entry;
    }
    delete app1Buf;
}


void DWORDToString(uchar * str,DWORD dvalue,bool isLittleEndian)
{
    if(isLittleEndian)
    {
        str[3] = dvalue >> 24;
        str[2] = dvalue << 8 >> 24;
        str[1] = dvalue  << 16 >> 24;
        str[0] = dvalue << 24 >> 24;
    }
    else
    {
        str[0] = dvalue >> 24;
        str[1] = dvalue << 8 >> 24;
        str[2] = dvalue  << 16 >> 24;
        str[3] = dvalue << 24 >> 24;
    }
}
void WORDToString(uchar * str,DWORD wvalue,bool isLittleEndian)
{
    if (isLittleEndian)
    {
        str[1] = wvalue >> 8;
        str[0] = wvalue << 8 >> 8;
    }
    else
    {
        str[0] = wvalue >> 8;
        str[1] = wvalue << 8 >> 8;
    }
}

void SetEXIFType(uchar * str,int ntype)
{
    switch(ntype)
    {
    case 0:
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    case 5:
        break;
    }
}

long CJpeg::makeEXIFBuf(uchar * exifbuf,long &length,JPEGInfo jpgInfo)
{
    long exifBufIndex = 0;
    exifbuf[0] = 0xff;exifbuf[1] = APP1;//标识符 2个uchar
    DWORD temp = length >> 8;
    exifbuf[2] = (DWORD)(length-2) >> 8;exifbuf[3] = (DWORD)(length-2) << 8 >> 8;//size 2个uchar
    //exif标识 6个
    uchar exifString[6] = "Exif";
    memcpy(exifbuf + 4,exifString,6);
    //TIFF 信息 8个uchar
    exifbuf[10] = 'M';exifbuf[11] = 'M';exifbuf[12] = 0;exifbuf[13] = 0x2a;
    DWORDToString((uchar *)(exifbuf + 14),0x8,false);

    //ifd count 2个uchar
    long ifdCount = 6;
    WORDToString((uchar *)(exifbuf + 18),ifdCount,false);


    exifBufIndex = 2 + 2 + 6 + 8 + 2; //ifd0 真实具体信息存储地址 标识符 + size + exif标识符 + Tiff + size
    long ifdInfoOffset = 8 + 2 + 12 * ifdCount + 4; //Tiff + size + ifdSize + endbuff

    uchar ifdU[12] = {0};
    DWORD ifdID = 0;

    memset(ifdU,0,12);
    ifdID = 0x8298;	//版权
    WORDToString((uchar *)ifdU,ifdID,false);
    ifdU[2] = 0;ifdU[3] = 2;
    DWORDToString((uchar *)(ifdU + 4),jpgInfo.copyright.size() + 1,false);
    DWORDToString((uchar *)(ifdU + 8),ifdInfoOffset,false);
    memcpy(exifbuf + exifBufIndex,ifdU,12);exifBufIndex += 12;

    memcpy(exifbuf + ifdInfoOffset + 10,jpgInfo.copyright.c_str(),jpgInfo.copyright.size() + 1);
    ifdInfoOffset += (jpgInfo.copyright.size() + 1);

    memset(ifdU,0,12);
    ifdID = 0x0131;		 // 创建软件名称
    WORDToString((uchar *)ifdU,ifdID,false);
    ifdU[2] = 0;ifdU[3] = 2;
    DWORDToString((uchar *)(ifdU + 4),jpgInfo.software.size() + 1,false);
    DWORDToString((uchar *)(ifdU + 8),ifdInfoOffset,false);
    memcpy(exifbuf + exifBufIndex,ifdU,12);exifBufIndex += 12;

    memcpy(exifbuf + ifdInfoOffset + 10,jpgInfo.software.c_str(),jpgInfo.software.size() + 1);
    ifdInfoOffset += (jpgInfo.software.size() + 1);


    memset(ifdU,0,12);
    ifdID = 0x010F;		//制造厂商
    WORDToString((uchar *)ifdU,ifdID,false);
    ifdU[2] = 0;ifdU[3] = 2;
    DWORDToString((uchar *)(ifdU + 4),jpgInfo.maker.size() + 1,false);
    DWORDToString((uchar *)(ifdU + 8),ifdInfoOffset,false);
    memcpy(exifbuf + exifBufIndex,ifdU,12);exifBufIndex += 12;

    memcpy(exifbuf + ifdInfoOffset + 10,jpgInfo.maker.c_str(),jpgInfo.maker.size() + 1);
    ifdInfoOffset += (jpgInfo.maker.size() + 1);

    memset(ifdU,0,12);
    ifdID = 0x010E;		//图像说明
    WORDToString((uchar *)ifdU,ifdID,false);
    ifdU[2] = 0;ifdU[3] = 2;
    DWORDToString((uchar *)(ifdU + 4),jpgInfo.description.size() + 1,false);
    DWORDToString((uchar *)(ifdU + 8),ifdInfoOffset,false);
    memcpy(exifbuf + exifBufIndex,ifdU,12);exifBufIndex += 12;

    memcpy(exifbuf + ifdInfoOffset + 10,jpgInfo.description.c_str(),jpgInfo.description.size() + 1);
    ifdInfoOffset += (jpgInfo.description.size() + 1);


    memset(ifdU,0,12);
    ifdID = 0x011A;				  // YDPI
    WORDToString((uchar *)ifdU,ifdID,false);
    ifdU[2] = 0;ifdU[3] = 5;
    DWORDToString((uchar *)(ifdU + 4),1,false);
    DWORDToString((uchar *)(ifdU + 8),ifdInfoOffset,false);
    memcpy(exifbuf + exifBufIndex,ifdU,12);exifBufIndex += 12;
    if (jpgInfo.yResolution.b == 0)
    {
        jpgInfo.yResolution.b = 1000;
    }
    if (jpgInfo.yResolution.a == 0)
    {
        jpgInfo.yResolution.a = 200000;
    }
    DWORDToString((uchar *)(exifbuf + ifdInfoOffset + 10),jpgInfo.yResolution.a,false);
    DWORDToString((uchar *)(exifbuf + ifdInfoOffset + 10 + 4),jpgInfo.yResolution.b,false);
    ifdInfoOffset += 8;


    memset(ifdU,0,12);
    ifdID = 0x011B;				  // XDPI
    WORDToString((uchar *)ifdU,ifdID,false);
    ifdU[2] = 0;ifdU[3] = 5;
    DWORDToString((uchar *)(ifdU + 4),1,false);
    DWORDToString((uchar *)(ifdU + 8),ifdInfoOffset,false);
    memcpy(exifbuf + exifBufIndex,ifdU,12);exifBufIndex += 12;

    if (jpgInfo.xResolution.b == 0)
    {
        jpgInfo.xResolution.b = 1000;
    }
    if (jpgInfo.xResolution.a == 0)
    {
        jpgInfo.xResolution.a = 200000;
    }
    DWORDToString((uchar *)(exifbuf + ifdInfoOffset + 10),jpgInfo.xResolution.a,false);
    DWORDToString((uchar *)(exifbuf + ifdInfoOffset + 10 + 4),jpgInfo.xResolution.b,false);
    ifdInfoOffset += 8;


    //memset(ifdU,0,12);
    //ifdID = 0x0103;				  // 压缩
    //WORDToString((uchar *)ifdU,ifdID,false);
    //ifdU[2] = 0;ifdU[3] = 4;
    //DWORDToString((uchar *)(ifdU + 4),1,false);
    //DWORDToString((uchar *)(ifdU + 8),jpgInfo.compression,false);
    //memcpy(exifbuf + exifBufIndex,ifdU,12);exifBufIndex += 12;

    memset(exifbuf + exifBufIndex,0,4);


    return 0;
}

long CJpeg::saveImageToJpeg(uchar * src,int width,int height,int bpp,char * filePath,JPEGInfo jpegInfo)
{
    long exifBufflength = 400; //带标志位长度TRUE
    long jfifbufflength = 18; //带标志位长度
    long bufSize = width * height;

    unsigned long outputsize = 0;
    uchar * jpgBuf = NULL;
    compresss_JPEG(jpgBuf,outputsize,src,width,height,bpp / 8,jpegInfo.compression);
    if (jpgBuf == NULL)
    {
        return -1;
    }
    uchar * desJpgBuf = new uchar[outputsize + exifBufflength];
    memcpy(desJpgBuf + exifBufflength,jpgBuf,outputsize);
    free(jpgBuf);

    uchar * exifBuff = new uchar[exifBufflength];
    memset(exifBuff,0,exifBufflength);
    //makeEXIFBuf(exifBuff,exifBufflength,jpegInfo);
    //拷贝标志位 + jfif信息TRUE
    memcpy(desJpgBuf,desJpgBuf + exifBufflength ,jfifbufflength + 2);
    //拷贝exif信息
    memcpy(desJpgBuf + jfifbufflength + 2,exifBuff,exifBufflength);

    //jfif写DPI

    desJpgBuf[0xd] = 1;
    if (jpegInfo.xResolution.b != 0 && jpegInfo.yResolution.b != 0)
    {
        long nXResolution = jpegInfo.xResolution.a / jpegInfo.xResolution.b;
        long nYResolution = jpegInfo.yResolution.a / jpegInfo.yResolution.b;
        WORDToString((uchar *)(desJpgBuf + 0xe),nXResolution,false);
        WORDToString((uchar *)(desJpgBuf + 0x10),nYResolution,false);
    }

    delete exifBuff;


    QFile file(filePath);
            if(file.open(QFile::ReadWrite | QFile::Append)){
                file.write((char*)desJpgBuf,exifBufflength + outputsize);
                file.close();
            }
    delete desJpgBuf;
    return true;
}

long CJpeg::readBufFromJpegBuf(uchar * jpgBuff,long ljpgBuffSize,long &lWidth,long &lHeight,uchar * * bgrBuff)
{

   //  printf("readBufFromJpegBuf start");
     JPEGInfo jpgInfo;
    if (jpgBuff == NULL || ljpgBuffSize == 0)
    {
        return -1;
    }
    int readIndex = 0;
    //判断是否是JPG
    if ((BYTE)jpgBuff[readIndex]!=0xFF | (BYTE)jpgBuff[readIndex + 1]!=SOI)
    {
        return -1;
    }
    //if(getJPEGInfoFromImage(jpgBuff,jpgInfo) == -1)//不是JPG图片
  //  {
   //     return -1;
   // }

    bool bret = false;
   // printf("readBufFromJpegBuf start 1");
    struct jpeg_decompress_struct cinfo;//JPEG图像在解码过程中，使用jpeg_decompress_struct类型的结构体来表示，图像的所有信息都存储在结构体中
    struct jpeg_error_mgr jerr;//定义一个标准的错误结构体，一旦程序出现错误就会调用exit()函数退出进程
    cinfo.err = jpeg_std_error(&jerr);//绑定错误处理结构对象
   // printf("readBufFromJpegBuf start 2");
    jpeg_create_decompress(&cinfo);//初始化cinfo结构
   //  printf("readBufFromJpegBuf start 3");
    jpeg_mem_src(&cinfo,jpgBuff,ljpgBuffSize);//指定解压缩数据源
    bret = jpeg_read_header(&cinfo,(boolean)TRUE);//获取文件信息
   //  printf("readBufFromJpegBuf start 3");
    if (bret == false)
    {
        jpeg_destroy_decompress(&cinfo);// 释放资源
        delete jpgBuff;
        return -1;
    }
 //printf("readBufFromJpegBuf start 4");
    //if (cinfo.num_components == 3)
    {
        cinfo.out_color_space = JCS_RGB;
    }

  //  cinfo.do_fancy_upsampling = (boolean)TRUE;
   // cinfo.do_block_smoothing = (boolean)TRUE;
    //cinfo.dct_method = JDCT_FLOAT;
    //cinfo.dither_mode = JDITHER_FS;
     cinfo.do_fancy_upsampling = (boolean)FALSE;
     cinfo.do_block_smoothing = (boolean)FALSE;
     cinfo.dct_method = JDCT_FASTEST;
     cinfo.dither_mode = JDITHER_NONE;
 //printf("readBufFromJpegBuf start 4");
    if(lWidth>0)
    {
        lWidth = lWidth > cinfo.image_width?cinfo.image_width:lWidth;
        cinfo.scale_num = lWidth;
        cinfo.scale_denom = cinfo.image_width;
    }
    bret = jpeg_start_decompress(&cinfo);//开始解压缩
    if (bret == false)
    {
        jpeg_destroy_decompress(&cinfo);// 释放资源
        delete jpgBuff;
        return -1;
    }
  //   printf("readBufFromJpegBuf start 5");
     unsigned long width = cinfo.output_width;
     unsigned long height = cinfo.output_height;
     lWidth = width;
     lHeight = height;
    jpgInfo.width = cinfo.output_width;//图像宽度
    jpgInfo.height = cinfo.output_height;//图像高度
    jpgInfo.colorSpace = cinfo.output_components;
    unsigned short depth = cinfo.output_components * cinfo.output_width;//图像深度
    long lbgrlength = depth * cinfo.output_height;
    if(*bgrBuff==NULL)
    {
       * bgrBuff = new uchar[lbgrlength];
    }
    //* bgrBuff = new uchar[lbgrlength];
    memset(* bgrBuff, 0, lbgrlength);//清0
    JSAMPARRAY buffer;//用于存取一行数据
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE,depth, 1);//分配一行数据空间
    unsigned char *point = * bgrBuff + depth * (cinfo.output_height - 1);
    long addStep =  - depth;

    point = * bgrBuff;
    addStep = depth;


    while(cinfo.output_scanline < cinfo.output_height)//逐行读取位图数据
    {
        jpeg_read_scanlines(&cinfo, buffer, 1); //读取一行jpg图像数据到buffer
        memcpy(point, *buffer, depth); //将buffer中的数据逐行给src_buff
        point += addStep; //指针偏移一行
    }
    //jpeg_finish_decompress(&cinfo);//解压缩完毕
    jpeg_destroy_decompress(&cinfo);// 释放资源
    return 0;

}

long CJpeg::readBufFromJpeg(char * filepath,uchar * * des_buff,JPEGInfo &jpgInfo,int desW,int desH)
{
    BYTE * jpgBuff;
    long ljpgBuffSize;
    QFile file(filepath);
            QByteArray fileContent;
            if (file.open(QIODevice::ReadOnly)){
                fileContent = file.readAll();

            }
            file.close();
            jpgBuff = (unsigned char *)fileContent.data();
    /*if(getJPEGInfoFromImage(jpgBuff,jpgInfo) == -1)//不是JPG图片
    {
        delete jpgBuff;
        return -1;
    }
*/

    bool bret = false;

    struct jpeg_decompress_struct cinfo;//JPEG图像在解码过程中，使用jpeg_decompress_struct类型的结构体来表示，图像的所有信息都存储在结构体中
    struct jpeg_error_mgr jerr;//定义一个标准的错误结构体，一旦程序出现错误就会调用exit()函数退出进程
    cinfo.err = jpeg_std_error(&jerr);//绑定错误处理结构对象
    jpeg_create_decompress(&cinfo);//初始化cinfo结构
    jpeg_mem_src(&cinfo,jpgBuff,ljpgBuffSize);//指ljpgBuffSize定解压缩数据源
    bret = jpeg_read_header(&cinfo,(boolean)TRUE);//获取文件信息
    if (bret == false)
    {
        jpeg_destroy_decompress(&cinfo);// 释放资源
        delete jpgBuff;
        return -1;
    }

    if (cinfo.num_components == 3)
    {
        cinfo.out_color_space = JCS_RGB;
    }
    else if (cinfo.num_components == 1)
    {
        cinfo.out_color_space = JCS_GRAYSCALE;
    }
    cinfo.do_fancy_upsampling = (boolean)TRUE;
    cinfo.do_block_smoothing = (boolean)TRUE;
    cinfo.dct_method = JDCT_FLOAT;
    cinfo.dither_mode = JDITHER_FS;

    if (desW > 0)
    {
        desW = desW > cinfo.image_width?cinfo.image_width: desW;
        cinfo.scale_num = desW;
        cinfo.scale_denom = cinfo.image_width  ;
    }


    bret = jpeg_start_decompress(&cinfo);//开始解压缩
    if (bret == false)
    {
        jpeg_destroy_decompress(&cinfo);// 释放资源
        delete jpgBuff;
        return -1;
    }
    jpgInfo.width = cinfo.output_width;//图像宽度
    jpgInfo.height = cinfo.output_height;//图像高度
    jpgInfo.colorSpace = cinfo.output_components;
    unsigned short depth = (cinfo.output_components * cinfo.output_width * 8  + 31)/32 * 4;//图像深度
    long lbgrlength = depth * cinfo.output_height;

    * des_buff = new uchar[lbgrlength];
    memset(* des_buff, 0, lbgrlength);//清0
    JSAMPARRAY buffer;//用于存取一行数据
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE,depth, 1);//分配一行数据空间
    unsigned char *point = * des_buff + depth * (cinfo.output_height - 1);
    long addStep =  - depth;

    point = * des_buff;
    addStep = depth;


    while(cinfo.output_scanline < cinfo.output_height)//逐行读取位图数据
    {
        jpeg_read_scanlines(&cinfo, buffer, 1); //读取一行jpg图像数据到buffer
        memcpy(point, *buffer, depth); //将buffer中的数据逐行给src_buff
        point += addStep; //指针偏移一行
    }
    //jpeg_finish_decompress(&cinfo);//解压缩完毕
    jpeg_destroy_decompress(&cinfo);// 释放资源
    jpgBuff=NULL;
    return 0;
}

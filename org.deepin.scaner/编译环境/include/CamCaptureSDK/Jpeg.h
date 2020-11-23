#pragma once
#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string>
#include <vector>
using namespace std;
typedef unsigned char uchar;


#define DWORD unsigned long
#define WORD unsigned int
struct Ufraction
{
	DWORD a; //分子
	DWORD b; //分母
};
struct Sfraction
{
	long a; //分子
	long b; //分母
};
struct JPEGInfo
{
	string description;            //图像描述
	string maker;                  //制造厂商
	string model;                  //照相机型号
	Ufraction xResolution;         //x分辨率
	Ufraction yResolution;         //y分辨率
	WORD resolutionUnit;           //分辨率单位,参数含义详见TIFF标准
	string software;               //创建该图像的软件名和软件版本
	string createTime;             //文件创建时间,格式为YYYY-MM-DD HH:MM:SS
	WORD YCbCrPosition;            //YCbCr位置,参数含义详见TIFF标准
	string copyright;              //版权信息
	WORD compression;              //压缩信息,参数含义详见TIFF标准
	Ufraction exposureTime;        //曝光时间
	Ufraction fNumber;             //F-值
	WORD exposureProgram;          //曝光设定
	WORD ISOSpeedRatings;          //ISO速率
	string orgTime;                //拍摄时间
	string digTime;                //图像被软件修改的时间
	Ufraction compressBit;         //每像素压缩位数
	Sfraction shutterSpeed;        //快门速度,例如1/30秒
	Ufraction aperture;            //光圈值,例如F2.8
	Sfraction exposureBias;        //曝光补偿值
	Ufraction maxAperture;         //最大光圈
	WORD meteringMode;             //测光模式
	WORD lightSource;              //光源
	WORD flash;                    //闪光灯
	Ufraction focalLength;         //焦距
	WORD colorSpace;               //色彩空间
	DWORD width;                   //图像宽度
	DWORD height;                  //图像高度
	Ufraction exposureIndex;       //曝光指数
	WORD sensingMethod;            //感光方式
};
struct TIFDEntry
{
	WORD tag;                      //Tag的含义可以查阅TIFF的规范文档
	WORD type;                     //指明Entry中记录的数据类型,TIFF规范定义了5种类型,EXIF增加了3种
	DWORD size;                    //大小
	DWORD value;                   //取值,根据type含义会改变
};

class CJpeg
{
public:
	CJpeg(void);
	~CJpeg(void);

	long getJPEGInfoFromImage(uchar * src,JPEGInfo &jpgInfo);
	long saveImageToJpeg(uchar * src,int width,int height,int bpp,char * filePath,JPEGInfo jpgInfo);
    long saveImageFormJpegBuf(uchar * src,int width,int height,int bpp,uchar ** filePathBuf,JPEGInfo jpgInfo);
	long readBufFromJpeg(char * filepath,uchar * * bgrBuff,JPEGInfo &jpgInfo,int desW,int desH);
    long readBufFromJpegBuf(uchar * jpgBuf,long lsize,long &lWidth,long &lHeight,uchar * * bgrBuff);

private:
	uchar * app1Buf;
	int app1Index;
	vector<TIFDEntry *> entries;   //保存TIFD信息,详见TIFF标准
	bool isLittleEndian;
	
	long makeEXIFBuf(uchar * exifbuf,long &length,JPEGInfo jpgInfo);

	void compresss_JPEG(uchar * & pOutJpgImg,unsigned long & outSize, uchar *inRgbImg,int image_width,
		int image_height,int nchannels, int quality /*= 90*/);
	void ReadIFD(uchar * app1Buf,int offset,bool isLittleEndian);
	void AnalyseTIFD(TIFDEntry * entry,JPEGInfo &info);
	void GetString(uchar * out,int size);
	void GetString(uchar * out,int size,int offset);
	string GetEntryASCII(TIFDEntry * entry);
	string GetEntryUndefined(TIFDEntry * entry);
	Sfraction GetEntrySfraction(TIFDEntry * entry);
	Ufraction GetEntryUfraction(TIFDEntry * entry);
};


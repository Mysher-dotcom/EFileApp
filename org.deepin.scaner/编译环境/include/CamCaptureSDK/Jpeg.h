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
	DWORD a; //����
	DWORD b; //��ĸ
};
struct Sfraction
{
	long a; //����
	long b; //��ĸ
};
struct JPEGInfo
{
	string description;            //ͼ������
	string maker;                  //���쳧��
	string model;                  //������ͺ�
	Ufraction xResolution;         //x�ֱ���
	Ufraction yResolution;         //y�ֱ���
	WORD resolutionUnit;           //�ֱ��ʵ�λ,�����������TIFF��׼
	string software;               //������ͼ��������������汾
	string createTime;             //�ļ�����ʱ��,��ʽΪYYYY-MM-DD HH:MM:SS
	WORD YCbCrPosition;            //YCbCrλ��,�����������TIFF��׼
	string copyright;              //��Ȩ��Ϣ
	WORD compression;              //ѹ����Ϣ,�����������TIFF��׼
	Ufraction exposureTime;        //�ع�ʱ��
	Ufraction fNumber;             //F-ֵ
	WORD exposureProgram;          //�ع��趨
	WORD ISOSpeedRatings;          //ISO����
	string orgTime;                //����ʱ��
	string digTime;                //ͼ������޸ĵ�ʱ��
	Ufraction compressBit;         //ÿ����ѹ��λ��
	Sfraction shutterSpeed;        //�����ٶ�,����1/30��
	Ufraction aperture;            //��Ȧֵ,����F2.8
	Sfraction exposureBias;        //�عⲹ��ֵ
	Ufraction maxAperture;         //����Ȧ
	WORD meteringMode;             //���ģʽ
	WORD lightSource;              //��Դ
	WORD flash;                    //�����
	Ufraction focalLength;         //����
	WORD colorSpace;               //ɫ�ʿռ�
	DWORD width;                   //ͼ����
	DWORD height;                  //ͼ��߶�
	Ufraction exposureIndex;       //�ع�ָ��
	WORD sensingMethod;            //�йⷽʽ
};
struct TIFDEntry
{
	WORD tag;                      //Tag�ĺ�����Բ���TIFF�Ĺ淶�ĵ�
	WORD type;                     //ָ��Entry�м�¼����������,TIFF�淶������5������,EXIF������3��
	DWORD size;                    //��С
	DWORD value;                   //ȡֵ,����type�����ı�
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
	vector<TIFDEntry *> entries;   //����TIFD��Ϣ,���TIFF��׼
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


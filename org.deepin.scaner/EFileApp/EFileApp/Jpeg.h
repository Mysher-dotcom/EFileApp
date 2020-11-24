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
    DWORD a; //????
    DWORD b; //???
};
struct Sfraction
{
    long a; //????
    long b; //???
};
struct JPEGInfo
{
    string description;            //???????
    string maker;                  //??????
    string model;                  //?????????
    Ufraction xResolution;         //x?????
    Ufraction yResolution;         //y?????
    WORD resolutionUnit;           //??????��,????????????TIFF???
    string software;               //????????????????????????��
    string createTime;             //??????????,????YYYY-MM-DD HH:MM:SS
    WORD YCbCrPosition;            //YCbCr��??,????????????TIFF???
    string copyright;              //??????
    WORD compression;              //??????,????????????TIFF???
    Ufraction exposureTime;        //??????
    Ufraction fNumber;             //F-?
    WORD exposureProgram;          //????څ
    WORD ISOSpeedRatings;          //ISO????
    string orgTime;                //???????
    string digTime;                //????????????????
    Ufraction compressBit;         //????????��??
    Sfraction shutterSpeed;        //???????,????1/30??
    Ufraction aperture;            //????,????F2.8
    Sfraction exposureBias;        //??????
    Ufraction maxAperture;         //???????
    WORD meteringMode;             //??????
    WORD lightSource;              //???
    WORD flash;                    //??????
    Ufraction focalLength;         //????
    WORD colorSpace;               //?????
    DWORD width;                   //???????
    DWORD height;                  //??????
    Ufraction exposureIndex;       //??????
    WORD sensingMethod;            //?��??
};
struct TIFDEntry
{
    WORD tag;                      //Tag????????????TIFF??�Z???
    WORD type;                     //???Entry?��????????????,TIFF?�Z??????5??????,EXIF??????3??
    DWORD size;                    //??��
    DWORD value;                   //??,????type?????????
};

class CJpeg
{
public:
    CJpeg(void);
    ~CJpeg(void);

    long getJPEGInfoFromImage(uchar * src,JPEGInfo &jpgInfo);
    long saveImageToJpeg(uchar * src,int width,int height,int bpp,char * filePath,JPEGInfo jpgInfo);

    long readBufFromJpeg(char * filepath,uchar * * bgrBuff,JPEGInfo &jpgInfo,int desW,int desH);
    long readBufFromJpegBuf(uchar * jpgBuf,long lsize,long &lWidth,long &lHeight,uchar * * bgrBuff);

private:
    uchar * app1Buf;
    int app1Index;
    vector<TIFDEntry *> entries;   //????TIFD???,????TIFF???
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

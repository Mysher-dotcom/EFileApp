#ifndef DATATYPE_H
#define DATATYPE_H
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iterator>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
typedef int (*DevScanReciveCallback)(const unsigned char*data,int nSize,int nWidth,int nHeight,int nBpp,int nDPI);
typedef int (*ScanResultCallback)(int nScanResultType);
typedef struct
{
  uint8_t *data;
  int width;
  int height;
  int x;
  int y;
}
Image;
#endif // DATATYPE_H

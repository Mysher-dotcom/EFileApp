#include "stdafx.h"
#include "MImageDataPool.h"


unsigned long long CMImageDataPool::s_nIndex = 0;
bool CMImageDataPool::m_bLock = false;
int CMImageDataPool::m_nSleepTime = 20;
map<unsigned long long, cv::Mat> CMImageDataPool::s_mapMemory = CMImageDataPool::CreateMap();

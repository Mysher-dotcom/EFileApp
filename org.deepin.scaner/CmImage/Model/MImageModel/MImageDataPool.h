#pragma once
#include "stdafx.h"
#include <unistd.h>
#include <map>
using namespace std;
class CMImageDataPool
{
public:
	//current Key,when create a new memory,it will add 1
	static unsigned long long s_nIndex;
	//memory map,access the memory by appropriate key 
	typedef map<unsigned long long, cv::Mat> MapMemory;
	static MapMemory s_mapMemory;
	static bool m_bLock;
	static int m_nSleepTime;

	static void Lock()
	{
		while(m_bLock)
		{
            #ifdef _WINDOWS_
                Sleep(m_nSleepTime);
            #else
                sleep(m_nSleepTime);
            #endif

			continue;
		}
		m_bLock = true;
	}

	static void UnLock()
	{
		m_bLock = false;
	}

	//return the Key of memory
	static int Add(cv::Mat src)
	{
		Lock();
		s_nIndex ++;
		/*if(s_nIndex > ULONG_MAX)
		{
			s_nIndex = 0;
		}*/
		s_mapMemory.insert(pair<unsigned long long, cv::Mat>(s_nIndex,src));
		UnLock();
		return s_nIndex;
	};

	//if success to delet appropriate memory,return 1,else return 0
	static int Delete(unsigned long long nIndex)
	{
		Lock();
		map<unsigned long long, cv::Mat>::iterator find = s_mapMemory.find(nIndex);
		if(find != s_mapMemory.end())
		{
			pair<unsigned long long, cv::Mat> value = *find;
			value.second.release();
		}
		int n = s_mapMemory.erase(nIndex);
		UnLock();
		return n;
	}


	//if success to access appropriate memory,return available object of map,else return unavailable object
	static Mat Access(unsigned long long nIndex)
	{
		Mat dst;
		Lock();
		map<unsigned long long, cv::Mat>::iterator find = s_mapMemory.find(nIndex);
		if(find != s_mapMemory.end())
		{
			pair<unsigned long long, cv::Mat> value = *find;
			dst = value.second;
		}
		UnLock();
		return dst;
	}

	static map<unsigned long long, cv::Mat> CreateMap()
	{
		map<unsigned long long, cv::Mat> mapData;
		return mapData;
	}
};



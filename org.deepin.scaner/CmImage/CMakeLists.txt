cmake_minimum_required(VERSION 2.8.12)
project(CmImage)
# C++11 编译
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
link_directories(../../Lib/lib)

include_directories(
	../../Lib/include/freetype2
	../../Lib/include
   ../../Lib/include/opencv2
   ../../Lib/include/include
   ./
   ./ImgProc/BookProcess
   ./ImgProc/ColorSpace
   ./ImgProc/Enhancement
   ./ImgProc/Feature
   ./ImgProc/Transform
   ./Model/MImageModel
   ) 
#创建共享库
#add_library(MysherBook SHARED MysherBook.cpp MysherBook.h BookProcess.cpp BookProcess.h MImage.h
#			DetectRectByContours.h DetectRectByContours.cpp AdaptiveThreshold.h AdaptiveThreshold.cpp
#			Rotate.h Rotate.cpp Test_Book.cpp Test_Book.h Adapter.cpp Adapter.h)

add_library(
	CmImage SHARED cmimage.cpp cmimage.h MImage.h
	#./ImgProc/Libjpeg/Jpeg.cpp ./ImgProc/Libjpeg/Jpeg.h
	./Model/MImageModel/Adapter.cpp ./Model/MImageModel/Adapter.h 
	./ImgProc/BookProcess/BookProcess.cpp ./ImgProc/BookProcess/BookProcess.h
	./ImgProc/BookProcess/Test_Book.cpp ./ImgProc/BookProcess/Test_Book.h
	./ImgProc/ColorSpace/AdaptiveThreshold.cpp ./ImgProc/ColorSpace/AdaptiveThreshold.h
	./ImgProc/Enhancement/ColorSpace.cpp ./ImgProc/Enhancement/ColorSpace.hpp
	./ImgProc/Enhancement/Curves.cpp ./ImgProc/Enhancement/Curves.hpp
	./ImgProc/Enhancement/DocumentBackgroundSeparation.cpp ./ImgProc/Enhancement/DocumentBackgroundSeparation.h
	./ImgProc/Enhancement/FilterEffect.cpp ./ImgProc/Enhancement/FilterEffect.h
	./ImgProc/Enhancement/Histogram.cpp ./ImgProc/Enhancement/Histogram.h
	./ImgProc/Enhancement/HSL.cpp ./ImgProc/Enhancement/HSL.hpp
	./ImgProc/Enhancement/DetectionWhitePage.cpp ./ImgProc/Enhancement/DetectionWhitePage.h
	./ImgProc/Enhancement/SharpImage.cpp ./ImgProc/Enhancement/SharpImage.h
	./ImgProc/Enhancement/Halftone.cpp ./ImgProc/Enhancement/Halftone.h
	./ImgProc/Enhancement/AutoLevel.cpp ./ImgProc/Enhancement/AutoLevel.h
	./ImgProc/Enhancement/BrightnessBalance.cpp ./ImgProc/Enhancement/BrightnessBalance.h
	./ImgProc/Enhancement/newColorEnhance.cpp ./ImgProc/Enhancement/newColorEnhance.h
	./ImgProc/Enhancement/ImageEnhancement.cpp ./ImgProc/Enhancement/ImageEnhancement.h
	./ImgProc/Enhancement/ImgAdjust.cpp ./ImgProc/Enhancement/ImgAdjust.h
	./ImgProc/Enhancement/ColorEnhance.cpp ./ImgProc/Enhancement/ColorEnhance.h
	./ImgProc/Enhancement/FilterEffect.cpp ./ImgProc/Enhancement/FilterEffect.h
	./ImgProc/Enhancement/Noise.cpp ./ImgProc/Enhancement/Noise.h
	./ImgProc/Enhancement/ImageDecoder.cpp ./ImgProc/Enhancement/ImageDecoder.h
	./ImgProc/Enhancement/Compare.cpp ./ImgProc/Enhancement/Compare.h
	./ImgProc/Enhancement/RemoveNoise.cpp ./ImgProc/Enhancement/RemoveNoise.h
	./ImgProc/Enhancement/DeleteNoise_BW.cpp ./ImgProc/Enhancement/DeleteNoise_BW.h
	#./ImgProc/Enhancement/WaterMark.cpp ./ImgProc/Enhancement/WaterMark.h
	./ImgProc/Feature/AdjustSkew.cpp ./ImgProc/Feature/AdjustSkew.h
	./ImgProc/Feature/DetectEdge.cpp ./ImgProc/Feature/DetectEdge.h
	./ImgProc/Feature/DetectRectByContours.cpp ./ImgProc/Feature/DetectRectByContours.h
	./ImgProc/Feature/DetectRectByContours_new.cpp ./ImgProc/Feature/DetectRectByContours_new.h
	./ImgProc/Feature/DetectRectBySegmation.cpp ./ImgProc/Feature/DetectRectBySegmation.h
	./ImgProc/Feature/FillBorder.cpp ./ImgProc/Feature/FillBorder.h
	./ImgProc/Feature/PunchHold.cpp ./ImgProc/Feature/PunchHold.h
	./ImgProc/Transform/CropRectByCV.cpp ./ImgProc/Transform/CropRectByCV.h
	./ImgProc/Transform/MergerImage.cpp ./ImgProc/Transform/MergerImage.h
	./ImgProc/Transform/Rotate.cpp ./ImgProc/Transform/Rotate.h
	./ImgProc/WaterMark/CvText.cpp ./ImgProc/WaterMark/CvText.h
	./ImgProc/WaterMark/tools.cpp ./ImgProc/WaterMark/tools.h
	./Model/MImageModel/MImageDataPool.cpp ./Model/MImageModel/MImageDataPool.h
	)
	
target_link_libraries(CmImage 
	libopencv_photo.so 
	libopencv_highgui.so 
	libopencv_imgproc.so
	libopencv_core.so
	libopencv_calib3d.so
	libfreetype.so
	#libjpeg.so.9
	)
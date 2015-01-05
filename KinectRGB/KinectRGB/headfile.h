#ifndef HEADFILE_H
#define HEADFILE_H

#include <iostream>
#include <vector>
#include <Windows.h>
#include <NuiApi.h>
#include <opencv2\opencv.hpp>


#if _DEBUG
  #pragma comment(lib, "opencv_highgui231d.lib")
  #pragma comment(lib, "opencv_core231d.lib")
  #pragma comment(lib, "opencv_video231d.lib")
  #pragma comment(lib, "opencv_imgproc231d.lib")
  #pragma comment(lib, "opencv_calib3d231d.lib")
  #pragma comment(lib, "opencv_flann231d.lib")
  #pragma comment(lib, "opencv_features2d231d.lib")
  #pragma comment(lib, "Kinect10.lib")
#else
  #pragma comment(lib, "opencv_highgui231.lib")
  #pragma comment(lib, "opencv_core231.lib")
  #pragma comment(lib, "opencv_video231.lib")
  #pragma comment(lib, "opencv_imgproc231.lib")
  #pragma comment(lib, "opencv_calib3d231.lib")
  #pragma comment(lib, "opencv_flann231.lib")
  #pragma comment(lib, "opencv_features2d231.lib")
  #pragma comment(lib, "Kinect10.lib")
#endif




#endif //HEADFILE_H
#ifndef KINECTCV_H
#define KINECTCV_H


#include "headfile.h"


class KinectCV
{
private:
	cv::Mat image;
	HRESULT hr;
	HANDLE nextColorFrameEvent;
	HANDLE colorStreamHandle;
	INuiFrameTexture* pTexture;
	NUI_LOCKED_RECT LockedRect;
	const NUI_IMAGE_FRAME* pImageFrame;

public:
	KinectCV(int width, int height);
	cv::Mat GetImage();
	void GetFrame();
	void ToMat(NUI_LOCKED_RECT LockedRect, cv::Mat& output);
	void Show();
	void Update();
	void Release();
};


cv::Mat KinectCV::GetImage()
{
	return this->image;
}


KinectCV::KinectCV(int width, int height)
{
	//初始化图像大小
	this->image.create(width, height, CV_8UC3);

	//初始化NUI
	this->hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR);

	if (FAILED(hr))
	{
		std::cout<< "NuiInitialize failed!"<< std::endl;
		return;
	}

	//2、定义事件句柄   
	//创建读取下一帧的信号事件句柄，控制KINECT是否可以开始读取下一帧数据  
	this->nextColorFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );  
	this->colorStreamHandle = NULL; //保存图像数据流的句柄，用以提取数据   

	//3、打开KINECT设备的彩色图信息通道，并用colorStreamHandle保存该流的句柄，以便于以后读取  
	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480,   
		0, 2, nextColorFrameEvent, &colorStreamHandle);   
	if( FAILED( hr ) )//判断是否提取正确   
	{        
		std::cout<<"Could not open color image stream video"<< std::endl;   
		NuiShutdown();   
		return;   
	}  

	cv::namedWindow("ColorImage", CV_WINDOW_AUTOSIZE);  
}

void KinectCV::ToMat(NUI_LOCKED_RECT LockedRect0, cv::Mat& output)
{
	if( LockedRect0.Pitch != 0 ) 
	{ 
		//4.5、将数据转换为OpenCV的Mat格式
		for (int i=0; i<image.rows; i++) 
		{
			uchar *ptr = image.ptr<uchar>(i);  //第i行的指针
			//每个字节代表一个颜色信息，直接使用uchar
			uchar *pBuffer = (uchar*)(LockedRect0.pBits) + i * LockedRect0.Pitch;
			for (int j=0; j<image.cols; j++) 
			{ 
				ptr[3*j]   = pBuffer[4*j];  //内部数据是4个字节，0-1-2是BGR，第4个现在未使用 
				ptr[3*j+1] = pBuffer[4*j+1]; 
				ptr[3*j+2] = pBuffer[4*j+2]; 
			} 
		} 
	}
	else
	{
		std::cout<<"Buffer length of received texture is bogus\r\n"<<std::endl; 
		return;
	}
}



void KinectCV::GetFrame()
{
	this->pImageFrame = NULL;

	if (WaitForSingleObject(this->nextColorFrameEvent, INFINITE) == 0)
	{
		this->hr = NuiImageStreamGetNextFrame(this->colorStreamHandle, 0, &pImageFrame);

		if (FAILED(hr))
		{
			std::cout<< "Could not get color image"<< std::endl;
			NuiShutdown();
			return;
		}

		this->pTexture = pImageFrame->pFrameTexture;

		//4.3、提取数据帧到LockedRect，它包括两个数据对象：pitch每行字节数，pBits第一个字节地址
		//并锁定数据，这样当我们读数据的时候，kinect就不会去修改它
		pTexture->LockRect(0, &LockedRect, NULL, 0);

		//将读取到的数据转换成cv::Mat格式
		this->ToMat(LockedRect, this->image);
	}
}

void KinectCV::Show()
{
	cv::imshow("ColorImage", this->image);
}


void KinectCV::Update()
{
	//5、这帧已经处理完了，所以将其解锁
	this->pTexture->UnlockRect(0);
	//6、释放本帧数据，准备迎接下一帧 
	NuiImageStreamReleaseFrame(this->colorStreamHandle, this->pImageFrame); 
}


void KinectCV::Release()
{
	NuiShutdown(); 
}


#endif //KINECTCV_H
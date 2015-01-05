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
	//��ʼ��ͼ���С
	this->image.create(width, height, CV_8UC3);

	//��ʼ��NUI
	this->hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR);

	if (FAILED(hr))
	{
		std::cout<< "NuiInitialize failed!"<< std::endl;
		return;
	}

	//2�������¼����   
	//������ȡ��һ֡���ź��¼����������KINECT�Ƿ���Կ�ʼ��ȡ��һ֡����  
	this->nextColorFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );  
	this->colorStreamHandle = NULL; //����ͼ���������ľ����������ȡ����   

	//3����KINECT�豸�Ĳ�ɫͼ��Ϣͨ��������colorStreamHandle��������ľ�����Ա����Ժ��ȡ  
	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480,   
		0, 2, nextColorFrameEvent, &colorStreamHandle);   
	if( FAILED( hr ) )//�ж��Ƿ���ȡ��ȷ   
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
		//4.5��������ת��ΪOpenCV��Mat��ʽ
		for (int i=0; i<image.rows; i++) 
		{
			uchar *ptr = image.ptr<uchar>(i);  //��i�е�ָ��
			//ÿ���ֽڴ���һ����ɫ��Ϣ��ֱ��ʹ��uchar
			uchar *pBuffer = (uchar*)(LockedRect0.pBits) + i * LockedRect0.Pitch;
			for (int j=0; j<image.cols; j++) 
			{ 
				ptr[3*j]   = pBuffer[4*j];  //�ڲ�������4���ֽڣ�0-1-2��BGR����4������δʹ�� 
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

		//4.3����ȡ����֡��LockedRect���������������ݶ���pitchÿ���ֽ�����pBits��һ���ֽڵ�ַ
		//���������ݣ����������Ƕ����ݵ�ʱ��kinect�Ͳ���ȥ�޸���
		pTexture->LockRect(0, &LockedRect, NULL, 0);

		//����ȡ��������ת����cv::Mat��ʽ
		this->ToMat(LockedRect, this->image);
	}
}

void KinectCV::Show()
{
	cv::imshow("ColorImage", this->image);
}


void KinectCV::Update()
{
	//5����֡�Ѿ��������ˣ����Խ������
	this->pTexture->UnlockRect(0);
	//6���ͷű�֡���ݣ�׼��ӭ����һ֡ 
	NuiImageStreamReleaseFrame(this->colorStreamHandle, this->pImageFrame); 
}


void KinectCV::Release()
{
	NuiShutdown(); 
}


#endif //KINECTCV_H
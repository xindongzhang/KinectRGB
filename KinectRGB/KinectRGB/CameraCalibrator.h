#ifndef CAMERACALIBRATOR_H
#define CAMERACALIBRATOR_H


#include "headfile.h"

class CameraCalibrator
{
private:
	std::vector<std::vector<cv::Point3f>> objectPoints;
	std::vector<std::vector<cv::Point2f>> imagePoints;
	//Square Lenght
	float squareLenght;
	//output Matrices
	cv::Mat cameraMatrix; //intrinsic
	cv::Mat distCoeffs;
	//flag to specify how calibration is done
	int flag;
	//used in image undistortion
	cv::Mat map1,map2;
	bool mustInitUndistort;
public:
	CameraCalibrator(): flag(0), squareLenght(38.0), mustInitUndistort(true){};
	
	int addChessboardPoints(const std::vector<cv::Mat>& filelist,cv::Size& boardSize)
	{
		std::vector<cv::Mat>::const_iterator itImg;
		std::vector<cv::Point2f> imageCorners;
		std::vector<cv::Point3f> objectCorners;
		//initialize the chessboard corners in the chessboard reference frame
		//3d scene points
		for(int i = 0; i<boardSize.height; i++)
		{
			for(int j=0;j<boardSize.width;j++)
			{
				objectCorners.push_back(cv::Point3f(float(i)*squareLenght,float(j)*squareLenght,0.0f));
			}
		}
		//2D Image points:
		cv::Mat image; //to contain chessboard image
		int successes = 0;
		//cv::namedWindow("Chess");
		for(itImg=filelist.begin(); itImg!=filelist.end(); itImg++)
		{
			cv::cvtColor(*itImg, image, cv::COLOR_BGR2GRAY);
			bool found = cv::findChessboardCorners(image, boardSize, imageCorners);
			cv::drawChessboardCorners(image, boardSize, imageCorners, found);
			std::cout<< "corner : "<< imageCorners.size()<< std::endl;
			cv::imshow("Chess",image);
			cv::waitKey(1000);
			cv::cornerSubPix(image, imageCorners, cv::Size(5,5),cv::Size(-1,-1),
				             cv::TermCriteria(cv::TermCriteria::MAX_ITER+cv::TermCriteria::EPS,30,0.1));
			//if we have a good board, add it to our data
			if(imageCorners.size() == boardSize.area())
			{
				addPoints(imageCorners,objectCorners);
				successes++;
			}
		}
		return successes;
	}

	void addPoints(const std::vector<cv::Point2f>& imageCorners,const std::vector<cv::Point3f>& objectCorners)
	{
		//2D image point from one view
		imagePoints.push_back(imageCorners);
		//corresponding 3D scene points
		objectPoints.push_back(objectCorners);
	}

	double calibrate(cv::Size &imageSize)
	{
		mustInitUndistort = true;
		std::vector<cv::Mat> rvecs,tvecs;
		return
			cv::calibrateCamera(objectPoints, //the 3D points
			imagePoints,
			imageSize, 
			cameraMatrix, //output camera matrix
			distCoeffs,
			rvecs,tvecs,
			flag);
	}

	void remap(const cv::Mat &image, cv::Mat &undistorted)
	{
		std::cout << cameraMatrix;
		if(mustInitUndistort){ //called once per calibration
			cv::initUndistortRectifyMap(
				cameraMatrix,
				distCoeffs,
				cv::Mat(),
				cameraMatrix,
				image.size(),
				CV_32FC1,
				map1,map2);
			mustInitUndistort = false;
		}
		//apply mapping functions
		cv::remap(image,undistorted,map1,map2,cv::INTER_LINEAR);
	}
};

#endif //CAMERACALIBRATOR_H


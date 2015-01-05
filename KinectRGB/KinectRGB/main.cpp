#include "headfile.h"
#include "KinectCV.h"
#include "CameraCalibrator.h"

int main(void)
{
	//≥ı ºªØKinectCV
	KinectCV Kcv(480, 640);
	std::vector<cv::Mat> Shoot;
	int index = 1;
	while (1)
	{
		Kcv.GetFrame();
		Kcv.Show();
		Kcv.Update();

		char PressKey = cv::waitKey(20);
		if (PressKey == 27 || index == 10)
			break;
		else if (PressKey == 'S' || PressKey == 's')
		{
			Shoot.push_back(Kcv.GetImage());
			++ index;
		}
	}
	Kcv.Release();

	cv::imshow("hefang", Shoot[0]);

	cv::waitKey();

	return 0;
}
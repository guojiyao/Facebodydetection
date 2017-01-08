// BodyMeasurement.cpp : main project file.

//#include "stdafx.h"

#include "ArgumentParser.h"
#include "BodySegmentator.h"


#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <fstream>
#include <iostream>
#include "imageProcessing.h"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

cv::Mat oFresult_;
cv::Mat oSresult_;
cv::Mat Fresult_;
cv::Mat Sresult_;
imageProcessing improc = imageProcessing();
float height = 5.72f;
float Wast = 0.0f;
int fpixelx = 400;
int spixelx = 400;
bool mouseDown = false;
int pointIndex = -1;
Point* changePoint;

void FDrawPoints();
void SDrawPoints();

float calculateMeasures();
bool isAround(Point mouse, Point point)
{
	if (point.x > mouse.x - 5 && point.x < mouse.x + 5
		&& point.y > mouse.y - 5 && point.y < mouse.y + 5)
		return true;
	return false;
}
void FgetPointIndex(Point mouse)
{
	if (isAround(mouse, improc.shoulder[0]))
		changePoint = &improc.shoulder[0];
	if (isAround(mouse, improc.shoulder[1]))
		changePoint = &improc.shoulder[1];
	if (isAround(mouse, improc.chest[0]))
		changePoint = &improc.chest[0];
	if (isAround(mouse, improc.chest[1]))
		changePoint = &improc.chest[1];
	if (isAround(mouse, improc.wast[0]))
		changePoint = &improc.wast[0];
	if (isAround(mouse, improc.wast[1]))
		changePoint = &improc.wast[1];
	if (isAround(mouse, improc.Arm[0]))
		changePoint = &improc.Arm[0];
	if (isAround(mouse, improc.Arm[1]))
		changePoint = &improc.Arm[1];
	if (isAround(mouse, improc.hip[0]))
		changePoint = &improc.hip[0];
	if (isAround(mouse, improc.hip[1]))
		changePoint = &improc.hip[1];
	if (isAround(mouse, improc.toe))
		changePoint = &improc.toe;

}
void SgetPointIndex(Point mouse)
{


	if (isAround(mouse, improc.Schest[0]))
		changePoint = &improc.Schest[0];
	if (isAround(mouse, improc.Schest[1]))
		changePoint = &improc.Schest[1];

	if (isAround(mouse, improc.Swast[0]))
		changePoint = &improc.Swast[0];
	if (isAround(mouse, improc.Swast[1]))
		changePoint = &improc.Swast[1];

	if (isAround(mouse, improc.Ship[0]))
		changePoint = &improc.Ship[0];
	if (isAround(mouse, improc.Ship[1]))
		changePoint = &improc.Ship[1];

	if (isAround(mouse, improc.Sarm[0]))
		changePoint = &improc.Sarm[0];
	if (isAround(mouse, improc.Sarm[1]))
		changePoint = &improc.Sarm[1];

	if (isAround(mouse, improc.Sleg[0]))
		changePoint = &improc.Sleg[0];
	if (isAround(mouse, improc.Sleg[1]))
		changePoint = &improc.Sleg[1];

}
void FCallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		mouseDown = true;
		FgetPointIndex(Point(x, y));
	}
	else if (event == EVENT_LBUTTONUP)
	{
		mouseDown = false;
		changePoint = NULL;
	}
	else if (event == EVENT_MOUSEMOVE)
	{
		if (mouseDown && changePoint != NULL) {
			if (x > 0 & x < Fresult_.cols)
				(*changePoint).x = x;
			if (y > 0 & y < Fresult_.rows)
				(*changePoint).y = y;

			Fresult_ = oFresult_.clone();
			Sresult_ = oSresult_.clone();

			FDrawPoints();
			SDrawPoints();
			calculateMeasures();
			//imshow("FOutput", Fresult_);
			//imshow("SOutput", Sresult_);
		}
	}
}
void SCallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		mouseDown = true;
		SgetPointIndex(Point(x, y));
	}
	else if (event == EVENT_LBUTTONUP)
	{
		mouseDown = false;
		changePoint = NULL;
	}
	else if (event == EVENT_MOUSEMOVE)
	{
		if (mouseDown && changePoint != NULL) {
			if (x > 0 & x < Sresult_.cols)
				(*changePoint).x = x;
			if (y > 0 & y < Fresult_.rows)
				(*changePoint).y = y;

			Fresult_ = oFresult_.clone();
			Sresult_ = oSresult_.clone();

			FDrawPoints();
			SDrawPoints();
			calculateMeasures();
			//imshow("FOutput", Fresult_);
			//imshow("SOutput", Sresult_);
		}
	}
}

float calculateMeasures()
{
	int heightPixels = improc.toe.y - improc.headTip.y;
	int x = improc.shoulder[1].x - improc.shoulder[0].x;
	int y = improc.shoulder[1].y - improc.shoulder[0].y;
	int shoulderPixels = sqrt((x * x) + (y * y));

	x = improc.chest[1].x - improc.chest[0].x;
	y = improc.chest[1].y - improc.chest[0].y;
	int chestPixels = sqrt((x * x) + (y * y));

	x = improc.Schest[1].x - improc.Schest[0].x;
	y = improc.Schest[1].y - improc.Schest[0].y;
	int SchestPixels = sqrt((x * x) + (y * y));

	x = improc.wast[1].x - improc.wast[0].x;
	y = improc.wast[1].y - improc.wast[0].y;
	int wastPixels = sqrt((x * x) + (y * y));

	x = improc.Swast[1].x - improc.Swast[0].x;
	y = improc.Swast[1].y - improc.Swast[0].y;
	int SwastPixels = sqrt((x * x) + (y * y));

	x = improc.hip[1].x - improc.hip[0].x;
	y = improc.hip[1].y - improc.hip[0].y;
	int hipPixels = sqrt((x * x) + (y * y));

	x = improc.Ship[1].x - improc.Ship[0].x;
	y = improc.Ship[1].y - improc.Ship[0].y;
	int ShipPixels = sqrt((x * x) + (y * y));

	x = improc.Arm[0].x - improc.shoulder[0].x;
	y = improc.Arm[0].y - improc.shoulder[0].y;
	int ArmPixels = sqrt((x * x) + (y * y));

	x = improc.Sarm[1].x - improc.Sarm[0].x;
	y = improc.Sarm[1].y - improc.Sarm[0].y;
	int SarmPixels = sqrt((x * x) + (y * y));

	x = improc.Sleg[1].x - improc.Sleg[0].x;
	y = improc.Sleg[1].y - improc.Sleg[0].y;
	int SlegPixels = sqrt((x * x) + (y * y));

	//getting front arm measurment with respect to frnt height
	float Marm = height / heightPixels * (ArmPixels);

	//finding side arm measurment with respect to pixel and lenth rtion of arm in front
	float Msleg = Marm / SarmPixels * SlegPixels;

	//finding all the side measurments with respect to side arm measurment
	float Msarm = Marm / SarmPixels * SarmPixels;
	float Mschest = Marm / SarmPixels * SchestPixels;
	float Mswast = Marm / SarmPixels * SwastPixels;
	float Mship = Marm / SarmPixels * ShipPixels;

	//fining measurment with respect to height for front + side calculated above = total round measurments
	float MShoulder = height / heightPixels * (shoulderPixels);
	float Mchest = height / heightPixels * (chestPixels * 2) + Mschest;
	float Mwast = height / heightPixels * (wastPixels * 2) + Mswast;
	float Mhip = height / heightPixels * (hipPixels * 2) + Mship;
	//printf("hp=%f\n", heightPixels);
	//printf("wp=%f\n", wastPixels);

	////Marm = Marm * factor;
	////Msleg = Msleg * factor;	
	////Msarm = Msarm * factor;
	////height = height * factor;

	float cosinus = Wast == 0.0f? 1.0f : Wast / Mwast;
	float Owast = Mwast*cosinus;
	//if (cosinus > 1.0) cosinus = 1.0f;
	MShoulder = MShoulder * cosinus;
	Mchest = Mchest * cosinus;
	Mhip = Mhip * cosinus;
	Mschest = Mschest * cosinus;
	Mswast = Mswast * cosinus;
	Mship = Mship * cosinus;

	{
		FILE * fp;
		fp = fopen("ipo.txt", "w");
		fprintf(fp, "input height = %f\n", height*12);
		fprintf(fp, "input waist = %f\n", Wast*12);
		fprintf(fp, "cosinus = %f\n", cosinus);

		fprintf(fp, "shoulder = %f\n", MShoulder*12);
		fprintf(fp, "chest = %f\n", Mchest*12);
		fprintf(fp, "waist = %f\n", Owast*12);
		fprintf(fp, "Mwaist = %f\n", Mwast * 12);
		fprintf(fp, "hip = %f\n", Mhip*12);
		fprintf(fp, "arm = %f\n", Marm*12);

		fprintf(fp, "S chest = %f\n", Mschest*12);
		fprintf(fp, "S waist = %f\n", Mswast*12);
		fprintf(fp, "S hip = %f\n", Mship*12);
		fprintf(fp, "S arm = %f\n", Msarm*12);
		fprintf(fp, "S leg = %f\n", Msleg*12);
		fclose(fp);
	}
        /*
	putText(Sresult_, to_string(Msleg * 12), improc.Sleg[1],
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 0), 1, CV_AA);

	putText(Fresult_, to_string(MShoulder * 12), improc.shoulder[1],
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 0), 1, CV_AA);
	putText(Fresult_, to_string(Mchest * 12), improc.chest[1],
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 0), 1, CV_AA);
	putText(Fresult_, to_string(Mwast * 12), improc.wast[1],
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 0), 1, CV_AA);

	putText(Fresult_, to_string(Mhip * 12), improc.hip[1],
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 0), 1, CV_AA);

	putText(Fresult_, to_string(Marm * 12), improc.Arm[0],
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 0), 1, CV_AA);
       */
	return Mwast;
}

void FDrawPoints()
{
	int dotsize = 6;
	//Drawing HeadTip Point

	cv::RotatedRect headTipRect = cv::RotatedRect(Point2f(improc.headTip.x, improc.headTip.y), Size2f(dotsize, dotsize), 90);
	cv::ellipse(Fresult_, headTipRect, Scalar(0, 255, 0), 1, 4);

	//Drawing Head Point

	cv::RotatedRect headRect = cv::RotatedRect(Point2f(improc.head.x, improc.head.y), Size2f(dotsize, dotsize), 0);
	cv::ellipse(Fresult_, headRect, Scalar(0, 255, 0), 1, 4);

	//Drawing shoulder points
	putText(Fresult_, "S", Point(improc.shoulder[0].x - 15, improc.shoulder[0].y - 5),
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 102, 0), 1, CV_AA);

	putText(Fresult_, "S", Point(improc.shoulder[1].x, improc.shoulder[1].y - 5),
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 102, 0), 1, CV_AA);

	cv::RotatedRect shoulderLRect = cv::RotatedRect(Point2f(improc.shoulder[0].x, improc.shoulder[0].y), Size2f(dotsize, dotsize), 0);
	cv::ellipse(Fresult_, shoulderLRect, Scalar(255, 102, 0), 1, 4);

	cv::RotatedRect shoulderRRect = cv::RotatedRect(Point2f(improc.shoulder[1].x, improc.shoulder[1].y), Size2f(dotsize, dotsize), 0);
	cv::ellipse(Fresult_, shoulderRRect, Scalar(255, 102, 0), 1, 4);

	line(Fresult_, improc.shoulder[0], improc.shoulder[1], Scalar(255, 102, 0), 1, 8);

	//Drawing Chest points
	putText(Fresult_, "C", Point(improc.chest[0].x - 15, improc.chest[0].y - 5),
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 255, 0), 1, CV_AA);

	putText(Fresult_, "C", Point(improc.chest[1].x, improc.chest[1].y - 5),
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 255, 0), 1, CV_AA);

	cv::RotatedRect chestLRect = cv::RotatedRect(Point2f(improc.chest[0].x, improc.chest[0].y), Size2f(dotsize, dotsize), 0);
	cv::ellipse(Fresult_, chestLRect, Scalar(255, 255, 0), 1, 4);

	cv::RotatedRect chestRRect = cv::RotatedRect(Point2f(improc.chest[1].x, improc.chest[1].y), Size2f(dotsize, dotsize), 0);
	cv::ellipse(Fresult_, chestRRect, Scalar(255, 255, 0), 1, 4);

	line(Fresult_, improc.chest[0], improc.chest[1], Scalar(255, 255, 0), 1, 8);

	//Drawing Wast points
	putText(Fresult_, "W", Point(improc.wast[0].x - 15, improc.wast[0].y - 5),
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 255, 255), 1, CV_AA);

	putText(Fresult_, "W", Point(improc.wast[1].x, improc.wast[1].y - 5),
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 255, 255), 1, CV_AA);

	cv::RotatedRect wastLRect = cv::RotatedRect(Point2f(improc.wast[0].x, improc.wast[0].y), Size2f(dotsize, dotsize), 0);
	cv::ellipse(Fresult_, wastLRect, Scalar(0, 255, 255), 1, 4);


	cv::RotatedRect wastRRect = cv::RotatedRect(Point2f(improc.wast[1].x, improc.wast[1].y), Size2f(dotsize, dotsize), 0);
	cv::ellipse(Fresult_, wastRRect, Scalar(0, 255, 255), 1, 4);


	line(Fresult_, improc.wast[0], improc.wast[1], Scalar(0, 255, 255), 1, 8);

	//Drawing hip points
	putText(Fresult_, "H", Point(improc.hip[0].x - 15, improc.hip[0].y - 5),
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 153, 204), 1, CV_AA);

	putText(Fresult_, "H", Point(improc.hip[1].x, improc.hip[1].y - 5),
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 153, 204), 1, CV_AA);

	cv::RotatedRect hipLRect = cv::RotatedRect(Point2f(improc.hip[0].x, improc.hip[0].y), Size2f(dotsize, dotsize), 0);
	cv::ellipse(Fresult_, hipLRect, Scalar(255, 153, 0), 1, 4);

	cv::RotatedRect hipRRect = cv::RotatedRect(Point2f(improc.hip[1].x, improc.hip[1].y), Size2f(dotsize, dotsize), 0);
	cv::ellipse(Fresult_, hipRRect, Scalar(255, 153, 0), 1, 4);

	line(Fresult_, improc.hip[0], improc.hip[1], Scalar(255, 153, 204), 1, 8);

	//Drawing Arm Left
	putText(Fresult_, "A", Point(improc.Arm[0].x - 15, improc.Arm[0].y - 5),
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);

	cv::RotatedRect ArmLRect = cv::RotatedRect(Point2f(improc.Arm[0].x, improc.Arm[0].y), Size2f(dotsize, dotsize), 0);
	cv::ellipse(Fresult_, ArmLRect, Scalar(0, 0, 255), 1, 4);

	line(Fresult_, improc.shoulder[0], improc.Arm[0], Scalar(0, 0, 255), 1, 8);

	//Drawing Arm Right
	putText(Fresult_, "A", Point(improc.Arm[1].x - 15, improc.Arm[1].y - 5),
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);

	cv::RotatedRect ArmRRect = cv::RotatedRect(Point2f(improc.Arm[1].x, improc.Arm[1].y), Size2f(dotsize, dotsize), 0);
	cv::ellipse(Fresult_, ArmRRect, Scalar(0, 0, 255), 1, 4);


	line(Fresult_, improc.shoulder[1], improc.Arm[1], Scalar(0, 0, 255), 1, 8);



	//Drawing toe point
	cv::RotatedRect toeRect = cv::RotatedRect(Point2f(improc.toe.x, improc.toe.y), Size2f(dotsize, dotsize), 0);
	cv::ellipse(Fresult_, toeRect, Scalar(0, 255, 0), 2, 4);


	cv::Rect rect = improc.face;
	cv::rectangle(Fresult_, rect, Scalar(0, 255, 0), 4, 8, 0);

    //cv::imshow("FRESULT", Fresult_);
    //cv::imwrite("FRESULT.png", Fresult_);
    //cv::waitKey(0);
}
void SDrawPoints()
{
	int dotsize = 4;
	//Drawing side Chest points
	putText(Sresult_, "C", Point(improc.Schest[0].x - 15, improc.Schest[0].y - 5),
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 255, 0), 1, CV_AA);

	putText(Sresult_, "C", Point(improc.Schest[1].x, improc.Schest[1].y - 5),
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 255, 0), 1, CV_AA);

	cv::Rect SchestLRect = cv::Rect(improc.Schest[0].x - 2, improc.Schest[0].y - 2, 4, 4);
	cv::rectangle(Sresult_, SchestLRect, Scalar(255, 255, 0), dotsize, 4, 0);

	cv::Rect SchestRRect = cv::Rect(improc.Schest[1].x - 2, improc.Schest[1].y - 2, 4, 4);
	cv::rectangle(Sresult_, SchestRRect, Scalar(255, 255, 0), dotsize, 4, 0);

	line(Sresult_, improc.Schest[0], improc.Schest[1], Scalar(255, 255, 0), 1, 8);

	//Drawing side Wast points
	putText(Sresult_, "W", Point(improc.Swast[0].x - 15, improc.Swast[0].y - 5),
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 255, 255), 1, CV_AA);

	putText(Sresult_, "W", Point(improc.Swast[1].x, improc.Swast[1].y - 5),
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 255, 255), 1, CV_AA);

	cv::Rect SwastLRect = cv::Rect(improc.Swast[0].x - 2, improc.Swast[0].y - 2, 4, 4);
	cv::rectangle(Sresult_, SwastLRect, Scalar(0, 255, 255), dotsize, 4, 0);

	cv::Rect SwastRRect = cv::Rect(improc.Swast[1].x - 2, improc.Swast[1].y - 2, 4, 4);
	cv::rectangle(Sresult_, SwastRRect, Scalar(0, 255, 255), dotsize, 4, 0);

	line(Sresult_, improc.Swast[0], improc.Swast[1], Scalar(0, 255, 255), 1, 8);


	//Drawing side hip points
	putText(Sresult_, "H", Point(improc.Ship[0].x - 15, improc.Ship[0].y - 5),
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 153, 204), 1, CV_AA);

	putText(Sresult_, "H", Point(improc.Ship[1].x, improc.Ship[1].y - 5),
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 153, 204), 1, CV_AA);

	cv::Rect ShipLRect = cv::Rect(improc.Ship[0].x - 2, improc.Ship[0].y - 2, 4, 4);
	cv::rectangle(Sresult_, ShipLRect, Scalar(255, 153, 204), dotsize, 4, 0);

	cv::Rect ShipRRect = cv::Rect(improc.Ship[1].x - 2, improc.Ship[1].y - 2, 4, 4);
	cv::rectangle(Sresult_, ShipRRect, Scalar(255, 153, 204), dotsize, 4, 0);

	line(Sresult_, improc.Ship[0], improc.Ship[1], Scalar(255, 153, 204), 1, 8);


	//Drawing side arm points
	putText(Sresult_, "A", Point(improc.Sarm[0].x - 15, improc.Sarm[0].y - 5),
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);

	putText(Sresult_, "A", Point(improc.Sarm[1].x, improc.Sarm[1].y - 5),
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);

	cv::Rect SarmLRect = cv::Rect(improc.Sarm[0].x - 2, improc.Sarm[0].y - 2, 4, 4);
	cv::rectangle(Sresult_, SarmLRect, Scalar(0, 0, 255), dotsize, 4, 0);

	cv::Rect SarmRRect = cv::Rect(improc.Sarm[1].x - 2, improc.Sarm[1].y - 2, 4, 4);
	cv::rectangle(Sresult_, SarmRRect, Scalar(0, 0, 255), dotsize, 4, 0);

	line(Sresult_, improc.Sarm[0], improc.Sarm[1], Scalar(0, 0, 255), 1, 8);

	//Drawing side leg points
	putText(Sresult_, "L", Point(improc.Sleg[0].x - 15, improc.Sleg[0].y - 5),
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 255, 0), 1, CV_AA);

	putText(Sresult_, "l", Point(improc.Sleg[1].x, improc.Sleg[1].y - 5),
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 255, 0), 1, CV_AA);

	cv::Rect SlegLRect = cv::Rect(improc.Sleg[0].x - 2, improc.Sleg[0].y - 2, 4, 4);
	cv::rectangle(Sresult_, SlegLRect, Scalar(0, 255, 0), dotsize, 4, 0);

	cv::Rect SlegRRect = cv::Rect(improc.Sleg[1].x - 2, improc.Sleg[1].y - 2, 4, 4);
	cv::rectangle(Sresult_, SlegRRect, Scalar(0, 255, 0), dotsize, 4, 0);

	line(Sresult_, improc.Sleg[0], improc.Sleg[1], Scalar(0, 255, 0), 1, 8);
   // cv::imshow("SRESULT", Sresult_);
    //cv::imwrite("SRESULT.png", Sresult_);
   // cv::waitKey(0);
}

static void FExportPoints()
{
	FILE * fp;
	fp = fopen("fpot.txt", "w");
	fprintf(fp, "headtipx = %d\n", improc.headTip.x);
	fprintf(fp, "headtipy = %d\n", improc.headTip.y);
	fprintf(fp, "headx = %d\n", improc.head.x);
	fprintf(fp, "heady = %d\n", improc.head.y);
	fprintf(fp, "shoulder0x = %d\n", improc.shoulder[0].x);
	fprintf(fp, "shoulder0y = %d\n", improc.shoulder[0].y);
	fprintf(fp, "shoulder1x = %d\n", improc.shoulder[1].x);
	fprintf(fp, "shoulder1y = %d\n", improc.shoulder[1].y);
	fprintf(fp, "chest0x = %d\n", improc.chest[0].x);
	fprintf(fp, "chest0y = %d\n", improc.chest[0].y);
	fprintf(fp, "chest1x = %d\n", improc.chest[1].x);
	fprintf(fp, "chest1y = %d\n", improc.chest[1].y);
	fprintf(fp, "wast0x = %d\n", improc.wast[0].x);
	fprintf(fp, "wast0y = %d\n", improc.wast[0].y);
	fprintf(fp, "wast1x = %d\n", improc.wast[1].x);
	fprintf(fp, "wast1y = %d\n", improc.wast[1].y);
	fprintf(fp, "hip0x = %d\n", improc.hip[0].x);
	fprintf(fp, "hip0y = %d\n", improc.hip[0].y);
	fprintf(fp, "hip1x = %d\n", improc.hip[1].x);
	fprintf(fp, "hip1y = %d\n", improc.hip[1].y);
	fprintf(fp, "Arm0x = %d\n", improc.Arm[0].x);
	fprintf(fp, "Arm0y = %d\n", improc.Arm[0].y);
	fprintf(fp, "Arm1x = %d\n", improc.Arm[1].x);
	fprintf(fp, "Arm1y = %d\n", improc.Arm[1].y);
	fprintf(fp, "toex = %d\n", improc.toe.x);
	fprintf(fp, "toey = %d\n", improc.toe.y);
	fprintf(fp, "face = %d %d %d %d \n", improc.face.x, improc.face.y, improc.face.width, improc.face.height);

	fprintf(fp, "schest0x = %d\n", improc.Schest[0].x);
	fprintf(fp, "schest0y = %d\n", improc.Schest[0].y);
	fprintf(fp, "schest1x = %d\n", improc.Schest[1].x);
	fprintf(fp, "schest1y = %d\n", improc.Schest[1].y);
	fprintf(fp, "swast0x = %d\n", improc.Swast[0].x);
	fprintf(fp, "swast0y = %d\n", improc.Swast[0].y);
	fprintf(fp, "swast1x = %d\n", improc.Swast[1].x);
	fprintf(fp, "swast1y = %d\n", improc.Swast[1].y);
	fprintf(fp, "ship0x = %d\n", improc.Ship[0].x);
	fprintf(fp, "ship0y = %d\n", improc.Ship[0].y);
	fprintf(fp, "ship1x = %d\n", improc.Ship[1].x);
	fprintf(fp, "ship1y = %d\n", improc.Ship[1].y);
	fprintf(fp, "sArm0x = %d\n", improc.Sarm[0].x);
	fprintf(fp, "sArm0y = %d\n", improc.Sarm[0].y);
	fprintf(fp, "sArm1x = %d\n", improc.Sarm[1].x);
	fprintf(fp, "sArm1y = %d\n", improc.Sarm[1].y);
	fprintf(fp, "sleg0x = %d\n", improc.Sleg[0].x);
	fprintf(fp, "sleg0y = %d\n", improc.Sleg[0].y);
	fprintf(fp, "sleg1x = %d\n", improc.Sleg[1].x);
	fprintf(fp, "sleg1y = %d\n", improc.Sleg[1].y);

	fclose(fp);
}

int main(int argc, char *argv[])
{
    string FrontPath = "";
     string SidePath = "";

     ArgumentParser p(argc, argv);
     if(!p.isValid()) {
        std::cout << "Input arguments are Invalid"<< std::endl << p.help() << std::endl;
        return -1;// TO DO add Enums for each error code
     }
     if(p.getFrontPath() == ArgumentParser::s_invalidPath) {
        std::cout << "Frontal path is not specified" << std::endl;
        return -2;
     }

    if(p.getHeight() != ArgumentParser::s_invalidNumericalValue)
       height = p.getHeight();
    if (p.getFPixelX() != ArgumentParser::s_invalidNumericalValue)
       fpixelx = p.getFPixelX();
    if (p.getSPixelX() != ArgumentParser::s_invalidNumericalValue)
       spixelx = p.getSPixelX();
    if (p.getWast() != ArgumentParser::s_invalidNumericalValue)
       Wast = p.getWast();
    if (p.getFrontPath() != ArgumentParser::s_invalidPath)
        FrontPath = p.getFrontPath();


    const int success = 0;
    BodySegmentator seg;
    int retCode = seg.run(p);
    if(retCode != success) {
      return -1;
   }

    Fresult_ = seg.frontalImage();
    resize(Fresult_, Fresult_, Size(fpixelx, (int)(Fresult_.rows * fpixelx / Fresult_.cols)), 0, 0, INTER_CUBIC);
	oFresult_ = Fresult_.clone();	
	improc.img = Fresult_.clone();
	improc.FdepressBackground();	
	improc.Frgb_to_gray();
	improc.Fdo_edge_detection();
    if(!seg.sideViewImage().empty()){
          Sresult_ = seg.sideViewImage().clone();
          std::cout << "SideView" << Sresult_ .size() << std::endl;
          resize(Sresult_, Sresult_, Size(spixelx, (int)(Sresult_.rows * spixelx / Sresult_.cols)), 0, 0, INTER_CUBIC);
          std::cout << "SideView After Resize"  << Sresult_.size() << std::endl;
          oSresult_ = Sresult_.clone();
          improc.Simg = Sresult_.clone();
          std::cout << "SideView After Resize clone"  << Sresult_.size() << std::endl;
          improc.SdepressBackground();
          std::cout << "SideView After Resize dep back"  << Sresult_.size() << std::endl;
          improc.Srgb_to_gray();
          std::cout << "SideView After Resize rgb to gray"  << Sresult_.size() << std::endl;
          improc.Sdo_edge_detection();
          std::cout << "SideView After Resize edge dete"  << Sresult_.size() << std::endl;
          improc.Sdo_contour();
          std::cout << "SideView After Resize sdo contour"  << Sresult_.size() << std::endl;
          //std::cout << "SideView______________" << std::endl;
          //imshow("Simg", improc.Simg);//
          //imshow("Sgrayscale", improc.Sgrayscale);//
          //imshow("Sedge_", improc.Sedge_);//
          //imshow("Sresult_", improc.Sresult_);//
         // if (!improc.Sresult_.empty()){
         //     std::cout << "REsult" << std::endl;
         // }

          //imshow("ScntIMage", improc.ScntIMage);//
    }

    //imshow("edge", improc.edge_);//
    cv::imwrite("edge.png", improc.edge_);

    improc.Fdo_contour();

    //imshow("contour", improc.result_);//
    cv::imwrite("contour.png", improc.result_);

	improc.getBodyPoints();

	//Drawing Points
    //FDrawPoints();
	FExportPoints();
   // SDrawPoints();

	//Calculating measurments
	float waist = calculateMeasures();

    printf("fp=%d sp=%d\n", fpixelx, spixelx);
	printf("height=%f waist=%f\n", height, waist*12);
    /*
	if (waist > 0)
	{
		FILE * fp;
		fp = fopen("ipo.txt", "w");
		fprintf(fp, "%f", waist);
		fclose(fp);
	}
	*/
	//cv::destroyAllWindows();
	//Sleep(5000);


	return 0;
}


//#include "stdafx.h"
#include "stdio.h"
#include "imageProcessing.h"
#include <iostream>

imageProcessing::imageProcessing()
{
  edge_thresh = 30;// use different threshold for optimum result  or use adaptice threshold
}

void imageProcessing::EquilizeLuminnce(Mat* image)
{
	cv::Mat lab_image;

	cv::cvtColor((*image), lab_image, CV_BGR2Lab);

	// Extract the L channel
	std::vector<cv::Mat> lab_planes(3);
	cv::split(lab_image, lab_planes);  // now we have the L image in lab_planes[0]

	// apply the CLAHE algorithm to the L channel
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
	clahe->setClipLimit(4);
	cv::Mat dst;
	clahe->apply(lab_planes[0], dst);

	// Merge the the color planes back into an Lab image
	dst.copyTo(lab_planes[0]);
	cv::merge(lab_planes, lab_image);

	// convert back to RGB
	//static cv::Mat image_clahe;
	cv::cvtColor(lab_image, (*image), CV_Lab2BGR);
}
void imageProcessing::depressBackground(Mat* image)
{
	//Removing Backgronud
	int b = 0;// result_.at<Vec3b>(Point(5, 5))[0];
	int g = 0;// result_.at<Vec3b>(Point(5, 5))[0];
	int r = 0;// result_.at<Vec3b>(Point(5, 5))[0];
	for (int y = 0; y < (*image).rows*0.2; y++)
	for (int x = 0; x < (*image).cols*0.2; x++)
	{
		//Vec3b color = result_.at<Vec3b>(Point(x, y));
		b += (*image).at<Vec3b>(Point(5, 5))[0];
		g += (*image).at<Vec3b>(Point(5, 5))[0];
		r += (*image).at<Vec3b>(Point(5, 5))[0];
	}

	b = b / ((*image).rows*0.2 * (*image).cols*0.2);
	g = g / ((*image).rows*0.2 * (*image).cols*0.2);
	r = r / ((*image).rows*0.2 * (*image).cols*0.2);

	int d1 = g - b;
	int d2 = r - g;
	int d3 = b - r;

	for (int y = 0; y < (*image).rows; y++)
	{
		for (int x = 0; x < (*image).cols; x++)
		{
			Vec3b color = (*image).at<Vec3b>(Point(x, y));
			/*if (color[0] > b-35 && color[0] < b + 35
			&& color[1] > g - 35 && color[1] < g + 35
			&& color[2] > r - 35 && color[2] < r + 35)*/

			//Vec3b color = result_.at<Vec3b>(Point(x, y));
			/*int dd1 = color[0] - color[1];
			int dd2 = color[1] - color[2];
			int dd3 = color[2] - color[0];
			if (dd1 > d1 - 5 && dd1 < d1 + 5
			&& dd2 > d2 - 5 && dd2 < d2 + 5
			&& dd3 > d3 - 5 && dd3 < d3 + 5)*/
			/*{
			Vec3b color2;
			color2[0] = 0;
			color2[1] = 0;
			color2[2] = 0;
			(*image).at<Vec3b>(Point(x, y)) = color2;
			}
			else
			{*/
			Vec3b color2;
			color2.val[0] = abs(color[0] - b);
			color2.val[1] = abs(color[1] - g);
			color2.val[2] = abs(color[2] - r);
			(*image).at<Vec3b>(Point(x, y)) = color2;
			//}
		}
	}
}
void imageProcessing::rgb_to_gray(Mat* image, Mat* gimage)
{
	(*gimage).create((*image).size(), (*image).type());
	cvtColor((*image), (*gimage), COLOR_BGR2GRAY);

	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(0, 0));
	erode((*gimage), (*gimage), element);

	blur((*gimage), (*gimage), Size(4, 4));
}
void imageProcessing::do_edge_detection(Mat* gimage, Mat* eimage)
{
	(*eimage) = (*gimage).clone();

	// Run the edge detector on grayscale
	Canny((*eimage), (*eimage), edge_thresh, edge_thresh * 3, 3);
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(0, 0));
	//erode(edge_,edge_,element);
	dilate((*eimage), (*eimage), element);
}
void imageProcessing::do_contour(Mat* eimage, Mat* rimage)
{

	std::cout << "Do Contour " << eimage->size() << " " << rimage->size() <<std::endl;
	findContours((*eimage), contours_, hierarchy_, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	std::cout << "After find contours " << eimage->size() << " " << rimage->size() <<"  "<< img.size() << std::endl;

	(*rimage) = Mat(img.rows, img.cols, CV_8SC1, Scalar(0));
	std::cout << "After rimage " << eimage->size() << " " << rimage->size() <<std::endl;
	if (contours_.size() != 0)
	{
		for (int i = 0; i< contours_.size(); i++)
		{
			std::cout << i << " " << contours_.size() <<  " After rimage " << eimage->size() << " " << rimage->size() <<std::endl;
			Scalar color = Scalar(255);
			drawContours((*rimage), contours_, i, color, 2, 2, hierarchy_, 0, Point());
		}
	}
	std::cout << "After rimage " << eimage->size() << " " << rimage->size() <<std::endl;

}

void imageProcessing::getBodyPoints()
{
	//Finding Head Point
	int inc = result_.cols / 40;
	int y;
	int heady = 0;
	for (y = 0; y < result_.rows - inc / 2; y += inc / 2) {
		for (int x = 0; x < result_.cols; x++)
		{
			Vec3b colour = result_.at<uchar>(Point(x, y));
			if (colour.val[0] > 0) {
				heady = y;
				break;
			}
		}
		if (heady > 0)
			break;
	}
	int headTipy = y;
	y += inc * 3;
	int startx = 0, endx = 0;
	for (int x = 0; x < result_.cols; x++)
	{
		Vec3b colour = result_.at<uchar>(Point(x, y));
		if (startx == 0 && colour.val[0] > 0) {
			startx = x;
		}
		if (colour.val[0] > 0) {
			endx = x;
		}
	}
	head = Point(startx + (endx - startx) / 2, y);
	headTip = Point(startx + (endx - startx) / 2, headTipy);

	int lastWidth = endx - startx;
	int nextWidth = 0;

	if (face.height == 0) {
		//Finding start of shoulders shoulders points
		printf("face.height is zero y=%d inc=%d\n", y, inc);
		for (y; y < result_.rows - inc / 2; y += inc / 2) {
			startx = 0, endx = 0;
			for (int x = 0; x < result_.cols; x++)
			{
				Vec3b colour = result_.at<uchar>(Point(x, y));
				if (startx == 0 && colour.val[0] > 0) {
					startx = x;
				}
				if (colour.val[0] > 0) {
					endx = x;
				}
			}
			nextWidth = endx - startx;
			//if (nextWidth > lastWidth + lastWidth*0.2) 
		    if (nextWidth > lastWidth + lastWidth*0.7) {
				lastWidth = nextWidth;
				break;
			}
		}

		//Finding Points of shoulders
		for (y += inc; y < result_.rows - inc; y += inc) {
			startx = 0, endx = 0;
			for (int x = 0; x < result_.cols; x++)
			{
				Vec3b colour = result_.at<uchar>(Point(x, y));
				if (startx == 0 && colour.val[0] > 0) {
					startx = x;
				}
				if (colour.val[0] > 0) {
					endx = x;
				}
			}
			nextWidth = endx - startx;
			if (nextWidth > lastWidth + lastWidth*0.2) {
				lastWidth = nextWidth;
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		//............................................
		printf("face.height is not zero %d\n", face.height);
		startx = 0, endx = 0;
		//y = face.y + face.height + face.height * 0.6;
		y = face.y + face.height + face.height * 0.6;
		for (int x = 0; x < result_.cols; x++)
		{
			Vec3b colour = result_.at<uchar>(Point(x, y));
			if (startx == 0 && colour.val[0] > 0) {
				startx = x;
			}
			if (colour.val[0] > 0) {
				endx = x;
			}
		}
		//................................................
	}
	printf("shoulder y=%d\n", y);

	shoulder[0] = Point(startx + inc / 2, y);
	shoulder[1] = Point(endx - inc / 2, y);

	//Finding Hand Points
	lastWidth = shoulder[1].x - shoulder[0].x;
	y = shoulder[0].y;
	int Lstartx = 0, Lendx = 0;
	for (y += inc; y < result_.rows - inc; y += inc) {
		Lstartx = startx;
		Lendx = endx;
		startx = 0, endx = 0;
		for (int x = 0; x < result_.cols; x++)
		{
			Vec3b colour = result_.at<uchar>(Point(x, y));
			if (startx == 0 && colour.val[0] > 0) {
				startx = x;
			}
			if (colour.val[0] > 0) {
				endx = x;
			}
		}
		nextWidth = endx - startx;
		if (nextWidth > lastWidth*0.9 && nextWidth < lastWidth * 1.4) {
			lastWidth = nextWidth;
		}
		else
		{
			break;
		}
	}
	y -= inc;
	int armYFull = y - shoulder[0].y;
	y -= armYFull * 0.17;
	Arm[0] = Point(Lstartx, y);
	Arm[1] = Point(Lendx, y);
	int xx = shoulder[0].x - Arm[0].x;
	int yy = shoulder[0].y - Arm[0].y;
	int armLnghtFull = sqrt((xx * xx) + (yy * yy));

	//Finding chest points
	y = shoulder[0].y + inc;  //Regaining y of shoulder to proceed for chest points calculation

	int startyChest = y + (y - head.y) / 4;
	int neckX = shoulder[0].x + (shoulder[1].x - shoulder[0].x) / 2;
	int xcount = 0;
	if (result_.cols / 2 > neckX)
		xcount = neckX;
	else
		xcount = result_.cols - neckX;

	int Cstartx = 0, Cendx = 0;
	for (y += (y - head.y) / 4; y < result_.rows - inc; y += 1) {
		Cstartx = 0, Cendx = 0;
		for (int x = 0; x < xcount; x++)
		{
			Vec3b colour = result_.at<uchar>(Point(neckX - x, y));
			if (Cstartx == 0 && colour.val[0] > 0) {
				//printf("x=%d Cstartx=%d\n", x, Cstartx);
				Cstartx = neckX - x;
			}
			colour = result_.at<uchar>(Point(neckX + x, y));
			if (Cendx == 0 && colour.val[0] > 0) {
				Cendx = neckX + x;
			}
			if (Cstartx != 0 && Cendx != 0
				&& Cendx - Cstartx < (shoulder[1].x - shoulder[0].x)
				&& Cendx - Cstartx >(shoulder[1].x - shoulder[0].x) / 2
				&& abs((neckX - Cstartx) - (Cendx - neckX)) < (shoulder[1].x - shoulder[0].x)*0.1)
				break;
		}
		if (Cstartx != 0 && Cendx != 0
			&& Cendx - Cstartx < (shoulder[1].x - shoulder[0].x)
			&& Cendx - Cstartx >(shoulder[1].x - shoulder[0].x) / 2
			//&& abs((neckX - Cstartx) - (Cendx - neckX)) < (shoulder[1].x - shoulder[0].x)*0.5)
			&& abs((neckX - Cstartx) - (Cendx - neckX)) < (shoulder[1].x - shoulder[0].x)*0.5)
			break;
	}
	chest[0] = Point(Cstartx, y - 10);
	chest[1] = Point(Cendx, y - 10);

	//Finding wast points
	int StartYWast = shoulder[0].y + armLnghtFull*0.7;

	//int StartYWast = shoulder[0].y + (shoulder[0].y - head.y) * 2;
	int Wstartx = 0, Wendx = 0;
	for (y = StartYWast; y < result_.rows - inc; y += 1) {
		Wstartx = 0, Wendx = 0;
		for (int x = 0; x < xcount; x++)
		{
			Vec3b colour = result_.at<uchar>(Point(neckX - x, y));
			if (Wstartx == 0 && colour.val[0] > 0) {
				Wstartx = neckX - x;
			}
			colour = result_.at<uchar>(Point(neckX + x, y));
			if (Wendx == 0 && colour.val[0] > 0) {
				Wendx = neckX + x;
			}
			if (Wstartx != 0 && Wendx != 0
				&& Wendx - Wstartx < shoulder[1].x - shoulder[0].x
				&& Wendx - Wstartx >(shoulder[1].x - shoulder[0].x) / 2
				&& (neckX - Wstartx) - (Wendx - neckX) < (shoulder[1].x - shoulder[0].x)*0.1)
				break;
		}
		if (Wstartx != 0 && Wendx != 0
			&& Wendx - Wstartx < shoulder[1].x - shoulder[0].x
			&& Wendx - Wstartx >(shoulder[1].x - shoulder[0].x) / 2
			&& (neckX - Wstartx) - (Wendx - neckX) < (shoulder[1].x - shoulder[0].x)*0.1)
			break;
	}
	wast[0] = Point(Wstartx, y - 10);
	wast[1] = Point(Wendx, y - 10);

	//Finding Hip points
	int StartYHip = shoulder[0].y + armLnghtFull;

	//int StartYWast = shoulder[0].y + (shoulder[0].y - head.y) * 2;
	int Hstartx = 0, Hendx = 0;
	for (y = StartYHip; y < result_.rows - inc; y += 1) {
		Hstartx = 0, Hendx = 0;
		for (int x = 0; x < xcount; x++)
		{
			Vec3b colour = result_.at<uchar>(Point(neckX - x, y));
			if (Hstartx == 0 && colour.val[0] > 0) {
				Hstartx = neckX - x;
			}
			colour = result_.at<uchar>(Point(neckX + x, y));
			if (Hendx == 0 && colour.val[0] > 0) {
				Hendx = neckX + x;
			}
			if (Hstartx != 0 && Hendx != 0
				&& Hendx - Hstartx < shoulder[1].x - shoulder[0].x
				&& Hendx - Hstartx >(shoulder[1].x - shoulder[0].x) / 2
				&& (neckX - Hstartx) - (Hendx - neckX) < (shoulder[1].x - shoulder[0].x)*0.1)
				break;
		}
		if (Hstartx != 0 && Hendx != 0
			&& Hendx - Hstartx < shoulder[1].x - shoulder[0].x
			&& Hendx - Hstartx >(shoulder[1].x - shoulder[0].x) / 2
			&& (neckX - Hstartx) - (Hendx - neckX) < (shoulder[1].x - shoulder[0].x)*0.1)
			break;
	}
	hip[0] = Point(Hstartx, y - 10);
	hip[1] = Point(Hendx, y - 10);

	//Finding toe point
	int toey = 0;
	for (y = result_.rows - 1; y > result_.rows*0.6; y -= 1) {
		for (int x = 0; x < result_.cols; x++)
		{
			Vec3b colour = result_.at<uchar>(Point(x, y));
			if (colour.val[0] > 0) {
				toey = y;
				break;
			}
		}
		if (toey > 0)
			break;
	}
	toe = Point(head.x, toey);

	/////////////////////////////////////////////////// Side points Identification /////////////////////////////////////////////////////

	// Getting Side points
	//Finding chest side points

	y = chest[0].y;
	startx = 0, endx = 0;
	for (int x = 0; x < Sresult_.cols; x++)
	{
		Vec3b colour = Sresult_.at<uchar>(Point(x, y));
		if (startx == 0 && colour.val[0] > 0) {
			startx = x;
		}
		if (colour.val[0] > 0) {
			endx = x;
		}
	}

	Schest[0] = Point(startx, y);
	Schest[1] = Point(endx, y);

	//Finding wast side points
	y = wast[0].y;
	startx = 0, endx = 0;
	for (int x = 0; x < Sresult_.cols; x++)
	//for (int x = 90; x < Sresult_.cols; x++)

	{
		Vec3b colour = Sresult_.at<uchar>(Point(x, y));
		if (startx == 0 && colour.val[0] > 0) {
			startx = x;
		}
		if (colour.val[0] > 0) {
			endx = x;
		}
	}

	Swast[0] = Point(startx, y);
	Swast[1] = Point(endx, y);

	//Finding Hip side points
	y = hip[0].y;
	startx = 0, endx = 0;
	for (int x = 0; x < Sresult_.cols; x++)
	{
		Vec3b colour = Sresult_.at<uchar>(Point(x, y));
		if (startx == 0 && colour.val[0] > 0) {
			startx = x;
		}
		if (colour.val[0] > 0) {
			endx = x;
		}
	}

	Ship[0] = Point(startx, y);
	Ship[1] = Point(endx, y);

	//Finding side arm points
	Sarm[0] = Point(Schest[0].x + (Schest[1].x - Schest[0].x) / 2, shoulder[0].y);
	Sarm[1] = Point(Schest[0].x + (Schest[1].x - Schest[0].x) / 2, shoulder[0].y + armLnghtFull);

	//Finding side leg points
	Sleg[0] = Point(Swast[0].x + (Swast[1].x - Swast[0].x) / 2, Swast[0].y);
	Sleg[1] = Point(Swast[0].x + (Swast[1].x - Swast[0].x) / 2, toe.y);

  // re-set y positions according to proportion
  int heightPixels = toey - headTipy;
  float headlen = heightPixels/7.5;
  chest[0].y = headTipy + 2*headlen;
  chest[1].y = headTipy + 2*headlen;
  wast[0].y = headTipy + 3*headlen;
  wast[1].y = headTipy + 3*headlen;
  hip[0].y = headTipy + 4*headlen;
  hip[1].y = headTipy + 4*headlen;
  Schest[0].y = headTipy + 2*headlen;
  Schest[1].y = headTipy + 2*headlen;
  Swast[0].y = headTipy + 3*headlen;
  Swast[1].y = headTipy + 3*headlen;
  Ship[0].y = headTipy + 4*headlen;
  Ship[1].y = headTipy + 4*headlen;
}


void imageProcessing::FEquilizeLuminnce() {
	EquilizeLuminnce(&img);
}
void imageProcessing::FdepressBackground() {
	depressBackground(&img);
}
void imageProcessing::Frgb_to_gray() {
	rgb_to_gray(&img, &grayscale);
}
void imageProcessing::Fdo_edge_detection() {
	do_edge_detection(&grayscale, &edge_);
}
void imageProcessing::Fdo_contour() {
	do_contour(&edge_, &result_);
}


void imageProcessing::SEquilizeLuminnce() {
	EquilizeLuminnce(&Simg);
}
void imageProcessing::SdepressBackground() {
	depressBackground(&Simg);
}
void imageProcessing::Srgb_to_gray() {
	rgb_to_gray(&Simg, &Sgrayscale);
}
void imageProcessing::Sdo_edge_detection() {
	do_edge_detection(&Sgrayscale, &Sedge_);
}
void imageProcessing::Sdo_contour() {
	do_contour(&Sedge_, &Sresult_);
}

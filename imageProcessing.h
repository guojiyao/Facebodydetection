
//#include "stdafx.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;



class imageProcessing
{
	void EquilizeLuminnce(Mat* image);
	void depressBackground(Mat* image);
	void rgb_to_gray(Mat* image, Mat* gimage);
	void do_edge_detection(Mat* gimage, Mat* eimage);
	void do_contour(Mat* eimage, Mat* rimage);
public:
	Rect face;
	vector<vector<cv::Point> > contours_;
	vector<Vec4i> hierarchy_;
	int edge_thresh;// use different threshold for optimum result  or use adaptice threshold
	Mat img, grayscale, edge_, result_, cntIMage;
	Mat Simg, Sgrayscale, Sedge_, Sresult_, ScntIMage;
	cv::Point headTip, head, toe, shoulder[2], wast[2], chest[2], hip[2], Arm[2];
	cv::Point Swast[2], Schest[2], Ship[2], Sarm[2], Sleg[2];
	bool isFornt;

	void FEquilizeLuminnce();
	void FdepressBackground();
	void Frgb_to_gray();
	void Fdo_edge_detection();
	void Fdo_contour();

	void SEquilizeLuminnce();
	void SdepressBackground();
	void Srgb_to_gray();
	void Sdo_edge_detection();
	void Sdo_contour();

	void preprocessImages();
	void getBodyPoints();
	imageProcessing();
};

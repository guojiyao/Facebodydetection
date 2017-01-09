#ifndef SIDEEARDETECTION_H
#define SIDEEARDETECTION_H

#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>

std::vector<cv::Rect> detectEar(cv::Mat img, bool sampleflag, bool fliptag);
std::vector<cv::Rect> detectNose(cv::Mat img, bool sampleflag, bool fliptag);

#endif


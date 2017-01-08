#ifndef COLOR_TRANSFER_H
#define COLOR_TRANSFER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

cv::Mat color_transfer(cv::Mat source, cv::Mat target);
std::vector<double> image_stats(cv::Mat image);

#endif // COLOR_TRANSFER_H

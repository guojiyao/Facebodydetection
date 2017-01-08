#ifndef EDGE_H
#define EDGE_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

cv::Mat cut(const cv::Mat& img, cv::Mat gc_mask, cv::Rect rect, cv::Mat&, int use_mask = 1, int iters = 3, bool downsample = false);

#endif // EDGE_H

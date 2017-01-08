#ifndef IMUTILS_H
#define IMUTILS_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

cv::Mat read_image(std::string img_path);
bool save_image( cv::Mat img, std::string filename);
cv::Mat translate(const cv::Mat& image, int x, int y);
cv::Mat rotate(const cv::Mat& image, double angle, cv::Point2f center = cv::Point2f(0.F, 0.F), double scale = 1.0);
cv::Mat  resize(const cv::Mat& image, int width = 0, int height = 0, int inter = cv::INTER_AREA);
cv::Mat brightness_contrast(const cv::Mat& img, double alpha = 1, int beta = 0);
cv::Mat kernel_constr(int size = 3);

#endif // IMUTILS_H

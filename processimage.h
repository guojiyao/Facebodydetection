#ifndef PROCESSIMAGE_H
#define PROCESSIMAGE_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "face_detection.h"

bool compareContourAreas (const std::vector<cv::Point>& a, const std::vector<cv::Point>& b);
cv::Mat calc_face_hist(cv::Mat img_hsv, cv::Rect face,  double scale = 0.25);
cv::Mat draw_faces(const cv::Mat& img, const std::vector<cv::Rect>& faces);
cv::Mat find_hands(const cv::Mat& img, std::vector<std::vector<cv::Point> >  cnts, int MIN_AREA, int MAX_AREA, int body_x, int body_y, int body_w, int body_h, std::vector<cv::Rect>& right_hand, std::vector<cv::Rect>& left_hand,  FaceDetection fd);
cv::Mat apply_morphology(const cv::Mat& img, const std::string& morph_type, int kernel = 3);
std::vector<std::vector<cv::Point> > get_contours(const cv::Mat& img_hsv, const cv::Mat& roihist, cv::Mat& thresh, bool face = false);
cv::Mat grab_cut_combination(const cv::Mat& img, const cv::Mat& fd_mask, cv::Mat fd_mask_hands, const cv::Rect& rect, bool preproc);
std::vector<cv::Rect> find_face_from_skin(const cv::Mat& img_side_hsv, const cv::Mat& roihist, int MIN_AREA, int MAX_AREA, int height, int width, int x, int y);
cv::Mat preprocessor(const cv::Mat& img);

#endif // PROCESSIMAGE_H

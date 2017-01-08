#ifndef FACE_DETECTION_H
#define FACE_DETECTION_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

class FaceDetection{

 public:
    FaceDetection(int height, int width);
    std::pair<cv::Mat, std::vector<cv::Rect> > get_body_mask(const cv::Mat& img, bool body_rect = true, bool polyshape = true, bool coordinates = false, bool face_border = false);
    cv::Rect body_from_face(bool side_view = false);
    std::vector<cv::Rect> _detect_faces(cv::Mat img);
    void img_to_gray(cv::Mat& img);
    cv::Mat draw_rect(cv::Mat img, const std::vector<cv::Rect>& faces, bool border = false);
    cv::Mat draw_polyshape(cv::Mat img, const cv::Rect& body, bool hands = false);

    int mHeight;
    int mWidth;
    std::vector<cv::Rect> mFaces;

};

#endif // FACE_DETECTION_H

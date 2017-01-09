#ifndef BODYSEGMENTATOR_H
#define BODYSEGMENTATOR_H

#include "face_detection.h"
#include "imutils.h"
#include "processimage.h"
#include "color_transfer.h"
#include "ArgumentParser.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


class BodySegmentator
{
public:
    BodySegmentator();
    int run(const ArgumentParser& p);
    cv::Mat frontalImage() const
    {
        return m_frontalViewResultImage;
    }
    cv::Mat sideViewImage() const
    {
        return m_sideViewResultImage;
    }
    std::tuple<bool,cv::Rect,cv::Rect> IoU(double old_h, double old_w, double new_h, double new_w, cv::Rect ear, cv::Rect face);

private:
    int IMG_WIDTH;
    int MIN_AREA ;
    int MAX_AREA ;
    cv::Mat m_frontalViewResultImage;
    cv::Mat m_sideViewResultImage;
};

#endif // BODYSEGMENTATOR_H

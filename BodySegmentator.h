#ifndef BODYSEGMENTATOR_H
#define BODYSEGMENTATOR_H

#include "face_detection.h"
#include "imutils.h"
#include "processimage.h"
#include "color_transfer.h"
#include "ArgumentParser.h"

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

private:
    int IMG_WIDTH;
    int MIN_AREA ;
    int MAX_AREA ;
    cv::Mat m_frontalViewResultImage;
    cv::Mat m_sideViewResultImage;
};

#endif // BODYSEGMENTATOR_H

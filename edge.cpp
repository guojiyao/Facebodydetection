#include "edge.h"

cv::Mat cut(const cv::Mat& img, cv::Mat gc_mask, cv::Rect rect,  cv::Mat& dst, int use_mask, int iters, bool downsample) //argument em avelacrel
{
    cv::Mat img2 = img.clone();
    if (downsample){
        cv::pyrDown(img2, img2);
        cv::resize(gc_mask, gc_mask, cv::Size(img2.cols, img2.rows));
        rect.x = rect.x / 2;
        rect.y = rect.y / 2;
        rect.width = rect.width / 2;
        rect.height = rect.height / 2;
    }
    cv::imwrite("myMask.png", gc_mask);

    for(int i = 0; i < gc_mask.rows;++ i) {
        for(int j = 0; j < gc_mask.cols; ++ j) {
            if(gc_mask.at<uchar>(i, j) == 255) {
                gc_mask.at<uchar>(i, j) = cv::GC_PR_FGD;
            } else {
                gc_mask.at<uchar>(i, j) = 0;
            }
        }
    }

    int mode;
    if (use_mask == 0)  // grabcut with rect
        mode = cv::GC_INIT_WITH_RECT;
    else
        mode = cv::GC_INIT_WITH_MASK;

    cv::Mat bgmodel(1, 65, CV_64FC1, cv::Scalar::all(0));
    cv::Mat fgmodel(1, 65, CV_64FC1, cv::Scalar::all(0));


    cv::grabCut(img2, gc_mask, rect, bgmodel, fgmodel, iters, mode);
    // Convert to 0, 255 binary mask

    cv::Mat mask2 = gc_mask.clone();

    for(int i = 0; i < gc_mask.rows;++ i) {
        for(int j = 0; j < gc_mask.cols; ++ j) {
            if(gc_mask.at<uchar>(i, j) == cv::GC_PR_FGD || gc_mask.at<uchar>(i, j) == cv::GC_FGD) {
                mask2.at<uchar>(i, j) = 255;
            } else {
                mask2.at<uchar>(i, j) = 0;
            }
        }
    }

    cv::Mat img_gc;
    cv::bitwise_and(img2, img2, img_gc, mask2);
    cv::pyrUp(img_gc, dst);
    cv::resize(mask2, mask2, cv::Size(img.cols, img.rows));
    return mask2;
  }

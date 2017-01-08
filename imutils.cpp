#include "imutils.h"

cv::Mat read_image(std::string img_path)
{
    // read image
    return cv::imread(img_path);
}

bool save_image(cv::Mat img, std::string filename)
{
    return cv::imwrite(filename, img);
}

cv::Mat translate(const cv::Mat& image, int x, int y)
{
    // Define the translation matrix and perform the translation
    cv::Mat shifted;
    cv::Mat M = (cv::Mat_<float>(2, 3) << 1, 0, x, 0, 1, y);
    cv::warpAffine(image, shifted, M, cv::Size(image.size().width, image.size().height));

    // Return the translated image
    return shifted;
}

cv::Mat rotate(const cv::Mat& image, double angle, cv::Point2f center, double scale)
{
    // Grab the dimensions of the image
    int w = image.size().width;
    int h = image.size().height;

    // If the center is None, initialize it as the center of
    // the image
    if (center == cv::Point2f(0.F, 0.F)){
        center = cv::Point2f(w / 2.0F, h / 2.0F);
    }
    // Perform the rotation
    cv::Mat rotated;
    cv::Mat M = cv::getRotationMatrix2D(center, angle, scale);
    cv::warpAffine(image,rotated, M, cv::Size(w, h));

    // Return the rotated image
    return rotated;
}

cv::Mat  resize(const cv::Mat& image, int width, int height, int inter)
{
    // initialize the dimensions of the image to be resized and
    // grab the image size
    cv::Size dim (0, 0);
    int w = image.size().width;
    int h = image.size().height;

    // if both the width and height are None, then return the
    // original image
    if (!width && !height){
        return image;
    }
    // check to see if the width is None
    if (!width){
        // calculate the ratio of the height and construct the
        // dimensions
        float r = height / float(h);
        dim = cv::Size(int(w * r), height);
      }

    // otherwise, the height is None
    else{
        // calculate the ratio of the width and construct the
        // dimensions
        float r = width / float(w);
        dim = cv::Size(width, int(h * r));
     }
    // resize the image
    cv::Mat resized;
    cv::resize(image, resized, dim, 0, 0, inter);

    // return the resized image
    return resized;
}

cv::Mat brightness_contrast(const cv::Mat& img, double alpha, int beta)
{
    /*Adjust brightness and contrast.
    Args:
        alpha -- contrast coefficient (1.0 - 3.0)
        beta -- brightness increment (0 - 100)
    Returns:
        result -- image with adjustments applied
    */
    cv::Mat result = cv::Mat::zeros(img.size(), CV_8UC3);
    //TODO: Use cv::addweithed instead
    for( int y = 0; y < img.rows; y++){
        for( int x = 0; x < img.cols; x++){
            for( int c = 0; c < 3; c++){
               result.at<cv::Vec3b>(y,x)[c] = cv::saturate_cast<uchar>(alpha * (img.at<cv::Vec3b>(y,x)[c]) + beta);
            }
          }
       }

    return result;
 }

cv::Mat kernel_constr(int size)
{
    cv::Mat kernel = cv::Mat::ones(size, size, CV_8UC1);

    return kernel;
 }

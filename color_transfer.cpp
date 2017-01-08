#include "color_transfer.h"
#include <numeric>
#include <functional>

cv::Mat color_transfer(cv::Mat source, cv::Mat target)
{
    /*
    Transfers the color distribution from the source to the target
    image using the mean and standard deviations of the L*a*b*
    color space.

    This implementation is (loosely) based on to the "Color Transfer
    between Images" paper by Reinhard et al., 2001.

    Parameters:
    -------
    source: NumPy array
        OpenCV image in BGR color space (the source image)
    target: NumPy array
        OpenCV image in BGR color space (the target image)

    Returns:
    -------
    transfer: NumPy array
        OpenCV image (w, h, 3) NumPy array (uint8)
    */
    // convert the images from the RGB to L*ab* color space, being
    // sure to utilizing the floating point data type (note: OpenCV
    // expects floats to be 32-bit, so use that instead of 64-bit)

    cv::Mat transfer;

    cv::cvtColor(source, source, CV_BGR2Lab);
    cv::cvtColor(target, target, CV_BGR2Lab);

    source.convertTo(source, CV_32F);
    target.convertTo(target, CV_32F);

    //cv::imshow("IMG1", source);
    //cv::imshow("IMG2", target);
    // compute color statistics for the source and target images
    std::vector<double> srcState = image_stats(source);
    std::vector<double> tarState = image_stats(target);

    // subtract the means from the target image
     std::vector<cv::Mat> mv;
     cv::split(target, mv);
     cv::Mat l = mv[0];
     cv::Mat a = mv[1];
     cv::Mat b = mv[2];

     /*pixel color modify*/
        for (int i = 0; i<l.rows; i++){
            for (int j = 0; j<l.cols; j++){
                double li =l.at<float>(i, j);
                if (i == 426 && j == 467)
                    std::cout << "i:" << i << "j:" << j << " " << li << std::endl;
                li -= tarState[0];
                li = (tarState[1] / srcState[1])*li;
                li += srcState[0];
               // li = (int)li % 256;
                l.at<float>(i, j) = cv::saturate_cast<uchar>(li);
            }
        }

        for (int i = 0; i<a.rows; i++){
            for (int j = 0; j<a.cols; j++){
                double ai = a.at<float>(i, j);
                ai -= tarState[2];
                ai = (tarState[3] / srcState[3])*ai;
                ai += srcState[2];
                //ai = (int)ai % 256;
                a.at<float>(i, j)= cv::saturate_cast<uchar>(ai);
            }
        }
    //(lMeanSrc, lStdSrc, aMeanSrc, aStdSrc, bMeanSrc, bStdSrc) =
        for (int i = 0; i<b.rows; i++){
            for (int j = 0; j<b.cols; j++){
                double bi = b.at<float>( i,  j);
                bi -= tarState[4];
                bi = (tarState[5] / srcState[5])*bi;
                bi += srcState[4];
                //bi = (int)bi % 256;
                b.at<float>(i, j) = cv::saturate_cast<uchar>(bi);
            }
        }

        mv.clear();
        mv.push_back(l);
        mv.push_back(a);
        mv.push_back(b);


    // merge the channels together and convert back to the RGB color
    // space, being sure to utilize the 8-bit unsigned integer data
    // type
    cv::merge(mv, transfer);
    transfer.convertTo(transfer, CV_8UC1);
    cv::cvtColor(transfer, transfer, CV_Lab2BGR);

    // return the color transferred image
    return transfer;
}

std::vector<double> image_stats(cv::Mat image)
{
    /*
    Parameters:
    -------
    image: NumPy array
        OpenCV image in L*a*b* color space

    Returns:
    -------
    Tuple of mean and standard deviations for the L*, a*, and b*
    channels, respectively
    */
    // compute the mean and standard deviation of each channel
    std::vector<double> chn;

    cv::Scalar mean_l_Sc, mean_l_stdDev_Sc;
    cv::meanStdDev(image, mean_l_Sc, mean_l_stdDev_Sc);

    //double sq_sum_l = std::inner_product(diff_l.begin(), diff_l.end(), diff_l.begin(), 0.0);
    //double stdev_l = std::sqrt(sq_sum_l / vl.size());
    chn.push_back(mean_l_Sc[0]);
    chn.push_back(mean_l_stdDev_Sc[0]);
    chn.push_back(mean_l_Sc[1]);
    chn.push_back(mean_l_stdDev_Sc[1]);
    chn.push_back(mean_l_Sc[2]);
    chn.push_back(mean_l_stdDev_Sc[2]);

    //std::cout << mean_l_Sc[0] << "  " << mean_l_Sc[1] << "  " << mean_l_Sc[2] << "  "<< mean_l_stdDev_Sc[0] << "  "<< mean_l_stdDev_Sc[1] << "  "<< mean_l_stdDev_Sc << std::endl;
    return  chn;

//    std::vector<cv::Mat> mv;
//    cv::split(image, mv);
//    cv::Mat l = mv[0];
//    cv::Mat a = mv[1];
//    cv::Mat b = mv[2];

//    std::vector<double> chn;
//    std::vector<int> vl, va, vb;

//    /*statistics L space*/
//        for (int i = 0; i<l.rows; i++){
//            for (int j = 0; j<l.cols; j++){
//                int li = l.data[l.step[0] * i + l.step[1] * j];
//                vl.push_back(li);
//            }
//        }
//        double sum_l = std::accumulate(vl.begin(), vl.end(), 0.0);
//        double mean_l = sum_l / vl.size();
//        std::vector<double> diff_l(vl.size());
//        std::transform(vl.begin(), vl.end(), diff_l.begin(),std::bind2nd(std::minus<double>(), mean_l));


//        /*statistics A space*/
//        for (int i = 0; i<a.rows; i++){
//            for (int j = 0; j<a.cols; j++){
//                int ai = a.data[a.step[0] * i + a.step[1] * j];
//                va.push_back(ai);
//            }
//        }
//        double sum_a = std::accumulate(va.begin(), va.end(), 0.0);
//        double mean_a = sum_a / va.size();
//        std::vector<double> diff_a(va.size());
//        std::transform(va.begin(), va.end(), diff_a.begin(),std::bind2nd(std::minus<double>(), mean_a));
//        double sq_sum_a = std::inner_product(diff_a.begin(), diff_a.end(), diff_a.begin(), 0.0);
//        double stdev_a = std::sqrt(sq_sum_a / va.size());
//        chn.push_back(mean_a);
//        chn.push_back(stdev_a);

//        /*statistics B space*/
//        for (int i = 0; i<b.rows; i++){
//            for (int j = 0; j<b.cols; j++){
//                int bi = b.data[b.step[0] * i + b.step[1] * j];
//                vb.push_back(bi);
//            }
//        }
//        double sum_b = std::accumulate(vb.begin(), vb.end(), 0.0);
//        double mean_b = sum_b / vb.size();
//        std::vector<double> diff_b(vb.size());
//        std::transform(vb.begin(), vb.end(), diff_b.begin(),
//            std::bind2nd(std::minus<double>(), mean_b));
//        double sq_sum_b = std::inner_product(diff_b.begin(), diff_b.end(), diff_b.begin(), 0.0);
//        double stdev_b = std::sqrt(sq_sum_b / vb.size());
//        chn.push_back(mean_b);
//        chn.push_back(stdev_b);

//    // return the color statistics

//    return  chn;
}

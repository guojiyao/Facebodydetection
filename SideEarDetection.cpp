#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

#include <dlib/svm_threaded.h>
#include <dlib/string.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>
#include <dlib/cmd_line_parser.h>
#include <dlib/image_transforms.h>

#include <iostream>
#include <fstream>

#include "SideEarDetection.h"

std::vector<cv::Rect> detectEar(cv::Mat img, bool sampleflag, bool fliptag){
    std::vector<cv::Rect> output_ear_cnt;
    const unsigned long upsample_amount =2;
    const unsigned long image_batch =1;
    typedef dlib::scan_fhog_pyramid<dlib::pyramid_down<24> > image_scanner_type_small;
    typedef dlib::scan_fhog_pyramid<dlib::pyramid_down<6> > image_scanner_type_large;

    std::ifstream fin_ear("SideEar.svm", std::ios::binary);
    if (fliptag) {
        std::cout<<"Flip Image..."<<std::endl;
        fin_ear.close();
        fin_ear.clear();
        fin_ear.open("SideEarFlip.svm", std::ios::binary);
    }
    if (!fin_ear)
        {
            std::cout << "Can't find a trained object detector file SideEar.svm. " << std::endl;
            std::cout << "You need to train one using the -t option." << std::endl;
            std::cout << "\nTry the -h option for more information." << std::endl;
       
        }
    
    dlib::object_detector<image_scanner_type_large> large_ear_detector;
    dlib::object_detector<image_scanner_type_small> small_ear_detector;
    if(sampleflag==true) dlib::deserialize(small_ear_detector, fin_ear);
    else dlib::deserialize(large_ear_detector, fin_ear);

    dlib::array2d<dlib::bgr_pixel> cimg;
    dlib::assign_image(cimg, dlib::cv_image<dlib::bgr_pixel>(img));

    if (sampleflag == true){
    for (unsigned long i = 0; i < upsample_amount; ++i){
        std::cout<<"Upsample image"<<std::endl;
        dlib::pyramid_up(cimg);
        }
    }

    std::vector<dlib::rectangle> rects;
    std::vector<dlib::rectangle> ear_rects;
 
    if (sampleflag==true){
        ear_rects = small_ear_detector(cimg);
        std::cout << "Number of Ear detections: "<< ear_rects.size() << std::endl;
    }
    else{
        ear_rects = large_ear_detector(cimg);
        std::cout << "Number of Ear detections: "<< ear_rects.size() << std::endl;
    }

    for (int i =0; i< ear_rects.size();i++){
        
        auto d = ear_rects.at(i);
        cv::Rect c(d.left(), d.top(), d.width(), d.height());
        std::cout << d.left() << "  "<< d.top()<< "  "<< d.width() << "  "<< d.height() <<  std::endl;
        output_ear_cnt.push_back(c);
    }

    std::cout << "Hit enter to compute body size.";
    std::cin.get();
    return output_ear_cnt;
}


std::vector<cv::Rect> detectNose(cv::Mat img, bool sampleflag, bool fliptag){
    std::vector<cv::Rect> output_nose_cnt;
    const unsigned long upsample_amount =2;
    const unsigned long image_batch =1;
    typedef dlib::scan_fhog_pyramid<dlib::pyramid_down<24> > image_scanner_type_small;
    typedef dlib::scan_fhog_pyramid<dlib::pyramid_down<6> > image_scanner_type_large;
    
    std::ifstream fin_nose("SideEyNose.svm", std::ios::binary);
    if (fliptag) {
        std::cout<<"Flip Image..."<<std::endl;
        fin_nose.close();
        fin_nose.clear();
        fin_nose.open("SideEyNoseFlip.svm", std::ios::binary);
    }
    if (!fin_nose)
        {
            std::cout << "Can't find a trained object detector file SideEyNose.svm. " << std::endl;
            std::cout << "You need to train one using the -t option." << std::endl;
            std::cout << "\nTry the -h option for more information." << std::endl;

        }

    dlib::object_detector<image_scanner_type_large> large_nose_detector;
    dlib::object_detector<image_scanner_type_small> small_nose_detector;
    if(sampleflag==true) dlib::deserialize(small_nose_detector, fin_nose);
    else dlib::deserialize(large_nose_detector, fin_nose);

    dlib::array2d<dlib::bgr_pixel> cimg;
    dlib::assign_image(cimg, dlib::cv_image<dlib::bgr_pixel>(img));

    if (sampleflag == true){
    for (unsigned long i = 0; i < upsample_amount; ++i){
        std::cout<<"Upsample image"<<std::endl;
        dlib::pyramid_up(cimg);
        }
    }

    std::vector<dlib::rectangle> rects;

    if (sampleflag==true){
        rects = small_nose_detector(cimg);
        std::cout << "Number of Nose  detections: "<< rects.size() << std::endl;
    }
    else{
        rects = large_nose_detector(cimg);
        std::cout << "Number of Nose detections: "<< rects.size() << std::endl;
    }

    for (int i =0; i< rects.size();i++){

        auto d = rects.at(i);
        cv::Rect c(d.left(), d.top(), d.width(), d.height());
        std::cout << d.left() << "  "<< d.top()<< "  "<< d.width() << "  "<< d.height() <<  std::endl;
        output_nose_cnt.push_back(c);
    }

    std::cout << "Hit enter to compute body size.";
    std::cin.get();
    return output_nose_cnt;
}

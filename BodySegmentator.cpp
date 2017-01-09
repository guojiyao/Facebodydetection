#include "BodySegmentator.h"
#include "ArgumentParser.h"
#include "face_detection.h"
#include "SideEarDetection.h"
#include <opencv2/core/core.hpp>

BodySegmentator::BodySegmentator()
{
    IMG_WIDTH = 700;
    MIN_AREA = -1;
    MAX_AREA = -1;
}

int BodySegmentator::run(const ArgumentParser& p)
{
    m_frontalViewResultImage = cv::Mat();
    m_sideViewResultImage = cv::Mat();

    if(p.getWidth() != ArgumentParser::s_invalidNumericalValue) {
        IMG_WIDTH = p.getWidth();
    }

    MIN_AREA = IMG_WIDTH * 5;
    MAX_AREA = IMG_WIDTH * 15;
    cv::Mat img_hsv;

    cv::Mat img = cv::imread(p.getFrontPath());
	std::cout << "Img.size() " << img.size()<<std::endl;
    if(img.empty()) {
        std::cout << "Specified frontal path: " << p.getFrontPath() << "  is not valid " << std::endl;
        exit(-1);
    }
    img = resize(img, IMG_WIDTH);
    
    std::cout << "Img.size() after resize " << img.size()<<std::endl;
    cv::cvtColor(img, img_hsv, cv::COLOR_BGR2HSV);
    int height = img.rows;
    int width = img.cols;
    cv::Mat img_side;
    cv::Mat img_side_hsv;
    cv::Mat img_side_trans;
    cv::Mat img_side_ear;
    int height_side;
    int width_side;
    int SIDE_VIEW = 0;

    if(p.getSidePath() != ArgumentParser::s_invalidPath) {
        SIDE_VIEW = 1;
        img_side = cv::imread(p.getSidePath());
        img_side_ear = img_side.clone();
        img_side = resize(img_side, IMG_WIDTH);
        
        // transfer the color distribution from the source image

        img_side_trans = color_transfer(img.clone(), img_side.clone());
        cv::cvtColor(img_side_trans, img_side_hsv, cv::COLOR_BGR2HSV);
        height_side = img_side.rows;
        width_side = img_side.cols;

    }

    //cv::imshow("Image", img);
    FaceDetection fd = FaceDetection(int(height), int(width));
    std::vector<cv::Rect> face_coords_lst = fd.get_body_mask(img, false, false, true).second;
    cv::Rect bodyRect = fd.body_from_face();
    int body_x = bodyRect.x;
    int body_y = bodyRect.y;
    int body_w = bodyRect.width;
    int body_h = bodyRect.height;
    cv::Mat img_face = draw_faces(img, face_coords_lst);
    //cv::imshow("FRONT face detected", img_face);
    // Assume that the first face detected is the correct one
    cv::Mat skin_hist = calc_face_hist(img_hsv, face_coords_lst[0]);
    //cv::imshow("FRONT", skin_hist);
    // Detect hands with face colors histogram (skinhist)
    std::vector<std::vector<cv::Point> > cnts;
    cv::Mat thresh;
    cnts = get_contours(img_hsv, skin_hist, thresh, false);
    //cv::imshow("thresh", thresh);
    cv::Mat fd_mask = fd.get_body_mask(img, true, false, false, true).first;
    //cv::imshow("h sk", fd_mask);
    std::vector<cv::Rect> right_hand;
    std::vector<cv::Rect> left_hand;
    cv::Mat hands_mask = find_hands(img, cnts, MIN_AREA, MAX_AREA, body_x, body_y, body_w, body_h, right_hand, left_hand, fd);

    cv::Mat fd_mask_hands;
    cv::bitwise_or(fd_mask, hands_mask, fd_mask_hands);
    //cv::imshow("hands_mask", hands_mask);
    // Calc rectangle for GrabCut alg
    int x = right_hand.at(0).x;
    int y = fd.mFaces.at(0).y;
    int h = fd.body_from_face().height;
    int w = (left_hand.at(0).x + left_hand.at(0).width) - (right_hand.at(0).x);
    cv::Rect rect(x, y, w, h);
  
    cv::Mat complex_mask = grab_cut_combination(img, fd_mask, fd_mask_hands, rect, p.getPreproc());
    cv::Mat img_masked;
    cv::Mat img_side_face;
    std::vector<cv::Rect> face_coords_lst_side;
    //cv::imshow("M2", fd_mask_hands);
    if (SIDE_VIEW){
         //img_side_hsv = cv::imread("C:/Users/User6/Desktop/img_side_hsv.png");
         //cv::imshow("img_side_hsv", img_side_hsv);

        face_coords_lst_side = find_face_from_skin(img_side_hsv, skin_hist, MIN_AREA, MAX_AREA, height, width, x, y);
        //std::cout << face_coords_lst_side.size();
        img_side_face = img_side.clone();
        img_side_face = draw_faces(img_side_face, face_coords_lst_side);

        //cv::imshow("SIDE face detected", img_side_face);

        // Create instance of FaceDetect just to use some of its methods
        FaceDetection fd_side = FaceDetection(int(height_side), int(width_side));
        //Ear detection
        if(p.getEar()){ 
        std::vector<cv::Rect> ear = detectEar(img_side_ear.clone(),false,p.getFlip());
        if (ear.size() > 0){
            cv::Rect biggest_ear = ear[0];
            for (int i = 1; i < ear.size(); ++i) {
                if (biggest_ear.area() < ear.at(i).area()) {
                    biggest_ear = ear.at(i);
                }
            }
            ear = std::vector<cv::Rect>(1, biggest_ear);
            cv::Mat ear_image = fd_side.draw_rect(img_side_ear,ear);
            cv::imwrite("ear_detectoin.png",ear_image);
        }
        }

        //Nose detection
        if (p.getNose()){
        cv::Mat img_side_nose = img_side_ear.clone(); 
        std::vector<cv::Rect> nose = detectNose(img_side_nose.clone(),false,p.getFlip());
        if (nose.size() > 0){
            //cv::Rect biggest_nose = nose[0];
            //for (int i = 1; i < nose.size(); ++i) {
            //    if (biggest_nose.area() < nose.at(i).area()) {
            //        biggest_nose = nose.at(i);
            //    }
            //}
            //nose = std::vector<cv::Rect>(1, biggest_nose);
            cv::Mat nose_image = fd_side.draw_rect(img_side_nose,nose);
            cv::imwrite("nose_detectoin.png",nose_image);
        }
        }
        
        if (face_coords_lst_side.size() != 0)
            fd_side.mFaces = face_coords_lst_side;

        // Create body mask manually
        cv::Mat fd_mask_side = cv::Mat::zeros(cv::Size(width_side, height_side), CV_8UC3);
        fd_mask_side = fd_side.draw_rect(fd_mask_side, fd_side.mFaces, true);
        cv::Rect body = fd_side.body_from_face(true);
        fd_mask_side = fd_side.draw_rect(fd_mask_side, std::vector<cv::Rect>(1, body));
        fd_side.img_to_gray(fd_mask_side);

        // Calc rectangle for GrabCut alg
        int x = fd_side.body_from_face(true).x;
        int y = fd_side.mFaces.at(0).y;
        int h = fd_side.body_from_face(true).height;
        int w = fd_side.body_from_face(true).width;
        cv::Rect rect_side(x, y, w, h);

        cv::Mat complex_mask_side = grab_cut_combination(img_side, fd_mask_side, fd_mask_side, rect_side, p.getPreproc());

        // mask out the human body region
        cv::bitwise_and(img_side, img_side, img_masked, fd_mask_side);
        cv::Mat tmpImage;
        cv::bitwise_and(img_side, img_side, tmpImage, complex_mask_side);
        //cv::imshow("SIDE GC", tmpImage);
        save_image(tmpImage, "masked_bodyhands_side.png");

        m_sideViewResultImage = tmpImage.clone();
    }

   // mask out the human body region
   cv::bitwise_and(img, img, img_masked, fd_mask);
	std::cout << img.size() << " " << img_masked.size() << " " << fd_mask.size()<<std::endl;
   cv::Mat tmpImage;
   cv::bitwise_and(img, img, tmpImage, complex_mask);


   //cv::imshow("FRONT GC", tmpImage);
   cv::bitwise_and(img, img, tmpImage, complex_mask);
   std::cout <<"TmpImage size = " << tmpImage.size() <<std::endl;
   save_image(tmpImage, "masked_bodyhands.png");
   m_frontalViewResultImage = tmpImage.clone();
   //cv::waitKey(0);
   // close any open windows
   cv::destroyAllWindows();


    return 0;
}

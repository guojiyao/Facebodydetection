#include "processimage.h"

#include "imutils.h"
#include "edge.h"

/*
Foreground segmentation with face detection (front view) and skin detection (hands on front, face on side view) and
GrabCut algorithm.
*/

 bool compareContourAreas (const std::vector<cv::Point>& a, const std::vector<cv::Point>& b)
 {
       return contourArea(a) > contourArea(b);
 }


const int HAND_TO_FACE = 6;  //Hand area to face area ratio
        // Resize each image to specified size

cv::Mat calc_face_hist(cv::Mat img_hsv,  cv::Rect face,  double scale)
{
    ///Calculate face region color histogram
    int x_face = face.x;
    int y_face = face.y;
    int h_face = face.height;
    int w_face = face.width;

    // Scale down to be sure we have only face pixels
    x_face += int(w_face * scale);
    w_face -= int(2 * w_face * scale);
    y_face += int(h_face * scale);
    h_face -= int(2 * h_face * scale);

    // get ROI
    cv::Mat face_roi_hsv = img_hsv(cv::Rect(x_face, y_face, w_face, h_face));

    // calculating ROI histogram
    cv::Mat roihist;
    std::vector<int> channels;
    channels.push_back(0);
    channels.push_back(1);
    std::vector<int> histSize;
    histSize.push_back(180);
    histSize.push_back(256);
    std::vector<float> ranges;
    ranges.push_back(0);
    ranges.push_back(180);
    ranges.push_back(0);
    ranges.push_back(256);
    std::vector<cv::Mat> chls;
    cv::split(face_roi_hsv, chls);
    cv::calcHist(chls, channels, cv::Mat(), roihist, histSize, ranges);

    // normalize histogram and apply backprojection
    cv::normalize(roihist, roihist, 0, 255, cv::NORM_MINMAX);

    return roihist;
}

cv::Mat draw_faces(const cv::Mat& img, const std::vector<cv::Rect>& faces)
{
    cv::Mat img_face = img.clone();
    for(int i  = 0; i < faces.size(); ++i){
        cv::Rect rect = faces.at(i);
        int x = rect.x;
        int y = rect.y;
        int w = rect.width;
        int h = rect.height;
        cv::rectangle(img_face, cv::Point(x, y), cv::Point(x + w, y + h), cv::Scalar(0, 255, 0), 2);
    }

    return img_face;
}

cv::Mat find_hands(const cv::Mat& img, std::vector<std::vector<cv::Point> >  cnts, int MIN_AREA, int MAX_AREA, int body_x, int body_y, int body_w, int body_h, std::vector<cv::Rect>& right_hand, std::vector<cv::Rect>& left_hand,  FaceDetection fd)
{
   int height = img.rows;
   int width = img.cols;
   cv::Mat hands_mask = cv::Mat::zeros(cv::Size(width, height), CV_8UC1);
   // loop over the contours

   for (int c = 0; c < cnts.size(); ++c) {
       cv::Rect rect;
       int x, y, w, h;
       double cnt_area = cv::contourArea(cnts[c]);
       if (MIN_AREA / HAND_TO_FACE <= cnt_area  && cnt_area < MAX_AREA / HAND_TO_FACE){
            // compute the bounding box for the contour, draw it on the frame,
            // and update the text
            rect = cv::boundingRect(cnts[c]);
            x = rect.x;
            y = rect.y;
            w = rect.width;
            h = rect.height;
            if ((x < body_x) && (y > body_y) && (y < body_y + body_h)){
                           std::cout << "Right hand detected" << std::endl;
                           right_hand.push_back(rect);
            }else if ((x > body_x + body_w) && (y > body_y) && (y < body_y + body_h)){
                           std::cout << "Left hand detected"  << std::endl;
                           left_hand.push_back(rect);
            }
       }
   }

   // If no hand detected - TRY TO GUESS its position
   int l_x = width / 3 * 2;
   int l_y = height / 8 * 3;
   int l_w = width / 3;
   int l_h = height / 8 * 2;

   int r_x = 0;
   int r_y = l_y;
   int r_w = l_w;
   int r_h = l_h;

   if (right_hand.empty()){
       std::cout << "No RIGHT hand detected!" << std::endl;
       right_hand.push_back(cv::Rect(r_x, r_y, r_w, r_h));
   }
   if (left_hand.empty()){
       std::cout << "No LEFT hand detected!" << std::endl;
       left_hand.push_back(cv::Rect(l_x, l_y, l_w, l_h));
   }
   cv::Mat img_hands = img.clone();
   for(int i  = 0; i < left_hand.size(); ++i)
       cv::rectangle(img_hands, left_hand.at(i), cv::Scalar(0, 255, 0), 2);

   for(int i  = 0; i < right_hand.size(); ++i)
       cv::rectangle(img_hands,right_hand.at(i), cv::Scalar(0, 0, 255), 2);
   std::cout << "l_x, l_y, l_w, l_h RRRRR)" <<  r_x << "  " <<  r_y << "  " <<  r_w <<  r_h << std::endl;
   std::cout << "l_x, l_y, l_w, l_h)    "  << l_x << "  " <<  l_y << "  " <<  l_w <<  l_h << std::endl;

   // Draw hands on mask
   int x = right_hand[0].x;
   int y = right_hand[0].y;
   int w = right_hand[0].width;
   int h = right_hand[0].height;
   cv::Point rh_bottom_1 (x, y + h);
   cv::Point rh_bottom_2 (x + w, y + h);

   int x1 = left_hand[0].x;
   int y1 = left_hand[0].y;
   int w1 = left_hand[0].width;
   int h1 = left_hand[0].height;
   cv::Point lh_bottom_1 (x1 + w1, y1 + h1);
   cv::Point lh_bottom_2 (x1, y1 + h1);

   cv::Rect  bodyRect = fd.body_from_face(fd.mFaces.size());  //poxel em mFaces-n false

   int x2 = bodyRect.x;
   int y2 = bodyRect.y;
   int w2 = bodyRect.width;
   int h2 = bodyRect.height;

   cv::Point r_shoulder_1 (x2, y2);
   cv::Point r_shoulder_2 (x2, y2 + h2 / 4);
   cv::Point l_shoulder_1 (x2 + w2, y2);
   cv::Point l_shoulder_2 (x2 + w2, y2 + h2 / 4);

   std::vector<cv::Point2i> pts(4, cv::Point2i(0, 0));
   pts[0] = r_shoulder_1;
   pts[1] = rh_bottom_1;
   pts[2] = rh_bottom_2;
   pts[3] = r_shoulder_2;
   cv::fillConvexPoly(hands_mask, pts, cv::Scalar(255, 255, 255), 1);
   std::cout << "pts";
   std::cout << pts[0] << " " << pts[1] << "  " << pts[2] << "  " << pts[3];

   pts[0] = l_shoulder_1;
   pts[1] = lh_bottom_1;
   pts[2] = lh_bottom_2;
   pts[3] = l_shoulder_2;
   cv::fillConvexPoly(hands_mask, pts, cv::Scalar(255, 255, 255), 1);
   std::cout << "pts";
   std::cout << pts[0] << " " << pts[1] << "  " << pts[2] << "  " << pts[3];
   // Cut white space on the mask if hands detected lower than some thresh
   double vertical_thresh = height / 8 * 5;  // lower than 5/8 of the image height - no hand

   hands_mask(cv::Rect(0, vertical_thresh, hands_mask.cols, hands_mask.rows - vertical_thresh)) = cv::Scalar::all(0);

   return hands_mask;// right_hand, left_hand
}

std::vector<cv::Rect> find_face_from_skin(const cv::Mat& img_side_hsv, const cv::Mat& roihist, int MIN_AREA, int MAX_AREA, int height, int width, int x, int y) ///avelacel em 2 argument
{
    // Find skin regions on img_side
    std::vector<std::vector<cv::Point> >  cnts;
    cv::Mat thresh;
    cnts = get_contours(img_side_hsv, roihist, thresh, true);

    // Get face from cnts
    // face_mask = np.zeros((height, width), dtype='uint8')
    std::vector<cv::Rect> face_coords_lst;
    // loop over the contours
    for(int c = 0; c < cnts.size(); ++c){
        // if the contour is too small, ignore it
        double cnt_area = cv::contourArea(cnts[c]);

        if (MIN_AREA <= cnt_area && cnt_area < MAX_AREA){
            // compute the bounding box for the contour, draw it on the frame,
             cv::Rect rect = cv::boundingRect(cnts[c]);
             std::cout << width << "  " << rect.x << rect.x  << "  " << height;
            // Check if the contour is within probable face region or not
            if ((width / 4 <= rect.x && rect.x < width / 4 * 3) && (10 <= rect.y && rect.y < height / 2)){
                std::cout << "Face detected on side view" << std::endl;
                std::cout << rect.x << "  " << rect.y << std::endl;
                face_coords_lst.push_back(rect);
                // face_mask = cv2.rectangle(face_mask, (x, y), (x + w, y + h), (0, 255, 0), -1)
            }
          }
    }

    return face_coords_lst;
}

cv::Mat grab_cut_combination(const cv::Mat& img, const cv::Mat& fd_mask, cv::Mat fd_mask_hands, const cv::Rect& rect, bool preproc)
{
    ///GrabCut mask

    cv::Mat img_processed = img.clone();
    cv::Mat img_gc_mask1;
    cv::Mat gc_mask1;
    // Apply different filters to highlight the edges
    if (preproc)
        img_processed = preprocessor(img_processed);

    gc_mask1 = cut(img_processed, fd_mask_hands, rect, img_gc_mask1, 1, 10, false);

    return gc_mask1;
}

std::vector<std::vector<cv::Point> > get_contours(const cv::Mat& img_hsv, const cv::Mat& roihist, cv::Mat& thresh, bool face) //avelacrel em argument thresh
{
    /// Get list of the first 10 contours that matches the skin histogram.

    cv::Mat dst;
    std::vector<int> channels;
    channels.push_back(0);
    channels.push_back(1);
    std::vector<float> ranges;
    ranges.push_back(0);
    ranges.push_back(180);
    ranges.push_back(0);
    ranges.push_back(256);

    std::vector<cv::Mat> chls;
    cv::split(img_hsv, chls);
    cv::calcBackProject(chls, channels, roihist, dst, ranges, 0.5);

    cv::Mat kernel;
    cv::Mat kernel_erode;
    if (face){
        // Pre-processing to extract skin on FACE
        kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
        kernel_erode = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));

        cv::erode(dst, dst, kernel_erode, cv::Point(-1,-1), 1);
        cv::dilate(dst, dst, kernel, cv::Point(-1,-1), 5);
        cv::filter2D(dst, dst, -1, kernel);

    }else{
        // Pre-processing to extract skin on HANDS and other regions
        kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7));
        cv::filter2D(dst, dst, -1,  kernel);
    }

    double ret = cv::threshold(dst, thresh, 125, 255, 0);

    // draw lines from head to hands
    std::vector<std::vector<cv::Point> > cnts;

    cv::findContours(thresh.clone(), cnts, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    std::sort(cnts.begin(), cnts.end(), compareContourAreas);
    std::vector<std::vector<cv::Point> > res;
    for (int i = 0; i < cnts.size() && i < 10; ++i) {
        res.push_back(cnts.at(i));
    }
    return res;
}

 cv::Mat  apply_morphology(const cv::Mat& img, const std::string& morph_type, int kernel)
{
   cv::Mat dst;
   if (morph_type == "erode"){
            cv::erode(img, dst, kernel_constr(kernel), cv::Point(-1,-1), 1);
   }  else if (morph_type == "dilate") {
            cv::dilate(img, dst, kernel_constr(kernel), cv::Point(-1,-1), 1);
   }  else if (morph_type == "open") {
            cv::morphologyEx(img, dst, cv::MORPH_OPEN, kernel_constr(kernel));
   }  else if (morph_type == "close") {
            cv::morphologyEx(img, dst, cv::MORPH_CLOSE, kernel_constr(kernel));
   }  else if (morph_type == "ellipse") {
            cv::morphologyEx(img, dst, cv::MORPH_ELLIPSE, kernel_constr(kernel));
   }

    return dst;
}

 void prewitt(cv::Mat img, cv::Mat res, bool h)
 {

     cv::Mat_<float> kernel(3, 3);
     kernel.at<float>(0 , 0) = -1;
     kernel.at<float>(0 , 1) = -1;
     kernel.at<float>(0 , 2) = -1;

     kernel.at<float>(1 , 0) = 0;
     kernel.at<float>(1 , 1) = 0;
     kernel.at<float>(1 , 2) = 0;

     kernel.at<float>(2 , 0) = +1;
     kernel.at<float>(2 , 1) = +1;
     kernel.at<float>(2 , 2) = +1;


     if(h) {
       cv::filter2D(img, res, CV_32F, kernel);
     } else {
       cv::filter2D(img, res, CV_32F, kernel.t());
     }
 }
 void roberts(cv::Mat img, cv::Mat res, bool h)
 {

     cv::Mat_<float> kernel(3, 3);
     if(h) {
         kernel.at<float>(0 , 0) = 1;
         kernel.at<float>(0 , 1) = 0;
         kernel.at<float>(0 , 2) = 0;

         kernel.at<float>(1 , 0) = 0;
         kernel.at<float>(1 , 1) = 0;
         kernel.at<float>(1 , 2) = 0;

         kernel.at<float>(2 , 0) = 0;
         kernel.at<float>(2 , 1) = 0;
         kernel.at<float>(2 , 2) = -1;


     } else {
         kernel.at<float>(0 , 0) = 0;
         kernel.at<float>(0 , 1) = 0;
         kernel.at<float>(0 , 2) = 1;

         kernel.at<float>(1 , 0) = 0;
         kernel.at<float>(1 , 1) = 0;
         kernel.at<float>(1 , 2) = 0;

         kernel.at<float>(2 , 0) = -1;
         kernel.at<float>(2 , 1) = 0;
         kernel.at<float>(2 , 2) = 0;
     }
     cv::filter2D(img, res, CV_32F, kernel);
 }


cv::Mat preprocessor(const cv::Mat& img)
{
    cv::Mat img_hsl;
    cv::cvtColor(img, img_hsl, cv::COLOR_BGR2HLS);

    cv::Mat img_lig;
    {
        std::vector<cv::Mat> chnls;
        cv::split(img_hsl, chnls);
        img_lig = chnls.at(1);
    }

    // horizontal filters
    cv::Mat img_sobel_h;
    cv::Sobel(img_lig, img_sobel_h, CV_32F, 0, 1);

    cv::Mat img_scharr_h;
    cv::Scharr(img_lig, img_scharr_h, CV_32F, 0, 1);

    cv::Mat img_prewitt_h;
    prewitt(img_lig, img_prewitt_h, true);

    // diagonal filters
    cv::Mat img_roberts_pos;
    cv::Mat img_roberts_neg;
    roberts(img_lig, img_roberts_neg, false);
    roberts(img_lig, img_roberts_pos, true);

    // vertical filters    
    cv::Mat img_sobel_v;
    cv::Sobel(img_lig, img_sobel_v, CV_32F, 1, 0);

    cv::Mat img_scharr_v;
    cv::Scharr(img_lig, img_scharr_v, CV_32F, 1, 0);

    cv::Mat img_prewitt_v;
    prewitt(img_lig, img_prewitt_v, false);

    cv::Mat horizontal;
    cv::add(img_sobel_h, img_scharr_h, horizontal, img_prewitt_h);
    horizontal = apply_morphology(horizontal, "dilate", 35);
    horizontal = apply_morphology(horizontal, "ellipse", 21);

    cv::Mat diagonal;
    cv::add(img_roberts_pos, img_roberts_neg, diagonal);
    diagonal = apply_morphology(diagonal,  "dilate", 35);
    diagonal = apply_morphology(diagonal, "ellipse", 35);

    cv::Mat vertical;
    cv::add(img_sobel_v, img_scharr_v, vertical, img_prewitt_v);
    vertical = apply_morphology(vertical, "dilate", 9);

    cv::Mat result;
    cv::add(horizontal, diagonal, result);
    cv::subtract(result, vertical, result);
    double minVal;
    double maxVal;
    cv::minMaxLoc(result, &minVal, &maxVal);
    result *= 255.0 / maxVal;
    cv::convertScaleAbs(result, result);

    cv::Mat mask;
    double thresh = cv::threshold(result, mask, 40, 255, cv::THRESH_BINARY);
    mask = apply_morphology(mask, "dilate", 35);
    cv::medianBlur(mask, mask, 35);
    mask = apply_morphology(mask, "ellipse", 35);
    cv::Mat mask_inv;
    cv::bitwise_not(mask, mask_inv);

    // apply blur to regions selected by mask
    cv::Mat blurred;
    cv::medianBlur(img, blurred, 81);
    cv::bitwise_and(blurred, blurred, blurred, mask);

    cv::Mat img_orig;
    cv::Mat select_blur;
    cv::GaussianBlur(img, img_orig, cv::Size(3, 3), 0);
    cv::bitwise_and(img_orig, img_orig, select_blur, mask_inv);
    cv::add(select_blur, blurred, select_blur);

    //
    cv::Mat img_proc; //enhance L-channel with contrast limited adaptive histogram equalization
    cv::cvtColor(select_blur, img_proc, cv::COLOR_BGR2HLS);
    std::vector<cv::Mat> channels;
    cv::split(img_proc, channels);
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(3.0, cv::Size(15, 15));
    clahe->apply(channels[1], channels[1]);
    cv::merge(channels, img_proc);
    cv::cvtColor(img_proc, img_proc, cv::COLOR_HLS2BGR);

    return img_proc;
}


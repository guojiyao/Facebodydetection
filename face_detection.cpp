#include "face_detection.h"
#include "dlib/image_processing/frontal_face_detector.h"
#include "dlib/opencv.h"


#define BODY_W_COEFF   2.5  // Human body to head width proportion
#define BODY_H_COEFF  9  // Human body to head height proportion
#define HAND_H_COEFF  4  // Human fingertips positon to head height proportion (from shoulder line)

 FaceDetection::FaceDetection(int height, int width)
     :mHeight(height), mWidth(width)
{
        // Set default face position and size (by default - rect in the top of an image)
        int x_face = width / 12 * 5;
        int y_face = height / 12;
        int h_face = height / 6;
        int w_face = width / 6;
        // Faces detected
        mFaces.push_back(cv::Rect(x_face, y_face, w_face, h_face));
}

 std::pair<cv::Mat, std::vector<cv::Rect> > FaceDetection::get_body_mask(const cv::Mat& img, bool body_rect, bool polyshape, bool coordinates, bool face_border)
 {

     std::vector<cv::Rect> faces = _detect_faces(img.clone());
     if (faces.size() > 0) {                              // Face coords detected
         cv::Rect biggest_face = faces[0];
         for (int i = 1; i < faces.size(); ++i) {
             if (biggest_face.area() < faces.at(i).area()) {
                 biggest_face = faces.at(i);
             }
         }
         mFaces = std::vector<cv::Rect>(1, biggest_face);

     }
     cv::Mat fgmask = cv::Mat::zeros(cv::Size(mWidth, mHeight), CV_8UC3);// check heigh width order

     // Draw faces
     if (face_border){
         fgmask = draw_rect(fgmask, mFaces, true);
      }
     else
         fgmask = draw_rect(fgmask, mFaces);

     cv::Rect body = body_from_face(mFaces.size());

     // Draw body
     if (body_rect) {
         fgmask = draw_rect(fgmask, std::vector<cv::Rect>(1, body));
     }
     if (polyshape) {
         fgmask = draw_polyshape(fgmask, body);
     }

     img_to_gray(fgmask);

     std::pair<cv::Mat, std::vector<cv::Rect> > res;
     res.first = fgmask;
     res.second = mFaces;
     return res;
 }

cv::Rect FaceDetection::body_from_face(bool side_view)
{
    /*
        Calculates approximate body rectangle from detected face rect.
        */
    int x = mFaces[0].x;
    int y = mFaces[0].y;
    int w = mFaces[0].width;
    int h = mFaces[0].height;

    int body_y = y + h;
    int body_h = h * BODY_H_COEFF;
    int body_w;
    int body_x;
    if (side_view){
        body_w = int(w * BODY_W_COEFF / 1.2);
        body_x = x - (body_w - w) / 2;
    }else{
        body_w = int(w * BODY_W_COEFF);
        body_x = x - (body_w - w) / 2;
    }
 
    return   cv::Rect(body_x, body_y, body_w, body_h);
}

std::vector<cv::Rect> FaceDetection::_detect_faces(cv::Mat img)
{
        /*Detects faces on the image.
        Args:
            img -- image
        Returns:
            faces -- [(x,y,w,h)]
        */
        dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();  // HOG+shift

        cv::Mat grayImg;
        cv::cvtColor(img, grayImg, CV_BGR2GRAY);
        std::vector<dlib::rectangle> dets = detector(dlib::cv_image<uchar>(grayImg), 1);

        std::vector<cv::Rect>  faces;
        for(int k = 0; k < dets.size(); ++ k) {
            auto d = dets.at(k);
            cv::Rect c(d.left(), d.top(), d.width(), d.height());
            std::cout << d.left() << "  "<< d.top()<< "  "<< d.width() << "  "<< d.height() <<  std::endl;
            faces.push_back(c);
        }

        return faces;
}

void FaceDetection::img_to_gray(cv::Mat& img)
{
        //Converts img (frame) to Grayscale, adjust contrast.
        cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
        cv::medianBlur(img, img, 3);
        cv::equalizeHist(img, img);  // Adj. contrast
}

cv::Mat FaceDetection::draw_rect(cv::Mat img, const std::vector<cv::Rect>& faces, bool border)
{       /*Draw WHITE rectangle on img.
        Args:
            img -- image
            faces -- face coords (x, y, w, h)
        Returns:
            img -- image with WHITE rectangle around the face
        "*/
    //cv::("Img BEFORE RECT", img);
    float THIKNESS = 0.2;
    for (int i = 0; i < faces.size(); ++i) {
        cv::Rect rect = faces.at(i);
        int x = rect.x;
        int y = rect.y;
        int h = rect.height;
        int w = rect.width;
        if (border) {

            // part of height, width
            cv::Point p1(x - int(w * THIKNESS), y - int(h * THIKNESS));
            cv::Point p2(x + w + int(w * THIKNESS), y + h + int(h * THIKNESS));
            cv::rectangle(img, p1, p2, cv::Scalar(255, 255, 255), CV_FILLED);
        }
        else{
            cv::rectangle(img, cv::Point(x, y), cv::Point(x + w, y + h), cv::Scalar(255, 255, 255), CV_FILLED);
       }
    }

    return img;
}

cv::Mat FaceDetection::draw_polyshape(cv::Mat img, const cv::Rect& body, bool hands)
{
        /*Draw WHITE rectangle on img.
        Args:
            img -- image
            body -- body coords (x, y, w, h)
        Returns:
            img -- image with WHITE polyshape around the body
        */

        int x = body.x;
        int y = body.y;
        int h = body.height;
        int w = body.width;

        int HANDS_OUTSIDE = 6;   // How many head_w are hands outside of body

        // Misc dimetions
        int img_width = img.cols;
        int head_w = w / 3;
        int half_body_h = h / 2;
        int body_center_x = x + int(head_w * 1.5);

        // Main dimetions
        cv::Point should_r (x + w, y);
        cv::Point should_l (x, y);
        cv::Point hand_r ((body_center_x + head_w * HANDS_OUTSIDE), y + half_body_h);
        cv::Point hand_l ((body_center_x - head_w * HANDS_OUTSIDE), y + half_body_h);

        std::vector<cv::Point2i> pts(4, cv::Point2i(0, 0));
        pts[0] = should_l;
        pts[1] = should_r;
        pts[2] = hand_r;
        pts[3] = hand_l;

        cv::fillConvexPoly(img, pts, cv::Scalar(255, 255, 255), 1);

        return img;
}

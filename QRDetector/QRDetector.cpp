#include "QRDetector.h"
#include<iostream>
#include<opencv2/highgui/highgui.hpp>  //imread
#include<opencv2/core/core.hpp>  //Mat
#include<opencv2/opencv.hpp>
#include<cmath>
#include<vector>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <exception>
#include<ctime>
//#include "zbar.h"

using namespace cv;
//using namespace zbar;

namespace hnurm
{
    bool QRdetector::QR_detect(Mat& src,Point2f boxpoints[4])
    {
        Mat srcCopy = src.clone();
        Point2f points[4];
        Mat canvas;//canvas????? ????????λ??????????
        canvas = Mat::zeros(src.size(), CV_8UC3);//??????????????????????????????????0,Scalar(0,0,0)

        Mat srcGray;
        std::vector<Point> center_all;//center_all???????????
        cvtColor(src, srcGray, COLOR_BGR2GRAY);

        // 3X3???
        //blur(srcGray, srcGray, Size(3, 3));
        //????????
        //equalizeHist(srcGray, srcGray);
        // ?????????????????? ?????????????????? ??????????
        //threshold(srcGray, srcGray, 50, 100, THRESH_BINARY);

        Canny(srcGray, srcGray, 50, 150, 3);
        //Mat erodeStruct = getStructuringElement(MORPH_RECT, Size(3, 3));
        //morphologyEx(srcGray, srcGray, MORPH_CLOSE, erodeStruct); //??????
        imshow("threshold", srcGray);
        waitKey(10);

        std::vector<std::vector<Point>> contours;
        std::vector<Vec4i> hierarchy;//????????????
        findContours(srcGray, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

        int numOfRec = 0;// С??????????

        // ??????
        int ic = 0;
        int parentIdx = -1;  //??????
        for (unsigned int i = 0; i < contours.size(); i++)
        {
            if (hierarchy[i][2] != -1 && ic == 0)
            {
                parentIdx = i;
                ic++;
            }
            else if (hierarchy[i][2] != -1)
            {
                ic++;
            }
            else if (hierarchy[i][2] == -1)
            {
                parentIdx = -1;
                ic = 0;
            }
            if (ic >= 3)
            {
                if (IsQrPoint(contours[parentIdx], src))
                {
                    //std::cout << "is QRpoint" << std::endl;
                    RotatedRect rect = minAreaRect(Mat(contours[parentIdx]));

                    // ???????
                    //Point2f points[4];
                    rect.points(points);
                    for (int j = 0; j < 4; j++) {
                        line(src, points[j], points[(j + 1) % 4], Scalar(0, 255, 0), 2);   //?????????????????
                    }
                    drawContours(canvas, contours, parentIdx, Scalar(0, 0, 255), -1);   //????????

                    // ??????????????????
                    center_all.push_back(rect.center);
                    numOfRec++;
                }
                ic = 0;
                parentIdx = -1;
            }
//            else
//                return false;
        }

        if (center_all.size() >= 3)
        {
            if (numOfRec < 3)
            {
                waitKey(20);
            }

            // ?????????????ε????
            for (unsigned int i = 0; i < center_all.size(); i++)
            {
                line(canvas, center_all[i], center_all[(i + 1) % center_all.size()], Scalar(255, 0, 0), 3);  //??????????
            }

            std::vector<std::vector<Point>> contours2;
            Mat canvasGray;
            cvtColor(canvas, canvasGray, COLOR_BGR2GRAY);
            //imshow("canvas", canvas);
            findContours(canvasGray, contours2, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

            std::vector<Point> maxContours;
            double maxArea = 0;
            // ?????л??????????????
            RotatedRect rect;
            //std::cout << "----------------------------------------" << std::endl;
            for (unsigned int i = 0; i < contours2.size(); i++)
            {
                rect = minAreaRect(contours2[i]);
            }

            if (IsQrCode(rect))
            {
                for (unsigned int i = 0; i < contours2.size(); i++)
                {
                    //RotatedRect rect = minAreaRect(contours2[i]);

                    rect.points(points);
                    for (int i = 0; i < 4; i++)
                    {
                        line(src, points[i], points[(i + 1) % 4], Scalar(0, 0, 255), 3);
                    }
                    boxpoints[0] = points[1];//tl
                    boxpoints[1] = points[2];//tr
                    boxpoints[2] = points[3];//br
                    boxpoints[3] = points[0];//bl
//                    _qrData.center_x = int(rect.center.x);
//                    _qrData.center_y = int(rect.center.y);
                    //std::cout << "center_x:" << _qrData.center_x << std::endl;
                    //std::cout << "center_y:" << _qrData.center_y << std::endl;
                    double area = contourArea(contours2[i]);
                    if (area > maxArea)
                    {
                        maxContours = contours2[i];
                        maxArea = area;
                    }
                }

                imshow("src", src);
                waitKey(10);
                return true;

//                // 扣出正過來的二維碼
//                int leftTopPointIndex = leftTopPoint(center_all);
//                std::vector<int> otherTwoPointIndex = otherTwoPoint(center_all, leftTopPointIndex);
//                // canvas?????????????????????
//                circle(canvas, center_all[leftTopPointIndex], 10, Scalar(255, 0, 255), -1);
//                for (int i = 0; i < otherTwoPointIndex.size(); i++)
//                    circle(canvas, center_all[otherTwoPointIndex[i]], 10, Scalar(0, 255, 0), -1);
//                // ?????????
//                double angle = rotateAngle(center_all[leftTopPointIndex], center_all[otherTwoPointIndex[0]], center_all[otherTwoPointIndex[1]]);
//
//                // ?ó???????????????????????
//                RotatedRect rect = minAreaRect(Mat(maxContours));
//                Mat image = transformQRcode(srcCopy, rect, angle);

                // ?????
//                imshow("QRcode", src);
//                //imshow("first", srcCopy);
//                imshow("canvas", canvas);
//                waitKey(10);
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    Mat QRdetector::transformCorner(Mat src, RotatedRect rect)
    {
        // ???????????
        Point center = rect.center;
        // ????????????????????????????????????Χ????????
        Point TopLeft = Point(cvRound(center.x), cvRound(center.y)) - Point(rect.size.height / 2, rect.size.width / 2);  //??????????λ??
        TopLeft.x = TopLeft.x > src.cols ? src.cols : TopLeft.x;
        TopLeft.x = TopLeft.x < 0 ? 0 : TopLeft.x;
        TopLeft.y = TopLeft.y > src.rows ? src.rows : TopLeft.y;
        TopLeft.y = TopLeft.y < 0 ? 0 : TopLeft.y;

        int after_width, after_height;
        if (TopLeft.x + rect.size.width > src.cols) {
            after_width = src.cols - TopLeft.x - 1;
        }
        else {
            after_width = rect.size.width - 1;
        }
        if (TopLeft.y + rect.size.height > src.rows) {
            after_height = src.rows - TopLeft.y - 1;
        }
        else {
            after_height = rect.size.height - 1;
        }
        // ??????????λ??
        Rect RoiRect = Rect(TopLeft.x, TopLeft.y, after_width, after_height);

        //	dst?????????? roi??????? mask????
        double angle = rect.angle;
        Mat mask, roi, dst;
        Mat image;
        // ?????н???????????????

        std::vector<Point> contour;
        // ??????ε??????
        Point2f points[4];
        rect.points(points);
        for (int i = 0; i < 4; i++)
            contour.push_back(points[i]);

        std::vector<std::vector<Point>> contours;
        contours.push_back(contour);
        // ???н?????л???????
        drawContours(mask, contours, 0, Scalar(255, 255, 255), -1);
        // ???mask?????src?????λ????????????dst?С?
        src.copyTo(dst, mask);
        // ???
        Mat M = getRotationMatrix2D(center, angle, 1);
        warpAffine(dst, image, M, src.size());
        // ???
        roi = image(RoiRect);

        return roi;
    }
    bool QRdetector::isCorner(Mat& image)
    {
        // ????mask
        Mat imgCopy, dstCopy;
        Mat dstGray;
        imgCopy = image.clone();

        // ??????????
        cvtColor(image, dstGray, COLOR_BGR2GRAY);
        // ???ж????
        threshold(dstGray, dstGray, 0, 255, THRESH_BINARY | THRESH_OTSU);
        //imshow("threshold", dstGray);  //?????????
        //Canny(dstGray, dstGray, 50, 150, 3);
        dstCopy = dstGray.clone();  //????

        Mat erodeStruct = getStructuringElement(MORPH_RECT, Size(3, 3));
        morphologyEx(dstCopy, dstCopy, MORPH_OPEN, erodeStruct);
        // ?????????????
        std::vector<std::vector<Point>> contours;
        std::vector<Vec4i> hierarchy;
        findContours(dstCopy, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

        for (unsigned int i = 0; i < contours.size(); i++)
        {
            //cout << i << endl;
            if (hierarchy[i][2] == -1 && hierarchy[i][3] != -1)
            {

                Rect rect = boundingRect(Mat(contours[i]));  //????????????
                rectangle(image, rect, Scalar(0, 0, 255), 2);
                //imshow("rectangle", image);

                // ???????????????????????ε???
                if (rect.width < imgCopy.cols * 2 / 7)      //2/7????????Щ?С?????
                    continue;
                if (rect.height < imgCopy.rows * 2 / 7)      //2/7????????Щ?С?????
                    continue;

                // ?ж????к????????????????
                if (Rate(dstGray) > 0.20)
                {
                    return true;
                }
            }
        }
        return  false;
    }
    bool QRdetector::IsQrPoint(std::vector<Point>& contour, const Mat& img)
    {
        double area = contourArea(contour);
        //std::cout << "area:" << area << std::endl;
         //????????С
        if (area < 15)
            return false;

        RotatedRect rect = minAreaRect(Mat(contour));
        double w = rect.size.width;
        double h = rect.size.height;
        double rate = min(w, h) / max(w, h);
        if (rate > 0.5)
        {
            //std::cout << "rate>0.5" << std::endl;
            // ?????????????????????????????????????
            Mat image = transformCorner(img, rect);
            //imshow("rotatedimg", img);
            if (isCorner(image))
            {
                return true;
            }
        }
        return false;
    }
    bool QRdetector::IsQrCode(RotatedRect rect)
    {
        if (fabs(rect.size.height - rect.size.width) > 50)
        {
            return false;
        }
        if (rect.size.aspectRatio() <= 0.9 || rect.size.aspectRatio() >= 1.1)
        {
            return false;
        }
        return true;
    }
    //??????????
    double QRdetector::Rate(Mat& count)
    {
        int number = 0;
        int allpixel = 0;
        for (int row = 0; row < count.rows; row++)
        {
            for (int col = 0; col < count.cols; col++)
            {
                if (count.at<uchar>(row, col) == 255)
                {
                    number++;
                }
                allpixel++;
            }
        }
        //std::cout << (double)number / allpixel << std::endl;
        return (double)number / allpixel;
    }




    int QRdetector::leftTopPoint(std::vector<Point> centerPoint)
    {
        int minIndex = 0;
        int multiple = 0;
        int minMultiple = 10000;
        multiple = (centerPoint[1].x - centerPoint[0].x) * (centerPoint[2].x - centerPoint[0].x) + (centerPoint[1].y - centerPoint[0].y) * (centerPoint[2].y - centerPoint[0].y);
        if (minMultiple > multiple) {
            minIndex = 0;
            minMultiple = multiple;
        }
        multiple = (centerPoint[0].x - centerPoint[1].x) * (centerPoint[2].x - centerPoint[1].x) + (centerPoint[0].y - centerPoint[1].y) * (centerPoint[2].y - centerPoint[1].y);
        if (minMultiple > multiple) {
            minIndex = 1;
            minMultiple = multiple;
        }
        multiple = (centerPoint[0].x - centerPoint[2].x) * (centerPoint[1].x - centerPoint[2].x) + (centerPoint[0].y - centerPoint[2].y) * (centerPoint[1].y - centerPoint[2].y);
        if (minMultiple > multiple) {
            minIndex = 2;
            minMultiple = multiple;
        }
        return minIndex;
    }
    std::vector<int> QRdetector::otherTwoPoint(std::vector<Point> centerPoint, int leftTopPointIndex)
    {
        std::vector<int> otherIndex;
        double tmp = (centerPoint[(leftTopPointIndex + 1) % 3].x - centerPoint[(leftTopPointIndex) % 3].x) *
            (centerPoint[(leftTopPointIndex + 2) % 3].y - centerPoint[(leftTopPointIndex) % 3].y) -
            (centerPoint[(leftTopPointIndex + 2) % 3].x - centerPoint[(leftTopPointIndex) % 3].x) *
            (centerPoint[(leftTopPointIndex + 1) % 3].y - centerPoint[(leftTopPointIndex) % 3].y);
        if (tmp > 0) {
            otherIndex.push_back((leftTopPointIndex + 1) % 3);
            otherIndex.push_back((leftTopPointIndex + 2) % 3);
        }
        else {
            otherIndex.push_back((leftTopPointIndex + 2) % 3);
            otherIndex.push_back((leftTopPointIndex + 1) % 3);
        }
        return otherIndex;
    }
    double QRdetector::rotateAngle(Point leftTopPoint, Point rightTopPoint, Point leftBottomPoint)
    {
        double dy = rightTopPoint.y - leftTopPoint.y;
        double dx = rightTopPoint.x - leftTopPoint.x;
        double k = dy / dx;
        double angle = atan(k) * 180 / CV_PI;//??????
        if (leftBottomPoint.y < leftTopPoint.y)
            angle -= 180;
        return angle;
    }
    Mat QRdetector::transformQRcode(Mat src, RotatedRect rect, double angle)
    {
        // ???????????
        Point center = rect.center;
        // ????????????????????????????????????Χ????????
        Point TopLeft = Point(cvRound(center.x), cvRound(center.y)) - Point(rect.size.height / 2, rect.size.width / 2);  //??????????λ??
        TopLeft.x = TopLeft.x > src.cols ? src.cols : TopLeft.x;
        TopLeft.x = TopLeft.x < 0 ? 0 : TopLeft.x;
        TopLeft.y = TopLeft.y > src.rows ? src.rows : TopLeft.y;
        TopLeft.y = TopLeft.y < 0 ? 0 : TopLeft.y;

        int after_width, after_height;
        if (TopLeft.x + rect.size.width > src.cols) {
            after_width = src.cols - TopLeft.x - 1;
        }
        else {
            after_width = rect.size.width - 1;
        }
        if (TopLeft.y + rect.size.height > src.rows) {
            after_height = src.rows - TopLeft.y - 1;
        }
        else {
            after_height = rect.size.height - 1;
        }
        // ??????????λ??
        Rect RoiRect = Rect(TopLeft.x, TopLeft.y, after_width, after_height);

        // dst????????????roi?????????mask????
        Mat mask, roi, dst;
        Mat image;
        // ?????н???????????????

        std::vector<Point> contour;
        // ??????ε??????
        Point2f points[4];
        rect.points(points);
        for (int i = 0; i < 4; i++)
            contour.push_back(points[i]);

        std::vector<std::vector<Point>> contours;
        contours.push_back(contour);
        // ???н?????л???????
        drawContours(mask, contours, 0, Scalar(255, 255, 255), -1);
        // ???mask?????src?????λ????????????dst?С?
        src.copyTo(dst, mask);
        // ???
        Mat M = getRotationMatrix2D(center, angle, 1);
        warpAffine(dst, image, M, src.size());
        // ???
        roi = image(RoiRect);

        return roi;
    }

}

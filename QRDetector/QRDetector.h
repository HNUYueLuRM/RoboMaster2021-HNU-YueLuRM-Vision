#pragma once
#include<opencv2/opencv.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include "../Protocol/protocol.h"
#include "../Buffer/SerialData.hpp"
using namespace cv;

namespace hnurm
{

    class QRdetector
    {
    public:
        QRdetector(){}
        ~QRdetector(){}
        bool QR_detect(Mat& src,Point2f boxpoints[4]);

        Mat transformCorner(Mat src, RotatedRect rect);
        bool isCorner(Mat& image);// 用于判断是否属于角上的正方形
        bool IsQrPoint(std::vector<Point>& contour, const Mat& img);// 该部分用于检测是否是角点，与下面两个函数配合
        bool IsQrCode(RotatedRect rect); //判断是否拟合出的矩阵是二维码
        double Rate(Mat& count);// 计算内部所有白色部分占全部的比率



        int leftTopPoint(std::vector<Point> centerPoint);
        std::vector<int> otherTwoPoint(std::vector<Point> centerPoint, int leftTopPointIndex);
        double rotateAngle(Point leftTopPoint, Point rightTopPoint, Point leftBottomPoint);
        Mat transformQRcode(Mat src, RotatedRect rect, double angle);


	public:
//		VideoCapture capture_plate;
//		Mat QR_img;
        SerialData result;

	private:
		//QRData _qrData;
        double _yaw_Angle = 0.0;
        double _pitch_Angle = 0.0;
        double _euclideanDistance = 0.0;
        Mat _tVec;
	};

}

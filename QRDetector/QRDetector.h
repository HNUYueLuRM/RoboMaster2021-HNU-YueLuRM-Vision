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
        bool isCorner(Mat& image);// �����ж��Ƿ����ڽ��ϵ�������
        bool IsQrPoint(std::vector<Point>& contour, const Mat& img);// �ò������ڼ���Ƿ��ǽǵ㣬�����������������
        bool IsQrCode(RotatedRect rect); //�ж��Ƿ���ϳ��ľ����Ƕ�ά��
        double Rate(Mat& count);// �����ڲ����а�ɫ����ռȫ���ı���



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

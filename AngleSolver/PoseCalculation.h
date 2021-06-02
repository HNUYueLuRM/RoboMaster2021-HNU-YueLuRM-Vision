#pragma once
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include "../Buffer/SerialData.hpp"

namespace hnurm
{

    enum ArmorColor
    {
        RED = 0, BLUE
    };

    enum ArmorSize
    {
        SMALL = 0, BIG
    };

    struct ArmorInfo
    {
        ArmorColor color;
        ArmorSize size;
        std::vector<cv::Point2d> object_points;
        // cv::Point2d left_top;      [0]
        // cv::Point2d right_top;     [1]
        // cv::Point2d right_bottom;  [2]
        // cv::Point2d left_bottom;   [3]
    };

    struct QRData
    {
        std::vector<cv::Point2d> boxpoint;
        int center_x;
        int center_y;
    };

	/* ����AngleSolver������������������װ�װ峤����λmm��*/
	struct AngleSolverParam 
	{
		void readFile();/* read the params of camera  and armor */

		cv::Mat camera_matrix;		//Camera Matrix
		cv::Mat distortion_coeff;	//Distortion matrix
		double big_armor_length, big_armor_height;//��װ�װ����
		double small_armor_length, small_armor_height;//Сװ�װ����
        double mineral_half_length;//��ʯ����
	};



	/*
	*	solve by PNP, that is, using four points to detect the angle and distance.
	*	It's not very useful if the armor is far.If it' far try solve by one point
	*/
	class AngleSolver
	{
	public:
		AngleSolver();
		//AngleSolver(const AngleSolverParam& AngleSolverParam);

		void init();/*Initialize with parameters*/


		
        SerialData solve(const std::vector<cv::Point2f> objectPoints);/* slove the angle by selected algorithm */
        SerialData solve(const ArmorInfo Armors);/* slove the angle by selected algorithm */
        SerialData pnpsolve(const QRData qrData);

        SerialData result;// the result of solver

    protected:
        void setTarget(const std::vector<cv::Point2f> objectPoints); /*  set 2d coordinates of corner points of armor   for PNP	*/
        void setArmorTarget(const std::vector<ArmorInfo> Armors);

        double getYaw();/* get the Yaw angle solved */
        double getPitch();/* get the Pitch angle solved */
        cv::Mat getTvec();/* @brief get the Tvec solved */
        double getDistance();/* @brief get the distance between the camera and the target*/

	private:
		AngleSolverParam params;//�������

		std::vector<cv::Point3f> point_3d_of_small_armor;//Сװ�װ��ĸ��������world���꣬��λmm
		std::vector<cv::Point3f> point_3d_of_big_armor;//��װ�װ�
        std::vector<cv::Point3f> point_3d_of_mineral;//��ʯ

		cv::Mat _rVec = cv::Mat::zeros(3, 1, CV_64FC1);//initialize rvec
		cv::Mat _tVec = cv::Mat::zeros(3, 1, CV_64FC1);//initialize tvec

		double _yaw_Angle = 0.0;//��ʼ��yawֵ��x�᷽��ת�ǣ��Ƕ��Ƶ�λ
		double _pitch_Angle = 0.0;//��ʼ��pictchֵ��y�᷽��ת�ǣ��Ƕ�ֵ��λ
		double _euclideanDistance = 0.0;//��ʼ�������Ŀ�꣨װ�װ����ģ��ľ���


		std::vector<cv::Point2f> point_2d_of_armor;//�������ԭʼpixel����
		std::vector<cv::Point2f> undistorted_point_2d_of_armor;//����У����Ŀ����pixel����
        std::vector<cv::Point2f> my_undistort_function();//�����������
	};

}

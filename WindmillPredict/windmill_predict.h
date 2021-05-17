#pragma once

#include "opencv2/opencv.hpp"
using namespace cv;

namespace hnurm {
	//所有角度统一单位：rad，所有时间统一单位：s

	static double fun(double time) {//风车角速度函数，用以计算预测角predict_rad
		/*
			spd = 0.785*sin（1.884*t)+1.305  rad/s
			double a=0.785;
			double k=1.884;
			double b=1.305;
			积分： integral = -a/k *cos(k*t) + b*t

			time单位：s
		*/
		return   0.785 * sin(1.884 * time) + 1.305;
	}

	class Windmill_Predict {
	public:
		Windmill_Predict();
		Windmill_Predict(cv::Point circle_center_point_, cv::Point target_point_, bool speed_flag_, float delta_time_, float constant_rotation_speed_ = 10.0 / 60);
		~Windmill_Predict() = default;
		
		//void run();//预测坐标
		void constant_run(cv::Point circle_center_point_, cv::Point target_point_, bool speed_flag_ ,  float delta_time_ , float constant_rotation_speed_=10.0 / 60);//匀速时预测坐标，角速度：rad/s，时间单位：s
		void vary_run(cv::Point circle_center_point_, cv::Point target_point_, bool speed_flag_, float delta_time_, double time_start, double time_now);//变速时预测坐标, double time_start:风车开始转动的时刻，double time_now：当前时刻 单位：s

		cv::Point predict_point;//预测出的击打点坐标
	
	private:
		float predict_rad;//预测提前角（弧度制）: 当前时间到发射子弹期间风车转过的角度
		float delta_time;//当前时间到发射子弹期间的时间差

		cv::Point circle_center_point;//风车圆心坐标
		cv::Point target_point;//目标装甲板中心坐标
		float target_polar_angle;//待击打装甲板的极坐标角度（弧度制）
				
		bool speed_flag;//0:风车匀速转动  1：风车变速转动
		float constant_rotation_speed;//风车匀速转动角速度 (rad/s)

		bool get_predict_rad(double time_start=0.0 ,double time_now=0.0);//计算预测提前角,大风车开始转的时间、当前时间，单位：s
		

		double my_integral(double (*f)(double), double min, double max) {//积分函数
			double result = 0;
			const int N = 10000;
			double delta = (max - min) / N;

			for (double i = min + delta; i < max; i += delta)
			{
				result += (*f)(i) * delta;
			}
			return result;
		}
		
	};


	

	

}

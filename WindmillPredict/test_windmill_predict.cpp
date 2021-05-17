#include"windmill_predict.h"
#include <iostream>
#include "opencv2\opencv.hpp"

#include <chrono>

using namespace std;
using namespace cv;

double my_integral(double(*f)(double), double min, double max) {
	double result = 0;
	const int N = 10000;
	double delta = (max - min) / N;
	for (double i = min + delta; i < max; i += delta)
	{
		result += f(i) * delta;
	}
	return result;
}


double my_fun(double time) {//求风车变速转动时，预测角predict_rad
			/*
				spd = 0.785*sin（1.884*t)+1.305  rad/s
				double a=0.785;
				double k=1.884;
				double b=1.305;
				积分： integral = -a/k *cos(k*t) + b*t
			*/
	double a = 0.785;
	double k = 1.884;
	double b = 1.305;
	return   0.785 * sin(1.884 * time) + 1.305;
};


int main() {
	/*
	调用方法：

	1、实例化一个Windmill_Predict对象
	hnurm::Windmill_Predict windmill_pre;

	2、调用 run（）函数

		//匀速:	constant_run()
		windmill_pre.constant_run(cv::Point circle_center_point_, cv::Point target_point_, bool speed_flag_ ,  float delta_time_ , float constant_rotation_speed_);
			参数说明：
			cv::Point circle_center_point_ ：	风车中心 的像素坐标
			cv::Point target_point_ ：			需要打击的装甲板（流水条扇叶上那个） 的中心坐标
			bool speed_flag_ ：					模式选择，输入 0    【0：风车匀速转  1：风车变速转】
			float delta_time_ ：				打击间隔时间，单位：s
			float constant_rotation_speed_ ：	风车匀速转动的角速度，单位 rad/s  
	
		//变速: vary_run()
		windmill_pre.vary_run(cv::Point circle_center_point_, cv::Point target_point_, bool speed_flag_, float delta_time_, double time_start, double time_now);
			参数说明：
			cv::Point circle_center_point_ ：	风车中心 的像素坐标
			cv::Point target_point_ ：			需要打击的装甲板（流水条扇叶上那个） 的中心坐标
			bool speed_flag_ ：					模式选择，输入 1    【0：风车匀速转  1：风车变速转】
			float delta_time_ ：				打击间隔时间，单位：s
			time_start:							风车开始转动的时刻,单位：s
			time_now：							当前时刻,单位：s


	3、获取预测坐标, 类型cv::Point
	cv::Point predict_result = windmill_pre.predict_point ;

	*/



	cv::Mat windmill_img = cv::imread("123.png");

    // 将图缩放到目标尺寸 640 * 480
    int dst_width = 640;
    int dst_height = 480;
    cv::resize(windmill_img, windmill_img, cv::Size(dst_width, dst_height));


	cv::Point circle_center_point=cv::Point(dst_width / 2, dst_height / 2);//风车中心点
	cv::Point target_point_ = cv::Point(dst_width / 2 + dst_width/4, dst_height / 2);//装甲板中心

	cv::circle(windmill_img, circle_center_point, 8, cv::Scalar(255, 255, 0), 5);
	cv::circle(windmill_img, target_point_, 5, cv::Scalar(0, 255, 255), 3);//装甲板中心:黄色
	imshow("windmill", windmill_img);

	
	float delta_time_ = 0.3;//间隔时间

	int flag;
	cout << "0:匀速 ； 1： 变速 "<<endl;
	cin>> flag;
	if (flag == 0) {
		
		float constant_rotation_speed_ = 30 ;//风车角速度，单位：rad/s     比赛为 10 RPM= 10/60 rad/s

		hnurm::Windmill_Predict windmill_pre;

		int cnt = 50000;
		for (int i = 1;i < cnt;i++) {
			cv::Mat tem_img=windmill_img.clone();

			//模拟一个正在匀速转的风车
			float r = dst_width / 4;
			float angle = (delta_time_ * i * constant_rotation_speed_);
			angle = int((angle * 100 + 0.5) / 100) % 360;
			cout << "angle: "<<angle << endl;
			float x = r * cos(angle * CV_PI / 180);
			float y = r * sin(angle * CV_PI / 180);
			target_point_ = circle_center_point+ cv::Point(x , y);
			cv::circle(tem_img, target_point_, 3, cv::Scalar(0, 255, 255), 3);//装甲板中心
			cv::circle(tem_img, circle_center_point, 8, cv::Scalar(255, 255, 0), 5);//风车中心

			//进行预测：
			windmill_pre.constant_run(circle_center_point, target_point_, 0, delta_time_, constant_rotation_speed_ * CV_PI / 180);
			cv::circle(tem_img, windmill_pre.predict_point, 10, cv::Scalar(0, 0, 255), 5);//预测出的装甲板中心

			imshow("匀速风车", tem_img);
			cv::waitKey(80);
			cout << i << endl<<endl;
		}
	
	}
	else {
		auto time_start = std::chrono::steady_clock::now();//风车开始转动时间
		auto time_end = std::chrono::steady_clock::now();
		std::chrono::duration<double, std::milli> elapsed = time_end - time_start;// std::milli 表示以毫秒为时间单位
		double time_1 = (time_start - time_start).count();// 开始时间，以毫秒为时间单位
		double time_2 = elapsed.count();// 当前时间，以毫秒为时间单位

		hnurm::Windmill_Predict windmill_pre;

		int cnt = 50000;
		for (int i = 1;i < cnt;i++) {

			cv::waitKey(50);
			cout << i << endl;

			cv::Mat tem_img = windmill_img.clone();

			//模拟一个正在变速转的风车
			//time_1 = time_2;
			auto time_end = std::chrono::steady_clock::now();
			std::chrono::duration<double, std::milli> elapsed = time_end - time_start;// std::milli 表示以毫秒为时间单位
			time_2 = elapsed.count();// 当前时间，以毫秒为时间单位
			cout << "time_1: " << time_1 / 1000 << endl;
			cout << "time_2: " << time_2 / 1000 << endl;


			float angle = my_integral(my_fun, time_1 / 1000, time_2 / 1000);//此时装甲板的角度
			//cout << "fun(time_1): " << fun(time_1) << endl;
			//cout << "fun(time_2): " << fun(time_2) << endl;

			angle = (double)(int(angle * 100) % 36000) / 100;
			cout << "angle: " << angle << endl;

			float r = dst_width / 4;

			float x = r * cos(angle * CV_PI / 180);
			float y = r * sin(angle * CV_PI / 180);
			target_point_ = circle_center_point + cv::Point(x, y);
			cv::circle(tem_img, target_point_, 3, cv::Scalar(0, 255, 255), 3);//装甲板中心
			cout << "armor:  " << target_point_ << endl;
			cv::circle(tem_img, circle_center_point, 8, cv::Scalar(255, 255, 0), 5);//风车中心

			//进行预测：
			windmill_pre.vary_run(circle_center_point, target_point_, 1, delta_time_, time_2/1000, time_2/1000 +delta_time_);
			//cout << windmill_pre.predict_point << endl;
			cv::circle(tem_img, windmill_pre.predict_point, 10, cv::Scalar(0, 0, 255), 5);//预测出的装甲板中心

			imshow("变速风车", tem_img);

		}
	}
	
	
	
	
	cv::waitKey();
	
	return 0;
}
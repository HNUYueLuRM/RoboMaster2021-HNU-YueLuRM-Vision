#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "WindmillSolver.h"
#include "GeneralDefs.h"
#include "windmill_predict.h"



int main(void)
{
	
	//cv::Mat windmill_img = cv::imread("456.png");
	// 使用方法十分简单，只需要两行代码就可以了，首先传入一个cv::Mat，就能拿到最终的预测的结果

	// 首先实例化一个hnurm::Windmill_solver, 初始化参数为 hnurm::TeamColor::RED | hnurm::TeamColor::Blue 代表自己队伍的颜色
	hnurm::Windmill_solver windmill_solver(hnurm::TeamColor::RED);
	hnurm::Windmill_Predict windmill_pre;

	// 然后直接调用 hnurm::Windmill_solver::detect_armor_cornor_and_center 方法就可以了
	cv::VideoCapture cap("windmill.mp4");
	
	auto time_start = std::chrono::steady_clock::now();//风车开始转动时间
	
	
	for(int i=0;;i++){
		cv::Mat windmill_img;
		cap >> windmill_img;
		//识别
		std::vector<hnurm::PixelPoint2d> windmill_detect_result{ windmill_solver.detect_armor_cornor_and_center(windmill_img) };
		std::vector<hnurm::PixelPoint2d> windmill_detect_result_2{ windmill_solver.detect_windmill_center_r_icon(windmill_img) };

		//预测
		//匀速
		cv::Point circle_center_point_ = windmill_solver.circle_center_point;//风车中心的像素坐标
		cv::Point target_point_ = windmill_solver.armor_point;//需要打击的装甲板的中心坐标
		bool speed_flag_ = 0;//0=风车匀速转动  1=变速转动
		float delta_time_ = 0.3;//打击间隔时间，单位：s
		float constant_rotation_speed_ = 10.0/60 ;	//风车匀速转动的角速度，单位 rad / s
		windmill_pre.constant_run(circle_center_point_, target_point_, speed_flag_, delta_time_, constant_rotation_speed_);
		//变速
		//auto time_end = std::chrono::steady_clock::now();
		//std::chrono::duration<double, std::milli> elapsed = time_end - time_start;// std::milli 表示以毫秒为时间单位
		//double time_1 = (time_start - time_start).count();// 开始时间，以毫秒为时间单位
		//double time_2 = elapsed.count();// 当前时间，以毫秒为时间单位
		//
		//cv::Point circle_center_point_ = windmill_solver.circle_center_point;//风车中心的像素坐标
		//cv::Point target_point_ = windmill_solver.armor_point;//需要打击的装甲板的中心坐标
		//bool speed_flag_ = 1;//0=风车匀速转动  1=变速转动
		//float delta_time_ = 0.3;//打击间隔时间，单位：s
		//double time_start = time_2 / 1000;//风车开始转动的时刻, 单位：s
		//double time_now =time_2 / 1000 + delta_time_;//当前时刻, 单位：s
		//windmill_pre.vary_run(circle_center_point_, target_point_, speed_flag_, delta_time_, time_start, time_now);
	

		//画图
		cv::Mat tem_img = windmill_img.clone();
		cv::resize(tem_img, tem_img, cv::Size(640, 480));
		cv::circle(tem_img, cv::Point(target_point_.x, target_point_.y), 15, cv::Scalar(0, 0, 255), 4);//识别出需要击打的装甲板
		cv::circle(tem_img, windmill_pre.predict_point, 8, cv::Scalar(255, 0, 255), 2);//预测出 0.3s 后的装甲板位置

		imshow("风车识别", tem_img);
		
		//输出
		//printf("windmill_detect_result.size() = %d\n", windmill_detect_result.size());
		if (!windmill_detect_result.empty()) {
			int armor_center_u = windmill_detect_result[0].u;
			int armor_center_v = windmill_detect_result[0].v;
			std::cout << i << std::endl;
			printf("detect result : u = %d, v = %d\n\n", armor_center_u, armor_center_v);
			windmill_detect_result.clear();
			windmill_detect_result_2.clear();
		}

		cv::waitKey(1);
	}

	return 0;
}
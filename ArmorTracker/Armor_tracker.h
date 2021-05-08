#pragma once
#include "kcf.h"
#include <opencv2/opencv.hpp>
#include <iostream>
namespace hnurm {
	class Armor_tracker {
	public:	
		// 构造函数
		Armor_tracker();

		// 析构函数
		~Armor_tracker() = default;

		//初始化tracker
        void init_tracker(const Rect& roi, const cv::Mat& image);

		// 更新目标框
		Rect Update(cv::Mat& image);

		Point2d Update_and_Get_center(cv::Mat& image);
		// 测试部分
		void test_case(string video_path, string groundturth_path);
		
	private:
		// kcf追踪器
        //KCF	*_kcf_tracker;
        unique_ptr<KCF> _kcf_tracker;
	};
}

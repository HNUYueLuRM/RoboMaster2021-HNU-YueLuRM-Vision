#pragma once
#include "kcf.h"
#include <opencv2/opencv.hpp>
#include <iostream>
namespace hnurm {
	class Armor_tracker {
	public:	
		// ���캯��
		Armor_tracker();

		// ��������
		~Armor_tracker() = default;

		//��ʼ��tracker
        void init_tracker(const Rect& roi, const cv::Mat& image);

		// ����Ŀ���
		Rect Update(cv::Mat& image);

		Point2d Update_and_Get_center(cv::Mat& image);
		// ���Բ���
		void test_case(string video_path, string groundturth_path);
		
	private:
		// kcf׷����
        //KCF	*_kcf_tracker;
        unique_ptr<KCF> _kcf_tracker;
	};
}

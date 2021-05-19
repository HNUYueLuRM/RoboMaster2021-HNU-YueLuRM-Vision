#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "WindmillSolver.h"
#include "GeneralDefs.h"
#include "windmill_predict.h"



int main(void)
{
	
	//cv::Mat windmill_img = cv::imread("456.png");
	// ʹ�÷���ʮ�ּ򵥣�ֻ��Ҫ���д���Ϳ����ˣ����ȴ���һ��cv::Mat�������õ����յ�Ԥ��Ľ��

	// ����ʵ����һ��hnurm::Windmill_solver, ��ʼ������Ϊ hnurm::TeamColor::RED | hnurm::TeamColor::Blue �����Լ��������ɫ
	hnurm::Windmill_solver windmill_solver(hnurm::TeamColor::RED);
	hnurm::Windmill_Predict windmill_pre;

	// Ȼ��ֱ�ӵ��� hnurm::Windmill_solver::detect_armor_cornor_and_center �����Ϳ�����
	cv::VideoCapture cap("windmill.mp4");
	
	auto time_start = std::chrono::steady_clock::now();//�糵��ʼת��ʱ��
	
	
	for(int i=0;;i++){
		cv::Mat windmill_img;
		cap >> windmill_img;
		//ʶ��
		std::vector<hnurm::PixelPoint2d> windmill_detect_result{ windmill_solver.detect_armor_cornor_and_center(windmill_img) };
		std::vector<hnurm::PixelPoint2d> windmill_detect_result_2{ windmill_solver.detect_windmill_center_r_icon(windmill_img) };

		//Ԥ��
		//����
		cv::Point circle_center_point_ = windmill_solver.circle_center_point;//�糵���ĵ���������
		cv::Point target_point_ = windmill_solver.armor_point;//��Ҫ�����װ�װ����������
		bool speed_flag_ = 0;//0=�糵����ת��  1=����ת��
		float delta_time_ = 0.3;//������ʱ�䣬��λ��s
		float constant_rotation_speed_ = 10.0/60 ;	//�糵����ת���Ľ��ٶȣ���λ rad / s
		windmill_pre.constant_run(circle_center_point_, target_point_, speed_flag_, delta_time_, constant_rotation_speed_);
		//����
		//auto time_end = std::chrono::steady_clock::now();
		//std::chrono::duration<double, std::milli> elapsed = time_end - time_start;// std::milli ��ʾ�Ժ���Ϊʱ�䵥λ
		//double time_1 = (time_start - time_start).count();// ��ʼʱ�䣬�Ժ���Ϊʱ�䵥λ
		//double time_2 = elapsed.count();// ��ǰʱ�䣬�Ժ���Ϊʱ�䵥λ
		//
		//cv::Point circle_center_point_ = windmill_solver.circle_center_point;//�糵���ĵ���������
		//cv::Point target_point_ = windmill_solver.armor_point;//��Ҫ�����װ�װ����������
		//bool speed_flag_ = 1;//0=�糵����ת��  1=����ת��
		//float delta_time_ = 0.3;//������ʱ�䣬��λ��s
		//double time_start = time_2 / 1000;//�糵��ʼת����ʱ��, ��λ��s
		//double time_now =time_2 / 1000 + delta_time_;//��ǰʱ��, ��λ��s
		//windmill_pre.vary_run(circle_center_point_, target_point_, speed_flag_, delta_time_, time_start, time_now);
	

		//��ͼ
		cv::Mat tem_img = windmill_img.clone();
		cv::resize(tem_img, tem_img, cv::Size(640, 480));
		cv::circle(tem_img, cv::Point(target_point_.x, target_point_.y), 15, cv::Scalar(0, 0, 255), 4);//ʶ�����Ҫ�����װ�װ�
		cv::circle(tem_img, windmill_pre.predict_point, 8, cv::Scalar(255, 0, 255), 2);//Ԥ��� 0.3s ���װ�װ�λ��

		imshow("�糵ʶ��", tem_img);
		
		//���
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
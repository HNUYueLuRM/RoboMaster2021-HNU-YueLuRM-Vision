#pragma once

#include "opencv2/opencv.hpp"
using namespace cv;

namespace hnurm {
	//���нǶ�ͳһ��λ��rad������ʱ��ͳһ��λ��s

	static double fun(double time) {//�糵���ٶȺ��������Լ���Ԥ���predict_rad
		/*
			spd = 0.785*sin��1.884*t)+1.305  rad/s
			double a=0.785;
			double k=1.884;
			double b=1.305;
			���֣� integral = -a/k *cos(k*t) + b*t

			time��λ��s
		*/
		return   0.785 * sin(1.884 * time) + 1.305;
	}

	class Windmill_Predict {
	public:
		Windmill_Predict();
		Windmill_Predict(cv::Point circle_center_point_, cv::Point target_point_, bool speed_flag_, float delta_time_, float constant_rotation_speed_ = 10.0 / 60);
		~Windmill_Predict() = default;
		
		//void run();//Ԥ������
		void constant_run(cv::Point circle_center_point_, cv::Point target_point_, bool speed_flag_ ,  float delta_time_ , float constant_rotation_speed_=10.0 / 60);//����ʱԤ�����꣬���ٶȣ�rad/s��ʱ�䵥λ��s
		void vary_run(cv::Point circle_center_point_, cv::Point target_point_, bool speed_flag_, float delta_time_, double time_start, double time_now);//����ʱԤ������, double time_start:�糵��ʼת����ʱ�̣�double time_now����ǰʱ�� ��λ��s

		cv::Point predict_point;//Ԥ����Ļ��������
	
	private:
		float predict_rad;//Ԥ����ǰ�ǣ������ƣ�: ��ǰʱ�䵽�����ӵ��ڼ�糵ת���ĽǶ�
		float delta_time;//��ǰʱ�䵽�����ӵ��ڼ��ʱ���

		cv::Point circle_center_point;//�糵Բ������
		cv::Point target_point;//Ŀ��װ�װ���������
		float target_polar_angle;//������װ�װ�ļ�����Ƕȣ������ƣ�
				
		bool speed_flag;//0:�糵����ת��  1���糵����ת��
		float constant_rotation_speed;//�糵����ת�����ٶ� (rad/s)

		bool get_predict_rad(double time_start=0.0 ,double time_now=0.0);//����Ԥ����ǰ��,��糵��ʼת��ʱ�䡢��ǰʱ�䣬��λ��s
		

		double my_integral(double (*f)(double), double min, double max) {//���ֺ���
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

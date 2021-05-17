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


double my_fun(double time) {//��糵����ת��ʱ��Ԥ���predict_rad
			/*
				spd = 0.785*sin��1.884*t)+1.305  rad/s
				double a=0.785;
				double k=1.884;
				double b=1.305;
				���֣� integral = -a/k *cos(k*t) + b*t
			*/
	double a = 0.785;
	double k = 1.884;
	double b = 1.305;
	return   0.785 * sin(1.884 * time) + 1.305;
};


int main() {
	/*
	���÷�����

	1��ʵ����һ��Windmill_Predict����
	hnurm::Windmill_Predict windmill_pre;

	2������ run��������

		//����:	constant_run()
		windmill_pre.constant_run(cv::Point circle_center_point_, cv::Point target_point_, bool speed_flag_ ,  float delta_time_ , float constant_rotation_speed_);
			����˵����
			cv::Point circle_center_point_ ��	�糵���� ����������
			cv::Point target_point_ ��			��Ҫ�����װ�װ壨��ˮ����Ҷ���Ǹ��� ����������
			bool speed_flag_ ��					ģʽѡ������ 0    ��0���糵����ת  1���糵����ת��
			float delta_time_ ��				������ʱ�䣬��λ��s
			float constant_rotation_speed_ ��	�糵����ת���Ľ��ٶȣ���λ rad/s  
	
		//����: vary_run()
		windmill_pre.vary_run(cv::Point circle_center_point_, cv::Point target_point_, bool speed_flag_, float delta_time_, double time_start, double time_now);
			����˵����
			cv::Point circle_center_point_ ��	�糵���� ����������
			cv::Point target_point_ ��			��Ҫ�����װ�װ壨��ˮ����Ҷ���Ǹ��� ����������
			bool speed_flag_ ��					ģʽѡ������ 1    ��0���糵����ת  1���糵����ת��
			float delta_time_ ��				������ʱ�䣬��λ��s
			time_start:							�糵��ʼת����ʱ��,��λ��s
			time_now��							��ǰʱ��,��λ��s


	3����ȡԤ������, ����cv::Point
	cv::Point predict_result = windmill_pre.predict_point ;

	*/



	cv::Mat windmill_img = cv::imread("123.png");

    // ��ͼ���ŵ�Ŀ��ߴ� 640 * 480
    int dst_width = 640;
    int dst_height = 480;
    cv::resize(windmill_img, windmill_img, cv::Size(dst_width, dst_height));


	cv::Point circle_center_point=cv::Point(dst_width / 2, dst_height / 2);//�糵���ĵ�
	cv::Point target_point_ = cv::Point(dst_width / 2 + dst_width/4, dst_height / 2);//װ�װ�����

	cv::circle(windmill_img, circle_center_point, 8, cv::Scalar(255, 255, 0), 5);
	cv::circle(windmill_img, target_point_, 5, cv::Scalar(0, 255, 255), 3);//װ�װ�����:��ɫ
	imshow("windmill", windmill_img);

	
	float delta_time_ = 0.3;//���ʱ��

	int flag;
	cout << "0:���� �� 1�� ���� "<<endl;
	cin>> flag;
	if (flag == 0) {
		
		float constant_rotation_speed_ = 30 ;//�糵���ٶȣ���λ��rad/s     ����Ϊ 10 RPM= 10/60 rad/s

		hnurm::Windmill_Predict windmill_pre;

		int cnt = 50000;
		for (int i = 1;i < cnt;i++) {
			cv::Mat tem_img=windmill_img.clone();

			//ģ��һ����������ת�ķ糵
			float r = dst_width / 4;
			float angle = (delta_time_ * i * constant_rotation_speed_);
			angle = int((angle * 100 + 0.5) / 100) % 360;
			cout << "angle: "<<angle << endl;
			float x = r * cos(angle * CV_PI / 180);
			float y = r * sin(angle * CV_PI / 180);
			target_point_ = circle_center_point+ cv::Point(x , y);
			cv::circle(tem_img, target_point_, 3, cv::Scalar(0, 255, 255), 3);//װ�װ�����
			cv::circle(tem_img, circle_center_point, 8, cv::Scalar(255, 255, 0), 5);//�糵����

			//����Ԥ�⣺
			windmill_pre.constant_run(circle_center_point, target_point_, 0, delta_time_, constant_rotation_speed_ * CV_PI / 180);
			cv::circle(tem_img, windmill_pre.predict_point, 10, cv::Scalar(0, 0, 255), 5);//Ԥ�����װ�װ�����

			imshow("���ٷ糵", tem_img);
			cv::waitKey(80);
			cout << i << endl<<endl;
		}
	
	}
	else {
		auto time_start = std::chrono::steady_clock::now();//�糵��ʼת��ʱ��
		auto time_end = std::chrono::steady_clock::now();
		std::chrono::duration<double, std::milli> elapsed = time_end - time_start;// std::milli ��ʾ�Ժ���Ϊʱ�䵥λ
		double time_1 = (time_start - time_start).count();// ��ʼʱ�䣬�Ժ���Ϊʱ�䵥λ
		double time_2 = elapsed.count();// ��ǰʱ�䣬�Ժ���Ϊʱ�䵥λ

		hnurm::Windmill_Predict windmill_pre;

		int cnt = 50000;
		for (int i = 1;i < cnt;i++) {

			cv::waitKey(50);
			cout << i << endl;

			cv::Mat tem_img = windmill_img.clone();

			//ģ��һ�����ڱ���ת�ķ糵
			//time_1 = time_2;
			auto time_end = std::chrono::steady_clock::now();
			std::chrono::duration<double, std::milli> elapsed = time_end - time_start;// std::milli ��ʾ�Ժ���Ϊʱ�䵥λ
			time_2 = elapsed.count();// ��ǰʱ�䣬�Ժ���Ϊʱ�䵥λ
			cout << "time_1: " << time_1 / 1000 << endl;
			cout << "time_2: " << time_2 / 1000 << endl;


			float angle = my_integral(my_fun, time_1 / 1000, time_2 / 1000);//��ʱװ�װ�ĽǶ�
			//cout << "fun(time_1): " << fun(time_1) << endl;
			//cout << "fun(time_2): " << fun(time_2) << endl;

			angle = (double)(int(angle * 100) % 36000) / 100;
			cout << "angle: " << angle << endl;

			float r = dst_width / 4;

			float x = r * cos(angle * CV_PI / 180);
			float y = r * sin(angle * CV_PI / 180);
			target_point_ = circle_center_point + cv::Point(x, y);
			cv::circle(tem_img, target_point_, 3, cv::Scalar(0, 255, 255), 3);//װ�װ�����
			cout << "armor:  " << target_point_ << endl;
			cv::circle(tem_img, circle_center_point, 8, cv::Scalar(255, 255, 0), 5);//�糵����

			//����Ԥ�⣺
			windmill_pre.vary_run(circle_center_point, target_point_, 1, delta_time_, time_2/1000, time_2/1000 +delta_time_);
			//cout << windmill_pre.predict_point << endl;
			cv::circle(tem_img, windmill_pre.predict_point, 10, cv::Scalar(0, 0, 255), 5);//Ԥ�����װ�װ�����

			imshow("���ٷ糵", tem_img);

		}
	}
	
	
	
	
	cv::waitKey();
	
	return 0;
}
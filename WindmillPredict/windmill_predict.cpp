#include "windmill_predict.h"
using namespace cv;

namespace hnurm {

	Windmill_Predict::Windmill_Predict() {}

	Windmill_Predict::Windmill_Predict(cv::Point circle_center_point_, cv::Point target_point_, bool speed_flag_, float delta_time_, float constant_rotation_speed_ ) {
		circle_center_point = circle_center_point_;
		target_point = target_point_;
		speed_flag = speed_flag_;
		constant_rotation_speed = constant_rotation_speed_;
		delta_time = delta_time_;

	}

	void Windmill_Predict::constant_run(cv::Point circle_center_point_, cv::Point target_point_, bool speed_flag_, float delta_time_, float constant_rotation_speed_ ) {
		circle_center_point = circle_center_point_;
		target_point = target_point_;
		speed_flag = speed_flag_;
		constant_rotation_speed = constant_rotation_speed_;
		delta_time = delta_time_;
		if (speed_flag_ == 0) {
			if (get_predict_rad()) {
				int x1, y1;//风车圆心坐标
				int x2, y2;//目标装甲板中心坐标

				//    为了减小强制转换的误差
				x1 = circle_center_point.x * 100;
				x2 = target_point.x * 100;
				y1 = circle_center_point.y * 100;
				y2 = target_point.y * 100;

				predict_point.x = static_cast<int>(
					(x1 + (x2 - x1) * cos(-predict_rad) - (y1 - y2) * sin(-predict_rad)) / 100);
				predict_point.y = static_cast<int>(
					(y1 - (x2 - x1) * sin(-predict_rad) - (y1 - y2) * cos(-predict_rad)) / 100);

				target_polar_angle = static_cast<float>( atan2(-1 * (target_point.y - circle_center_point.y),
					(target_point.x - circle_center_point.x)));
			}
			else {
				std::cout << "Failed to get predict angle of the windmill " << std::endl;
			}

		}
		else  {
			
				std::cout << "params error ! " << std::endl;
		}
		
	}

	void Windmill_Predict::vary_run(cv::Point circle_center_point_, cv::Point target_point_, bool speed_flag_, float delta_time_, double time_start, double time_now) {
		circle_center_point = circle_center_point_;
		target_point = target_point_;
		speed_flag = speed_flag_;
		delta_time = delta_time_;
		if (speed_flag_ == 1) {
			if (get_predict_rad(time_start, time_now)) {
				int x1, y1;//风车圆心坐标
				int x2, y2;//目标装甲板中心坐标

				//    为了减小强制转换的误差
				x1 = circle_center_point.x * 100;
				x2 = target_point.x * 100;
				y1 = circle_center_point.y * 100;
				y2 = target_point.y * 100;

				predict_point.x = static_cast<int>(
					(x1 + (x2 - x1) * cos(-predict_rad) - (y1 - y2) * sin(-predict_rad)) / 100);
				predict_point.y = static_cast<int>(
					(y1 - (x2 - x1) * sin(-predict_rad) - (y1 - y2) * cos(-predict_rad)) / 100);

				target_polar_angle = static_cast<float>( atan2(-1 * (target_point.y - circle_center_point.y),
					(target_point.x - circle_center_point.x)));
			}
			else {
				std::cout << "Failed to get predict angle of the windmill " << std::endl;
			}

		}
		else {

			std::cout << "params error ! " << std::endl;
		}

	}

	bool Windmill_Predict::get_predict_rad(double time_start, double time_now) {
		if (speed_flag == 0) {//0:风车匀速转动  1：风车变速转动
			predict_rad = constant_rotation_speed * delta_time;
			return true;
		}
		else {
			/*  spd = 0.785*sin（1.884*t)+1.305  rad/s  */
			predict_rad = my_integral(fun, time_start , time_now );
			return true;
		}

		std::cout << "Failed to get predict_rad of the windmill " << std::endl;
		return false;
	}
	

}
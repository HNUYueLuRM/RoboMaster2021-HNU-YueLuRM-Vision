#ifndef __WINDMILL_SOLVER__
#define  __WINDMILL_SOLVER__

#include "GeneralDefs.h"
#include <opencv.hpp>
#include <vector>

namespace hnurm {
	
	
	class Windmill_solver
	{
	public:
		Windmill_solver(const TeamColor& color);
		~Windmill_solver() = default;

		// 返回待击打的装甲板的像素坐标，如果没有检测到
		// 就返回当前的图像中心像素坐标 (resolution_width / 2, resolution_height/2)
		PixelPoint2d detect_armor_center(const cv::Mat& mat);


		// 返回待击打的装甲板的四个顶点（PNP控制点）的像素坐标 (u, v)
		// 返回待击打的装甲板的四个顶点的 几何中心的像素坐标 (u, v)
		// std::vector<PixelPoint2d> 中目前一共返回 5组 控制点
		// 下标[0] 是计算出的四个周围的点的中心点
		// 下标[1] - 下标[4] 是检测出的要打击的梯形装甲板的四个顶点
		// 如果没有检测到，就返回一个 std::vector<PixelPoint2d>{PixelPoint2d(u, v)};
		// 其中 (u, v) = (resolution_width / 2, resolution_height/2)
		std::vector<PixelPoint2d> detect_armor_cornor_and_center(const cv::Mat& mat);
		
		// 返回检测到的风车中心的 R 标识中心的像素坐标系下的坐标
		// 如果没有检测到，就返回当前的图像中心  PixelPoint2d(u, v);
		// 其中 (u, v) = (resolution_width / 2, resolution_height/2)
		PixelPoint2d detect_windmill_center_r_icon(const cv::Mat& mat);

		// 返回旋转速度
		// 通过 static 静态量来控制传入的 mat 对象迭代更新内部的 mat，累计算出旋转量 
		RotateInfo get_rotation_info(const cv::Mat& mat);
		

		// 预测函数
		// 传入一个时间间隙，从这个时刻开始会 运动到的像素坐标系的二维坐标
		PixelPoint2d predict_pos(std::chrono::milliseconds milliseconds);

		cv::Point circle_center_point;//风车圆心坐标
		cv::Point armor_point;//需要射击的装甲板的中心坐标
	private:
		TeamColor _own_color, _enemy_color;

		bool isValidCenterRContour(const std::vector<cv::Point>& center_R_contour);


		cv::RotatedRect target_armor;//目标装甲板（理论上仅一个）

		
		cv::RotatedRect centerR;//风车中心字母R的可能候选区
		cv::RotatedRect center_ROI;//风车中心候选区
		
		float resolution_height;
		float resolution_width ;//当前图象的高度和宽度
		
	};
}

#endif
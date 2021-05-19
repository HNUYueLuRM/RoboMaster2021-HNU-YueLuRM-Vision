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

		// ���ش������װ�װ���������꣬���û�м�⵽
		// �ͷ��ص�ǰ��ͼ�������������� (resolution_width / 2, resolution_height/2)
		PixelPoint2d detect_armor_center(const cv::Mat& mat);


		// ���ش������װ�װ���ĸ����㣨PNP���Ƶ㣩���������� (u, v)
		// ���ش������װ�װ���ĸ������ �������ĵ��������� (u, v)
		// std::vector<PixelPoint2d> ��Ŀǰһ������ 5�� ���Ƶ�
		// �±�[0] �Ǽ�������ĸ���Χ�ĵ�����ĵ�
		// �±�[1] - �±�[4] �Ǽ�����Ҫ���������װ�װ���ĸ�����
		// ���û�м�⵽���ͷ���һ�� std::vector<PixelPoint2d>{PixelPoint2d(u, v)};
		// ���� (u, v) = (resolution_width / 2, resolution_height/2)
		std::vector<PixelPoint2d> detect_armor_cornor_and_center(const cv::Mat& mat);
		
		// ���ؼ�⵽�ķ糵���ĵ� R ��ʶ���ĵ���������ϵ�µ�����
		// ���û�м�⵽���ͷ��ص�ǰ��ͼ������  PixelPoint2d(u, v);
		// ���� (u, v) = (resolution_width / 2, resolution_height/2)
		PixelPoint2d detect_windmill_center_r_icon(const cv::Mat& mat);

		// ������ת�ٶ�
		// ͨ�� static ��̬�������ƴ���� mat ������������ڲ��� mat���ۼ������ת�� 
		RotateInfo get_rotation_info(const cv::Mat& mat);
		

		// Ԥ�⺯��
		// ����һ��ʱ���϶�������ʱ�̿�ʼ�� �˶�������������ϵ�Ķ�ά����
		PixelPoint2d predict_pos(std::chrono::milliseconds milliseconds);

		cv::Point circle_center_point;//�糵Բ������
		cv::Point armor_point;//��Ҫ�����װ�װ����������
	private:
		TeamColor _own_color, _enemy_color;

		bool isValidCenterRContour(const std::vector<cv::Point>& center_R_contour);


		cv::RotatedRect target_armor;//Ŀ��װ�װ壨�����Ͻ�һ����

		
		cv::RotatedRect centerR;//�糵������ĸR�Ŀ��ܺ�ѡ��
		cv::RotatedRect center_ROI;//�糵���ĺ�ѡ��
		
		float resolution_height;
		float resolution_width ;//��ǰͼ��ĸ߶ȺͿ��
		
	};
}

#endif
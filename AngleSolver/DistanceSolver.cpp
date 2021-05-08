#include "DistanceSolver.h"
namespace hnurm
{
    void DistanceSolverParam::ReadFile()
    {
        cv::FileStorage fs_read1("../Param/camera_2_data.xml", cv::FileStorage::READ);
        if (!fs_read1.isOpened())
        {
			std::cerr << "failed to open CameraParams.xml" << std::endl;
			return;
		}
        fs_read1["camera_matrix"] >> CameraMatrix;
		fs_read1["distortion_coefficients"] >> DistortionMatrix;
		fs_read1.release();
        cv::FileStorage fs_read2("../Param/ArmorParams.xml", cv::FileStorage::READ);
		if (!fs_read2.isOpened())
		{
			std::cerr << "failed to open ArmorParams.xml" << std::endl;
			return;
		}
		fs_read2["big_armor_length"] >> BigArmorLength;
		fs_read2["big_armor_height"] >> BigArmorHeight;
		fs_read2["small_armor_length"] >> SmallArmorLength;
		fs_read2["small_armor_height"] >> SmallArmorHeight;
        return ;
    }

    DistanceSolver::DistanceSolver()
    {
        _Params.ReadFile();
        _BigArmor_3DPoint = std::vector<cv::Point3f>
        {
            cv::Point3f(-_Params.BigArmorLength / 2.0, _Params.BigArmorHeight / 2.0, 0.0),
            cv::Point3f(_Params.BigArmorLength / 2.0, _Params.BigArmorHeight / 2.0, 0.0),
            cv::Point3f(_Params.BigArmorLength / 2.0, -_Params.BigArmorHeight / 2.0, 0.0),
            cv::Point3f(-_Params.BigArmorLength / 2.0, -_Params.BigArmorHeight / 2.0, 0.0)
        };

        _SmallArmor_3DPoint = std::vector<cv::Point3f>
        {
            cv::Point3f(-_Params.SmallArmorLength / 2.0, _Params.SmallArmorHeight / 2.0, 0.0),
            cv::Point3f(_Params.SmallArmorLength / 2.0, _Params.SmallArmorHeight / 2.0, 0.0),
            cv::Point3f(_Params.SmallArmorLength / 2.0, -_Params.SmallArmorHeight / 2.0, 0.0),
            cv::Point3f(-_Params.SmallArmorLength / 2.0, -_Params.SmallArmorHeight / 2.0, 0.0)  
        };

        _rVec = cv::Mat::zeros(3, 1, CV_64FC1);
        _tVec = cv::Mat::zeros(3, 1, CV_64FC1);
    }
    ArmorType DistanceSolver::JudgeArmor(cv::Rect &target_rect)
    {
        std::cout << (float)((float)target_rect.width / (float)target_rect.height) << std::endl;
        if((float)(target_rect.width / target_rect.height) >= 1.5)
        {
            return BIG;
        }
        else return SMALL;
    }
    float DistanceSolver::Solve_Distance(cv::Rect &target_rect)
    {

        cv::Point2f left_top, right_top, right_down, left_down;
        left_top = target_rect.tl();
        right_down = target_rect.br();
        right_top.y = left_top.y;
        right_top.x = left_top.x + target_rect.width;
        left_down.y = right_down.y;
        left_down.x = right_down.x - target_rect.width;
        std::vector<cv::Point2f> target_points = {left_top, right_top, right_down, left_down};
        cv::undistortPoints(target_points, target_points, _Params.CameraMatrix, _Params.DistortionMatrix, cv::noArray(), _Params.CameraMatrix);
        if(JudgeArmor(target_rect) == BIG)
        {
            cv::solvePnP(_BigArmor_3DPoint, target_points, _Params.CameraMatrix, _Params.DistortionMatrix, _rVec, _tVec, false, cv::SOLVEPNP_ITERATIVE);
        }
        else cv::solvePnP(_SmallArmor_3DPoint, target_points, _Params.CameraMatrix, _Params.DistortionMatrix, _rVec, _tVec, false, cv::SOLVEPNP_ITERATIVE);

        float Distance = sqrt(_tVec.at<double>(0, 0) * _tVec.at<double>(0, 0) + _tVec.at<double>(1, 0) * _tVec.at<double>(1, 0) + _tVec.at<double>(2, 0) * _tVec.at<double>(2, 0));
        return Distance;
    }

}
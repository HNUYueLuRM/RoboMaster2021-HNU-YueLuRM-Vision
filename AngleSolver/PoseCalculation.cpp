#include "PoseCalculation.h"
#include "opencv2/opencv.hpp"
#include "math.h"

using namespace cv;
using namespace std;

namespace hnurm {

	void AngleSolverParam::readFile()
	{
        
		//��ȡ�������
        cv::FileStorage fs_read1("../Param/camera_2_data.xml", cv::FileStorage::READ);
		if (!fs_read1.isOpened())
        {
			std::cerr << "failed to open CameraParams.xml" << std::endl;
			return;
		}
		fs_read1["camera_matrix"] >> camera_matrix;
		fs_read1["distortion_coefficients"] >> distortion_coeff;
		fs_read1.release();

		//��ȡװ�װ����
        cv::FileStorage fs_read2("../Param/ArmorParams.xml", cv::FileStorage::READ);
		if (!fs_read2.isOpened())
		{
			std::cerr << "failed to open ArmorParams.xml" << std::endl;
			return;
		}
		fs_read2["big_armor_length"] >> big_armor_length;
		fs_read2["big_armor_height"] >> big_armor_height;
		fs_read2["small_armor_length"] >> small_armor_length;
		fs_read2["small_armor_height"] >> small_armor_height;
        fs_read2["mineral_half_length"] >> mineral_half_length;
		return;
	};




    /* definition of class AngleSolver */
	AngleSolver::AngleSolver(){}

	void AngleSolver::init()
	{
		params.readFile();

		//set params  of armor
		point_3d_of_big_armor = std::vector<cv::Point3f>
		{
			//��װ�װ��Ӧ�������������꣬Z��ȡ0����λΪmm
			cv::Point3f(-params.big_armor_length / 2, params.big_armor_height / 2, 0),		//TopLeft 
			cv::Point3f(params.big_armor_length / 2, params.big_armor_height / 2, 0),		//TopRight
			cv::Point3f(params.big_armor_length / 2, -params.big_armor_height / 2, 0),		//BottomRight
			cv::Point3f(-params.big_armor_length / 2, -params.big_armor_height / 2, 0)		//BottomLeft 
		};
		point_3d_of_small_armor = std::vector<cv::Point3f>
		{
			//Сװ�װ��Ӧ�������������꣬Z��ȡ0����λΪmm
			cv::Point3f(-params.small_armor_length / 2, params.small_armor_height / 2, 0),		//TopLeft 
			cv::Point3f(params.small_armor_length / 2, params.small_armor_height / 2, 0),		//TopRight
			cv::Point3f(params.small_armor_length / 2, -params.small_armor_height / 2, 0),		//BottomRight
			cv::Point3f(-params.small_armor_length / 2, -params.small_armor_height / 2, 0)		//BottomLeft 
		};
        point_3d_of_mineral = std::vector<cv::Point3f>{
            cv::Point3f(-params.mineral_half_length, -params.mineral_half_length, 0),	//tl
            cv::Point3f(params.mineral_half_length, -params.mineral_half_length, 0),	//tr
            cv::Point3f(params.mineral_half_length, params.mineral_half_length, 0),	//br
            cv::Point3f(-params.mineral_half_length, params.mineral_half_length, 0)	//bl
        };

        //init result
        result.yaw=0;
        result.pitch=0;
        result.distance=0;
	}

	//set corner points for PNP
	void AngleSolver::setTarget(const std::vector<cv::Point2f> objectPoints)
	{
		point_2d_of_armor = objectPoints;
	}

    void AngleSolver::setArmorTarget(const std::vector<ArmorInfo> Armors)
    {
        // pass
    }

    std::vector<cv::Point2f> AngleSolver::my_undistort_function() {//**��������
		std::vector<cv::Point2f> InputArray_src(point_2d_of_armor);//δ��������
		std::vector<cv::Point2f> OutputArray_dst;//����������
        undistortPoints(InputArray_src, OutputArray_dst, params.camera_matrix, params.distortion_coeff, cv::noArray(), params.camera_matrix);//����

		return  OutputArray_dst;
	}
    
    SerialData AngleSolver::solve(const std::vector<cv::Point2f> objectPoints){
        //undistorted_point_2d_of_armor=my_undistort_function();//**��������
        //solvePnP(_params.POINT_3D_OF_BIG_ARMOR, undistorted_point_2d_of_armor, _params.camera_matrix, _params.distortion_coeff, _rVec, _tVec, false, SOLVEPNP_ITERATIVE);
		solvePnP(point_3d_of_big_armor, objectPoints, params.camera_matrix, params.distortion_coeff, _rVec, _tVec, false, SOLVEPNP_ITERATIVE);

		//����yaw��pictch�Ƕ�
		_yaw_Angle = atan(_tVec.at<double>(0, 0) / _tVec.at<double>(2, 0)) / CV_PI * 180;//yawֵ���Ƕ��Ƶ�λ
		_pitch_Angle = atan(_tVec.at<double>(1, 0) / _tVec.at<double>(2, 0)) / CV_PI * 180;//pitchֵ���Ƕ�ֵ��λ
		_euclideanDistance = sqrt(_tVec.at<double>(0, 0) * _tVec.at<double>(0, 0) + _tVec.at<double>(1, 0) * _tVec.at<double>(1, 0) + _tVec.at<double>(2, 0) * _tVec.at<double>(2, 0));

        result.yaw=_yaw_Angle;
        result.pitch=_pitch_Angle;
        result.distance=_euclideanDistance;

        return result;
	}

    SerialData AngleSolver::solve(const ArmorInfo Armors) {
		switch (Armors.size) 
		{
			case (ArmorSize::BIG): {//��װ�װ�
				solvePnP(point_3d_of_big_armor, Armors.object_points, params.camera_matrix, params.distortion_coeff, _rVec, _tVec, false, SOLVEPNP_ITERATIVE);
				break;
			}
			case (ArmorSize::SMALL): {//Сװ�װ�
				solvePnP(point_3d_of_small_armor, Armors.object_points, params.camera_matrix, params.distortion_coeff, _rVec, _tVec, false, SOLVEPNP_ITERATIVE);
				break;
			}
		}

		//����yaw��pictch�Ƕ�
		_yaw_Angle = atan(_tVec.at<double>(0, 0) / _tVec.at<double>(2, 0)) / CV_PI * 180;//yawֵ���Ƕ��Ƶ�λ
		_pitch_Angle = atan(_tVec.at<double>(1, 0) / _tVec.at<double>(2, 0)) / CV_PI * 180;//pitchֵ���Ƕ�ֵ��λ
		cout<<"tz"<<_tVec.at<double>(2, 0)<<endl;
		_euclideanDistance = sqrt(_tVec.at<double>(0, 0) * _tVec.at<double>(0, 0) + _tVec.at<double>(1, 0) * _tVec.at<double>(1, 0) + _tVec.at<double>(2, 0) * _tVec.at<double>(2, 0));

		//�ֶ�����yaw��pitch
		//undistorted_point_2d_of_armor = my_undistort_function();//**��������
		//double fx = params.camera_matrix.at<double>(0, 0);
		//double fy = params.camera_matrix.at<double>(1, 1);
		//double u0 = params.camera_matrix.at<double>(0, 2);
		//double v0 = params.camera_matrix.at<double>(1, 2);

		//double u = (undistorted_point_2d_of_armor[0].x + undistorted_point_2d_of_armor[1].x + undistorted_point_2d_of_armor[2].x + undistorted_point_2d_of_armor[3].x) / 4;
		//double v = (undistorted_point_2d_of_armor[0].y + undistorted_point_2d_of_armor[1].y + undistorted_point_2d_of_armor[2].y + undistorted_point_2d_of_armor[3].y) / 4;

		//_yaw_Angle = atan((u - u0) / fx) / CV_PI * 180;//yawֵ���Ƕ��Ƶ�λ
		//_pitch_Angle = atan((v - v0) / fy) / CV_PI * 180;//pitchֵ���Ƕ�ֵ��λ

        result.yaw=_yaw_Angle;
        result.pitch=_pitch_Angle;
        result.distance=_euclideanDistance;

        return result;
	}

    SerialData AngleSolver::pnpsolve(const QRData qrData)
    {
        //std::cout << qrData.boxpoint[0] << ' ' << qrData.boxpoint[1] << ' ' << qrData.boxpoint[2] << ' ' << qrData.boxpoint[3] << std::endl;
        solvePnP(point_3d_of_mineral, qrData.boxpoint, params.camera_matrix, params.distortion_coeff, _rVec, _tVec, false, SOLVEPNP_ITERATIVE);
        //std::cout << tVec << std::endl;
        //????yaw??pictch???
        _yaw_Angle = atan(_tVec.at<double>(0, 0) / _tVec.at<double>(2, 0)) / CV_PI * 180;//yawֵ���Ƕ��Ƶ�λ
        _pitch_Angle = atan(_tVec.at<double>(1, 0) / _tVec.at<double>(2, 0)) / CV_PI * 180;//pitchֵ���Ƕ�ֵ��λ
        _euclideanDistance = sqrt(_tVec.at<double>(0, 0) * _tVec.at<double>(0, 0) + _tVec.at<double>(1, 0) * _tVec.at<double>(1, 0) + _tVec.at<double>(2, 0) * _tVec.at<double>(2, 0));

        result.yaw=_yaw_Angle;
        result.pitch=_pitch_Angle;
        result.distance=_euclideanDistance;

        return result;

    }

	double AngleSolver::getYaw() {
		return _yaw_Angle;
	};
	double AngleSolver::getPitch() {
		return _pitch_Angle;
	};
	double AngleSolver::getDistance()
	{
		return _euclideanDistance;
	}
	cv::Mat AngleSolver::getTvec() {
		return _tVec;
	}


}

#include "AngleSolver.h"
#include <cmath>
namespace hnurm
{
    AngleSolver::AngleSolver()
    {
        //Air resistance parameters
        k1 = 0.47 * 1.169 * 2.0 * 3.14 * (0.0085 * 0.0085) / 2.0;
        k1 = k1 / 0.0032;
        _distanceSolver = std::make_unique<DistanceSolver>();
    }
    cv::Point2f AngleSolver::Model(float angle, float speed, float x)
    {
        float speed_parallel = speed * cos(angle);
        float speed_vertical = speed * sin(angle);
        float time = (exp(k1 * x) - 1.0) / (k1 * speed_parallel);
        float y = speed_vertical * time - 0.5 * 9.8 * time * time;
        cv::Point2f temp(x,y);
        return temp;
    }
    SerialData AngleSolver::Solve_Angle(float current_angle, float speed, cv::Rect &target_rect)
    {
        float distance = _distanceSolver->Solve_Distance(target_rect);
        float yaw_Angle = atan(_distanceSolver->_tVec.at<double>(0, 0) / _distanceSolver->_tVec.at<double>(2, 0)) / CV_PI * 180;
        float pitch_Angle = atan(_distanceSolver->_tVec.at<double>(1, 0) / _distanceSolver->_tVec.at<double>(2, 0)) ;
        float temp_Angle = current_angle - pitch_Angle;
        float x = distance * cos(temp_Angle);
        float y = distance * sin(temp_Angle);
        cv::Point2f target_point = cv::Point2f(x, y);
        cv::Point2f temp_point = target_point;
        cv::Point2f real_point;
        float real_angle = 0;
        float deltaH = 0.0;

        for(int i = 0; i < 50; i++)
        {
            real_angle = atan(temp_point.y / temp_point.x);
            real_point = Model(real_angle, speed, temp_point.x);
            deltaH = target_point.y - real_point.y;
            temp_point.y += deltaH;
        }
        real_angle = real_angle * 180 / 3.1415926;
        SerialData result;
        result.set(0, real_angle, yaw_Angle, distance);
        return result;

    }
}
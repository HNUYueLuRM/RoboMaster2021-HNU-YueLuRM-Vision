#include "DistanceSolver.h"
#include "../Buffer/SerialData.hpp"
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
namespace hnurm
{
    class AngleSolver
    {
    public:
        AngleSolver();
        ~AngleSolver() = default;

        //@return : return a angle which is be compensate
        //@param : current_angle : The angle that got from stm32
        //@brief : compensate angle
        SerialData Solve_Angle(float current_angle, float speed, cv::Rect &target_rect);

    private:
        cv::Point2f Model(float angle, float speed, float x);
        //DistanceSolver _distanceSolver;
        std::unique_ptr<DistanceSolver> _distanceSolver;
        float k1;
    };
}
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include "../Buffer/SerialData.hpp"
namespace hnurm
{

    struct DistanceSolverParam
    {
        cv::Mat CameraMatrix;       //Camera Matrix from Camera calibration
        cv::Mat DistortionMatrix;   //Distortion Matrix from Camera calibration
        float BigArmorLength, BigArmorHeight;   //Big Armor length and height
        float SmallArmorLength, SmallArmorHeight;   //small armor length and height
        void ReadFile();    //read params from file
    };
    enum ArmorType:int{BIG = 0, SMALL = 1};
    class DistanceSolver
    {
    public:
        DistanceSolver();
        ~DistanceSolver() = default;
        cv::Mat _rVec;          
        cv::Mat _tVec;  
        float Solve_Distance(cv::Rect &target_rect);       //get distance

    private:
        DistanceSolverParam _Params;
        ArmorType JudgeArmor(cv::Rect &target_rect);
        std::vector<cv::Point3f> _BigArmor_3DPoint;          //World coordinate system of Big Armor
        std::vector<cv::Point3f> _SmallArmor_3DPoint;        //World coordinate system of Big Armor
    };
}
#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

namespace hnurm
{
    class MineralDetector
    {
    public:
        MineralDetector(const Mat& raw_frame);
        ~MineralDetector() = default;

        void GetCooked(void);
        void CreateMineral(void);
        Point2f GetCore(void);

    private:
        Mat raw_img;
        Mat hsv_img;
        Mat cooked_img;
        vector<vector<Point>> contours;
        RotatedRect mineral_rect;
        Point2f mineral_core;
    };
}

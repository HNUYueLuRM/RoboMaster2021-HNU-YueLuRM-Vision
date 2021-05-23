#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <memory>
#include "../Protocol/protocol.h"
#include "../nanodet/nanodet.h"
using namespace std;
namespace hnurm
{
    struct object_rect
    {
        int x;
        int y;
        int width;
        int height;
    };

    class ArmorDetector
    {
    public:
        ArmorDetector();
        ~ArmorDetector() = default;
        int resize_uniform(cv::Mat &src, cv::Mat &dst, cv::Size dst_size, object_rect &effect_area);
        bool Detect(cv::Mat &raw_image, cv::Rect &target_rect);
        bool set_color(int color);

    private:
        unique_ptr<NanoDet> _NanoDet;
        //why so many color here...
        int _my_color;
        //Protocol::Self_color my_color;
    };
};
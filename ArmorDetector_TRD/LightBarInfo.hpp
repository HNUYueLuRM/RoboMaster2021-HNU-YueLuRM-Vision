#pragma once
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

namespace hnurm
{
    //cause the angle system for opencv4.*,we have to create a new class for RotatedRect
    //class to descript a light bar
    class LightBarInfo
    {
    public:

        LightBarInfo(){}

        LightBarInfo(RotatedRect& _light_rect,int _long_edge,int _short_edge)
        {
            light_rect=_light_rect;
            long_edge=_long_edge;
            short_edge=_short_edge;
            angle=long_edge==light_rect.size.width ? light_rect.angle : light_rect.angle-90;
        }

        LightBarInfo operator=(const LightBarInfo& light_bar_info)
        {
            LightBarInfo tmp;
            tmp.angle=light_bar_info.angle;
            tmp.light_rect=light_bar_info.light_rect;
            tmp.long_edge=light_bar_info.long_edge;
            tmp.short_edge=light_bar_info.short_edge;
            return tmp;
        }

    public:

        void DrawLightBar(Mat& canvas)
        {
            Point2f tmp_points[4];
            light_rect.points(tmp_points);
             for (int i = 0; i < 4; i++)
            {
                line(canvas, tmp_points[i], tmp_points[(i + 1) % 4], Scalar(0,255,0), 2);
            }
        }

    public:

        RotatedRect light_rect;
        int long_edge;
        int short_edge;
        double angle;
    };
}
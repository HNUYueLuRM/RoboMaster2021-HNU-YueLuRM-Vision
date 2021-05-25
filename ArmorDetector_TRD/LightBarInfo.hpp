/**************************************************************
                    MIT License
        Copyright (c) 2021 HNU-YueLuRM-Vision
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

        Authors: Zeng QingCheng, <neozng1@hnu.edu.cn>
**************************************************************/

#pragma once
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

namespace hnurm
{
    /**
     * @class LightBarInfo LightBarinfo.hpp in ArmorDetector_TRD
     * @brief cause the angle system for opencv4.*,we have to create a new class for RotatedRect
     *        member including longedge,shortedge,core,angle and a raw rotated rect
     */
    class LightBarInfo
    {
    public:

        LightBarInfo(){}

        LightBarInfo(RotatedRect& _light_rect,float _long_edge,float _short_edge)
        {
            light_rect=_light_rect;
            long_edge=_long_edge;
            short_edge=_short_edge;
            angle = (long_edge==light_rect.size.width) ? -light_rect.angle : -light_rect.angle+90;//regulate
            core=light_rect.center;
        }

        LightBarInfo operator=(const LightBarInfo& light_bar_info)
        {
            LightBarInfo tmp;
            tmp.angle=light_bar_info.angle;
            tmp.light_rect=light_bar_info.light_rect;
            tmp.long_edge=light_bar_info.long_edge;
            tmp.short_edge=light_bar_info.short_edge;
            tmp.core=light_bar_info.core;
            return tmp;
        }

    public:

        /**
         * @brief draw this lightbar in canvas img
         * 
         * @param canvas where the light bar will be drawn
         */
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
        Point2f core;
        float long_edge;
        float short_edge;
        double angle;  //anti-clockwise
    };

}//hnurm

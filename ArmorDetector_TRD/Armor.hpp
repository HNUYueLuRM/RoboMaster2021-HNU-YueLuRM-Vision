#pragma once
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <math.h>

using namespace cv;
using namespace std;

namespace hnurm
{

    class Armor
    {
    public:

        Armor(){}


        Armor(vector<Point2i> armor_arr)
        {
            for(int i=0;i<4;i++)
            {
                vertexes[i]=armor_arr[i];
            }
            core.x=(armor_arr[2].x+armor_arr[3].x)/2-(armor_arr[0].x+armor_arr[1].x)/2;
            core.y=(armor_arr[0].y+armor_arr[1].y+armor_arr[2].y+armor_arr[3].y)/4;
        }

        Armor operator=(const Armor& _armor)
        {
            Armor temp_armor;
            for(int i=0;i<4;i++)
            {
                temp_armor.vertexes[i]=_armor.vertexes[i];
            }
            temp_armor.core=_armor.core;
            return temp_armor;
        }

    public:

        void DrawArmor(Mat& canvas)
        {
            for (int i = 0; i < 4; i++)
            {
                line(canvas, vertexes[i], vertexes[(i + 1) % 4], Scalar(0,255,0), 2);
            }
        }

        void GetSVMImg(Mat& cut_remap)
        {
            //do some project transformation and cut here.
        }

    public:

        Point2i vertexes[4];
        Point2i core;

    };//Armor

}//hnurm

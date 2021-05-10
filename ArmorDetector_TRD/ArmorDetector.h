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
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <string>
#include <math.h>
#include "Armor.hpp"
#include "LightBarInfo.hpp"
#include "../Protocol/protocol.h"

using namespace std;

namespace hnurm
{
    //struct for param
    struct Param
    {
        int color_threshold_value;       //color threshold value
        int brightness_threshold_value;  //brightness threshold value
        int gamma_redress_value;         //for gamma redress
        int ele_erode_x, ele_erode_y;    //erode kernel
        int ele_dilated_x, ele_dilated_y;//dilate kernel

        //more params to go here...
    };



    //class for image pre-processing,such as split,dilate,errode
    class ArmorDetector
    {
    public:

        //we need to do something when constructing..
        ArmorDetector();

        //it seems that this could be deleted
        ~ArmorDetector()=default;

        //return with target Armor
        void Detect(const Mat& raw_frame,Armor& target);


    private:

        //split colorspace then subtract G/B
        void SubtractRB(void);

        //return image which has been processed
        void GetCooked(void);

        //get lightbar from the cooked image
        void CreateBar(void);

        //sift the contours of possible lightbar
        void SiftBar(void);

        //use two lightbars to create an armor
        void CreateArmor(LightBarInfo lrect,LightBarInfo rrect,Armor& dst);

        //pair those lightbars and mark them
        int PairBars(void);

        //choose one armor to attack
        //return with the index of choosed armor
        int Choose(void);


    private:
        Param param;
        Mat erode_kernel;
        Mat dilate_kernel;


    private:
        //mat for image processing,which store some mediate images
        //maybe we can pack these varas into a struct...

        Mat raw_img;				//1原始图像
        Mat gamma_img;              //2伽马矫正过后的图像
        Mat subtract_img;           //3image after subtracting G/B
        Mat filter_img;             //4filter these img above using a kernel
        Mat brightness_img;         //5颜色二值化图片
        Mat eroded_img;				//6【eroded】
        Mat dilated_img;		    //7【dilated]
        Mat cooked_img;             //process done


    private:
        //vetors for the lightbar(contours and bars)
        vector<vector<Point>>      light_contours;             //1 function cv::findcontour() will return a series of coutours,composed by points.
        vector<Vec4i>              light_hierachy;             //1
        vector<LightBarInfo>       light_rect;                 //2 refer {minAreaRect} to store lightbar rects
        vector<LightBarInfo>       final_lights;               //3 light bars sifted,with light information
        vector<Armor>              all_armors;                 //3 armors info,for vertexes and the center of amor

    private:

        Protocol::Self_color my_color;//where and when to get mycolor from serial or TaskSwitcher?

    };//armordetector


}//hnurm


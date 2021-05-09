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

#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

namespace hnurm
{

    //struct for param
    struct Param
    {
        int color_threshold_value;       //color threshold value
        int brightness_threshold_value;  //brightness threshold value
        int ele_erode_x, ele_erode_y;    //erode kernel
        int ele_dilated_x, ele_dilated_y;//dilate kernel
    };



    //structure for armor
    //the order of points is: top-left -> down-left -> top-right -> down-right
    struct Armor
    {
        Point2i vertexes[4];
        Point2i core;
        Armor();
        Armor(vector<Point2i> armor_arr)
        {
            vertexes[0]=armor_arr[0];
            vertexes[1]=armor_arr[1];
            vertexes[2]=armor_arr[2];
            vertexes[3]=armor_arr[3];
            core.x=0;
            core.y=0;
        }
    };


    //struct to descript a light bar
    struct LightBarInfo
    {
        RotatedRect light_bar;
        int long_edge;
        int short_edge;

        LightBarInfo();
        LightBarInfo(RotatedRect& _light_bar,int _long_edge,int _short_edge)
        {
            light_bar=_light_bar;
            long_edge=_long_edge;
            short_edge=_short_edge;
        }
    };



    //class for image pre-processing,such as split,dilate,errode
    class ArmorDetector
    {
    public:

        ArmorDetector();

        ~ArmorDetector()=default;

        //return with target rect
        int Detect(const Mat& raw_frame,Armor& target);

        //set debug flag,which can show raw image and images after
        void SetDebug(bool flag);

    private:

        //return image which has been processed
        void GetCooked(void);

        //split colorspace then subtract G/B
        void SubtractRB(void);

        //get lightbar from the cooked image
        void CreateBar(void);

        //sift the contours of possible lightbar
        void SiftBar(void);

        //
        void CreateArmor(int left_index, int right_index, Armor& dst);

        //
        void DrawRotatedRect(Mat& src, RotatedRect& rect, Scalar color, int thickness);

        //draw lines to indicate where the box is
        void DrawBox(Mat& src, Point2i* points, Scalar color, int thickness);

        //pair those lightbars and mark them
        int PairBars(void);

        //choose one armor to attack
        //return with the index of choosed armor
        int Choose(void);


    private:
        //mat for image processing,which store some mediate images
        //maybe we can pack these varas into a struct...
        Mat raw_img;					//1原始图像
        Mat gamma_img;                //2伽马矫正过后的图像
        Mat subtract_img;             //3image after subtracting G/B
        Mat filter_img;               //4filter these img above using a kernel
        Mat brightness_img;           //5颜色二值化图片
        Mat eroded_img;				//6【eroded】
        Mat dilated_img;				//7【dilated]
        Mat cooked_img;               //process done


    private:
        //vetors for the lightbar(contours and bars)
        vector<vector<Point>>      light_contours;             //1 function cv::findcontour() will return a series of coutours,composed by points.
        vector<Vec4i>              light_hierachy;          	//1
        vector<LightBarInfo>       light_rect;                 //2 refer {minAreaRect} to store lightbar rects
        vector<LightBarInfo>       final_lights;               //3 light bars sifted,with light information
        vector<Armor>              all_armors;                 //3 armors info,for vertexes and the center of amor

    private:
        bool  debug; //debug flag

    };//armordetector


}//hnurm


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
#include "../Protocol/protocol.h"
#include "Armor.hpp"
#include "LightBarInfo.hpp"
#include "Defaker.hpp"


using namespace std;
using namespace cv; 

namespace hnurm
{
    /**
     * @brief Params read from outer file
     */
    struct Param
    {
        int color_threshold_value;       //color threshold value
        int brightness_threshold_value;  //brightness threshold value
        int gamma_redress_value;         //for gamma redress
        int ele_erode_x, ele_erode_y;    //erode kernel
        int ele_dilated_x, ele_dilated_y;//dilate kernel
        Size vector_size;
        //more params to go here...
    };



    /**
     * @class ArmorDetector  ArmorDetector.h in root
     * @brief class for detecting armors;
     * 
     */
    class ArmorDetector
    {
    public:

        /**
         * @brief Construct a new Armor Detector object,initialize param
         * 
         */
        ArmorDetector();

        ~ArmorDetector()=default;

        /**
         * @brief public API of ArmorDetector
         * 
         * @param raw_frame raw_frame read from image_buffer
         * @param target    the best armor we recognize from this frame
         */
        void Detect(const Mat& raw_frame,Armor& target);

        /**
         * @brief Set the Target Color to tell which color to detect
         * 
         * @param _target_color target color: 0 blue / 1 red
         */
        void SetColor(int _color);

    private:

        /**
         * @brief split colorspace then subtract G/B(B/R)
         * @param private_member
         */
        void SubtractRB(void);

        /**
         * @brief Get the Cooked frame,including subtractRB,dialte,errode(close)
         * @param private_member
         */
        void GetCooked(void);

        /**
         * @brief get lightbars from the cooked image
         * @param private_member
         */
        void CreateBar(void);

        /**
         * @brief sift possible lightbar with more rigorous rules
         * @param private_member
         */
        void SiftBar(void);

        /**
         * @brief use two lightbars to create an armor
         * 
         * @param lrect the left lightbar of this possible armor,which described using a LightBarInfo
         * @param rrect the right lightbar of this possible armor,which described using a LightBarInfo
         * @param dst where we return the result
         */
        void CreateArmor(LightBarInfo lrect,LightBarInfo rrect,Armor& dst);

        /**
         * @brief a gamma redress function
         * @param private_member
         */
        void GammaRedress(void);

        /**
         * @brief pair those lightbars to create armor and mark them 
         * @param private_member
         * @return int how many armors we recognize in this frame
         */
        int PairBars(void);

        /**
         * @brief Choose one armor from buffer
         * @param private_member
         * @return int the index of choosed armor in private_member:all_armors
         */
        int Choose(void);

        /**
         * @brief clear all private buffer
         * @param private_member
         */
        void Reset(void);

    private:
        Param param;
        Mat erode_kernel;
        Mat dilate_kernel;


    private:
        //mat for image processing,which store some mediate images
        //maybe we can pack these varas into a struct...
        Mat raw_img;				//1 [raw] origin img
        Mat gamma_img;              //2 [GammaRedress] img after gamma redress
        Mat subtract_img;           //3 [SubtractRB] image after subtracting G/B
        Mat filter_img;             //4 [NoiseFilter] filt these img above using a kernel
        Mat brightness_img;         //5 [ThresholdBrightness]
        Mat eroded_img;				//6 [erode]
        Mat dilated_img;		    //7 [dilate]
        Mat cooked_img;             //process all done


    private:
        //vetors for the lightbar(contours and bars)
        vector<vector<Point>>      light_contours;     //1 function cv::findcontour() will return a series of coutours,composed by points.
        vector<Vec4i>              light_hierachy;     //1 funciton cv::findcontour() will return the hierachy of outline,useless temporary.
        vector<LightBarInfo>       light_rect;         //2 to store fitting rect from cv::minAreaRect()
        vector<LightBarInfo>       final_lights;       //3 light bars sifted,with light information
        vector<Armor>              all_armors;         //3 armors info,for vertexes and the center of amor

    private:

        int target_color;  //enemy color [0] for blue, [1] for red

    };//armordetector

}//hnurm


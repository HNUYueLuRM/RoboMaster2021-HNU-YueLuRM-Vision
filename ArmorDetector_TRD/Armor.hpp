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
#include <vector>
#include <math.h>

using namespace cv;
using namespace std;

namespace hnurm
{
//enum ArmorType {}

/**
 *  @class Armor  Armor.hpp in ArmorDetector_TRD
 * 
 *  @brief Armor is a class for the description of an armor,include for vertexes and the core of this armor
 */
class Armor
{

public:

    Armor()=default;

    /**
     * @brief Construct a new Armor object with armor_arr
     * 
     * @param armor_arr the pointer of array[4],order by top_left-> down_left-> top_right-> down_right
     */
    Armor(Point2f* armor_arr)
    {
        for(int i=0;i<4;i++)
        {
            vertexes[i]=armor_arr[i];
        }
        core.x=(armor_arr[2].x+armor_arr[3].x)/2-(armor_arr[0].x+armor_arr[1].x)/2;
        core.y=(armor_arr[0].y+armor_arr[1].y+armor_arr[2].y+armor_arr[3].y)/4;
    }


    /**
     * @brief overload '=' for Armor
     * 
     * @param _armor a receive value
     * @return Armor 
     */
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


    /**
     * @brief Draw this Armor in canvas
     * 
     * @param canvas where the armor gone be drew
     */
    void DrawArmor(Mat& canvas)
    {
        for (int i = 0; i < 4; i++)
        {
            line(canvas, vertexes[i], vertexes[(i + 1) % 4], Scalar(0,255,0), 2);
        }
    }

public:

    Point2f vertexes[4];
    Point2f core;

    //ArmorType type;

};//Armor

}//hnurm

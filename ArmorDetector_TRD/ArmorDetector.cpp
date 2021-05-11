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
#include "ArmorDetector.h"


namespace hnurm
{

ArmorDetector::ArmorDetector()
{
//    param;  read param files
    erode_kernel=getStructuringElement(MORPH_RECT, Size(param.ele_erode_x, param.ele_erode_y));
    dilate_kernel=getStructuringElement(MORPH_RECT, Size(param.ele_dilated_x, param.ele_dilated_y));
}



void ArmorDetector::Detect(const Mat& raw_frame, Armor& target)
{
    raw_img=raw_frame;
    GetCooked();
    CreateBar();
    SiftBar();
    if(PairBars()) //for overboundary check
    {
        target=all_armors[Choose()];
    }
    Reset();
}


//we need to test this process to tell which is faster
void ArmorDetector::SubtractRB()
{
    uchar *praw_pixel = (uchar*) raw_img.data;
    uchar *pdst_pixel = (uchar*) subtract_img.data;
    int pixel_number =  raw_img.rows *  raw_img.cols;

    if (my_color == Protocol::Self_color::blue)
    {
        for (int i = 0; i < pixel_number; i++)
        {
            if (*(praw_pixel + 2) - *praw_pixel > param.brightness_threshold_value)
            {
               *pdst_pixel = 255;
            }
            praw_pixel += 3;
            pdst_pixel++;
        }
    }
    else if (my_color == Protocol::Self_color::red)
    {
        for (int i = 0; i < pixel_number; i++)
        {
            if (*praw_pixel - *(praw_pixel+2) > param.brightness_threshold_value)
            {
                *pdst_pixel = 255;
            }
            praw_pixel += 3;
            pdst_pixel++;
        }
    }
}



void ArmorDetector::GetCooked(void)
{
    GammaRedress();
//  split
//  subtract
//  threshold
    SubtractRB();
    dilate( subtract_img,  dilated_img, dilate_kernel);
    erode( dilated_img,  eroded_img, erode_kernel);
    cvtColor(raw_img, brightness_img, COLOR_BGR2GRAY);
    threshold( brightness_img,  brightness_img, param.brightness_threshold_value, 255, THRESH_BINARY);

    cooked_img = eroded_img & brightness_img;

#ifdef DEBUG

    imshow("bi value R/B", subtract_img);
    imshow("bi value gray", brightness_img);
    imshow("cooked", cooked_img);
    imshow("dilate",dilated_img);
    imshow("errode",eroded_img);
    waitKey(1);

#endif
}



void ArmorDetector::CreateBar()
{
    RotatedRect temp_rrect;

    findContours(cooked_img, light_contours, /*light_hierachy,*/ RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
    for (int i = 0; i < light_contours.size(); i++)
    {
        if ((3 <= light_contours[i].size()) && (light_contours[i].size() <= 200))
        {
            temp_rrect = minAreaRect(light_contours[i]);

            //pre-sift lightbars
            /*  due to the fking opencv4 angle system,we gotta do this process:
            *   ————————————————————>  x-axis
            *   |   ________ width
            *   |   |      |           opencv angle system: spin the x axis anti-clockwise,
            *   |   |______| height    when it is parallel to an edge of your RotatedRect,that
            *   |                      edge would be set as width,though another pair would be
            *   |                      height.by doing this,the angle x-axis has passed would be
            *   |                      RotatedRect.angle
            *   v                      so angle would ranged between (-90,0]
            *  y-axis                  then we regulate them using a class: LightBarInfo
            *                          which record angle/longedge/shortedge/core of a light bar
            *   
            */

            if(temp_rrect.size.width>temp_rrect.size.height)
            {
                if(temp_rrect.angle > -63)
                {
                    continue;
                }
                 light_rect.push_back(LightBarInfo(temp_rrect,temp_rrect.size.width,temp_rrect.size.height));
            }
            else
            {
                if(temp_rrect.angle < -27)
                {
                    continue;
                }
                 light_rect.push_back(LightBarInfo(temp_rrect,temp_rrect.size.height,temp_rrect.size.width));
            }
        }
    } //loop
}

void ArmorDetector::SiftBar()
{
    double self_ratio;

    for (int i = 0; i < light_rect.size(); i++)
    {
        self_ratio = light_rect[i].long_edge / light_rect[i].short_edge;

        //sift ratio
        if ((self_ratio < 1.5) || (self_ratio > 12))
        {
            continue;
        }
        //sift area
        if ((light_rect[i].light_rect.size.area() < 10) || (light_rect[i].light_rect.size.area() > 60000))
        {
            continue;
        }
        final_lights.push_back(light_rect[i]);
    }

#ifdef DEBUG

    for (int i = 0; i < final_lights.size(); i++)
    {
        final_lights[i].DrawLightBar(raw_img);
    }
    imshow("sifted light rect", raw_img);
    waitKey(1)

#endif
}

void ArmorDetector::CreateArmor(LightBarInfo lrect, LightBarInfo rrect, Armor &dst)
{
    Point2f tmp_points[4]; // 0tl 1dl 2tr 3dr
    //left light
    tmp_points[0].x = lrect.core.x + lrect.long_edge * cos(lrect.angle);
    tmp_points[0].y = lrect.core.y - lrect.long_edge * sin(lrect.angle);
    tmp_points[1].x = lrect.core.x - lrect.long_edge * cos(lrect.angle);
    tmp_points[1].y = lrect.core.y + lrect.long_edge * sin(lrect.angle);
    //right light
    tmp_points[2].x = rrect.core.x + rrect.long_edge * cos(rrect.angle);
    tmp_points[2].y = rrect.core.y - rrect.long_edge * sin(rrect.angle);
    tmp_points[3].x = rrect.core.x - rrect.long_edge * cos(rrect.angle);
    tmp_points[3].y = rrect.core.y + rrect.long_edge * sin(rrect.angle);

    Armor temp_armor(tmp_points);
    dst = temp_armor;
}

void ArmorDetector::GammaRedress(void)
{
    unsigned char lut[256];
    for(int i=0;i < 256;i++)
    {
        lut[i] = saturate_cast<uchar>(pow((float)i/255.0,param.gamma_redress_value) * 255.0f);
    }
    gamma_img = raw_img.clone();
    int channels = raw_img.channels();
    switch(channels)
    {
        case 1:
        {
            MatIterator_<uchar> it = gamma_img.begin<uchar>();
            MatIterator_<uchar> end = gamma_img.end<uchar>();
            while(it != end)
            {
                *it = lut[(*it)];
                it ++;
            }
            break;
        }
        case 3:
            {
                MatIterator_<Vec3b> it = gamma_img.begin<Vec3b>();
                MatIterator_<Vec3b> end = gamma_img.end<Vec3b>();
                while(it != end)
                {
                    (*it)[0] = lut[(*it)[0]];
                    (*it)[1] = lut[(*it)[1]];
                    (*it)[2] = lut[(*it)[2]];
                    it ++;
                }
            break;
            }
        default:
            break;
    }
}



int ArmorDetector::PairBars()
{
//    int mark = 0;

    pair<float,float> height_s,width_s;
    //for each pair of bars;
    double /*angle_diff,*/ angle_diff, center_diff_ratio, relative_x, height_ratio,width_ratio;

    for (int i = 0; i <  final_lights.size(); i++)
    {
        for (int j = i + 1; j <  final_lights.size(); j++)
        {
//            angle_diff = abs(SiftedLightRect[i].angle - SiftedLightRect[j].angle);

            height_s = std::minmax(final_lights[i].long_edge, final_lights[j].long_edge);
            height_ratio = height_s.second / height_s.first;

            width_s = std::minmax(final_lights[i].short_edge, final_lights[j].short_edge);
            width_ratio = width_s.second / width_s.first;

            //ratio indicated by y-axis diff
            center_diff_ratio = height_s.first / abs(final_lights[i].light_rect.center.y - final_lights[j].light_rect.center.y);

            //the ratio of their len and distance between two centers
            relative_x = abs(2 * (final_lights[i].light_rect.center.x - final_lights[j].light_rect.center.x) 
                               / (final_lights[i].long_edge + final_lights[j].long_edge));

            //judge conditions
            //TODO:
            //use mark to fluentlize this process,make it less sharp

            if (height_ratio > 1.8)
            {
                continue;
            }
            else if (width_ratio > 4)
            {
                continue;
            }
            else if (center_diff_ratio < 2.5)
            {
                continue;
            }
            else if ((relative_x > 5) || (relative_x < 0.2)) //lightlength 5.5  armorbiglength 23.5  armorsmalllength 13.7
            {
                continue;
            }

            Defaker defaker(param.vector_size);
            Armor tmp_armor;
            int armor_num=0;
            //tell left or right
            if (final_lights[i].light_rect.center.x < final_lights[j].light_rect.center.x)
            {
                CreateArmor(final_lights[i], final_lights[j], tmp_armor);
            }
            else
            {
                CreateArmor(final_lights[j], final_lights[i], tmp_armor);
            }
            //continue if there is no number between these two lightbars
            armor_num=defaker.Defake(gamma_img,tmp_armor);
            if (!armor_num)
            {
                continue;
            }
//            tmp_armor.type=armor_num;
            all_armors.push_back(tmp_armor);//after all sift conditions,it proves to be a real armor
           //traverse all pairs
        }//loop 1
    }//loop 2

#ifdef DEBUG

    Mat all_possible_armor = raw_img.clone();
    for (int i = 0; i <  all_armors.size(); i++)
    {
        all_armors[i].DrawArmor(all_possible_armor);
    }
    imshow("all possible armor", all_possible_armor);
    waitKey(1);

#endif

    return (int) all_armors.size();

}



int ArmorDetector::Choose()
{
    int distance=10000000;
    int tmp_distance=9999999;
    int index = -1;
    for(int i=0;i< all_armors.size();i++)
    {
        tmp_distance=pow(all_armors[i].core.x,2)+pow(all_armors[i].core.y,2);
        if(tmp_distance<distance)
        {
            distance=tmp_distance;
            index=i;
        }
    }
    return index;
}



void ArmorDetector::Reset()
{
    light_rect.clear();
    final_lights.clear();
    all_armors.clear();
}

}//namespace hnrm

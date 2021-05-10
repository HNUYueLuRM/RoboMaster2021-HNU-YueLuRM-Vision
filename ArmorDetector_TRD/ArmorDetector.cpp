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

#include "../ArmorDetector_TRD/ArmorDetector.h"
#include "../ArmorDetector_TRD/Armor.hpp"


namespace hnurm
{


ArmorDetector::ArmorDetector()
{
    param;
    erode_kernel=getStructuringElement(MORPH_RECT, Size(param.ele_erode_x, param.ele_erode_y));
    dilate_kernel=getStructuringElement(MORPH_RECT, Size(param.ele_dilated_x, param.ele_dilated_y));
}




void ArmorDetector::Detect(const Mat& raw_frame, Armor& target)
{
    raw_img=raw_frame;
    GetCooked();
    CreateBar();
    SiftBar();
    PairBars();
    target=all_armors[Choose()];
}



//void ArmorDetector::SubtractRB()
//{
//    uchar *pdata = (uchar*) raw_img.data;
//    uchar *qdata = (uchar*) subtract_img.data;
//    int srcData =  raw_img.rows *  raw_img.cols;
//    if (enemycolor == RED)
//    {
//        for (int i = 0; i < srcData; i++)
//        {
//            if (*(pdata + 2) - *pdata > armorParam.color_threshold)
//                *qdata = 255;
//            pdata += 3;
//            qdata++;
//        }
//    }
//    else if (enemyColor == BLUE)
//    {
//        for (int i = 0; i < srcData; i++)
//        {
//            if (*pdata - *(pdata+2) > armorParam.color_threshold)
//                *qdata = 255;
//            pdata += 3;
//            qdata++;
//        }
//    }

//}



void ArmorDetector::GetCooked(void)
{
    SubtractRB();
    dilate( subtract_img,  dilated_img, dilate_kernel);
    erode( dilated_img,  eroded_img, erode_kernel);
    cvtColor(raw_img, brightness_img, cv::ColorConversionCodes::COLOR_BGR2GRAY);
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
     light_contours.clear();
    //light_hierachy.clear();
    findContours(cooked_img,  light_contours, /*light_hierachy,*/ RETR_EXTERNAL , CHAIN_APPROX_SIMPLE, Point(0, 0));
    for (int i = 0; i <  light_contours.size(); i++)
    {
        if ((3 <=  light_contours[i].size()) && ( light_contours[i].size() <= 200))
        {
            temp_rrect = minAreaRect( light_contours[i]);


    //pre-sift lightbars
    /*  due to the fking opencv4 angle system,we gotta do this process:
     *   ————————————————————>  x-axis
     *   |   ________ width
     *   |   |      |           opencv angle system: spin the x axis anti-clockwise,
     *   |   |______| height    when it is parallel to an edge of your RotatedRect,that
     *   |                      edge would be set as width,though another pair would be
     *   |                      height.by doing this,the angle x-axis has passed would be
     *   |                      RotatedRect.angle
     *   v                      so angle would ranged between [0,90)
     *   y-axis
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
    }//loop
}



void ArmorDetector::SiftBar()
{
    final_lights.clear();

   double self_ratio;

   for(int i=0;i< light_rect.size();i++)
   {
       self_ratio= light_rect[i].long_edge/ light_rect[i].short_edge;

       //sift ration
       if((self_ratio<1.5) || (self_ratio>12))
       {
           continue;
       }
       //sift area
       if(( light_rect[i].light_bar.size.area()<5) || ( light_rect[i].light_bar.size.area()>60000))
       {
           continue;
       }

        final_lights.push_back(light_rect[i]);
   }


#ifdef DEBUG

   for (int i = 0; i <  final_lights.size(); i++)
   {
       final_lights[i].DrawLightBar();
   }
   imshow("sifted light rect", raw_img);
   waitKey(1)

#endif
}


void ArmorDetector::CreateArmor(int left_index,int right_index, Armor& dst)
{


}


int ArmorDetector::PairBars()
{
     all_armors.clear();
     all_armors.clear();

    //int mark = 0;

    pair<float,float> height_s,width_s;
    //for each pair of bars;
    double /*angle_diff,*/ angle_diff, center_diff_ratio, relative_x, height_ratio,width_ratio;

    for (int i = 0; i <  final_lights.size(); i++)
    {
        for (int j = i + 1; j <  final_lights.size(); j++)
        {
            //angle_diff = abs(SiftedLightRect[i].angle - SiftedLightRect[j].angle);

            height_s=std::minmax( final_lights[i].long_edge ,  final_lights[j].long_edge);
            height_ratio = height_s.second/height_s.first;

            width_s = std::minmax( final_lights[i].short_edge ,  final_lights[j].short_edge);
            width_ratio = width_s.second / width_s.first;

            //center difference indicated by y-axis diff
            center_diff_ratio=height_s.first/abs( final_lights[i].light_bar.center.y- final_lights[j].light_bar.center.y);

            //the ratio of their len and distance between two centers
            relative_x=abs(2*( final_lights[i].light_bar.center.x- final_lights[j].light_bar.center.x)
                           /( final_lights[i].long_edge+ final_lights[j].long_edge));

            //judge conditions
            //TODO:
            //use mark to fluentlize this process,make it less sharp

            //problems happen here,cause the annoying angle system of opencv!
            //maybe we need four if-elses... that's too complex!
            //probably we gonna delete this judge item
            //    if(angle_diff>40)// ANGLE!!!!
            //    {
            //        continue;
            //    }
            if(height_ratio>1.8)
            {
                continue;
            }
            else if(width_ratio>4)
            {
                continue;
            }
            else if(center_diff_ratio<2.5)
            {
                continue;
            }
            else if( (relative_x > 5) || (relative_x < 0.2) )  //light 5.5  armorbig 23.5  armorsmall 13.7
            {
                //more thing to do here!
                //tell big armors and small armors using relative_x?
                continue;
            }
            Armor tmp_armor;
            //tell left or right
            if ( final_lights[i].light_bar.center.x <  final_lights[j].light_bar.center.x)
            {
                CreateArmor(i, j, tmp_armor);
            }
            else
            {
                CreateArmor(j, i, tmp_armor);
            }
           //traverse all pairs
        }//loop 1
    }//loop 2

#ifdef DEBUG

    Mat AllPossibleArmor_img = raw_img.clone();
    for (int i = 0; i <  all_armors.size(); i++)
    {
        DrawBox(AllPossibleArmor_img,  all_armors[i].vertexes, Scalar(255, 255, 255), 4);
    }
    imshow("AllPossibleArmor_img", AllPossibleArmor_img);
    waitKey(1);

#endif

    return (int) all_armors.size();

}

int ArmorDetector::Choose()
{
    int distance=100000;
    int tmp_distance=99999;
    for(int i=0;i< all_armors.size();i++)
    {
         all_armors[i].GetCenter().x;
    }
}


}//namespace hnrm

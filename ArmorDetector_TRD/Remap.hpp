#pragma once
#include <opencv4/opencv2/opencv.hpp>
#include "Armor.hpp"

using namespace std;
using namespace cv;


namespace hnurm
{
    class Remapor
    {
    public:

        Remapor(Size _armor_size):armor_size(_armor_size){}

        ~Remapor(){}

        void GetSVMImg(Mat& _origin_img,Armor& box,Mat& dst)
        {
            origin_img=_origin_img;
            PreTreat();
            GetTransMat(box);
            Transform();
            dst=remaped_mat;
        }


    private:

        void PreTreat(void)
        {
            cvtColor(origin_img, origin_img, 6);
            threshold(origin_img, origin_img, 10, 255, THRESH_BINARY);
        }


        void GetTransMat(Armor& raw_box)
        {
            Point2f dst_points[4];
            dst_points[0]=Point2f(0,0);
            dst_points[1]=Point2f(0,armor_size.height);
            dst_points[2]=Point2f(armor_size.width,0);
            dst_points[3]=Point2f(armor_size.width,armor_size.height);
            transform_mat=getPerspectiveTransform(raw_box.vertexes, dst_points);
        }


        void Transform(void)
        {
            warpPerspective(origin_img,remaped_mat,transform_mat,armor_size,INTER_NEAREST,BORDER_CONSTANT,Scalar(0));
        }


    private:
        Size armor_size;
        Mat origin_img;
        Mat transform_mat;
        Mat remaped_mat;

    };

}//hnurm

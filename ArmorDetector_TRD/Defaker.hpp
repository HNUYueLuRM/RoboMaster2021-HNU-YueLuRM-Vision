#pragma once
#include <opencv4/opencv2/opencv.hpp>
#include "Armor.hpp"

using namespace std;
using namespace cv;


namespace hnurm
{
    class Defaker 
    {
    public:
        
        Defaker(Size _vector_size):vector_size(_vector_size),svm(ml::SVM::create()))
        {
            svm->load("/trained SVM model path");
        }



        ~Defaker()=default;



        int Defake(Mat& _origin_img,Armor& box)
        {
            origin_img=_origin_img;

            PreTreat();
            GetTransMat(box);
            Transform();
            GetTrait();
            return Match();
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
            dst_points[1]=Point2f(0,vector_size.height);
            dst_points[2]=Point2f(vector_size.width,0);
            dst_points[3]=Point2f(vector_size.width,vector_size.height);
            transform_mat=getPerspectiveTransform(raw_box.vertexes, dst_points);
        }



        void Transform(void)
        {
            warpPerspective(origin_img,remaped_mat,transform_mat,vector_size,INTER_NEAREST,BORDER_CONSTANT,Scalar(0));
        }



        void GetTrait(void)
        {
            trait_vector = remaped_mat.reshape(1, 1);
            trait_vector.convertTo(trait_vector, CV_32FC1);
        }



        int Match(void)
        {
            return (int)svm->predict(trait_vector);
        }

    private:

        unique_ptr<ml::SVM> svm;

    private:

        Size vector_size;
        Mat origin_img;
        Mat transform_mat;
        Mat remaped_mat;
        Mat trait_vector;
    };

}//hnurm

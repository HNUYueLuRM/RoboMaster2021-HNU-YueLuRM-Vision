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

        /**
         * @brief Construct a new Defaker object,load svm;
         * 
         * @param _vector_size this is the size of trait vector
         */
        Defaker(Size _vector_size):vector_size(_vector_size),svm(ml::SVM::create())
        {
            svm->load("/trained SVM model path");
        }

        /**
         * @brief Destroy the Defaker object
         */
        ~Defaker()=default;

        /**
         * @brief public API of Defaker,cut ROI of possible armor then use SVM 
         *        to tell whether it's an real armor
         * @param _origin_img raw frame get from camera
         * @param box possible armor
         * @return int the number pattern in the armor
         */
        int Defake(Mat& _origin_img,Armor& box)
        {
            origin_img=_origin_img;
            GetTransMat(box);
            Transform();
            PreTreat();
            GetTrait();
            return Match();
        }

    private:

        /**
         * @brief Get tranform mat via cv::getPerspectiveTransform()
         * @param raw_box an Armor object
         */
        void GetTransMat(Armor& raw_box)
        {
            Point2f dst_points[4];
            dst_points[0]=Point2f(0,0);
            dst_points[1]=Point2f(0,vector_size.height);
            dst_points[2]=Point2f(vector_size.width,0);
            dst_points[3]=Point2f(vector_size.width,vector_size.height);
            transform_mat=getPerspectiveTransform(raw_box.vertexes, dst_points);
        }


        /**
         * @brief use transform_mat to multiply the origin img
         *        to get a vector_size img
         * @param private member
         */
        void Transform(void)
        {
            warpPerspective(origin_img,remaped_mat,transform_mat,vector_size,INTER_NEAREST,BORDER_CONSTANT,Scalar(0));
        }
    
        /**
         * @brief a gamma redress function
         * @param private_member
         */
        void GammaRedress(void)
        {
            unsigned char lut[256];
            for (int i = 0; i < 256; i++)
            {
                lut[i] = saturate_cast<uchar>(pow((float)i / 255.0, param.gamma_redress_value) * 255.0f);
            }
            gamma_img = remaped_mat.clone();
            int channels = remaped_mat.channels();
            switch (channels)
            {
            case 1:
            {
                MatIterator_<uchar> it = gamma_img.begin<uchar>();
                MatIterator_<uchar> end = gamma_img.end<uchar>();
                while (it != end)
                {
                    *it = lut[(*it)];
                    it++;
                }
                break;
            }
            case 3:
            {
                MatIterator_<Vec3b> it = gamma_img.begin<Vec3b>();
                MatIterator_<Vec3b> end = gamma_img.end<Vec3b>();
                while (it != end)
                {
                    (*it)[0] = lut[(*it)[0]];
                    (*it)[1] = lut[(*it)[1]];
                    (*it)[2] = lut[(*it)[2]];
                    it++;
                }
                break;
            }
            default:
                break;
            }
        }
        
        /**
         * @brief use gamma redress and cvtColor to get bi-value ima
         *        for svm->predicat()
         */
        void PreTreat(void)
        {
            GammaRedress();
            cvtColor(gamma_img, gamma_img, 6);
            threshold(gamma_img, gamma_img, 10, 255, THRESH_BINARY);
        }

        /**
         * @brief Get the Trait vector via resize
         * @param private_member 
         */
        void GetTrait(void)
        {
            trait_vector = gamma_img.reshape(1, 1);
            trait_vector.convertTo(trait_vector, CV_32FC1);
        }

        /**
         * @brief use trained svm to match trait vector
         * 
         * @return int  number in the armor
         */
        int Match(void)
        {
            return (int)svm->predict(trait_vector);
        }

    private:

        unique_ptr<ml::SVM> svm;

    private:

        Size vector_size;  //size of trait vetor
        Mat origin_img;    //origin img
        Mat transform_mat; //Mat for transformation
        Mat remaped_mat;   //Mat remaped
        Mat gamma_img;     //img after gamma redress
        Mat trait_vector;  //trait vetor
    };

}//hnurm

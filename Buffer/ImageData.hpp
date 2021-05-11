/*kiko@idiospace.com 2020.01*/
/*
 *用于确定一帧图像的类
*/

#include<opencv2/opencv.hpp>


#ifndef IMAGEDATA_HPP
#define IMAGEDATA_HPP

namespace hnurm {

class ImageData
{
private:

typedef unsigned char uint8_t;

public:
    uint8_t camera_id;  // this data will be sent back to STM32 with calc result
    cv::Mat mat;

public:
    ImageData():
        camera_id(0)
    {}
    ImageData(int camera_id_, const cv::Mat& mat_):
        camera_id(camera_id_&0xf),
        mat(mat_)
    {}
    ImageData(const int& camera_id_, const cv::Mat& mat_):
        camera_id(camera_id_&0xff),
        mat(mat_)
    {}
    ImageData(const int& camera_id_):
        camera_id(camera_id_)
    {}
    ImageData(const uint8_t& camera_id_, const cv::Mat& mat_):
        camera_id(camera_id_),
        mat(mat_)
    {

    }
    void set(const uint8_t& camera_id_, const cv::Mat& mat_)
    {
        camera_id = camera_id_;
        mat = mat_;
    }

};

}

#endif IMAGEDATA_H


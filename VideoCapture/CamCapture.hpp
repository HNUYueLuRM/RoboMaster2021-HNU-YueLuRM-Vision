#pragma once
#define DEBUG
//#define RELEASE
#include <iostream>
#include "opencv2/opencv.hpp"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <queue>
#include <thread>
#include <exception>
#include "../Buffer/ImageData.hpp"
#include "../Buffer/DataBuf.hpp"

using namespace std;
using namespace cv;

namespace hnurm 
{
	class CamCapture
	{
	private:
        int camera_id = 0;//相机id
		Mat frame;
        int PROP_BRIGHTNESS=0;//0-1
        unique_ptr<VideoCapture> mycap;

	public:
		CamCapture(){}
		
        void init(int camera_id_, float PROP_BRIGHTNESS_=0)
		{
			camera_id = camera_id_;
			PROP_BRIGHTNESS = PROP_BRIGHTNESS_;
            mycap = make_unique<VideoCapture>(camera_id_);
		}

        bool get_frame(Wrapped<ImageData>& tmp)
		{
		    mycap->set(6, cv::VideoWriter::fourcc('M','J','P','G'));
            try
            {

                if (!mycap->isOpened())
                {
                    throw "cam failed to open.Check your cam id.";
                }
                mycap->set(CAP_PROP_BRIGHTNESS, PROP_BRIGHTNESS);
                cv::Mat temp_frame;
                (*mycap)>>temp_frame;
                temp_frame.convertTo(tmp.raw_data.mat, temp_frame.type(), 1, 30);
                //(*mycap) >> tmp.raw_data.mat;

                tmp.raw_data.camera_id=camera_id;

                return true;
            }
            catch (char*&e)
            {
                std::cerr << "try catch test" << endl;
            }
            return false;
        }

};
}



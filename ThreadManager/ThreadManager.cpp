#pragma once
#include"ThreadManager.h"
#include<iostream>
#include<exception>
#include<memory>


namespace hnurm
{

ThreadManager::ThreadManager():
    serial_databuf_ptr(make_unique<DataBuf<Wrapped<SerialData>>>()),
    image_databuf_ptr(make_unique< DataBuf<Wrapped<ImageData>>>()),
    taskmode_buff_ptr(make_unique< DataBuf<Wrapped<Protocol::Work_mode>>>()),
    _serial_ptr(make_unique<SerialCodec>()),
    task_switcher_ptr(make_unique<Switcher>()),
    video_capture_ptr(make_unique<CamCapture>())
    {}

void ThreadManager::InitAll()
{
    video_capture_ptr->init(0);


    if(!_serial_ptr->open_port("/dev/ttyUSB0"))
    {
        cout<<"can't open port"<<endl;

    }
    //_serial_ptr->rm_init();
	//_serial_ptr->set_color();
}


void ThreadManager::GenerateThread()
{

//    for(;;)
//    {
//        Wrapped<ImageData> temp;
//        std::cout << "Generate Thread : Raw image is reading..." << std::endl;
//        video_capture_ptr->get_frame(temp);
//        image_databuf_ptr->update(temp);
//        std::cout << "Generate Thread : Raw image has been updated..." << std::endl;
//        std::this_thread::sleep_for(std::chrono::milliseconds(10));
//    }


//    Mat framet;
    //VideoCapture capture("/home/robomaster/algorithm_testing.avi");
    while(1)
    {
        Wrapped<ImageData> tmp;

        //Mat frame;
        //capture >> frame;
        //tmp.raw_data.mat = frame;
        //tmp.raw_data.camera_id = 0;
        video_capture_ptr->get_frame(tmp);
        image_databuf_ptr->update(tmp);
    }
}



void ThreadManager::ProcessThread()
{


    for(;;)
    {
        Wrapped<Protocol::Work_mode> tmp_mode;

        Wrapped<SerialData> tmp_solved_data;
        Wrapped<ImageData> tmp_img;
        if(!taskmode_buff_ptr->get(tmp_mode))continue;
        if(!image_databuf_ptr->get(tmp_img))continue;
        task_switcher_ptr->RunSwitcher(tmp_mode.raw_data, tmp_img, tmp_solved_data);
//        if(tmp_solved_data.raw_data.yaw == 0 && tmp_solved_data.raw_data.pitch == 0 && tmp_solved_data.raw_data.distance == 0)continue;
        serial_databuf_ptr->update(tmp_solved_data);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

}


void ThreadManager::ReceiveThread()
{
//	Protocol::Vision_recv_data temp_data(Protocol::Self_color::none, -1, Protocol::Work_mode::manual, Protocol::Tracker_mode::off);
//	int dur = 20;   //接收限制时长
//	for (;;)
//	{
//		if (_serial_ptr->try_get_recv_data_for(temp_data, dur))
//		{
//			Wrapped<Protocol::Work_mode> _work_mode = temp_data.mode;
    while(1)
            {


            taskmode_buff_ptr->update(Protocol::Work_mode::auto_grab
                                      );

        //std::cout << "Receive Thread : Task mode is receiving" << std::endl;

        //std::cout << "Receive Thread : Task mode has been updated" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
//		}
//		else
//		{
//			std::cout << "fail to update the taskmode buffer" << std::endl;
//		}
//	}
}


void ThreadManager::SendThread()
{
    Wrapped<SerialData> tmp;
    for(;;)
    {

        if(!serial_databuf_ptr->get(tmp))continue;
        _serial_ptr->send_data(tmp.raw_data.camera_id,tmp.raw_data.pitch,tmp.raw_data.yaw,0);
        std::cout<<tmp.raw_data.yaw<<" "<<tmp.raw_data.pitch<<" "<<tmp.raw_data.distance<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

}


}

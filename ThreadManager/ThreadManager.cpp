#pragma once
#include"../ThreadManager/ThreadManager.h"
#include<iostream>
#include<exception>
#include<memory>


namespace hnurm
{

ThreadManager::ThreadManager():
    serial_databuf_ptr(make_unique<DataBuf<Wrapped<SerialData>>>()),
    image_databuf_ptr(make_unique< DataBuf<Wrapped<ImageData>>>()),
    se_databuf_ptr(make_unique< DataBuf<Wrapped<SolveEssential>>>()),
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
    Mat framet;
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
        Wrapped<SolveEssential> temp_se;
        Wrapped<SerialData> tmp_solved_data;
        Wrapped<ImageData> tmp_img;
        if(!se_databuf_ptr->get(temp_se))continue;
        if(!image_databuf_ptr->get(tmp_img))continue;
        task_switcher_ptr->RunSwitcher(temp_se, tmp_img, tmp_solved_data);
        serial_databuf_ptr->update(tmp_solved_data);
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}


void ThreadManager::ReceiveThread()
{
	float temp_angle_data=0;
    Protocol::Vision_recv_data temp_data(Protocol::Self_color::none, -1, Protocol::Work_mode::manual, Protocol::Bullet_speed::infantry15);
	int dur = 20;   //接收限制时长
	for (;;)
	{
        Wrapped<SolveEssential> temp_se_data;
//		if (_serial_ptr->try_get_recv_data_for(temp_data,temp_angle_data, dur))
//        {
//		    temp_se_data.raw_data.abs_angle=temp_angle_data;
//		    temp_se_data.raw_data.work_mode=temp_data.mode;
//		    temp_se_data.raw_data.bullet_speed=temp_data.speed;
//            se_databuf_ptr->update(temp_se_data);
//		}
//		else
//		{
//			std::cout << "fail to update the taskmode buffer" << std::endl;
//		}
temp_se_data.raw_data.work_mode=Protocol::Work_mode::auto_shoot;
        se_databuf_ptr->update(temp_se_data);
	}
}


void ThreadManager::SendThread()
{

    for(;;)
    {
        Wrapped<SerialData> tmp;
        if(!serial_databuf_ptr->get(tmp))continue;
        _serial_ptr->send_data(tmp.raw_data.camera_id,tmp.raw_data.pitch,tmp.raw_data.yaw,0);
        std::cout<<tmp.raw_data.yaw<<" "<<tmp.raw_data.pitch<<" "<<tmp.raw_data.distance<<std::endl;
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

}


}

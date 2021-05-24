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
    task_switcher_ptr(make_unique<TaskSwitcher>()),
    video_capture_ptr(make_unique<CamCapture>())
    {}



void ThreadManager::InitAll()
{
    video_capture_ptr->init(0);
    if(!_serial_ptr->open_port("/dev/ttyUSB0"))
    {
        cout<<"can't open port"<<endl;
    }
    _serial_ptr->rm_init();
	_serial_ptr->set_color();
    task_switcher_ptr->SetEnemyColor(_serial_ptr->get_color());
}



void ThreadManager::GenerateThread()
{
    Mat frame;
    while(1)
    {
        Wrapped<ImageData> tmp;
        tmp.raw_data.camera_id = 0;
        if(video_capture_ptr->get_frame(tmp))
        {
            image_databuf_ptr->update(tmp);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}



void ThreadManager::ProcessThread()
{
    while(1)
    {
        Wrapped<SerialData> tmp_solved_data;
        Wrapped<ImageData> tmp_img;
        Wrapped<SolveEssential> temp_se;
        temp_se.raw_data.work_mode=Protocol::Work_mode::auto_shoot;
        if(!image_databuf_ptr->get(tmp_img))
        {
            continue;
        }
        task_switcher_ptr->RunSwitcher(temp_se, tmp_img, tmp_solved_data);
        serial_databuf_ptr->update(tmp_solved_data);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}



void ThreadManager::ReceiveThread()
{
	float temp_angle_data=0;
    Protocol::Vision_recv_data temp_data;
	int dur = 20;  
	while(1)
	{
        Wrapped<SolveEssential> temp_se_data;
		if (_serial_ptr->try_get_recv_data_for(temp_data,temp_angle_data, dur))
        {
		    temp_se_data.raw_data.abs_angle=temp_angle_data;
            se_databuf_ptr->update(temp_se_data);
		}
        se_databuf_ptr->update(temp_se_data);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}



void ThreadManager::SendThread()
{
    while(1)
    {
        Wrapped<SerialData> tmp;
        if(!serial_databuf_ptr->get(tmp))
        {
            continue;
        }
        _serial_ptr->send_data(tmp.raw_data.camera_id,tmp.raw_data.pitch,tmp.raw_data.yaw,tmp.raw_data.distance);
        //std::cout<<tmp.raw_data.yaw<<" "<<tmp.raw_data.pitch<<" "<<tmp.raw_data.distance<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

}

}//hnurm

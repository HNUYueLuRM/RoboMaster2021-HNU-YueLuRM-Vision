#pragma once
#include"../Serial/serialcodec.h"
#include"../Buffer/DataBuf.hpp"
#include"../Buffer/SerialData.hpp"
#include"../Buffer/ImageData.hpp"
#include"../TaskSwitcher/TaskSwitcher.h"
#include"../CamCapture/CamCapture.hpp"
#include"../Protocol/protocol.h"
#include"../Buffer/SolveEssential.h"


#include<iostream>
#include<memory>

namespace hnurm
{

class ThreadManager
{

public:
	ThreadManager();

    ~ThreadManager(){}

	void InitAll();

	void GenerateThread();

	void ProcessThread();

	void ReceiveThread();

    void SendThread();


private:
	//lots of pointers refer to all modules
	std::unique_ptr < DataBuf<Wrapped<SerialData>>> serial_databuf_ptr;

	std::unique_ptr < DataBuf<Wrapped<ImageData>>> image_databuf_ptr;

	std::unique_ptr	< DataBuf<Wrapped<SolveEssential>>> se_databuf_ptr;

    std::unique_ptr<SerialCodec> _serial_ptr;

    std::unique_ptr<TaskSwitcher> task_switcher_ptr;

    std::unique_ptr<CamCapture> video_capture_ptr;

};

}


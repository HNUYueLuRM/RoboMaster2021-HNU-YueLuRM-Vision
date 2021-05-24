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
	/**
	 * @brief Construct a new Thread Manager object,init all ptrs
	 */
	ThreadManager();

	/**
	 * @brief Destroy the Thread Manager object
	 */
    ~ThreadManager(){}

	/**
	 * @brief Init all task,serial/taskswicher/camera
	 */
	void InitAll();

	/**
	 * @brief thread where camera generate raw frame
	 */
	void GenerateThread();

	/**
	 * @brief process raw frame and solve them,save solved data to send buffer
	 */
	void ProcessThread();

	/**
	 * @brief receive color/angle and other info from stm32
	 */
	void ReceiveThread();

	/**
	 * @brief send data from buffer to serial(STM32)
	 */
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


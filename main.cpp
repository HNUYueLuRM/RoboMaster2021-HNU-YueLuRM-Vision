#include"ThreadManager/ThreadManager.h"
#include<thread>
#include<iostream>
int main()
{
    hnurm::ThreadManager _thread_manager;

    _thread_manager.InitAll();

    std::thread generate(&hnurm::ThreadManager::GenerateThread,std::ref(_thread_manager));
    std::thread process(&hnurm::ThreadManager::ProcessThread, std::ref(_thread_manager));
    std::thread send(&hnurm::ThreadManager::SendThread,std::ref(_thread_manager));
    //std::thread receive(&hnurm::ThreadManager::ReceiveThread, std::ref(_thread_manager));

    generate.join();
    process.join();
    //receive.join();
    send.join();

    return 0;
}



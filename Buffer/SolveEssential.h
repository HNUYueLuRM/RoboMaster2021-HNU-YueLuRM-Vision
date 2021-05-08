//
// Created by rm on 2021/4/24.
//
#pragma once
#include "../Protocol/protocol.h"


namespace hnurm
{

    class SolveEssential
    {
    public:
        SolveEssential():work_mode(Protocol::Work_mode::manual)
        {
        }

        SolveEssential(Protocol::Work_mode mode_,Protocol::Bullet_speed speed_,float abs_angle_):
                        work_mode(mode_),bullet_speed(speed_),abs_angle(abs_angle)
        {
        }

        ~SolveEssential()=default;

    public:
        Protocol::Work_mode work_mode;
        Protocol::Bullet_speed bullet_speed;
        float abs_angle;
    };

}

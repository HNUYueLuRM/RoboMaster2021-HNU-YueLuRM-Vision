#pragma once
#include <iostream>
#include <thread>
#include "../ArmorTracker/Armor_tracker.h"
#include "../Buffer/DataBuf.hpp"
#include "../Buffer/ImageData.hpp"
#include "../Buffer/SerialData.hpp"
#include "../AngleSolver/AngleSolver.h"
#include "../ArmorDetector_CNN/ArmorDetector.h"
#include "../Protocol/protocol.h"
#include "../Buffer/SolveEssential.h"
#include "../Protocol/protocol.h"

#define MAX_LOST_COUNT 5

namespace hnurm
{

class Switcher
{
public:
    Switcher();

    ~Switcher() = default;

    void RunSwitcher(Wrapped<SolveEssential> &tmp_se, Wrapped<ImageData> &raw_image, Wrapped<SerialData> &solved_data);

    void SetEnemyColor(Protocol::Self_color mycolor);

private:

    void GetPatchData(void);

    std::unique_ptr<Armor_tracker> _Armor_tracker;
    std::unique_ptr<AngleSolver> _Angle_solver;
    std::unique_ptr<ArmorDetector> _Armor_Detector;
    Protocol::Self_color enemy_color;

    int lost_count;
    SerialData makeshift_data;
	};

}

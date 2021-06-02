#include <iostream>
#include <thread>
#include "../ArmorTracker/Armor_tracker.h"
#include "../Buffer/DataBuf.hpp"
#include "../Buffer/ImageData.hpp"
#include "../Buffer/SerialData.hpp"
#include "../AngleSolver/PoseCalculation.h"
#include "../ArmorDetector/ArmorDetector.h"
#include "../Protocol/protocol.h"
#include "../QRDetector/QRDetector.h"

namespace hnurm
{

class Switcher
{
public:
    Switcher();
    ~Switcher() = default;
    void RunSwitcher(Protocol::Work_mode task_mode, Wrapped<ImageData> &raw_image, Wrapped<SerialData> &solved_data);

private:
    //int _Work_mode;
    std::unique_ptr<Armor_tracker> _Armor_tracker;
    std::unique_ptr<AngleSolver> _Angle_solver;
    std::unique_ptr<ArmorDetector> _Armor_Detector;
    std::unique_ptr<QRdetector> _QR_Detector;
    int frame_count;
    //ArmorInfo Armors;
    //ArmorInfo Armor_info;
    //DataBuf<Wrapped<ImageData>> _raw_img_data_buffer;

    //Wrapped<SerialData> _solved_data;

    //DataBuf<Wrapped<SerialData>> _Serial_data_buffer;
    //Wrapped<ImageData> _raw_img_data;//raw img from camera
    //Wrapped<SerialData> _solved_Serial_data;//solved data to stm32

	};
}

#include "Task_switcher.h"


namespace hnurm
{
    Switcher::Switcher()
        : _Armor_tracker(make_unique<Armor_tracker>()),
          _Angle_solver(make_unique<AngleSolver>()),
          _Armor_Detector(make_unique<ArmorDetector>()),
          _QR_Detector(make_unique<QRdetector>())
     {
           frame_count = 0;
     }

    void Switcher::RunSwitcher(Protocol::Work_mode task_mode, Wrapped<ImageData> &raw_image, Wrapped<SerialData> &solved_data)
    {

        cv::Mat current_frame = raw_image.raw_data.mat;
        cv::Rect target_rect;
        cv::Point2f points[4];
        //Armor's information
        ArmorInfo Armors;
        Armors.color = BLUE;
        Armors.size = BIG;      //输入式参数
        QRData qrData;
        //four points
        cv::Point2d left_top, right_top, right_down, left_down;
        //returned solved data
        //Wrapped<SerialData> _solved_data;
        //RotatedRect tmp_rRect;
        cv::Rect temp_rect;
        cv::Point2f tmp_points[4];
        _Angle_solver->init();

        switch (task_mode) {
            case Protocol::Work_mode::manual:	//manual mode, just a example
                std::cout << "Work mode: Manual" << std::endl;
                break;
            case Protocol::Work_mode::auto_shoot:	//Auto Shoot
                std::cout << "Work mode: Auto Shoot" << std::endl;
                break;
            case Protocol::Work_mode::auto_tracker:	//Tracker
                std::cout << "Work mode: Track" << std::endl;
                break;
            case Protocol::Work_mode::auto_grab:	//Grab
                std::cout << "Work mode: Grab" << std::endl;
                break;
            default:
                std::cout << "Unknown Mode" << std::endl;
                break;
            }
            //Auto shoot
            if (task_mode == Protocol::Work_mode::auto_shoot) {
                frame_count = 0;
                auto t = (double)cv::getTickCount();
                //...Armor_detector and Armor_solver
                if(_Armor_Detector->Detect(current_frame, temp_rect)){
                    target_rect = temp_rect;
                    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
                    auto fps = 1.0 / t;
                    std::cout << fps << std::endl;
                }
                else {
                    imshow("test",current_frame);
                    waitKey(10);
                    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
                    auto fps = 1.0 / t;
                    std::cout << fps << std::endl;
                    return;
                }
                //namedWindow("test");
                  rectangle(current_frame, target_rect, Scalar(0,255,255), 2);

                imshow("test",current_frame);
                waitKey(1);
                left_top = target_rect.tl();
                right_down = target_rect.br();
                right_top.y = left_top.y;
                right_top.x = left_top.x + target_rect.width;
                left_down.y = right_down.y;
                left_down.x = right_down.x - target_rect.width;
                Armors.object_points.push_back(left_top);
                Armors.object_points.push_back(right_top);
                Armors.object_points.push_back(right_down);
                Armors.object_points.push_back(left_down);
                SerialData result=_Angle_solver->solve(Armors);

                //set solved data
                solved_data.raw_data.set(raw_image.raw_data.camera_id,
                                         result.yaw,
                                         result.pitch,
                                         result.distance);
                return ;
            }
            else if (task_mode == Protocol::Work_mode::manual){
                return ;
            }
            // Armor Tracker
            else if (task_mode == Protocol::Work_mode::auto_tracker) {
                //Armor_tracker
                cv::Rect first_rect;			//the rect which is got from Armor_detector in first frame
                cv::Point2d target_point;
                //RotatedRect tmp;
                Rect tmp;
                if (frame_count == 0) {
                    if(!_Armor_Detector->Detect(current_frame, tmp)){
                        std::cout << "Armor Tracker failed!" << std::endl;
                        imshow("test", current_frame);
                        waitKey(1);
                        return ;
                    }
                    first_rect = tmp;
                    _Armor_tracker->init_tracker(first_rect, current_frame);
                    frame_count++;
                }
                else {
                    target_rect = _Armor_tracker->Update(current_frame);
                    rectangle(current_frame, target_rect, Scalar(255,255,255), 2);
                    imshow("test", current_frame);
                    waitKey(1);
                    //calculate the four points
                    left_top = target_rect.tl();
                    right_down = target_rect.br();
                    right_top.y = left_top.y;
                    right_top.x = left_top.x + target_rect.width;
                    left_down.y = right_down.y;
                    left_down.x = right_down.x - target_rect.width;

                    //solve points
                    Armors.object_points.push_back(left_top);
                    Armors.object_points.push_back(right_top);
                    Armors.object_points.push_back(right_down);
                    Armors.object_points.push_back(left_down);
                    SerialData result=_Angle_solver->solve(Armors);

                    //set solved_data
                    solved_data.raw_data.set(raw_image.raw_data.camera_id,
                                             result.yaw,
                                             result.pitch,
                                             result.distance);
                    return ;
                }
            }
            else if (task_mode == Protocol::Work_mode::auto_grab)
            {
                frame_count = 0;
                auto t = (double)cv::getTickCount();
                if(_QR_Detector->QR_detect(current_frame,tmp_points))
                {
                    for(int i=0;i<4;i++)
                        points[i]=tmp_points[i];
                    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
                    auto fps = 1.0 / t;
                    std::cout << fps << std::endl;
                }
                else {
                    imshow("test",current_frame);
                    waitKey(10);
                    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
                    auto fps = 1.0 / t;
                    std::cout << fps << std::endl;
                    return;
                }
                for(int i=0;i<4;i++)
                    qrData.boxpoint.push_back(points[i]);
                SerialData result=_Angle_solver->pnpsolve(qrData);

                solved_data.raw_data.set(raw_image.raw_data.camera_id,
                                         result.yaw,
                                         result.pitch,
                                         result.distance);
                return ;
            }

    }
}

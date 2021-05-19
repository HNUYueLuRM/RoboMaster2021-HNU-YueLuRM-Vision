#include "WindmillSolver.h"
#include <cmath>
using namespace cv;

#define DEBUG
#ifdef DEBUG
#include <iostream>
#endif // DEBUG


namespace hnurm {

    // 二值化的阈值
    static int thresh_lower{ 80 };
    static int thresh_upper{ 255 };

    // 进行二值化时的回调函数
    static void thresh_track(int, void* user_data)
    {
        try {
            cv::Mat img = *((cv::Mat*)(user_data));
            cv::Mat dst = img.clone();
            cv::imshow("img in the trace bar", img);
            cv::threshold(img, dst, thresh_lower, thresh_upper, cv::ThresholdTypes::THRESH_BINARY);        //阈值要自己调

#ifdef DEBUG
            cv::imshow("threshold_img", dst);
            cv::waitKey(0);
#endif // DEBUG

        }
        catch (const char* msg)
        {
            std::cout << "[ERROR] : " << msg << std::endl;
        }
    }

    Windmill_solver::Windmill_solver(const TeamColor& own_color):
        _own_color(own_color),
        _enemy_color(static_cast<TeamColor>(!own_color))
    {

    }

    PixelPoint2d hnurm::Windmill_solver::detect_armor_center(const cv::Mat& mat)
    {
        std::vector<PixelPoint2d> re = detect_armor_cornor_and_center(mat);
        return re[0];
    }

    //---------------------------------------------------------------------------------------------------------
    // 此函数用于识别风车旋翼
    //---------------------------------------------------------------------------------------------------------
    std::vector<PixelPoint2d> Windmill_solver::detect_armor_cornor_and_center(const cv::Mat& img)
    {
        cv::Mat image = img.clone(), binary;
        image.copyTo(binary);

        std::vector <PixelPoint2d> return_result;

        // 将图缩放到目标尺寸 640 * 480
        int dst_width = 640;
        int dst_height = 480;
        //---------------------------------------------------------------------------------------------------------------------
        //更新图象尺寸
        //---------------------------------------------------------------------------------------------------------------------
        resolution_height= dst_height;//当前图象的高度和宽度
        resolution_width=dst_width;

        // 计算缩放因子，分别计算宽高方向的缩放比例
        // 后面计算实际位置，使用位姿解算模块的的时候需要用到
        int factor_width = dst_width / image.cols;
        int factor_height = dst_height / image.rows;


        cv::resize(image, image, cv::Size(640, 480));
        cv::resize(binary, binary, cv::Size(640, 480));



        // 转为灰度图像
        std::vector <cv::Mat> imgChannels;

//        // 对图像进行膨胀操作，填补受损的像素
        cv::Mat elementRect;
        elementRect = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4), cv::Point(-1, -1));
        cv::dilate(image, image, elementRect);

//        // 将 BGR 图像的三个通道分别分到三个 channel 中
          cv::split(image, imgChannels);
//        
//        // 通道相减：这是一种常见的思路，很多博客和开源方案中都提到
//        // 但是测试效果不好，没法用，可能是因为只有图片的原因？
//        // mid_image = imgChannels.at(2) - imgChannels.at(0);
//
#ifdef DEBUG        
        cv::imshow("blue channel", imgChannels.at(0));
        //cv::imshow("green channel", imgChannels.at(1));
       // cv::imshow("red channel", imgChannels.at(2));
#endif


        cv::Mat mid_image;
        if (_own_color == TeamColor::RED)
        {
            mid_image = imgChannels.at(0);//敌方风车为蓝色
            //mid_image = imgChannels.at(0) - imgChannels.at(2);  // 图片白光影响较大，暂时没法使用这种方法
        }
        else {
            mid_image = imgChannels.at(2);//敌方风车为红色
            //mid_image = imgChannels.at(2) - imgChannels.at(0);
        }

         //cv::imshow("original_mid_image", mid_image);

         int thresh = 100;  // 设置阈值为 100
         cv::threshold(mid_image, image, thresh, 255, cv::ThresholdTypes::THRESH_BINARY);        // 这里后期要改成从文件读
#ifdef DEBUG
         cv::imshow("thresholded_img", image);
#endif // DEBUG

        // 漫水法填充
        floodFill(image, cv::Point(0, 0), cv::Scalar(255), 0, cv::FLOODFILL_FIXED_RANGE);
#ifdef DEBUG
        cv::imshow("image filled", image);

#endif // DEBUG


        std::vector<std::vector<cv::Point>> contours;

        findContours(image, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);


        for (size_t i = 0; i < contours.size(); i++) {
            double area = contourArea(contours[i]);  // 计算区域的面积
            if (area < 50 || 1e4 < area) continue;   // 筛选面积，不满足范围就跳过这个区域

            // 在原图上画出轮廓
            cv::drawContours(image, contours, static_cast<int>(i), cv::Scalar(0), 2);   

            std::vector<cv::Point> points_1 = contours[i];
            cv::RotatedRect rrect = cv::fitEllipse(points_1);
            cv::Point2f* vertices = new cv::Point2f[4];
            rrect.points(vertices);


            float aim = rrect.size.height / rrect.size.width;  // 宽高比，筛选检测出的旋转的矩形
            if (aim > 1.7 && aim < 2.6) {                      // 限制宽高比在 1.7 到 2.6 之间
                for (int j = 0; j < 4; j++)                    // 找到满足要求的矩形的四个边界点并划线，使用绿色圈出
                {
                    return_result.push_back(PixelPoint2d(vertices[j].x, vertices[j].y));
                    cv::line(binary, vertices[j], vertices[(j + 1) % 4], cv::Scalar(0, 255, 0), 4);//画矩形框
                }
                delete[] vertices;
                float middle = 100000;

                for (size_t j = 1; j < contours.size(); j++) {
                    double area = contourArea(contours[j]);
                    if (area < 50 || 1e4 < area) continue;

                    std::vector<cv::Point> points_2 = contours[j];
                    cv::RotatedRect rrect_2 = fitEllipse(points_2);

                    float aim_2 = rrect_2.size.height / rrect_2.size.width;
                    //猜测：两种矩形的比较？，求这两种矩形中心的距离，并赋值给middle
                    if (aim_2 > 3.0) {
                        float distance = sqrt((rrect.center.x - rrect_2.center.x) * (rrect.center.x - rrect_2.center.x) +
                            (rrect.center.y - rrect_2.center.y) * (rrect.center.y - rrect_2.center.y));

                        if (middle > distance)
                            middle = distance;
                    }
                }
                if (middle > 60) {//  这个距离也要根据实际情况调,和图像尺寸和物体远近有关。
                    //也就是说当一个框框和其他的红矩形距离都超过60时,此时该框框的悬臂就是<<<了。
					//有一说一，这种识别方式太随机了，随便切换一下尺寸就不行了
					//最简单的方法是把60用一个和大风车有关的相对数据来替代，这样稳定性会高一些；进阶方法就是用SVM了。    
                    armor_point = rrect.center;//更新装甲板中心坐标
                    cv::circle(binary, cv::Point(rrect.center.x, rrect.center.y), 15, cv::Scalar(0, 0, 255), 4);//红色圈出流水灯，即需要击打的旋翼

                    cv::circle(binary, circle_center_point, 6, cv::Scalar(0, 255, 255), 2);//R区域

                }
            }
        }

        std::vector<std::vector<cv::Point>>().swap(contours);

#ifdef DEBUG
        imshow("识别结果", binary);
        //imshow("Original", image);

#endif       
        return return_result;
    }



    //---------------------------------------------------------------------------------------------------------
    // 此函数用于识别风车中心R字符
    //---------------------------------------------------------------------------------------------------------
    PixelPoint2d Windmill_solver::detect_windmill_center_r_icon(const cv::Mat& img) {
        
        PixelPoint2d return_result = PixelPoint2d(resolution_width / 2, resolution_height / 2);
        
        cv::Mat image = img.clone(), binary;
        //图象缩放
        cv::resize(image, image, cv::Size(640, 480));
        image.copyTo(binary);


        //处理图象: 灰度--》腐蚀、膨胀--》findcountours寻找R区域
       // 转为灰度图像
        std::vector <cv::Mat> imgChannels_;

        // 对图像进行膨胀操作，填补受损的像素
        cv::Mat elementRect;
        elementRect = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4), cv::Point(-1, -1));
        cv::dilate(image, image, elementRect);

        cv::Mat tmp_img;
        // 将 BGR 图像的三个通道分别分到三个 channel 中
        cv::split(image, imgChannels_);
        if (_own_color == TeamColor::RED)
        {
            //tmp_img = imgChannels_.at(0);//敌方风车为蓝色
            tmp_img = imgChannels_.at(0) - imgChannels_.at(2);  // 图片白光影响较大，暂时没法使用这种方法
        }
        else {
            //tmp_img = imgChannels_.at(2);//敌方风车为红色
             tmp_img = imgChannels_.at(2) - imgChannels_.at(0);
        }
        int thresh = 100;  // 设置阈值为 100
        cv::threshold(tmp_img, image, thresh, 255, cv::ThresholdTypes::THRESH_BINARY);        // 这里后期要改成从文件读
        //cv::imshow("R thresholded_img", image);

        //腐蚀膨胀核可能需要具体调整
        Mat element_dilate_1 = getStructuringElement(MORPH_RECT, Size(4, 4));
        Mat element_erode_1 = getStructuringElement(MORPH_RECT, Size(2, 1));
        Mat element_dilate_2 = getStructuringElement(MORPH_RECT, Size(3, 3));
        Mat element_erode_2 = getStructuringElement(MORPH_RECT, Size(4, 4));

        erode(image, image, element_erode_2);
        dilate(image, image, element_dilate_2);

       // cv::imshow("R detect img", image);

        std::vector<std::vector<cv::Point> > center_R_contours;
        findContours(image, center_R_contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

        for (auto& center_R_contour : center_R_contours) {
            if (!isValidCenterRContour(center_R_contour)) {
                continue;
            }
            //std::cout << "get R rigion successfully********************************************" << std::endl;

            centerR = cv::minAreaRect(center_R_contour);
            cv::Point2f* vertices = new cv::Point2f[4];
            centerR.points(vertices);

            for (int j = 0; j < 4; j++)                    // 找到满足要求的矩形的四个边界点并划线，使用绿色圈出
            {
                cv::line(binary, vertices[j], vertices[(j + 1) % 4], cv::Scalar(0, 255, 255), 2);//画矩形框
            }
            cv::imshow("R region",binary);

            float target_length =
                target_armor.size.height > target_armor.size.width ? target_armor.size.height : target_armor.size.width;
            circle_center_point = centerR.center;
            circle_center_point.y += target_length / 7.5;//实际最小二乘得到的中心在R的下方

            return_result = PixelPoint2d(circle_center_point.x, circle_center_point.y);

            return return_result;
        }
        //std::cout << "fail to get R rigion &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << std::endl;
        return return_result;
    }
    

    //---------------------------------------------------------------------------------------------------------
    // 此函数用于判断找到的矩形候选区是否为可能的风车中心R候选区
    //---------------------------------------------------------------------------------------------------------
    bool Windmill_solver::isValidCenterRContour(const std::vector<cv::Point>& center_R_contour) {
        //一些参数
        long CENTER_R_AREA_MAX = 120;//风车中心R面积最大值
        long CENTER_R_AREA_MIN = 15;//风车中心R面积最小值
        long CENTER_R_CONTOUR_LENGTH_MIN = 6;//风车中心R长边长度最小值
        long CENTER_R_CONTOUR_WIDTH_MIN = 6;//风车中心R长边长度最大值
        long CENTER_R_CONTOUR_LENGTH_MAX = 20;//风车中心R宽边长度最小值
        long CENTER_R_CONTOUR_WIDTH_MAX = 20;//风车中心R宽边长度最大值
        float CENTER_R_CONTOUR_HW_RATIO_MAX = 1.2;//风车中心R长宽比最大值
        float CENTER_R_CONTOUR_HW_RATIO_MIN = 0.8;//风车中心R长宽比最小值
        float CENTER_R_CONTOUR_AREA_RATIO_MIN = 0.6;//装甲板轮廓占旋转矩形面积比最小值
        float CENTER_R_CONTOUR_INTERSETION_AREA_MIN = 10;//中心R占ROI区的面积最小值

        double R_area = contourArea(center_R_contour);
        if (R_area > CENTER_R_AREA_MAX ||R_area < CENTER_R_AREA_MIN) {
            //std::cout<<cur_contour_area<<" "<< CENTER_R_CONTOUR_AREA_MIN <<" "<< CENTER_R_CONTOUR_AREA_MAX <<std::endl;
            //std::cout<<"选区面积大小不合适**************"<<std::endl;
            return false;
            //选区面积大小不合适
        }

        RotatedRect cur_rect = minAreaRect(center_R_contour);
        Size2f cur_size = cur_rect.size;
        float length = cur_size.height > cur_size.width ? cur_size.height : cur_size.width;//将矩形的长边设置为长
        float width = cur_size.height < cur_size.width ? cur_size.height : cur_size.width;//将矩形的短边设置为宽

        float length_width_ratio = length / width;//计算矩形长宽比
        if (length_width_ratio > CENTER_R_CONTOUR_HW_RATIO_MAX ||
            length_width_ratio < CENTER_R_CONTOUR_HW_RATIO_MIN) {
            //std::cout<<"长宽比不合适"<<std::endl;
    //        cout << "HW: " << length_width_ratio << '\t' << cur_rect.center << endl;
            return false;
            //长宽比不合适
        }

        return true;
    }
}
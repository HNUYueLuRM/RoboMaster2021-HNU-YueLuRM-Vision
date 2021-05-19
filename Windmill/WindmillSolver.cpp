#include "WindmillSolver.h"
#include <cmath>
using namespace cv;

#define DEBUG
#ifdef DEBUG
#include <iostream>
#endif // DEBUG


namespace hnurm {

    // ��ֵ������ֵ
    static int thresh_lower{ 80 };
    static int thresh_upper{ 255 };

    // ���ж�ֵ��ʱ�Ļص�����
    static void thresh_track(int, void* user_data)
    {
        try {
            cv::Mat img = *((cv::Mat*)(user_data));
            cv::Mat dst = img.clone();
            cv::imshow("img in the trace bar", img);
            cv::threshold(img, dst, thresh_lower, thresh_upper, cv::ThresholdTypes::THRESH_BINARY);        //��ֵҪ�Լ���

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
    // �˺�������ʶ��糵����
    //---------------------------------------------------------------------------------------------------------
    std::vector<PixelPoint2d> Windmill_solver::detect_armor_cornor_and_center(const cv::Mat& img)
    {
        cv::Mat image = img.clone(), binary;
        image.copyTo(binary);

        std::vector <PixelPoint2d> return_result;

        // ��ͼ���ŵ�Ŀ��ߴ� 640 * 480
        int dst_width = 640;
        int dst_height = 480;
        //---------------------------------------------------------------------------------------------------------------------
        //����ͼ��ߴ�
        //---------------------------------------------------------------------------------------------------------------------
        resolution_height= dst_height;//��ǰͼ��ĸ߶ȺͿ��
        resolution_width=dst_width;

        // �����������ӣ��ֱ�����߷�������ű���
        // �������ʵ��λ�ã�ʹ��λ�˽���ģ��ĵ�ʱ����Ҫ�õ�
        int factor_width = dst_width / image.cols;
        int factor_height = dst_height / image.rows;


        cv::resize(image, image, cv::Size(640, 480));
        cv::resize(binary, binary, cv::Size(640, 480));



        // תΪ�Ҷ�ͼ��
        std::vector <cv::Mat> imgChannels;

//        // ��ͼ��������Ͳ���������������
        cv::Mat elementRect;
        elementRect = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4), cv::Point(-1, -1));
        cv::dilate(image, image, elementRect);

//        // �� BGR ͼ�������ͨ���ֱ�ֵ����� channel ��
          cv::split(image, imgChannels);
//        
//        // ͨ�����������һ�ֳ�����˼·���ܶ಩�ͺͿ�Դ�����ж��ᵽ
//        // ���ǲ���Ч�����ã�û���ã���������Ϊֻ��ͼƬ��ԭ��
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
            mid_image = imgChannels.at(0);//�з��糵Ϊ��ɫ
            //mid_image = imgChannels.at(0) - imgChannels.at(2);  // ͼƬ�׹�Ӱ��ϴ���ʱû��ʹ�����ַ���
        }
        else {
            mid_image = imgChannels.at(2);//�з��糵Ϊ��ɫ
            //mid_image = imgChannels.at(2) - imgChannels.at(0);
        }

         //cv::imshow("original_mid_image", mid_image);

         int thresh = 100;  // ������ֵΪ 100
         cv::threshold(mid_image, image, thresh, 255, cv::ThresholdTypes::THRESH_BINARY);        // �������Ҫ�ĳɴ��ļ���
#ifdef DEBUG
         cv::imshow("thresholded_img", image);
#endif // DEBUG

        // ��ˮ�����
        floodFill(image, cv::Point(0, 0), cv::Scalar(255), 0, cv::FLOODFILL_FIXED_RANGE);
#ifdef DEBUG
        cv::imshow("image filled", image);

#endif // DEBUG


        std::vector<std::vector<cv::Point>> contours;

        findContours(image, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);


        for (size_t i = 0; i < contours.size(); i++) {
            double area = contourArea(contours[i]);  // ������������
            if (area < 50 || 1e4 < area) continue;   // ɸѡ����������㷶Χ�������������

            // ��ԭͼ�ϻ�������
            cv::drawContours(image, contours, static_cast<int>(i), cv::Scalar(0), 2);   

            std::vector<cv::Point> points_1 = contours[i];
            cv::RotatedRect rrect = cv::fitEllipse(points_1);
            cv::Point2f* vertices = new cv::Point2f[4];
            rrect.points(vertices);


            float aim = rrect.size.height / rrect.size.width;  // ��߱ȣ�ɸѡ��������ת�ľ���
            if (aim > 1.7 && aim < 2.6) {                      // ���ƿ�߱��� 1.7 �� 2.6 ֮��
                for (int j = 0; j < 4; j++)                    // �ҵ�����Ҫ��ľ��ε��ĸ��߽�㲢���ߣ�ʹ����ɫȦ��
                {
                    return_result.push_back(PixelPoint2d(vertices[j].x, vertices[j].y));
                    cv::line(binary, vertices[j], vertices[(j + 1) % 4], cv::Scalar(0, 255, 0), 4);//�����ο�
                }
                delete[] vertices;
                float middle = 100000;

                for (size_t j = 1; j < contours.size(); j++) {
                    double area = contourArea(contours[j]);
                    if (area < 50 || 1e4 < area) continue;

                    std::vector<cv::Point> points_2 = contours[j];
                    cv::RotatedRect rrect_2 = fitEllipse(points_2);

                    float aim_2 = rrect_2.size.height / rrect_2.size.width;
                    //�²⣺���־��εıȽϣ����������־������ĵľ��룬����ֵ��middle
                    if (aim_2 > 3.0) {
                        float distance = sqrt((rrect.center.x - rrect_2.center.x) * (rrect.center.x - rrect_2.center.x) +
                            (rrect.center.y - rrect_2.center.y) * (rrect.center.y - rrect_2.center.y));

                        if (middle > distance)
                            middle = distance;
                    }
                }
                if (middle > 60) {//  �������ҲҪ����ʵ�������,��ͼ��ߴ������Զ���йء�
                    //Ҳ����˵��һ�����������ĺ���ξ��붼����60ʱ,��ʱ�ÿ������۾���<<<�ˡ�
					//��һ˵һ������ʶ��ʽ̫����ˣ�����л�һ�³ߴ�Ͳ�����
					//��򵥵ķ����ǰ�60��һ���ʹ�糵�йص��������������������ȶ��Ի��һЩ�����׷���������SVM�ˡ�    
                    armor_point = rrect.center;//����װ�װ���������
                    cv::circle(binary, cv::Point(rrect.center.x, rrect.center.y), 15, cv::Scalar(0, 0, 255), 4);//��ɫȦ����ˮ�ƣ�����Ҫ���������

                    cv::circle(binary, circle_center_point, 6, cv::Scalar(0, 255, 255), 2);//R����

                }
            }
        }

        std::vector<std::vector<cv::Point>>().swap(contours);

#ifdef DEBUG
        imshow("ʶ����", binary);
        //imshow("Original", image);

#endif       
        return return_result;
    }



    //---------------------------------------------------------------------------------------------------------
    // �˺�������ʶ��糵����R�ַ�
    //---------------------------------------------------------------------------------------------------------
    PixelPoint2d Windmill_solver::detect_windmill_center_r_icon(const cv::Mat& img) {
        
        PixelPoint2d return_result = PixelPoint2d(resolution_width / 2, resolution_height / 2);
        
        cv::Mat image = img.clone(), binary;
        //ͼ������
        cv::resize(image, image, cv::Size(640, 480));
        image.copyTo(binary);


        //����ͼ��: �Ҷ�--����ʴ������--��findcountoursѰ��R����
       // תΪ�Ҷ�ͼ��
        std::vector <cv::Mat> imgChannels_;

        // ��ͼ��������Ͳ���������������
        cv::Mat elementRect;
        elementRect = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4), cv::Point(-1, -1));
        cv::dilate(image, image, elementRect);

        cv::Mat tmp_img;
        // �� BGR ͼ�������ͨ���ֱ�ֵ����� channel ��
        cv::split(image, imgChannels_);
        if (_own_color == TeamColor::RED)
        {
            //tmp_img = imgChannels_.at(0);//�з��糵Ϊ��ɫ
            tmp_img = imgChannels_.at(0) - imgChannels_.at(2);  // ͼƬ�׹�Ӱ��ϴ���ʱû��ʹ�����ַ���
        }
        else {
            //tmp_img = imgChannels_.at(2);//�з��糵Ϊ��ɫ
             tmp_img = imgChannels_.at(2) - imgChannels_.at(0);
        }
        int thresh = 100;  // ������ֵΪ 100
        cv::threshold(tmp_img, image, thresh, 255, cv::ThresholdTypes::THRESH_BINARY);        // �������Ҫ�ĳɴ��ļ���
        //cv::imshow("R thresholded_img", image);

        //��ʴ���ͺ˿�����Ҫ�������
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

            for (int j = 0; j < 4; j++)                    // �ҵ�����Ҫ��ľ��ε��ĸ��߽�㲢���ߣ�ʹ����ɫȦ��
            {
                cv::line(binary, vertices[j], vertices[(j + 1) % 4], cv::Scalar(0, 255, 255), 2);//�����ο�
            }
            cv::imshow("R region",binary);

            float target_length =
                target_armor.size.height > target_armor.size.width ? target_armor.size.height : target_armor.size.width;
            circle_center_point = centerR.center;
            circle_center_point.y += target_length / 7.5;//ʵ����С���˵õ���������R���·�

            return_result = PixelPoint2d(circle_center_point.x, circle_center_point.y);

            return return_result;
        }
        //std::cout << "fail to get R rigion &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << std::endl;
        return return_result;
    }
    

    //---------------------------------------------------------------------------------------------------------
    // �˺��������ж��ҵ��ľ��κ�ѡ���Ƿ�Ϊ���ܵķ糵����R��ѡ��
    //---------------------------------------------------------------------------------------------------------
    bool Windmill_solver::isValidCenterRContour(const std::vector<cv::Point>& center_R_contour) {
        //һЩ����
        long CENTER_R_AREA_MAX = 120;//�糵����R������ֵ
        long CENTER_R_AREA_MIN = 15;//�糵����R�����Сֵ
        long CENTER_R_CONTOUR_LENGTH_MIN = 6;//�糵����R���߳�����Сֵ
        long CENTER_R_CONTOUR_WIDTH_MIN = 6;//�糵����R���߳������ֵ
        long CENTER_R_CONTOUR_LENGTH_MAX = 20;//�糵����R��߳�����Сֵ
        long CENTER_R_CONTOUR_WIDTH_MAX = 20;//�糵����R��߳������ֵ
        float CENTER_R_CONTOUR_HW_RATIO_MAX = 1.2;//�糵����R��������ֵ
        float CENTER_R_CONTOUR_HW_RATIO_MIN = 0.8;//�糵����R�������Сֵ
        float CENTER_R_CONTOUR_AREA_RATIO_MIN = 0.6;//װ�װ�����ռ��ת�����������Сֵ
        float CENTER_R_CONTOUR_INTERSETION_AREA_MIN = 10;//����RռROI���������Сֵ

        double R_area = contourArea(center_R_contour);
        if (R_area > CENTER_R_AREA_MAX ||R_area < CENTER_R_AREA_MIN) {
            //std::cout<<cur_contour_area<<" "<< CENTER_R_CONTOUR_AREA_MIN <<" "<< CENTER_R_CONTOUR_AREA_MAX <<std::endl;
            //std::cout<<"ѡ�������С������**************"<<std::endl;
            return false;
            //ѡ�������С������
        }

        RotatedRect cur_rect = minAreaRect(center_R_contour);
        Size2f cur_size = cur_rect.size;
        float length = cur_size.height > cur_size.width ? cur_size.height : cur_size.width;//�����εĳ�������Ϊ��
        float width = cur_size.height < cur_size.width ? cur_size.height : cur_size.width;//�����εĶ̱�����Ϊ��

        float length_width_ratio = length / width;//������γ����
        if (length_width_ratio > CENTER_R_CONTOUR_HW_RATIO_MAX ||
            length_width_ratio < CENTER_R_CONTOUR_HW_RATIO_MIN) {
            //std::cout<<"����Ȳ�����"<<std::endl;
    //        cout << "HW: " << length_width_ratio << '\t' << cur_rect.center << endl;
            return false;
            //����Ȳ�����
        }

        return true;
    }
}
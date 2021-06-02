#include "ArmorDetector.h"
namespace hnurm{
    ArmorDetector::ArmorDetector()
    {
        _NanoDet = make_unique<NanoDet>("../Param/nanodet.param",
                                        "../Param/nanodet.bin", true);
        //my_color = Protocol::Self_color::none;
        _my_color = 1;//0 = red ;;;  1 = blue
    }
    bool ArmorDetector::Detect(cv::Mat &raw_image, cv::Rect &target_rect) {
        std::vector<BoxInfo> target_boxs;
        std::vector<cv::Rect> temp_rects;
        cv::Rect temp_rect;
        object_rect effect_roi;
        cv::Mat resized_img;
        resize_uniform(raw_image, resized_img, cv::Size(320, 320), effect_roi);
        target_boxs = _NanoDet->detect(resized_img, 0.5, 0.1);
        if(target_boxs.size() == 0)return false;
        int src_w = raw_image.cols;
        int src_h = raw_image.rows;
        int dst_w = effect_roi.width;
        int dst_h = effect_roi.height;
        float width_ratio = (float)src_w / (float)dst_w;
        float height_ratio = (float)src_h / (float)dst_h;
        for(size_t i = 0; i < target_boxs.size(); i++){
            const BoxInfo& box = target_boxs[i];
            if(_my_color == box.label){ // red

                    temp_rect = cv::Rect(
                            cv::Point((box.x1 - effect_roi.x) * width_ratio, (box.y1 - effect_roi.y) * height_ratio),
                            cv::Point((box.x2 - effect_roi.x) * width_ratio, (box.y2 - effect_roi.y) * height_ratio));
                    temp_rects.push_back(temp_rect);

            }
            else {//blue

                    temp_rect = cv::Rect(
                            cv::Point((box.x1 - effect_roi.x) * width_ratio, (box.y1 - effect_roi.y) * height_ratio),
                            cv::Point((box.x2 - effect_roi.x) * width_ratio, (box.y2 - effect_roi.y) * height_ratio));
                    temp_rects.push_back(temp_rect);

            }
        }

        if(temp_rects.size() == 0)return false;
        cv::Rect finnal_rect;
        int image_center_x = (raw_image.size().width) / 2;
        int image_center_y = (raw_image.size().height) / 2;
        int temp_rect_center_x = 0;
        int temp_rect_center_y = 0;
        int final_index = 0;
        float distance = 10000000;
        float MIN_distance = 10000000;
        int i = 0;
        for(i = 0; i < temp_rects.size(); i++){
            cv::rectangle(raw_image, temp_rects[i], cv::Scalar(0,255,255),2);
        }
        for(i = 0; i < temp_rects.size(); i++){
            temp_rect_center_x = temp_rects[i].x + temp_rects[i].width / 2;
            temp_rect_center_y = temp_rects[i].y + temp_rects[i].height / 2;
            distance = sqrt((temp_rect_center_x - image_center_x) * (temp_rect_center_x - image_center_x) + (temp_rect_center_y - image_center_y) * (temp_rect_center_y - image_center_y));
            if(MIN_distance > distance){
                MIN_distance = distance;
                final_index = i;
            }
        }
        target_rect = temp_rects[final_index];
        //target_rect = temp_rects[0];
        return true;
    }
    bool ArmorDetector::set_color(int color) {
        // 0 = blue
        // 1 = red
        _my_color = color;
    }
    int ArmorDetector::resize_uniform(cv::Mat &src, cv::Mat &dst, cv::Size dst_size, object_rect &effect_area) {
        int w = src.cols;
        int h = src.rows;
        int dst_w = dst_size.width;
        int dst_h = dst_size.height;
        //std::cout << "src: (" << h << ", " << w << ")" << std::endl;
        dst = cv::Mat(cv::Size(dst_w, dst_h), CV_8UC3, cv::Scalar(0));

        float ratio_src = w * 1.0 / h;
        float ratio_dst = dst_w * 1.0 / dst_h;

        int tmp_w = 0;
        int tmp_h = 0;
        if (ratio_src > ratio_dst) {
            tmp_w = dst_w;
            tmp_h = floor((dst_w * 1.0 / w) * h);
        }
        else if (ratio_src < ratio_dst) {
            tmp_h = dst_h;
            tmp_w = floor((dst_h * 1.0 / h) * w);
        }
        else {
            cv::resize(src, dst, dst_size);
            effect_area.x = 0;
            effect_area.y = 0;
            effect_area.width = dst_w;
            effect_area.height = dst_h;
            return 0;
        }

        //std::cout << "tmp: (" << tmp_h << ", " << tmp_w << ")" << std::endl;
        cv::Mat tmp;
        cv::resize(src, tmp, cv::Size(tmp_w, tmp_h));

        if (tmp_w != dst_w) {
            int index_w = floor((dst_w - tmp_w) / 2.0);
            //std::cout << "index_w: " << index_w << std::endl;
            for (int i = 0; i < dst_h; i++) {
                memcpy(dst.data + i * dst_w * 3 + index_w * 3, tmp.data + i * tmp_w * 3, tmp_w * 3);
            }
            effect_area.x = index_w;
            effect_area.y = 0;
            effect_area.width = tmp_w;
            effect_area.height = tmp_h;
        }
        else if (tmp_h != dst_h) {
            int index_h = floor((dst_h - tmp_h) / 2.0);
            //std::cout << "index_h: " << index_h << std::endl;
            memcpy(dst.data + index_h * dst_w * 3, tmp.data, tmp_w * tmp_h * 3);
            effect_area.x = 0;
            effect_area.y = index_h;
            effect_area.width = tmp_w;
            effect_area.height = tmp_h;
        }
        else {
            printf("error\n");
        }
        //cv::imshow("dst", dst);
        //cv::waitKey(0);
        return 0;
    }
}
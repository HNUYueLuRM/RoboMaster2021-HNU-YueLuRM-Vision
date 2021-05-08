//
// Create by RangiLyu
// 2020 / 10 / 2
//

#ifndef NANODET_H
#define NANODET_H

#include <opencv2/core/core.hpp>
#include <net.h>

typedef struct HeadInfo
{
    std::string cls_layer;
    std::string dis_layer;
    int stride;
};

typedef struct BoxInfo 
{
    float x1;
    float y1;
    float x2;
    float y2;
    float score;
    int label;
} BoxInfo;

class NanoDet
{
public:
    NanoDet(const char* param, const char* bin, bool useGPU);

    ~NanoDet();

    static NanoDet* detector;
    ncnn::Net* Net;
    static bool hasGPU;

    std::vector<HeadInfo> heads_info{
        // cls_pred|dis_pred|stride
            {"792", "795",    8},
            {"814", "817",   16},
            {"836", "839",   32},
    };

    std::vector<BoxInfo> detect(cv::Mat image, float score_threshold, float nms_threshold);

    std::vector<std::string> labels{ "armor_red", "armor_blue"
    };
private:
    void preprocess(cv::Mat& image, ncnn::Mat& in);
    void decode_infer(ncnn::Mat& cls_pred, ncnn::Mat& dis_pred, int stride, float threshold, std::vector<std::vector<BoxInfo>>& results);
    BoxInfo disPred2Bbox(const float*& dfl_det, int label, float score, int x, int y, int stride);
    static void nms(std::vector<BoxInfo>& result, float nms_threshold);
    int input_size = 320;
    int num_class = 2;
    int reg_max = 7;

};


#endif //NANODET_H
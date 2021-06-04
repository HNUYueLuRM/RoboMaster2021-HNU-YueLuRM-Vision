#pragma once
#include "MineralDetector.h"

namespace hnurm
{
    MineralDetector::MineralDetector(const Mat& raw_frame)
    {
        raw_img = raw_frame;
    }

    void MineralDetector::GetCooked()
    {
        cvtColor(raw_img, hsv_img, COLOR_BGR2HSV);
        inRange(hsv_img, Scalar(19, 136, 144), Scalar(40, 242, 255), cooked_img);
    }

    void MineralDetector::CreateMineral()
    {
        findContours(cooked_img, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
        unsigned max_contours_size = 0;
        unsigned max_contours_index = 0;
        for (unsigned i = 0; i < contours.size(); i++)
        {
            if (contours[i].size() > max_contours_size)
            {
                max_contours_size = contours[i].size();
                max_contours_index = i;
            }
        }
        mineral_rect = minAreaRect(contours[max_contours_index]);

        Point2f tmp_points[4];
        mineral_rect.points(tmp_points);
        mineral_core.x = (tmp_points[0].x + tmp_points[2].x) / 2;
        mineral_core.y = (tmp_points[0].y + tmp_points[2].y) / 2;
    }

    Point2f MineralDetector::GetCore()
    {
        return mineral_core;
    }
}

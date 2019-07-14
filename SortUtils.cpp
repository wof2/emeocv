#include "SortUtils.h"

SortUtils::SortUtils()
{
}

SortUtils::~SortUtils()
{
}


float  SortUtils::getLinePointAvgY(cv::Vec2f line) {
        float rho = line[0];
        float theta = line[1];
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        cv::Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
        cv::Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));

        return (pt1.y + pt2.y )*0.5f;
}

#include "SortUtils.h"

SortUtils::SortUtils()
{
}

SortUtils::~SortUtils()
{
}

float SortUtils::getMedianFromSortedVector(std::vector<float> v) {
	float median;
	int s= v.size(); 
	if(v.size() % 2 == 0 ) { 		
		median = v[s/2] + v[(s/2) - 1];
		median /= 2; 
	}else {
		median =v[floor(s/2)];
	}
	return median;
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

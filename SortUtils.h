#ifndef SORTUTILS_H
#define SORTUTILS_H
#include <opencv2/imgproc/imgproc.hpp>
class SortUtils
{
public:
	SortUtils();
	~SortUtils();
	float static getLinePointAvgY(cv::Vec2f line);
	float static getMedianFromSortedVector(std::vector<float> v);
};
/**
 * Functor to help sorting rectangles by their x-position.
 */
class sortRectByX {
public:
    bool operator()(cv::Rect const & a, cv::Rect const & b) const {
        return a.x < b.x;
    }
};
class sortFloats {
public:
    bool operator()(float const & a, float const & b) const {
        return a < b;
    }
};
class sortRectByY {
public:
    bool operator()(cv::Rect const & a, cv::Rect const & b) const {
        return a.y < b.y;
    }
};
class sortLinesByAvgY {
public:
    bool operator()(cv::Vec2f const & a, cv::Vec2f const & b) const {
        return  SortUtils::getLinePointAvgY(a) < SortUtils::getLinePointAvgY(b);
    }
};



#endif // SORTUTILS_H

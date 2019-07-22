#ifndef AUTOMATICBOUNDINGBOXEXTRACTOR_H
#define AUTOMATICBOUNDINGBOXEXTRACTOR_H

#include "AbstractDigitBoundingBoxesExtractor.h" // Base class: AbstractDigitBoundingBoxesExtractor


#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include "Config.h"

class AutomaticBoundingBoxExtractor : public AbstractDigitBoundingBoxesExtractor
{
public:
	AutomaticBoundingBoxExtractor(const Config & config);
	~AutomaticBoundingBoxExtractor();

private:
	std::vector<cv::Rect> find(cv::Mat counterArea);
	void filterContours(cv::Mat& counterArea, std::vector<std::vector<cv::Point>>& contours,
        std::vector<cv::Rect>& boundingBoxes,
        std::vector<std::vector<cv::Point>>& filteredContours);
	void findAlignedBoxes(std::vector<cv::Rect>::const_iterator begin,
        std::vector<cv::Rect>::const_iterator end,
        std::vector<cv::Rect>& result);
	 bool _debugContours;
	
};

#endif // AUTOMATICBOUNDINGBOXEXTRACTOR_H

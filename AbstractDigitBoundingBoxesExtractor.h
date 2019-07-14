#ifndef ABSTRACTDIGITBOUNDINGBOXESEXTRACTOR_H
#define ABSTRACTDIGITBOUNDINGBOXESEXTRACTOR_H


#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include "Config.h"

class AbstractDigitBoundingBoxesExtractor
{
public:
	AbstractDigitBoundingBoxesExtractor(const Config & config);
	~AbstractDigitBoundingBoxesExtractor();
	
	virtual std::vector<cv::Rect> find(cv::Mat counterArea) = 0;
	
protected:
	Config _config;
};

#endif // ABSTRACTDIGITBOUNDINGBOXESEXTRACTOR_H

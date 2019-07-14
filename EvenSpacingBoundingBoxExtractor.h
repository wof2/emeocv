#ifndef EVENSPACINGBOUNDINGBOXEXTRACTOR_H
#define EVENSPACINGBOUNDINGBOXEXTRACTOR_H

#include "AbstractDigitBoundingBoxesExtractor.h" // Base class: AbstractDigitBoundingBoxesExtractor


class EvenSpacingBoundingBoxExtractor : public AbstractDigitBoundingBoxesExtractor
{
public:
	EvenSpacingBoundingBoxExtractor(const Config & config);
	~EvenSpacingBoundingBoxExtractor();

private:
	std::vector<cv::Rect> find(cv::Mat counterArea);
};

#endif // EVENSPACINGBOUNDINGBOXEXTRACTOR_H

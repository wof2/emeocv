#include "EvenSpacingBoundingBoxExtractor.h"


EvenSpacingBoundingBoxExtractor::EvenSpacingBoundingBoxExtractor(const Config & config) : AbstractDigitBoundingBoxesExtractor(config) {
	
}
std::vector<cv::Rect> EvenSpacingBoundingBoxExtractor::find(cv::Mat counterArea) {
	//log4cpp::Category& rlog = log4cpp::Category::getRoot();
    std::vector<cv::Rect> boundingBoxes;
	const float digitOccupationPercentageX = 0.55f;
	const float digitOccupationPercentageY = 0.65f;
	
	float height = digitOccupationPercentageY * counterArea.rows;
	float y = (1 - digitOccupationPercentageY)* 0.5f * height;
	float width = counterArea.cols * digitOccupationPercentageX  / _config.getDigitCount();	
	float step = (1.0f-digitOccupationPercentageX) *  counterArea.cols/ _config.getDigitCount();	
	float x = width * 0.5f;
	for(int i=0; i<_config.getDigitCount(); i++) {			
		boundingBoxes.push_back(cv::Rect2f(x,y, width, height));
		x+=width;
		x+=step;
		
	}
	
	return boundingBoxes;
}


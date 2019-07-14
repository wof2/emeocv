#include "AutomaticBoundingBoxExtractor.h"

#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>

#include <opencv2/highgui/highgui.hpp>
#include "SortUtils.h"

AutomaticBoundingBoxExtractor::AutomaticBoundingBoxExtractor(const Config & config) : AbstractDigitBoundingBoxesExtractor(config), _debugContours(true) {
	
}

AutomaticBoundingBoxExtractor::~AutomaticBoundingBoxExtractor()
{
}

std::vector<cv::Rect> AutomaticBoundingBoxExtractor::find(cv::Mat counterArea) {
	log4cpp::Category& rlog = log4cpp::Category::getRoot();
    // find contours in whole image
    std::vector<std::vector<cv::Point> > contours, filteredContours;
    std::vector<cv::Rect> boundingBoxes;

#if CV_MAJOR_VERSION == 2
    cv::findContours(counterArea, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
#elif CV_MAJOR_VERSION == 3
    cv::findContours(counterArea, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
#endif

    if (_debugContours) {
        // draw contours
        cv::Mat contb = cv::Mat::zeros(counterArea.rows, counterArea.cols, CV_8UC1);
        cv::drawContours(contb, contours, -1, cv::Scalar(255));
        cv::imshow("contours before", contb);
    }
    // filter contours by bounding rect size
    filterContours(contours, boundingBoxes, filteredContours);

    // find bounding boxes that are aligned at y position
    std::vector<cv::Rect> alignedBoundingBoxes, tmpRes;
    for (std::vector<cv::Rect>::const_iterator ib = boundingBoxes.begin(); ib != boundingBoxes.end(); ++ib) {
        tmpRes.clear();
        findAlignedBoxes(ib, boundingBoxes.end(), tmpRes);
        if (tmpRes.size() > alignedBoundingBoxes.size()) {
            alignedBoundingBoxes = tmpRes;
        }
    }
  
     // sort bounding boxes from left to right
    std::sort(alignedBoundingBoxes.begin(), alignedBoundingBoxes.end(), sortRectByX());   
    alignedBoundingBoxes.erase( std::unique( alignedBoundingBoxes.begin(), alignedBoundingBoxes.end() ), alignedBoundingBoxes.end() );   
	return alignedBoundingBoxes;
}



/**
 * Filter contours by size of bounding rectangle.
 */
void AutomaticBoundingBoxExtractor::filterContours(std::vector<std::vector<cv::Point> >& contours,
        std::vector<cv::Rect>& boundingBoxes, std::vector<std::vector<cv::Point> >& filteredContours) {
    // filter contours by bounding rect size
    log4cpp::Category& rlog = log4cpp::Category::getRoot();

     rlog << log4cpp::Priority::INFO << "number of contours before filtering: " << contours.size();
     std::vector<cv::Rect> tmpBoxes;
	
    
    for (size_t i = 0; i < contours.size(); i++) {
        cv::Rect bounds = cv::boundingRect(contours[i]);
		tmpBoxes.push_back(bounds);
	}	
	std::sort(tmpBoxes.begin(), tmpBoxes.end(), sortRectByX());
	
	
	for (size_t i = 0; i < tmpBoxes.size(); i++) {			
		cv::Rect bounds = tmpBoxes[i];		
		 rlog << log4cpp::Priority::INFO << "bounding : " << bounds;                    
        if (bounds.height > 0.25f *_digitsRegion.height && bounds.height < 0.9f *_digitsRegion.height
                && bounds.width > 4 && bounds.width < bounds.height) {	
			if(i>0) {		
					boundingBoxes.push_back(bounds); 				
			} else {			
				boundingBoxes.push_back(bounds); 			
			}  
        }
		//rlog << log4cpp::Priority::INFO << "Qualifed region : " <<  (*ib);
		//cv::rectangle(_img, *ib, cv::Scalar(255, 255, 0), 2);
	}
	if(_debugDigits) {
		for (size_t i = 0; i < boundingBoxes.size(); i++) {	
			cv::Rect shifted = boundingBoxes[i];
			shifted.x += _digitsRegion.x;
			shifted.y += _digitsRegion.y;
			cv::rectangle(_img,  shifted, cv::Scalar(0, 255, 0), 1);
		}
	}
	
   
}


/**
 * Find bounding boxes that are aligned at y position.
 */
void AutomaticBoundingBoxExtractor::findAlignedBoxes(std::vector<cv::Rect>::const_iterator begin,
        std::vector<cv::Rect>::const_iterator end, std::vector<cv::Rect>& result) {
    std::vector<cv::Rect>::const_iterator it = begin;
    cv::Rect start = *it;
    ++it;
    result.push_back(start);
	cv::Rect lastRect;
	
    for (; it != end; ++it) {		
		if (abs(start.y - it->y) < _config.getDigitYAlignment() && (1.0f*abs(start.height - it->height) / start.height) < 0.5f) {
			if(lastRect.area() == 0) { // bo boxes so far
				result.push_back(*it);
				lastRect = *it;
			}else{
				float xdiff = abs(it->x - (lastRect.x + lastRect.width));
				if(xdiff > (it->width + lastRect.width)*0.3f ) {
					result.push_back(*it);
					lastRect = *it;
				}
			}
			
		}
		
    }
}




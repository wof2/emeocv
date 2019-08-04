#include "AutomaticBoundingBoxExtractor.h"

#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>

#include <opencv2/highgui/highgui.hpp>
#include "SortUtils.h"

AutomaticBoundingBoxExtractor::AutomaticBoundingBoxExtractor(const Config & config) : AbstractDigitBoundingBoxesExtractor(config), _debugContours(false) {
	
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
    filterContours(counterArea, contours, boundingBoxes, filteredContours);

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
void AutomaticBoundingBoxExtractor::filterContours(cv::Mat& counterArea, std::vector<std::vector<cv::Point> >& contours,
        std::vector<cv::Rect>& boundingBoxes, std::vector<std::vector<cv::Point> >& filteredContours) {
    // filter contours by bounding rect size
    log4cpp::Category& rlog = log4cpp::Category::getRoot();

     rlog << log4cpp::Priority::DEBUG << "number of contours before filtering: " << contours.size();
     std::vector<cv::Rect> tmpBoxes;
	
    
    for (size_t i = 0; i < contours.size(); i++) {
        cv::Rect bounds = cv::boundingRect(contours[i]);
		tmpBoxes.push_back(bounds);
	}	
	std::sort(tmpBoxes.begin(), tmpBoxes.end(), sortRectByX());
	
	
	for (size_t i = 0; i < tmpBoxes.size(); i++) {			
		cv::Rect bounds = tmpBoxes[i];		
	//	 rlog << log4cpp::Priority::DEBUG << "bounding : " << bounds;                    
        if (bounds.height > 0.25f *counterArea.rows && bounds.height < 0.99f *counterArea.rows
                && bounds.width > 6 && bounds.width < bounds.height) {	
			if(i>0) {		
					boundingBoxes.push_back(bounds); 				
			} else {			
				boundingBoxes.push_back(bounds); 			
			}  
        }
		//rlog << log4cpp::Priority::INFO << "Qualifed region : " <<  (*ib);
		//cv::rectangle(_img, *ib, cv::Scalar(255, 255, 0), 2);
	}
	/*if(_debugDigits) {
		for (size_t i = 0; i < boundingBoxes.size(); i++) {	
			cv::Rect shifted = boundingBoxes[i];
			shifted.x += _digitsRegion.x;
			shifted.y += _digitsRegion.y;
			cv::rectangle(_img,  shifted, cv::Scalar(0, 255, 0), 1);
		}
	}*/
	
   
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
	cv::Rect lastRect = start;
	std::vector<float> xdiffs;
	float avgxdiff = 0;
	// area diff
    for (; it != end; ++it) {		
		if (abs(start.y - it->y) < _config.getDigitYAlignment() && (1.0f*abs(start.height - it->height) / start.height) < 0.5f) {
			float xdiff = abs(it->x - (lastRect.x + lastRect.width)); 
				float areaDiffPercentage = 1.0f*abs(lastRect.area() - it->area()) / (1.0f*it->area());
				if(areaDiffPercentage < 2.0f){
					result.push_back(*it);
					lastRect = *it;
					xdiffs.push_back(xdiff);
					avgxdiff+=xdiff;
				}
		}
    }

		
	std::vector<cv::Rect>::const_iterator it2 = begin;
	// result is 1 element bigger than xdiffs
	if(result.size() <2 || xdiffs.size()<1 ) return;
	std::vector<float> xdiffsSorted= xdiffs;
	
	std::sort(xdiffsSorted.begin(), xdiffsSorted.end(), sortFloats());  
	float median = SortUtils::getMedianFromSortedVector(xdiffsSorted);
   
	
	// remove bounding boxes that are packed to close together.
	it2 = result.begin();
	it2++; // start from the secound rect
	
	int j = 0;

	while(it2 != result.end()) {
		float diff = xdiffs.at(j);		
		if(1 && (diff < median * 0.7f || diff > median * 1.3f)) {
			//std::cout<<"Kasuje"<<prev<<" \n";
			 it2 = result.erase(it2);
			 if(it2 != result.end()) { // there are more elements
				 // update next element xDiff info				
				 cv::Rect prev = result[j];	
				 float newXDiff = abs(it2->x - (prev.x + prev.width));
 				 xdiffs.at(j+1) = newXDiff;
			 }				 
			
		}
		else ++it2;
		++j;
		
	}
	
}




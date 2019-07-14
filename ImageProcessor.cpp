/*
 * ImageProcessor.cpp
 *
 */

#include <vector>
#include <iostream>
#include <exception>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>

#include "ImageProcessor.h"
#include "Config.h"
#include "EvenSpacingBoundingBoxExtractor.h"


ImageProcessor::ImageProcessor(const Config & config) :
        _config(config), _debugWindow(true), _debugSkew(false), _debugDigits(true), _debugEdges(false) {
}


ImageProcessor::~ImageProcessor() {
	if(_boundingBoxExtractor != NULL) {
		delete _boundingBoxExtractor;
	}
}

/**
 * Set the input image.
 */
void ImageProcessor::setInput(cv::Mat & img) {
	
    _img = img;    
}

/**
 * Get the vector of output images.
 * Each image contains the edges of one isolated digit.
 */
const std::vector<cv::Mat> & ImageProcessor::getOutput() {	
    return _digits;
}

void ImageProcessor::debugWindow(bool bval) {
    _debugWindow = bval;
    if (_debugWindow) {
        cv::namedWindow("ImageProcessor");
    }
}

void ImageProcessor::debugSkew(bool bval) {
    _debugSkew = bval;
}

void ImageProcessor::debugEdges(bool bval) {
    _debugEdges = bval;
}

void ImageProcessor::debugDigits(bool bval) {
    _debugDigits = bval;
}

void ImageProcessor::showImage() {
    cv::imshow("ImageProcessor", _img);
    cv::waitKey(1);
}

void ImageProcessor::createGray() {
    // convert to gray
	#if CV_MAJOR_VERSION == 2
		cvtColor(_img, _imgGray, CV_BGR2GRAY); 
	#elif CV_MAJOR_VERSION == 3 | 4
		cvtColor(_img, _imgGray, cv::COLOR_BGR2GRAY);
	#endif	
}
/**
 * Main processing function.
 * Read input image and create vector of images for each digit.
 */
void ImageProcessor::process() {
    _digits.clear();
	_boundingBoxExtractor = new EvenSpacingBoundingBoxExtractor(_config);
    log4cpp::Category& rlog = log4cpp::Category::getRoot();        
        
    if(_img.rows > _maxImageHeight) {
        float ratio = 1.0f * _img.cols / _img.rows;
        cv::Size size = cv::Size(floor(_maxImageHeight* ratio),  _maxImageHeight);
        rlog << log4cpp::Priority::INFO << "Resizing image to : " << size.width << ", " << size.height;
        cv::Mat mat =_img.clone();
    //    delete *_img;
        _img = resize(mat, size);

    }
	
	
    
	createGray();

    // initial rotation to get the digits up
    rotate(_config.getRotationDegrees());

    // detect and correct remaining skew (+- 30 deg)
    float skew_deg = detectSkew();
    rotate(skew_deg);

   
    // find and isolate counter digits
    findCounterDigits();

    if (_debugWindow) {
        showImage();
    }
}

/**
 * Rotate image.
 */
void ImageProcessor::rotate(double rotationDegrees) {
    cv::Mat M = cv::getRotationMatrix2D(cv::Point(_imgGray.cols / 2, _imgGray.rows / 2), rotationDegrees, 1);
    cv::Mat img_rotated;
    cv::warpAffine(_imgGray, img_rotated, M, _imgGray.size());
    _imgGray = img_rotated;
    if (_debugWindow) {
        cv::warpAffine(_img, img_rotated, M, _img.size());
        _img = img_rotated;
    }
}

/**
 * Draw lines into image.
 * For debugging purposes.
 */
void ImageProcessor::drawLines(std::vector<cv::Vec2f>& lines) {
    // draw lines
    for (size_t i = 0; i < lines.size(); i++) {
        float rho = lines[i][0];
        float theta = lines[i][1];
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        cv::Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
        cv::Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));
        cv::line(_img, pt1, pt2, cv::Scalar(255, 0, 0), 1);
    }
}

/**
 * Draw lines into image.
 * For debugging purposes.
 */
void ImageProcessor::drawLines(std::vector<cv::Vec4i>& lines, int xoff, int yoff) {
    for (size_t i = 0; i < lines.size(); i++) {
        cv::line(_img, cv::Point(lines[i][0] + xoff, lines[i][1] + yoff),
                cv::Point(lines[i][2] + xoff, lines[i][3] + yoff), cv::Scalar(255, 0, 0), 1);
    }
}

/**
 * Detect the skew of the image by finding almost (+- 30 deg) horizontal lines.
 */
float ImageProcessor::detectSkew() {
    log4cpp::Category& rlog = log4cpp::Category::getRoot();

    cv::Mat edges = cannyEdges(_imgGray, _config.getCannyThreshold1(), _config.getCannyThreshold2());

    // find lines
    std::vector<cv::Vec2f> lines;
    cv::HoughLines(edges, lines, 1, CV_PI / 180.f, 140);

    // filter lines by theta and compute average
    std::vector<cv::Vec2f> filteredLines;
    float theta_min = 60.f * CV_PI / 180.f;
    float theta_max = 120.f * CV_PI / 180.0f;
    float theta_avr = 0.f;
    float theta_deg = 0.f;
    for (size_t i = 0; i < lines.size(); i++) {
        float theta = lines[i][1];
        if (theta >= theta_min && theta <= theta_max) {
            filteredLines.push_back(lines[i]);
            theta_avr += theta;
        }
    }
    if (filteredLines.size() > 0) {
        theta_avr /= filteredLines.size();
        theta_deg = (theta_avr / CV_PI * 180.f) - 90;
        rlog.info("detectSkew: %.1f deg", theta_deg);
    } else {
        rlog.warn("failed to detect skew");
    }

    if (_debugSkew) {
        drawLines(filteredLines);
    }

    return theta_deg;
}

/**
 * Detect edges using Canny algorithm.
 */
cv::Mat ImageProcessor::cannyEdges(cv::Mat & mat,  int lower, int upper) {
    cv::Mat edges;
    // detect edges
      log4cpp::Category& rlog = log4cpp::Category::getRoot();
    
        rlog << log4cpp::Priority::INFO << "Canny thresholds : " << lower << ", " <<upper;

        cv::Canny(mat, edges, lower, upper);
    
    return edges;
}





cv::Mat ImageProcessor::resize(cv::Mat & image, cv::Size size) {
    cv::Mat resized;
    cv::resize(image, resized, size);
    return resized;
}




cv::Rect ImageProcessor::findCounterArea(cv::Mat & img) {
	log4cpp::Category& rlog = log4cpp::Category::getRoot();
	cv::Scalar lower = cv::Scalar(40, 40, 100);
	cv::Scalar upper = cv::Scalar(80, 255, 250);
	cv::Mat blur; 
	cv::Mat hsv; 
	cv::Mat thrs; 
	cv::GaussianBlur(img, blur, cv::Size(5,5), 2);
	//	cv::imshow("blur", blur);
	cvtColor(blur, hsv, cv::COLOR_BGR2HSV);
	cv::inRange(hsv, lower, upper, thrs);
	
	 
	std::vector<std::vector<cv::Point> > contours, filteredContours;
    std::vector<cv::Rect> bb;

    #if CV_MAJOR_VERSION == 2
        cv::findContours(thrs, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    #elif CV_MAJOR_VERSION == 3 | 4
        cv::findContours(thrs, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    #endif   
	
	 cv::Mat approx;
	 
    for (size_t i = 0; i < contours.size(); i++) {
		cv::approxPolyDP(contours[i],approx , 0.1f*cv::arcLength(contours[i], true), true);
		if(approx.total()==4) {		
			cv::Rect bounds = cv::boundingRect(approx);
			if(bounds.area() > _img.rows * _img.cols * 0.005f) { // at least 0,5% of the image
 				bb.push_back(bounds);
				rlog << log4cpp::Priority::INFO << " Square marker detected " << approx;					
				cv::rectangle(img, cv::Point2f(bounds.x, bounds.y), cv::Point2f(bounds.x + bounds.width, bounds.y + bounds.height) , cv::Scalar(255,0,0), 2);
		
			}				
		}
	}
	
	if(_debugEdges) {
		cv::imshow("THrs", thrs);
		//cv::waitKey(0);

	//cv::imshow("HSV", hsv);
	}
	
	if(bb.size() != 2 ) {
		rlog << log4cpp::Priority::ERROR << " Marker detection failed. Found " << contours.size() << " contours instead of 2";
		cv::imshow("Image error", img);
		cv::waitKey(0);
//        exit(-1);
		throw new cv::Exception();
	}
	std::sort(bb.begin(), bb.end(), sortRectByX());  
	return cv::Rect2f(bb[0].x + bb[0].width,  bb[0].y,  bb[1].x - (bb[0].x + bb[0].width), bb[0].height > bb[1].height ? bb[0].height : bb[1].height );
}

cv::Mat ImageProcessor::replaceRedWithBlack(cv::Mat & img) {
	
    cv::Mat mask, mask2, ret,hsv, maskOut;
	cv::Scalar minCol = cv::Scalar(150,80,100);
	cv::Scalar maxCol = cv::Scalar(180,255,255);// hue is in range 0 - 179
    cvtColor(img, hsv, cv::COLOR_BGR2HSV);
 
    cv::inRange(hsv, minCol, maxCol, mask);
	cv::inRange(hsv, cv::Scalar(0,80,100), cv::Scalar(10,255,255), mask2);
  
    cv::bitwise_not(mask, mask);
	cv::bitwise_not(mask2, mask2);
	cv::bitwise_and(mask, mask2, maskOut);  
	cv::imshow("Mask", maskOut);  
    cv::bitwise_and(img, img, ret, maskOut);
   // #cv2.imshow("no red", res)
    return ret;
}
/**
 * Find and isolate the digits of the counter,
 */
void ImageProcessor::findCounterDigits() {
    log4cpp::Category& rlog = log4cpp::Category::getRoot();

    _digitsRegion = findCounterArea(_img);	
	//_img = replaceRedWithBlack(_img);
	//createGray();
	
	cv::Mat edges = cannyEdges(_imgGray, _config.getCannyThreshold1(), _config.getCannyThreshold2());
	if(_debugEdges){
		cv::imshow("Edges", edges);	
	}

    cv::Mat edgesSmall = edges(_digitsRegion);
    cv::imshow("CounterArea", edgesSmall);

	 cv::waitKey(1);
       
    edges = edgesSmall.clone();
    cv::Mat img_ret = edges.clone();
	//std::vector<cv::Rect> alignedBoundingBoxes = findAlignedBoxesFromCounterArea(edges);
	std::vector<cv::Rect> alignedBoundingBoxes = _boundingBoxExtractor->find(edges);
	//std::vector<cv::Rect> alignedBoundingBoxes = findAlignedBoxesFromCounterAreaManual(edges);
	
	rlog << log4cpp::Priority::INFO << "max number of alignedBoxes: " << alignedBoundingBoxes.size();
    // cut out found rectangles from edged image
    for (int i = 0; i < alignedBoundingBoxes.size(); ++i) {
        cv::Rect roi = alignedBoundingBoxes[i];
        _digits.push_back(img_ret(roi));

        rlog << log4cpp::Priority::INFO << "Digit: " << i << " " << roi;
		roi.x +=_digitsRegion.x;
        roi.y += _digitsRegion.y;
        if (_debugDigits) {
            cv::rectangle(_img, roi, cv::Scalar(255, 255, 0), 2);
            cv::putText(_img, std::to_string(i+1), cv::Point2d(roi.x, roi.y), cv::FONT_HERSHEY_PLAIN, 0.99f, cv::Scalar(0,0,255) );
            cv::putText(_img, std::to_string(i+1)+": "+std::to_string(floor(1000*roi.height / _img.rows)/10 )+ "%", cv::Point2d(0.2f * _img.rows, (i+1) * (15)), cv::FONT_HERSHEY_PLAIN, 0.99f, cv::Scalar(0,0,255) );
         
        }
    }
    
    //if(_digits.size() != _config.getDigitCount()) {
     //     rlog << log4cpp::Priority::ERROR << "Expected : " << _config.getDigitCount()<< " digits. Got: " << _digits.size();
        
   // }
}

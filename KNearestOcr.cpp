/*
 * KNearestOcr.cpp
 *
 * OCR to train and recognize digits with the KNearest model.
 *
 */

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/ml/ml.hpp>

#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>
#include <map>
#include <iostream>
#include <exception>

#include "KNearestOcr.h"

KNearestOcr::KNearestOcr(const Config & config) :
#if CV_MAJOR_VERSION == 2
    _pModel(0),
#elif CV_MAJOR_VERSION == 3 | 4
    _pModel(),
#endif
    _config(config) {
}

KNearestOcr::~KNearestOcr() {
#if CV_MAJOR_VERSION == 2
    if (_pModel) {
        delete _pModel;
    }
#endif
}

/**
 * Learn a single digit.
 */
int KNearestOcr::learn(const cv::Mat & img) {
    cv::imshow("Learn", img);
    int key = cv::waitKey(0) & 255;
	switch(key) {
		case 176: key = 48; // zero on numpad
		break;
		case 177: key = 49; // 1 on numpad
		break;
		case 178: key = 50; // 2 on numpad
		break;
		case 179: key = 51; // 3 on numpad
		break;
		case 180: key = 52; // 4 on numpad
		break;
		case 181: key = 53; // 5 on numpad
		break;
		case 182: key = 54; // 6 on numpad
		break;
		case 183: key = 55; // 7 on numpad
		break;
		case 184: key = 56; // 8 on numpad
		break;
		case 185: key = 57; // 9 on numpad
		break;
	}
	std::cout<< "Key pressed  "<<char(key)<<"\n";
    if (key >= '0' && key <= '9') {
        _responses.push_back(cv::Mat(1, 1, CV_32F, (float) key - '0'));
        _samples.push_back(prepareSample(img));
		std::cout<< "Learned so far: "<<_responses.size()<<"\n";
		//saveTrainingData();
    }else if (key != 's' && key != 'q' ){
		std::cout<< "Unrecognized key pressed. Only 1-9, 's','q' are allowed \n";
	}
	
    return key;
}

/**
 * Learn a vector of digits.
 */
int KNearestOcr::learn(const std::vector<cv::Mat>& images) {
    int key = 0;
    for (std::vector<cv::Mat>::const_iterator it = images.begin();
            it < images.end() && key != 's' && key != 'q'; ++it) {
        key = learn(*it);
    }
    return key;
}

bool KNearestOcr::hasTrainingData() {
    return !_samples.empty() && !_responses.empty();
}

/**
 * Save training data to file.
 */
void KNearestOcr::saveTrainingData() {
    cv::FileStorage fs(_config.getTrainingDataFilename(), cv::FileStorage::WRITE);
    fs << "samples" << _samples;
    fs << "responses" << _responses;
    fs.release();
}
std::string KNearestOcr::getStatitics() {
	if(!this->hasTrainingData()) {
		throw new std::exception();
	}
	std::string ret;
	cv::MatIterator_<cv::Mat> it; // = src_it.begin<cv::Vec3b>();
	std::map<int, int> histogram;
	ret.append("Trained responses: "); ret.append(std::to_string(_responses.rows));
	ret.append("\n");
	for(int i=0; i<_responses.rows; i++) {				
		float value = _responses.at<float>(i,0);
		int valueInt = (int)value;
		 if (histogram.find(valueInt) != histogram.end()) {
			 histogram[valueInt] ++;
		 }else {
			 histogram[valueInt] = 1;
		 }
	}
	ret.append("Trained responses histogram:\n");
	// Create a map iterator and point to beginning of map
	std::map<int, int>::iterator it2 = histogram.begin();
	while (it2 != histogram.end())
	{	
		ret.append("Digit '"+ std::to_string(it2->first)+"'->");		
		ret.append(std::to_string(it2->second));
		ret.append("\n");
		it2++;
	}
	return ret;
	
}

/**
 * Load training data from file and init model.
 */
bool KNearestOcr::loadTrainingData() {
    cv::FileStorage fs(_config.getTrainingDataFilename(), cv::FileStorage::READ);
    if (fs.isOpened()) {
        fs["samples"] >> _samples;
        fs["responses"] >> _responses;
        fs.release();

        initModel();
    } else {
        return false;
    }
    return true;
}

/**
 * Recognize a single digit.
 */
char KNearestOcr::recognize(const cv::Mat& img) {
    log4cpp::Category& rlog = log4cpp::Category::getRoot();
    char cres = '?';
    try {
#if CV_MAJOR_VERSION == 2
        if (!_pModel) {
#elif CV_MAJOR_VERSION == 3 | 4
        if (_pModel.empty()) {
#endif
            throw std::runtime_error("Model is not initialized");
        }
        cv::Mat results, neighborResponses, dists;
#if CV_MAJOR_VERSION == 2
        float result = _pModel->find_nearest(prepareSample(img), 2, results, neighborResponses, dists);
#elif CV_MAJOR_VERSION == 3 | 4
        float result = _pModel->findNearest(prepareSample(img), 2, results, neighborResponses, dists);
#endif
        if (0 == int(neighborResponses.at<float>(0, 0) - neighborResponses.at<float>(0, 1))
                && dists.at<float>(0, 0) < _config.getOcrMaxDist()) {
            // valid character if both neighbors have the same value and distance is below ocrMaxDist
            cres = '0' + (int) result;
        }else if (0 != int(neighborResponses.at<float>(0, 0) - neighborResponses.at<float>(0, 1))
                && (dists.at<float>(0, 0) == 0 || dists.at<float>(0, 1) == 0)) {
            // valid character if neighbours are different but one distance is 0 
			//rlog<< log4cpp::Priority::DEBUG <<" Neigh 0: "<<int(neighborResponses.at<float>(0, 0) << " neigh 1: "<<int(neighborResponses.at<float>(0, 1) 
			
			result =  dists.at<float>(0, 0) == 0 ? int(neighborResponses.at<float>(0, 0)) : int(neighborResponses.at<float>(0, 1) );
            cres = '0' + (int) result;
        }
		
		else if (rlog.isInfoEnabled()) {
            rlog << log4cpp::Priority::INFO << "OCR rejected: " << (int) result;
        }
        rlog << log4cpp::Priority::DEBUG << "results: " << results;
        rlog << log4cpp::Priority::DEBUG << "neighborResponses: " << neighborResponses;
        rlog << log4cpp::Priority::DEBUG << "dists: " << dists;
    } catch (std::exception & e) {
        rlog << log4cpp::Priority::ERROR << e.what();
    }
    return cres;
}

/**
 * Recognize a vector of digits.
 */
std::string KNearestOcr::recognize(const std::vector<cv::Mat>& images) {
    std::string result;
    for (std::vector<cv::Mat>::const_iterator it = images.begin();
            it != images.end(); ++it) {
        result += recognize(*it);
    }
    return result;
}

/**
 * Prepare an image of a digit to work as a sample for the model.
 */
cv::Mat KNearestOcr::prepareSample(const cv::Mat& img) {
    cv::Mat roi, sample;
	//cv::imshow("img ", img);
  //    cv::resize(img, roi, cv::Size(28, 28));

    cv::resize(img, roi, cv::Size(28, 28), cv::INTER_AREA );
//	cv::imshow("img resize", roi);
    
//	cv::dilate(roi,  roidil, cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3)));//, cv::Size(3,3));
//	cv::imshow("roidil", roidil);
  
//	    roi = cv2.resize(roi, (28, 28), interpolation=cv2.INTER_AREA)
   //         roi = cv2.dilate(roi, (3, 3))
	
    roi.reshape(1, 1).convertTo(sample, CV_32F);
	
//	cv::waitKey(1);
	
    return sample;
}

/**
 * Initialize the model.
 */
void KNearestOcr::initModel() {
#if CV_MAJOR_VERSION == 2
    _pModel = new CvKNearest(_samples, _responses);
#elif CV_MAJOR_VERSION == 3 | 4
    _pModel = cv::ml::KNearest::create();
    // load persistent model
    cv::Ptr<cv::ml::TrainData> trainData = cv::ml::TrainData::create(_samples, cv::ml::ROW_SAMPLE, _responses);
    _pModel->train(trainData);
#endif
}

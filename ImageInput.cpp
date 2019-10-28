/*
 * ImageInput.cpp
 *
 */

#include <ctime>
#include <string>
#include <list>
#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>
#include <libexif/exif-data.h>
#include <libexif/exif-utils.h>

#include "ImageInput.h"

ImageInput::~ImageInput() {
}

cv::Mat& ImageInput::getImage() {
    return _img;
}

time_t ImageInput::getTime() {
    return _time;
}

void ImageInput::setOutputDir(const std::string & outDir) {
    _outDir = outDir;
}

void ImageInput::saveImage() {
    struct tm date;
    localtime_r(&_time, &date);
    char filename[PATH_MAX];
    strftime(filename, PATH_MAX, "/%Y%m%d-%H%M%S.png", &date);
    std::string path = _outDir + filename;
    if (cv::imwrite(path, _img)) {
        log4cpp::Category::getRoot() << log4cpp::Priority::INFO << "Image saved to " + path;
    }
}

DirectoryInput::DirectoryInput(const Directory& directory) :
        _directory(directory) {
    _filenameList = _directory.list();
    _filenameList.sort();
    _itFilename = _filenameList.begin();
}

tm DirectoryInput::readFilenameDate(const std::string filename) {
	  // read time from file name
    struct tm date;
    memset(&date, 0, sizeof(date));
    date.tm_year = atoi(_itFilename->substr(0, 4).c_str()) - 1900;
    date.tm_mon = atoi(_itFilename->substr(4, 2).c_str()) - 1;
    date.tm_mday = atoi(_itFilename->substr(6, 2).c_str());
    date.tm_hour = atoi(_itFilename->substr(9, 2).c_str());
    date.tm_min = atoi(_itFilename->substr(11, 2).c_str());
    date.tm_sec = atoi(_itFilename->substr(13, 2).c_str());
	return date;
}

tm DirectoryInput::readExifCreatedDate(const std::string fullpath) {
	
	ExifData *exifData = exif_data_new_from_file((fullpath).c_str());
	if (exifData) {
    ExifByteOrder byteOrder = exif_data_get_byte_order(exifData);
	
    ExifEntry *exifEntry = exif_data_get_entry(exifData,
                                               EXIF_TAG_DATE_TIME_ORIGINAL);
			
	char buf[1024];
	exif_entry_get_value(exifEntry, buf, sizeof(buf));			
	struct tm date;
    memset(&date, 0, sizeof(date));
	std::string bufstr(buf);
    date.tm_year = atoi(bufstr.substr(0, 4).c_str()) - 1900;
    date.tm_mon = atoi(bufstr.substr(5, 2).c_str()) - 1;
    date.tm_mday = atoi(bufstr.substr(8, 2).c_str());
    date.tm_hour = atoi(bufstr.substr(11, 2).c_str());
    date.tm_min = atoi(bufstr.substr(14, 2).c_str());
    date.tm_sec = atoi(bufstr.substr(17, 2).c_str());
 //   if (exifEntry)
  //    ExifSRational ret = exif_get_srational(exifEntry->data, byteOrder);

	//std::cout<<orientation;
    exif_data_free(exifData);
	return date;
  }
}

std::string DirectoryInput::getCurrentFilename() {
	return _curFilename;
}
bool DirectoryInput::nextImage() {
    if (_itFilename == _filenameList.end()) {
        return false;
    }
    std::string path = _directory.fullpath(*_itFilename);
	_curFilename = *_itFilename;
    _img = cv::imread(path.c_str());

	tm date = DirectoryInput::readExifCreatedDate(path);
//tm date2 = DirectoryInput::readFilenameDate(*_itFilename);
	
	
    _time = mktime(&date);

    log4cpp::Category::getRoot() << log4cpp::Priority::INFO << "Processing " << *_itFilename << " of " << ctime(&_time);

    // save copy of image if requested
    if (!_outDir.empty()) {
        saveImage();
    }

    _itFilename++;
    return true;
}

CameraInput::CameraInput(int device) {
	
    //_capture.open(device);
	//_capture.set(cv::CAP_PROP_FRAME_WIDTH, 800);
	//_capture.set(cv::CAP_PROP_FRAME_HEIGHT, 600);
	//_capture.set(cv::CAP_PROP_FPS, 5);
	
	//_capture.set(cv::CAP_PROP_AUTO_EXPOSURE, 0.25);
	//_capture.set(cv::CAP_PROP_EXPOSURE, 0.1);
	
}
bool CameraInput::nextImage() {
    time(&_time);
    // read image from camera
 //   bool success = _capture.read(_img);
	std::string path = "/dev/shm/image_emeocv.jpg";
	
	std::string str = "raspistill -w 800 -h 600  -o "; 
	str = str + path;
	const char *command = str.c_str();
	
	std::cout << "Capture using commmand: " << command << std::endl; 
	int status =system(command); 
	
	_img = cv::imread(path.c_str());

  
	bool success = status ==0 && _img.data != NULL;
    log4cpp::Category::getRoot() << log4cpp::Priority::INFO << "Image captured: " << success;

    // save copy of image if requested
    if (success && !_outDir.empty()) {
        saveImage();
    }

    return success;
}



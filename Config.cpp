/*
 * Config.cpp
 *
 */

#include <opencv2/highgui/highgui.hpp>

#include "Config.h"

Config::Config() :
        _rotationDegrees(0), _digitCount(7), _ocrMaxDist(2100000), _digitYAlignment(20), _cannyThreshold1(160), _cannyThreshold2(
                255), _trainingDataFilename("trainctr.yml"), _cliCaptureTemporaryPath("/dev/shm/image_emeocv.png"), _cliCaptureCommand("raspistill -w 800 -h 600 -n -o"), _counterMarkerHLOW(30), _counterMarkerHHI(85), _counterMarkerSLOW(35), _counterMarkerVLOW(80), _maxImageHeight(600) {
}


void Config::saveConfig() {
    cv::FileStorage fs("config.yml", cv::FileStorage::WRITE);
    fs << "rotationDegrees" << _rotationDegrees;
    fs << "cannyThreshold1" << _cannyThreshold1;
    fs << "cannyThreshold2" << _cannyThreshold2;
    fs << "digitYAlignment" << _digitYAlignment;
    fs << "ocrMaxDist" << _ocrMaxDist;
    fs << "trainingDataFilename" << _trainingDataFilename;	
    fs << "cliCaptureCommand" << _cliCaptureCommand;
    fs << "cliCaptureTemporaryPath" << _cliCaptureTemporaryPath;
    fs << "counterMarkerHLOW" << _counterMarkerHLOW;
    fs << "counterMarkerHHI" << _counterMarkerHHI;
    fs << "counterMarkerSLOW" << _counterMarkerSLOW;
    fs << "counterMarkerVLOW" << _counterMarkerVLOW;
    fs << "maxImageHeight" << _maxImageHeight;
    fs << "digitCount" << _digitCount;
    fs.release();
}

void Config::loadConfig() {
    cv::FileStorage fs("config.yml", cv::FileStorage::READ);
    if (fs.isOpened()) {
        fs["rotationDegrees"] >> _rotationDegrees;
        fs["cannyThreshold1"] >> _cannyThreshold1;
        fs["cannyThreshold2"] >> _cannyThreshold2;
        fs["digitYAlignment"] >> _digitYAlignment;
        fs["ocrMaxDist"] >> _ocrMaxDist;
        fs["trainingDataFilename"] >> _trainingDataFilename;
        fs["cliCaptureCommand"] >> _cliCaptureCommand;
        fs["cliCaptureTemporaryPath"] >> _cliCaptureTemporaryPath;
        fs["counterMarkerHLOW"] >> _counterMarkerHLOW;
        fs["counterMarkerHHI"] >> _counterMarkerHHI;
        fs["counterMarkerSLOW"] >> _counterMarkerSLOW;
        fs["counterMarkerVLOW"] >> _counterMarkerVLOW;
        fs["maxImageHeight"] >> _maxImageHeight;
        fs["digitCount"] >> _digitCount;
        fs.release();
    } else {
        // no config file - create an initial one with default values
        saveConfig();
    }
}


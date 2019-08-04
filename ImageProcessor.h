/*
 * ImageProcessor.h
 *
 */

#ifndef IMAGEPROCESSOR_H_
#define IMAGEPROCESSOR_H_

#include <vector>

#include <opencv2/imgproc/imgproc.hpp>

#include "Config.h"
#include "ImageInput.h"

#include "AbstractDigitBoundingBoxesExtractor.h"

class ImageProcessor
{
public:
    ImageProcessor(const Config& config);
	~ImageProcessor();

    void setOrientation(int rotationDegrees);
    void setInput(cv::Mat& img);
    void process();
    const std::vector<cv::Mat>& getOutput();

    void debugWindow(bool bval = true);
    void debugSkew(bool bval = true);
    void debugEdges(bool bval = true);
    void debugDigits(bool bval = true);
    void showImage();
    void saveConfig();
    void loadConfig();
    cv::Mat resize(cv::Mat& image, cv::Size size);
    cv::Mat replaceRedWithBlack(cv::Mat& img);

   

private:
    void rotate(double rotationDegrees);
    void findCounterDigits();
    std::vector<cv::Rect> findAlignedBoxesFromCounterArea(cv::Mat edges);
    std::vector<cv::Rect> findAlignedBoxesFromCounterAreaManual(cv::Mat edges);
    cv::Rect findCounterArea(cv::Mat& edges);


    float detectSkew();
    void drawLines(std::vector<cv::Vec2f>& lines);
    void drawLines(std::vector<cv::Vec4i>& lines, int xoff = 0, int yoff = 0);
    cv::Mat cannyEdges(cv::Mat& image, int lower, int upper);

    void createGray();


    cv::Mat _img;
    cv::Mat _imgGray;
    cv::Rect2d _digitsRegion;
    std::vector<cv::Mat> _digits;
    Config _config;
  
    bool _debugWindow;
    bool _debugSkew;
    bool _debugEdges;
    bool _debugDigits;
	
	AbstractDigitBoundingBoxesExtractor* _boundingBoxExtractor;
};

#endif /* IMAGEPROCESSOR_H_ */

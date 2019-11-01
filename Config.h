/*
 * Config.h
 *
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>

class Config
{
public:
    Config();

    int getCounterMarkerHHI() const
    {
	return _counterMarkerHHI;
    }
    int getCounterMarkerHLOW() const
    {
	return _counterMarkerHLOW;
    }
    int getCounterMarkerVLOW() const
    {
	return _counterMarkerVLOW;
    }
    int getCounterMarkerSLOW() const
    {
	return _counterMarkerSLOW;
    }
    void saveConfig();
    void loadConfig();

    int getDigitYAlignment() const
    {
	return _digitYAlignment;
    }

    std::string getTrainingDataFilename() const
    {
	return _trainingDataFilename;
    }
	
	std::string getCliCaptureCommand() const {return _cliCaptureCommand;}
	std::string getCliCaptureTemporaryPath() const {return _cliCaptureTemporaryPath;}

    float getOcrMaxDist() const
    {
	return _ocrMaxDist;
    }

    int getRotationDegrees() const
    {
	return _rotationDegrees;
    }

    int getCannyThreshold1() const
    {
	return _cannyThreshold1;
    }

    int getCannyThreshold2() const
    {
	return _cannyThreshold2;
    }
    int getDigitCount() const
    {
	return _digitCount;
    }
    int getMaxImageHeight() const
    {
	return _maxImageHeight;
    }
	

private:
    int _rotationDegrees;
    float _ocrMaxDist;
    int _digitYAlignment;
    int _cannyThreshold1;
    int _cannyThreshold2;
    int _counterMarkerHLOW;
    int _counterMarkerHHI;
    int _counterMarkerSLOW;
    int _counterMarkerVLOW;
    std::string _trainingDataFilename;
    int _maxImageHeight;
    std::string _cliCaptureCommand;
    std::string _cliCaptureTemporaryPath;

   
    int _digitCount;
};

#endif /* CONFIG_H_ */

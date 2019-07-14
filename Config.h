/*
 * Config.h
 *
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>

class Config {
public:
    Config();
    void saveConfig();
    void loadConfig();

    float getDigitMaxHeight() const {
        return _digitMaxHeight;
    }

    float getDigitMinHeight() const {
        return _digitMinHeight;
    }

    int getDigitYAlignment() const {
        return _digitYAlignment;
    }

    std::string getTrainingDataFilename() const {
        return _trainingDataFilename;
    }

    float getOcrMaxDist() const {
        return _ocrMaxDist;
    }

    int getRotationDegrees() const {
        return _rotationDegrees;
    }

    int getCannyThreshold1() const {
        return _cannyThreshold1;
    }

    int getCannyThreshold2() const {
        return _cannyThreshold2;
    }

private:
    int _rotationDegrees;
    float _ocrMaxDist;
    float _digitMinHeight;
    float _digitMaxHeight;
    int _digitYAlignment;
    int _cannyThreshold1;
    int _cannyThreshold2;
    std::string _trainingDataFilename;
};

#endif /* CONFIG_H_ */

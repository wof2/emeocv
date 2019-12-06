/*
 * Plausi.h
 *
 */

#ifndef PLAUSI_H_
#define PLAUSI_H_

#include <string>
#include <deque>
#include <utility>
#include <ctime>
#include "RecognitionStats.h"
#include "Config.h"

class Plausi {
public:
    Plausi(const Config& config, double maxPower = 50. /*kW*/, int window = 13);
    bool check(const std::string & value, time_t time);
    double getCheckedValue();
    time_t getCheckedTime();
	std::string getStats();
	void resetStats();
	void registerNotRecognized();

private:
    std::string queueAsString();

    double _maxPower;
    int _window;
    std::deque<std::pair<time_t, double> > _queue;
    time_t _time;
    double _value;
    Config _config;
	RecognitionStats _stats;

};

#endif /* PLAUSI_H_ */

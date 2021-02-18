/*
 * Plausi.cpp
 *
 */

#include <string>
#include <deque>
#include <utility>
#include <ctime>
#include <cstdlib>
#include <unistd.h>

#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>

#include "Plausi.h"

Plausi::Plausi(const Config & config, double maxPower, int window) :
        _config(config), _maxPower(maxPower), _window(window), _value(-1.), _time(0), _stats() {
}

bool Plausi::check(const std::string& value, time_t time) {
    log4cpp::Category& rlog = log4cpp::Category::getRoot();

    if (rlog.isInfoEnabled()) {
        rlog.info("Plausi check: %s of %s", value.c_str(), ctime(&time));
    }

    if (value.length() != _config.getDigitCount()) {
        // exactly 7 digits
        rlog.info("Plausi rejected: exactly 7 digits");
		_stats.incrNotRecognized();
        return false;
    }
    if (value.find_first_of('?') != std::string::npos) {
        // no '?' char
        rlog.info("Plausi rejected: no '?' char");
        _stats.incrNotRecognized();
		return false;
    }


    double dval = atof(value.c_str()) / 10.;
    _queue.push_back(std::make_pair(time, dval));
  if (rlog.isDebugEnabled()) {
        rlog.debug(queueAsString());
    }
    if (_queue.size() < _window) {
        rlog.info("Plausi rejected: not enough values: %d", _queue.size());
		_stats.incrRecognized();
        return false;
    }
    if (_queue.size() > _window) {
        _queue.pop_front();
    }

    // iterate through queue and check that all values are ascending
    // and consumption of energy is less than limit
    std::deque<std::pair<time_t, double> >::const_iterator it = _queue.begin();
    time = it->first;
    dval = it->second;
    ++it;
    while (it != _queue.end()) {
        if (it->second < dval) {
            // value must be >= previous value
            rlog.info("Plausi rejected: value in queue (%.3f) must be >= previous value in queue (%.3f)", it->second, dval );
			if(dval - it->second > 1.0f) {
					 rlog.error("Rejected value way smaller than the last");
					 usleep(1000*1000*10);

			}

			//exit(1);
			_stats.incrRecognized();
            return false;
        }
        double power = (it->second - dval) / (it->first - time) * 3600.;
        if (power > _maxPower) {
            // consumption of energy must not be greater than limit
            rlog.info("Plausi rejected: consumption of energy %.3f must not be greater than limit %.3f", power, _maxPower);
			_stats.incrRecognized();
            return false;
        }
        time = it->first;
        dval = it->second;
        ++it;
    }

    // values in queue are ok: use the center value as candidate, but test again with latest checked value
    if (rlog.isDebugEnabled()) {
        rlog.debug(queueAsString());
    }
    time_t candTime = _queue.at(_window/2).first;
    double candValue = _queue.at(_window/2).second;
    if (candValue < _value) {
        rlog.info("Plausi rejected: value must be >= previous checked value");
		_stats.incrRecognized();
        return false;
    }
    double power = (candValue - _value) / (candTime - _time) * 3600.;
    if (power > _maxPower) {
        rlog.info("Plausi rejected: consumption of energy (checked value) %.3f must not be greater than limit %.3f", power, _maxPower);
		_stats.incrRecognized();
        return false;
    }

    // everything is OK -> use the candidate value
    _time = candTime;
    _value = candValue;

	_stats.incrRecAndPlausiChecked();
    if (rlog.isInfoEnabled()) {
        rlog.info("Plausi accepted: %.1f of %s", _value, ctime(&_time));
    }
    return true;
}

double Plausi::getCheckedValue() {
    return _value;
}

time_t Plausi::getCheckedTime() {
    return _time;
}

std::string Plausi::queueAsString() {
    std::string str;
    char buf[20];
    str += "[";
    std::deque<std::pair<time_t, double> >::const_iterator it = _queue.begin();
    for (; it != _queue.end(); ++it) {
        sprintf(buf, "%.1f", it->second);
        str += buf;
        str += ", ";
    }
    str += "]";
    return str;
}

std::string Plausi::getStats()
{
	return _stats.getStatistics();
}

void Plausi::resetStats()
{
	_stats.reset();
}

void Plausi::registerNotRecognized()
{
	_stats.incrNotRecognized();
}

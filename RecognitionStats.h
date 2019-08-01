#ifndef RECOGNITIONSTATS_H
#define RECOGNITIONSTATS_H

#include <unistd.h>
#include <iostream>     // std::cout
#include <sstream>    
#include <string>

class RecognitionStats
{
public:
	RecognitionStats();
	~RecognitionStats();
	void reset();
	std::string getStatistics();
	void incrRecognized();
	void incrNotRecognized();
	void incrRecAndPlausiChecked();
	
private:
	int recognized = 0;
	int rcandplausichecked = 0;
	int all = 0;
	
};

#endif // RECOGNITIONSTATS_H

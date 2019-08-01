#include "RecognitionStats.h"

RecognitionStats::RecognitionStats()
{
}

RecognitionStats::~RecognitionStats()
{
}

void RecognitionStats::reset()
{
	recognized = all = rcandplausichecked =0;
	
}

std::string RecognitionStats::getStatistics()
{
	std::stringstream sstm;
	std::string ret;
	 sstm << ret +  "Recognized count: "<< recognized << "/" << all << " plausi: " << rcandplausichecked << "/" << all;
	 return sstm.str();
}

void RecognitionStats::incrRecognized()
{
	++recognized; ++all;
}

void RecognitionStats::incrRecAndPlausiChecked()
{
	++rcandplausichecked; incrRecognized();
}


void RecognitionStats::incrNotRecognized()
{
	++all;
}

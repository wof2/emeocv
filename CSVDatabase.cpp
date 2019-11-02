/*
 * CSVDatabase.cpp
 *
 */

#include <iostream>

#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>

#include "CSVDatabase.h"

CSVDatabase::CSVDatabase(const char* filename) : _handle(filename, std::ofstream::app) {
    _filename = new char[strlen(filename) + 1];
    strcpy(_filename, filename);
	if(_handle.tellp() == 0) {
		_handle << "DATE_TIME,TIMESTAMP,kWh,WS,\n";
		
	}		
}

CSVDatabase::~CSVDatabase() {	
    delete[] _filename;
	_handle.close();
}


int CSVDatabase::update(time_t rawtime, double counter) {
	char values[256];	
	struct tm * timeinfo;
	char buffer[80];
	//time (&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",timeinfo);
	std::string str(buffer);
    snprintf(values, 255, "%s,%ld,%.1f,%.0f,\n", buffer,(long)rawtime, counter/*kWh*/, counter * 3600000. /*Ws*/);
  
	_handle << values;
	_handle.flush();
    //if (res) {
   //     log4cpp::Category::getRoot() << log4cpp::Priority::ERROR << rrd_get_error();
   // }
	
	return 0;

}

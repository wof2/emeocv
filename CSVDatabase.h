/*
 * CSVDATABASE.h
 *
 */

#ifndef CSVDATABASE_H_
#define CSVDATABASE_H_
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>

class CSVDatabase {
public:
    CSVDatabase(const char* filename);
    ~CSVDatabase();
    int update(time_t time, double value);
	
private:
    char* _filename;
	std::ofstream _handle;	
};

#endif /* CSVDATABASE_H_ */

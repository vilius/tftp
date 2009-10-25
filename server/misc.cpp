
#ifndef MISC_UTILS
#define MISC_UTILS

#include <string>
#include <iostream>
#include <fstream>

bool fileExists(char* filename) {

	bool flag = false;
	fstream fin;
	fin.open(filename, ios::in);
	if (fin.is_open()) {
		flag = true;
	}
	fin.close();

	return false;

}

#endif
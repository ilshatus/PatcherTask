#pragma once
#ifndef PATCHER_H
#define PATCHER_H

#include <fstream>

/*
	Flag which goes in the beginning of the file 
	and used to determine that file contains patch
*/
const int FLAG_LEN = 5;
const char FLAG[] = { 1, 2, 3, 4, 5, 0 };

/*
	Abstract class of patching algorithms
*/
class Patching {
protected:
	// first input file
	std::string file1_path;
	// second input file
	std::string file2_path;
public:
	Patching(std::string file1_path, std::string file2_path) : file1_path(file1_path), file2_path(file2_path) {}
	// will contain algorithm to manipulate with file1 and file2
	virtual bool apply() = 0;
	virtual ~Patching() {}
	// will decide which class to instantiate by looking at files
	static Patching* create_patching(std::string file1_path, std::string file2_path);
};

#endif

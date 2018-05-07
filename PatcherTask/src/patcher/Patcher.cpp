#include "Patcher.h"
#include <string>
#include <algorithm>

/*
	Class with algorithm to create patch file
*/
class PatchCreator : public Patching {

	// Get file name from file path
	std::string get_file_name(std::string file_path) {
		std::reverse(file_path.begin(), file_path.end());
		std::string file_name1 = file_path.substr(0, file_path.find_first_of('\\'));
		std::string file_name2 = file_path.substr(0, file_path.find_first_of('/'));
		if (file_name1.length() < file_name2.length()) {
			std::reverse(file_name1.begin(), file_name1.end());
			return file_name1;
		}
		std::reverse(file_name2.begin(), file_name2.end());
		return file_name2;
	}
public:
	PatchCreator(std::string file1_path, std::string file2_path) : Patching(file1_path, file2_path) {}
	bool apply() {
		std::ifstream file1(file1_path, std::ios_base::binary);
		std::ifstream file2(file2_path, std::ios_base::binary);

		if (!file1.is_open() || !file2.is_open()) {
			return false;
		}

		// Read first file as binary file to file1_buff
		file1.seekg(0, std::ios_base::end);
		int file1_size = file1.tellg();
		char* file1_buff = new char[file1_size];
		file1.seekg(0);
		file1.read(file1_buff, file1_size);
		file1.close();

		// Read second file as binary file to file2_buff
		file2.seekg(0, std::ios_base::end);
		int file2_size = file2.tellg();
		char* file2_buff = new char[file2_size];
		file2.seekg(0);
		file2.read(file2_buff, file2_size);
		file2.close();

		// Open file to write patch
		// Set name of patch file to file2's name with suffix "_patch"
		std::string file2_name = get_file_name(file2_path);
		std::ofstream patch(file2_name + "_patch", std::ios_base::binary);
		if (!patch.is_open()) {
			return false;
		}

		// Include second file's name to patch file and wrap it with FLAG bytes
		patch.write(FLAG, FLAG_LEN);
		patch.write(file2_name.c_str(), file2_name.length());
		patch.write(FLAG, FLAG_LEN);

		// Algorithm of creating of main part of patch
		// to each byte of second file apply XOR operation 
		// with corresponding(by module equal to size of this file) byte of first file 
		for (int i = 0; i < file2_size; i++) {
			char curr_byte = file2_buff[i] ^ file1_buff[i % file1_size];
			patch.write(&curr_byte, 1);
		}
		patch.close();
		
		delete[] file1_buff;
		delete[] file2_buff;

		return true;
	}
};

/*
	Class with algorithm to apply patch to file
*/
class PatchApplier : public Patching {

	// Get file name from patch file
	std::string get_file_name(std::ifstream &patch) {
		patch.seekg(FLAG_LEN);
		char c;
		patch.read(&c, 1);
		std::string file_name = "";
		while (c != FLAG[0]) {
			file_name += c;
			patch.read(&c, 1);
			if (!patch) return "";
		}
		return file_name;
	}
public:
	PatchApplier(std::string file1_path, std::string file2_path) : Patching(file1_path, file2_path) {}

	bool apply() {
		std::ifstream file1(file1_path, std::ios_base::binary);
		std::ifstream patch(file2_path, std::ios_base::binary);

		if (!file1.is_open() || !patch.is_open()) {
			return false;
		}

		// Read first file as binary file to file1_buff
		file1.seekg(0, std::ios_base::end);
		int file1_size = file1.tellg();
		char* file1_buff = new char[file1_size];
		file1.seekg(0);
		file1.read(file1_buff, file1_size);
		file1.close();

		// Try to get file name from patch file
		std::string file2_name = get_file_name(patch);
		if (file2_name == "") {
			return false;
		}

		// Compute header size: length of file name and two flags before and after
		// Read main part of patch to patch_buff
		int header_len = 2 * FLAG_LEN + file2_name.length();
		patch.seekg(0, std::ios_base::end);
		int patch_size = (int)patch.tellg() - header_len;
		char* patch_buff = new char[patch_size];
		patch.seekg(header_len);
		patch.read(patch_buff, patch_size);
		patch.close();

		// Open file wich will be the product of patching
		std::ofstream file2(file2_name, std::ios_base::binary);
		if (!file2.is_open()) {
			return false;
		}

		// Algorithm of patching (the inverse of the creation algorithm)
		// to each byte of patch file apply XOR operation 
		// with corresponding(by module equal to size of this file) byte of first file 
		for (int i = 0; i < patch_size; i++) {
			char curr_byte = patch_buff[i] ^ file1_buff[i % file1_size];
			file2.write(&curr_byte, 1);
		}
		file2.close();

		delete[] file1_buff;
		delete[] patch_buff;

		return true;
	}
};

/*
	Function to determine whether the file is patch
*/
bool is_patch(std::string file_path) {
	std::ifstream file(file_path, std::ios_base::binary);
	if (!file.is_open()) return 0;
	file.seekg(0, std::ios_base::end);
	if (file.tellg() < FLAG_LEN) return 0;
	file.seekg(0);
	char buff[FLAG_LEN + 1];
	buff[FLAG_LEN] = 0;
	file.read(buff, FLAG_LEN);
	file.close();
	return strcmp(buff, FLAG) == 0;
}

/*
	Method which instantiate object of patching sub-classes
	if second file is patch then PatchApplier
	otherwise PatchCreator
*/
Patching* Patching::create_patching(std::string file1_path, std::string file2_path) {
	Patching* p;
	if (is_patch(file2_path)) p = new PatchApplier(file1_path, file2_path);
	else p = new PatchCreator(file1_path, file2_path);
	return p;
}
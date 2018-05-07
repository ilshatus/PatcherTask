#include <iostream>
#include <string>
#include <fstream>
#include "Patcher.h"

using namespace std;

string tests_dir = "tests/";

bool equal(ifstream &file1, ifstream &file2) {
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

	bool result = true;

	if (file1_size != file2_size)
		result = false;
	for (int i = 0; i < file1_size; i++) {
		if (file1_buff[i] != file2_buff[i]) {
			result = false;
			break;
		}
	}

	delete[] file1_buff;
	delete[] file2_buff;
	return result;
}

int main()
{
	ifstream tests(tests_dir + "tests_list.txt");
	if (!tests) {
		cout << "tests_list.txt not found";
		return 0;
	}

	int n; // amount of test cases
	tests >> n;

	for (int i = 1; i <= n; i++) {
		string file1, file2;
		tests >> file1 >> file2;
		// Create patch with two input files
		Patching* patching = Patching::create_patching(tests_dir + file1, tests_dir + file2);
		if (patching->apply()) { // patch created succesfully
			delete patching;
			// Apply patch to file1
			patching = Patching::create_patching(tests_dir + file1, file2 + "_patch");
			if (patching->apply()) { // patch successfully applied
				ifstream file_original(tests_dir + file2, ios_base::binary); // original file
				ifstream file_test(file2, ios_base::binary); // file which was generated after patch application
				if (file_original && file_test) {
					if (!equal(file_original, file_test)) // files are not equal
						cout << "Test " << i << " failed (original and test files are not equal)\n";
					else // files are equal
						cout << "Test " << i << " passed\n";
				} 
				else {
					cout << "Test " << i << " failed (open files for comparison)\n";
				}
				file_original.close();
				file_test.close();
			}
			else {
				cout << "Test " << i << " failed (apply patch step)\n";
			}
		}
		else {
			cout << "Test " << i << " failed (create patch step)\n";
		}
	}
	tests.close();

	return 0;
}


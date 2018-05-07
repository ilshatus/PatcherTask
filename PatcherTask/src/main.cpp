#include <iostream>
#include <string>
#include "Patcher.h"

using namespace std;


int main(int argc, char** args)
{
	if (argc < 3) cout << "Arguments not passed\n";
	else {
		string file1(args[1]);
		string file2(args[2]);
		Patching* patching = Patching::create_patching(file1, file2);
		if (patching->apply()) {
			cout << "Done\n";
		}
		else {
			cout << "Not done\n";
		}
		delete patching;
	}
	return 0;
}


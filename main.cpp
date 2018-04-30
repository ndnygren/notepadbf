/* notepadbf - simple text editor with Blowfish encryption
* Copyright (C) 2012 Nick Nygren
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>. */

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include "CurseManager.h"
#include "blowfishStream.h"

using namespace std;

void testbf()
{
	blowfish *bf;
	char pass[] = "12345678";
	unsigned int in[2];
	unsigned int out[2];

	((unsigned int*)pass)[0] = 0x00000000;
	((unsigned int*)pass)[1] = 0x00000000;
	in[0] = 0x00000000;
	in[1] = 0x00000000;

	bf = new blowfish(pass, 8);
	bf->encblock(in, out);

	assert(out[0] == 0x4EF99745);
	assert(out[1] == 0x6198DD78);

	delete bf;

	((unsigned int*)pass)[0] = 0xFFFFFFFF;
	((unsigned int*)pass)[1] = 0xFFFFFFFF;
	in[0] = 0xFFFFFFFF;
	in[1] = 0xFFFFFFFF;

	bf = new blowfish(pass, 8);
	bf->encblock(in, out);

	assert(out[0] == 0x51866FD5);
	assert(out[1] == 0xB85ECB8A);


	vector<string> test = CurseManager::explode("dsfgsdfgb", "s");
	vector<string> test2 = CurseManager::explode("dsfgsdfgbs", "s");

	assert(test.size() == 3 && test[0] == "d"  && test[1] == "fg" && test[2] == "dfgb");
	assert(test2.size() == 4 && test2[0] == "d"  && test2[1] == "fg" && test2[2] == "dfgb");

	delete bf;
}


int encrypt(string filename, string password) {
	blowfishStream bf(password);
	std::ifstream ifs(filename);
	string body((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
	std::stringstream ssin(body), ss;

	if (ifs.fail() || ifs.bad())
	{
		cerr << "Error: Could not open file: " << filename << endl;
		return -1;
	}
	else
	{
		ifs.seekg(0, ios::beg);
	}

	bf.setOutStream(ss);
	bf.encrypt(ssin);

	cout << ss.str() << endl;
	return 0;
}

int decrypt(string filename, string password) {
	blowfishStream bf(password);
	fstream ifs;
	std::stringstream ss;

	ifs.open(filename.c_str(), ios::in | ios::binary | ios::ate);

	if (ifs.fail() || ifs.bad())
	{
		cerr << "Error: Could not open file: " << filename << endl;
		return -1;
	}
	else
	{
		ifs.seekg(0, ios::beg);
	}

	bf.setOutStream(ss);
	bf.decrypt(ifs);

	cout << ss.str() << endl;
	return 0;
}



int main(int argc, char **argv)
{
	testbf();

	if (argc < 3) { cerr << "Needs more arguments." << endl ; return -1; }

	string action = argv[1];
	string filename = argv[2];
	string password = "";

	cerr << "action: " << action << endl;
	cerr << "filename: " << filename << endl;
	cerr << "password: " << password << endl;

	if (action == "enc") {
		if (argc < 4) { cerr << "Needs more arguments." << endl ; return -1; }
		password = argv[3];
		return encrypt(filename, password);
	} else if (action == "dec") {
		if (argc < 4) { cerr << "Needs more arguments." << endl ; return -1; }
		password = argv[3];
		return decrypt(filename, password);
	} else if (action == "curses") {
		std::ifstream ifs(filename);
		string body((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
		ifs.close();
		CurseManager cm(body);
		if (cm.isSave()) {
			std::ofstream ofs(filename);
			ofs << cm.content();
			ofs.close();
		}
	} else {
		cerr << "error: action must be either \"enc\" or \"dec\"." << endl;
	}

	return 0;
}

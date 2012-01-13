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
#include <cassert>
#include "blowfishStream.h"
#include "gui/mainNPBFWindow.h"

using namespace std;

char copyright[] = "notepadbf v0.1 - simple text editor with Blowfish encryption Copyright (C) 2012 Nick Nygren";

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

	delete bf;
}

int main(int argc, char **argv)
{
	testbf();
	QApplication app(argc, argv);
	mainNPBFWindow window;
	window.show();
	app.exec();

	return 0;
}

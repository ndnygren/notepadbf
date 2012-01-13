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

#include "blowfishStream.h"
#include <cstdlib>

using namespace std;

void blowfishStream::setIV(unsigned int lhs, unsigned int rhs)
{
	iv[0] = lhs;
	iv[1] = rhs;
}

void blowfishStream::setOutStream(std::ostream& input)
{
	outstream = &input;
	outset = true;
}

void blowfishStream::encrypt(std::istream& input)
{
	unsigned int buffer[2], enc[2];
	unsigned int last[2];
	int readsize, i;

	if (!outset) { return; }

	input.seekg (0, ios::end);
	setIV(time(0),input.tellg());
	input.seekg (0, ios::beg);

	last[0] = iv[0]; last[1] = iv[1];

	while (8 == (readsize = input.readsome((char*)buffer, 8)))
	{
		for (i = 0; i < 8; i++)
		{
			*outstream << ((unsigned char*)last)[i];
		}

		bf->encblock(last, enc);
		last[0] = buffer[0] ^ enc[0];
		last[1] = buffer[1] ^ enc[1];
	}
	for (i = 0; i < 8; i++)
	{
		*outstream << ((unsigned char*)last)[i];
	}

	if (readsize != 0)
	{
		for (i = readsize; i < 8; i++)
		{
			((char*)buffer)[i] = 0;
		}

		bf->encblock(last, enc);
		last[0] = buffer[0] ^ enc[0];
		last[1] = buffer[1] ^ enc[1];

		for (i = 0; i < 8; i++)
		{
			*outstream << ((unsigned char*)last)[i];
		}
	}
}

void blowfishStream::decrypt(std::istream& input)
{
	unsigned int buffer[2], dec[2];
	unsigned int last[2];
	int readsize, i;
	unsigned int writesize = 0;

	if (!outset) { return; }

	readsize = input.readsome((char*)last, 8);

	iv[0] = last[0]; iv[1] = last[1];

	while (8 == (readsize = input.readsome((char*)buffer, 8)))
	{
		bf->encblock(last, dec);
		dec[0] = buffer[0] ^ dec[0];
		dec[1] = buffer[1] ^ dec[1];
		last[0] = buffer[0]; last[1] = buffer[1];
		
		for (i = 0; i < 8; i++)
		{
			if (writesize < iv[1])
			{
				*outstream << ((unsigned char*)dec)[i];
				writesize++;
			}
		}

	}
	

}

blowfishStream::blowfishStream(const std::string& password)
{
	bf = new blowfish(password.c_str(), password.length());
	
	outset = false;
}

blowfishStream::~blowfishStream()
{
	delete bf;
}


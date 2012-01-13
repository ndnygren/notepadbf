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
#include "blowfish.h"

#ifndef NN_BLOWFISHSTREAM_H
#define NN_BLOWFISHSTREAM_H

class blowfishStream
{
	private:
	blowfish *bf;
	std::ostream *outstream;
	unsigned int iv[2];
	bool outset;

	public:
	void setIV(unsigned int lhs, unsigned int rhs);
	void setOutStream(std::ostream& input);
	void encrypt(std::istream& input);
	void decrypt(std::istream& input);

	blowfishStream(const std::string& password);
	~blowfishStream();
};

#endif


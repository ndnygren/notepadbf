/* blowfish - implementation of Bruce Schneier's Blowfish algorithm
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

#ifndef NN_BLOWFISH_H
#define NN_BLOWFISH_H

class blowfish
{
	/**
	 * @brief initial values for first s-box (hex pi)
	 */
	static const unsigned int tsbox0[];
	/**
	 * @brief initial values for second s-box (hex pi)
	 */
	static const unsigned int tsbox1[];
	/**
	 * @brief initial values for third s-box (hex pi)
	 */
	static const unsigned int tsbox2[];
	/**
	 * @brief initial values for forth s-box (hex pi)
	 */
	static const unsigned int tsbox3[];
	/**
	 * @brief initial values for p-array (hex pi)
	 */
	static const unsigned int tparray[];

	/**
	 * @brief the working first sbox
	 */
	unsigned int* sbox0;
	/**
	 * @brief the working second sbox
	 */
	unsigned int* sbox1;
	/**
	 * @brief the working third sbox
	 */
	unsigned int* sbox2;
	/**
	 * @brief the working forth sbox
	 */
	unsigned int* sbox3;

	/**
	 * @brief the working p-array
	 */
	unsigned int* parray;

	/**
	 * @brief stores the keystring
	 */
	unsigned char* keystring;

	/**
	 * @brief stores the keystring's size
	 */
	int keylength;

	/**
	 * @brief applies the f-box transformation to the input integer
	 */
	unsigned int fbox(unsigned int input) const;

	/**
	 * @brief loads the initial values into the working s-boxes
	 */
	void loadsboxes();	

	/**
	 * @brief modifies the p-array by adding the keystring
	 */
	void loadkey();

	/**
	 * @brief modifies the p-array by re-encrypting it using the key
	 */
	void modpbox();

	/**
	 * @brief modifies the s-boxes by re-encrypting them using the key
	 */
	void modsbox();

	/**
	 * @brief casts the byte 4-tuple into a int
	 */
	static unsigned int toints(const unsigned char* arr);

	/**
	 * @brief reverses the byte order of a byte 4-tuple
	 */
	static unsigned int rev(unsigned int num);


	public:

	/**
	 * @brief encrypts the 64-bits in the input array and outputs it to the output array
	 */
	void encblock(unsigned int *input, unsigned int *output) const;

	/**
	 * @brief decrypts the 64-bits in the input array and outputs it to the output array
	 */
	void decblock(unsigned int *input, unsigned int *output) const;

	blowfish(const char* keystring, int keysize);
	~blowfish();
};

#endif

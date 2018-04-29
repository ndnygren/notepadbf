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
#include <vector>
#include <curses.h>
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

	delete bf;
}


int encrypt(string filename, string password) {
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
	bf.encrypt(ifs);

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


class CurseManager  {

	private:
		static const int head_height = 3;
		static const int left_margin = 5;
		enum WinF { PASS, BODY, MENU } winf;
		int h,w;
		vector<string> rows;
		int top_line;
		int cx,cy;

		string filename;
		WINDOW *win, *pwin, *numwin;

	void run () {
		int ch;
		nodelay(stdscr, FALSE);
		while (true) {

			switch (winf) {
				case PASS:
					drawPMenu();
					break;
				case BODY:
					wmove(win, cy+1,cx+1);
					drawBMenu();
					break;
				case MENU:
					break;
			}
			if ((ch = getch()) == ERR) {
			}
			else {
				switch (ch) {
					case KEY_BACKSPACE:
						break;
					case KEY_UP:
						if (cy > 0) { cy--; }
						if (cx > rows[cy].size()) { cx =  rows[cy].size(); }
						break;
					case KEY_DOWN:
						if (cy+1 < rows.size()) { cy++; }
						if (cx > rows[cy].size()) { cx =  rows[cy].size(); }
						break;
					case KEY_LEFT:
						if (cx > 0) { cx--; }
						break;
					case KEY_RIGHT:
						if (cx < rows[cy].size()) { cx++; }
						break;
				}
			}
		}
	}

	void drawPMenu() {
		box(pwin, 0, 0);
		wrefresh(pwin);
	}

	void drawNMenu() {
		wrefresh(numwin);
	}

	void drawBMenu() {
		box(win, 0, 0);
		wrefresh(win);
	}

	void redrawLine(int line) {
		chtype ch;

		if (line < top_line || h - 2*head_height - 2 < line - top_line) {
			//line out of screen
			return;
		}
		int y = (line - top_line);
		int x = 0;
		string padded = to_string(line);
		for (int j = padded.length(); j < left_margin; j++) {
			padded += ' ';
		}
		wmove(win, y+1, x+1);
		wmove(numwin, y+1, x);
		for (int idx = 0; idx < rows[line].length(); idx++) {
			ch = rows[line][idx];
			waddch(win, ch);
		}
		for (int idx = 0; idx < padded.length(); idx++) {
			ch = padded[idx];
			waddch(numwin, ch);
		}
	}

	void redrawAllLines() {
		for (int i = 0; i < rows.size(); i++) {
			redrawLine(i);
		}
	}

	public :
	CurseManager (string fname) {
		cx = 0;
		cy = 0;
		chtype ch = 'x';
		top_line = 0;
		winf = BODY;
		initscr();
		cbreak();
		noecho();
		keypad(stdscr, TRUE);
		filename = fname;
		getmaxyx(stdscr, h, w);
		win = newwin(h - 2*head_height, w - left_margin, head_height, left_margin);
		numwin = newwin(h - 2*head_height, left_margin, head_height, 0);
		pwin = newwin(head_height, w, 0, 0);
		rows.push_back("row one");
		rows.push_back("row two");
		rows.push_back("row three");

		waddch(win,ch);

		nodelay(stdscr, TRUE);
		redrawAllLines();

		getch();
		drawBMenu();
		drawPMenu();
		drawNMenu();
		run();
	}

	~CurseManager() {
		endwin();
		cerr << "closing" << endl;
	}
};

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
		CurseManager cm(filename);
	} else {
		cerr << "error: action must be either \"enc\" or \"dec\"." << endl;
	}

	return 0;
}

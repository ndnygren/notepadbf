

#ifndef NN_CURSEMANAGER_H
#define NN_CURSEMANAGER_H

#include <vector>
#include <curses.h>
#include "blowfishStream.h"

using namespace std;

class CurseManager  {

	private:
		static const int head_height = 3;
		static const int left_margin = 5;
		enum WinF { PASS, BODY, MENU } winf;
		int h,w;
		vector<string> rows;
		int top_line;
		int cx,cy,px,xx;
		string key;

		string filecont;
		WINDOW *win, *pwin, *numwin, *xwin;

	void run () {
		int ch;
		nodelay(stdscr, FALSE);
		while (true) {

			switch (winf) {
				case PASS: drawPMenu(); break;
				case BODY: drawBMenu(); break;
				case MENU: drawXMenu(); break;
			}
			if ((ch = getch()) == ERR) {
			}
			else {
				switch (ch) {
					case KEY_BACKSPACE:
					case 127:
					case 330:
						if (winf == PASS) { backSpace(key, px); px > 0 ? px-- : px; keyChange(); }
						if (winf == BODY) { backSpace(rows[cy], cx); cx > 0 ? cx-- : cx; redrawLine(cy); }
						break;
					case KEY_UP:
						if (winf == BODY && cy > 0) { cy--; }
						if (cx > rows[cy].size()) { cx =  rows[cy].size(); }
						break;
					case KEY_DOWN:
						if (winf == BODY && cy+1 < rows.size()) { cy++; }
						if (cx > rows[cy].size()) { cx =  rows[cy].size(); }
						break;
					case KEY_LEFT:
						if (winf == PASS && px > 0) { px--; }
						if (winf == MENU && xx > 0) { xx--; }
						if (winf == BODY && cx > 0) { cx--; }
						break;
					case KEY_RIGHT:
						if (winf == PASS && px < key.size()) { px++; }
						if (winf == MENU && xx < 2) { xx++; }
						if (winf == BODY && cx < rows[cy].size()) { cx++; }
						break;
					case '\t':
						switch (winf) {
							case PASS: winf = BODY; break;
							case BODY: winf = MENU; break;
							case MENU: winf = PASS; break;
						}
						break;
					default:
						//if ( (ch >= 'a' && <= 'z') || (ch >= 'A' && <= 'Z') || (ch >= '0' && <= '9') ) {}
						if (winf == PASS) { keySpace(ch, key, px); px++; keyChange(); }
						if (winf == BODY) { keySpace(ch, rows[cy], cx); cx++; redrawLine(cy); }

						mvwprintw(pwin, 1,1, (to_string(ch) + "____").c_str());
						drawPMenu();
				}
			}
		}
	}

	void keyChange() {
		if (key.length() > 2) {
			rows = decrypt(filecont, key);
			if (top_line >= rows.size()) { top_line = 0; }
			if (cy >= rows.size()) { cy = 0; }
			redrawAllLines();
			drawBMenu();
			drawNMenu();
		}
	}

	void drawPMenu() {
		wmove(pwin, 1,1);
		for (int i = 0; i < w; i++) {
			waddch(pwin, i < key.length() ? '*' : ' ');
		}
		wmove(pwin, 1,1+px);
		box(pwin, 0, 0);
		wrefresh(pwin);
	}

	void drawXMenu() {
		wmove(xwin, 1,1 + 5*xx);
		box(xwin, 0, 0);
		wrefresh(xwin);
	}

	void drawNMenu() {
		wrefresh(numwin);
	}

	void drawBMenu() {
		wmove(win, cy+1,cx+1);
		box(win, 0, 0);
		wrefresh(win);
	}

	void redrawLine(int line) {
		chtype ch;

		if (line < top_line || h - 2*head_height - 2 <= line - top_line) {
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
		for (int idx = 0; idx < w - left_margin -2; idx++) {
			ch = idx < rows[line].length() ? rows[line][idx] : ' ';
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

	vector<string> decrypt(const string& input, const string& password) {
		blowfishStream bf(key);
		std::stringstream ssin(input), ssout;


		bf.setOutStream(ssout);
		bf.decrypt(ssin);

		return explode(ssout.str(), "\n");
	}

	public :

	static vector<string> explode(const string& input, const string& delim) {
		vector<string> output;
		size_t last = 0;
		size_t idx = input.find(delim, last);

		while (idx != string::npos) {
			output.push_back(input.substr(last, idx-last));
			last = idx+1;
			idx = input.find(delim, last);
		}
		output.push_back(input.substr(last));

		return output;
	}

	static void backSpace(string& input, int idx) {
		if (idx < 1 || idx > input.length()) { return; }
		input = input.substr(0, idx-1) + (idx < input.length() ? input.substr(idx) : "");
	}

	static void keySpace(char ch, string& input, int idx) {
		if (idx < 0 || idx > input.length()) { return; }
		input = input.substr(0, idx) + ch + (idx < input.length() ? input.substr(idx) : "");
	}

	CurseManager (string fcont) {
		cx = 0;
		cy = 0;
		px = 0;
		xx = 0;
		key = "password";
		chtype ch = 'x';
		top_line = 0;
		winf = BODY;
		initscr();
		cbreak();
		noecho();
		keypad(stdscr, TRUE);
		filecont = fcont;
		getmaxyx(stdscr, h, w);
		win = newwin(h - 2*head_height, w - left_margin, head_height, left_margin);
		numwin = newwin(h - 2*head_height, left_margin, head_height, 0);
		pwin = newwin(head_height, w, 0, 0);
		xwin = newwin(head_height, w, h-head_height, 0);
		rows.push_back("");

		waddch(win,ch);

		nodelay(stdscr, TRUE);
		redrawAllLines();

		getch();
		drawBMenu();
		drawPMenu();
		drawNMenu();
		drawXMenu();
		run();
	}

	~CurseManager() {
		endwin();
		cerr << "closing" << endl;
	}
};

#endif


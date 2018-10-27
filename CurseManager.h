

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
		static const int stat_split = 12;
		enum WinF { PASS, BODY, MENU } winf;
		int h,w;
		vector<string> rows;
		int top_line;
		int cx,cy,px,xx;
		string key;
		bool save, done;

		string filecont;
		WINDOW *win, *pwin, *numwin, *xwin, *statwin;

	void run () {
		int ch;
		nodelay(stdscr, FALSE);
		while (!done) {

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
						if (winf == BODY && cy > 0) { cy--; rowChange(); }
						break;
					case KEY_DOWN:
						if (winf == BODY && cy+1 < rows.size()) { cy++;  rowChange(); }
						break;
					case KEY_LEFT:
						if (winf == PASS && px > 0) { px--; }
						if (winf == MENU && xx > 0) { xx--; }
						if (winf == BODY && cx > 0) { cx--; }
						break;
					case KEY_RIGHT:
						if (winf == PASS && px < key.size()) { px++; }
						if (winf == MENU && xx < 1) { xx++; }
						if (winf == BODY && cx < rows[cy].size()) { cx++; }
						break;
					case '\t':
						switch (winf) {
							case PASS: winf = BODY; break;
							case BODY: winf = MENU; break;
							case MENU: winf = PASS; break;
						}
						break;
					case KEY_ENTER: case 10:
						if (winf == BODY) { newLine(); }
						if (winf == MENU) { exit(); }
						break;
					default:
						//if ( (ch >= 'a' && <= 'z') || (ch >= 'A' && <= 'Z') || (ch >= '0' && <= '9') ) {}
						if (winf == PASS) { keySpace(ch, key, px); px++; keyChange(); }
						if (winf == BODY) { keySpace(ch, rows[cy], cx); cx++; redrawLine(cy); }

						mvwprintw(statwin, 1,1, (to_string(ch) + "____").c_str());
						drawSMenu();
				}
			}
		}
	}


	void exit() {
		save = (xx == 0);
		done = true;
		mvwprintw(statwin, 1,1, "closing");
		drawSMenu();
		endwin();
	}

	void newLine() {
		string old = rows[cy];
		rows.insert( rows.begin() + cy , old.substr(0,cx));
		rows[cy+1] = old.substr(cx);
		cy++;
		rowChange();
	}

	void rowChange() {
		if (cx > rows[cy].size()) { cx =  rows[cy].size(); }
		if (cy < top_line) {
			top_line = cy;
		} else if (cy - top_line >= h - 2*head_height - 2) {
			top_line++;
		}
		redrawAllLines();
		mvwprintw(statwin, 1,1, (to_string(top_line) + " line").c_str());
		drawSMenu();
		drawNMenu();
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
		wmove(xwin, 1,1 + 7*xx);
		box(xwin, 0, 0);
		wrefresh(xwin);
	}

	void drawSMenu() {
		box(statwin, 0, 0);
		wrefresh(statwin);
	}

	void drawNMenu() {
		wrefresh(numwin);
	}

	void drawBMenu() {
		wmove(win, cy - top_line + 1, cx + 1);
		box(win, 0, 0);
		wrefresh(win);
	}

	void blankLine(int line) {
		int y = (line - top_line);
		int x = 0;
		wmove(win, y+1, x+1);
		wmove(numwin, y+1, x);
		for (int idx = 0; idx < w - left_margin -2; idx++) {
			chtype ch = ' ';
			waddch(win, ch);
		}
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
			if (containsIllegal(rows[line])) {
				ch = ' ';
			} else {
				ch = idx < rows[line].length() ? rows[line][idx] : ' ';
			}
			waddch(win, ch);
		}

		for (int idx = 0; idx < padded.length(); idx++) {
			ch = padded[idx];
			waddch(numwin, ch);
		}
	}

	bool containsIllegal(const string& input) const {
		for (int i = 0; i < input.length(); i++) {
			if (input[i] > 127 || input[i] < 1) {
				return true;
			}
		}
		return false;
	}

	void redrawAllLines() {
		for (int i = 0; i < rows.size(); i++) {
			redrawLine(i);
		}
		for (int i = rows.size(); i - top_line < h - 2*head_height - 2; i++) {
			blankLine(i);
		}
	}

	vector<string> decrypt(const string& input, const string& password) const {
		blowfishStream bf(key);
		std::stringstream ssin(input), ssout;


		bf.setOutStream(ssout);
		bf.decrypt(ssin);

		return explode(ssout.str(), "\n");
	}

	string encrypt(const string& input, const string& password) const {
		blowfishStream bf(key);
		std::stringstream ssin(input), ssout;


		bf.setOutStream(ssout);
		bf.encrypt(ssin);

		return ssout.str();
	}

	public :

	bool isSave() const { return save; }

	string content() const {
		string output = rows[0];
		for (int i = 1; i < rows.size(); i++) {
			output += '\n' + rows[i];
		}

		return encrypt(output, key);
	}

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
		xx = 0;
		key = "password";
		px = key.length();
		chtype ch = 'x';
		top_line = 0;
		save = false;
		done = false;
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
		xwin = newwin(head_height, w-stat_split, h-head_height, 0);
		statwin = newwin(head_height, stat_split, h-head_height, w-stat_split);
		rows.push_back("");

		waddch(win,ch);

		nodelay(stdscr, TRUE);
		redrawAllLines();

		getch();
		mvwprintw(xwin, 1,1, "SAVE   CANCEL");
		drawBMenu();
		drawPMenu();
		drawNMenu();
		drawXMenu();
		drawSMenu();
		run();
	}

	~CurseManager() {
		cerr << "closing " << isSave() << endl;
	}
};

#endif


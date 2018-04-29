

#ifndef NN_CURSEMANAGER_H
#define NN_CURSEMANAGER_H

#include <vector>
#include <curses.h>

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

		string filename;
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
						if (winf == PASS) { backSpace(key, px); px > 0 ? px-- : px; }
						if (winf == BODY) {
							backSpace(rows[cy], cx);
							cx > 0 ? cx-- : cx;
							redrawLine(cy);
						}
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
						mvwprintw(pwin, 1,1, (to_string(ch) + "____").c_str());
						drawPMenu();
				}
			}
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

	public :

	static void backSpace(string& input, int idx) {
		if (idx < 1 || idx > input.length()) { return; }
		input = input.substr(0, idx-1) + (idx < input.length() ? input.substr(idx) : "");
	}

	CurseManager (string fname) {
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
		filename = fname;
		getmaxyx(stdscr, h, w);
		win = newwin(h - 2*head_height, w - left_margin, head_height, left_margin);
		numwin = newwin(h - 2*head_height, left_margin, head_height, 0);
		pwin = newwin(head_height, w, 0, 0);
		xwin = newwin(head_height, w, h-head_height, 0);
		rows.push_back("row one");
		rows.push_back("row two");
		rows.push_back("row three");
		rows.push_back("row four");
		rows.push_back("row five");
		rows.push_back("row six");
		rows.push_back("row seven");
		rows.push_back("row eight");
		rows.push_back("row nine");
		rows.push_back("row ten");
		rows.push_back("row eleven");

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


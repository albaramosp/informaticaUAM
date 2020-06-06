#include <ncurses.h>

void screen_init() {
	initscr();
	clear();
	noecho();
	cbreak();
	keypad(stdscr, FALSE);
	curs_set(0);
}

void screen_addch(int row, int col, char symbol)
{
	mvaddch(row,col,symbol);
}

void screen_refresh()
{
	refresh();
}


void screen_end() {
	endwin();
}


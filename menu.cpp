#include <ncurses.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <fstream>
#include "GameState.h"

const int title_len = 87;
const int title_h = 7;

void print_title(int y, int x) {
	mvprintw(y, x,		 " _______  _______  _______  _______  ___      _______  _______  __   __  ___   _______ \n");
	mvprintw(y + 1, x, "|  _    ||   _   ||       ||       ||   |    |       ||       ||  | |  ||   | |       |\n");
	mvprintw(y + 2, x, "| |_|   ||  |_|  ||_     _||_     _||   |    |    ___||  _____||  |_|  ||   | |    _  |\n");
	mvprintw(y + 3, x, "|       ||       |  |   |    |   |  |   |    |   |___ | |_____ |       ||   | |   |_| |\n");
	mvprintw(y + 4, x, "|  _   | |       |  |   |    |   |  |   |___ |    ___||_____  ||       ||   | |    ___|\n");
	mvprintw(y + 5, x, "| |_|   ||   _   |  |   |    |   |  |       ||   |___  _____| ||   _   ||   | |   |    \n");
	mvprintw(y + 6, x, "|_______||__| |__|  |___|    |___|  |_______||_______||_______||__| |__||___| |___|    \n");
}

void print_centered_title(int row, int col, int height) {
	print_title(((row - height) / 2 - title_h) / 2, (col - title_len) / 2);
}

WINDOW* create_menu(int height, int width, bool with_title) {
	int row, col;
	getmaxyx(stdscr, row, col);

	// clear the screen by filling it with spaces
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			mvprintw(i, j, " ");
		}
	}

	if (with_title) {
		print_centered_title(row, col, height);
	}

	WINDOW* shadow = newwin(height, width, (row - height) / 2 + 1, (col - width) / 2 + 1);
	wbkgd(shadow, COLOR_PAIR(3));

	WINDOW* menu = newwin(height, width, (row - height) / 2, (col - width) / 2);
	wbkgd(menu, COLOR_PAIR(2));
	box(menu, 0, 0);

	refresh();
	wrefresh(shadow);
	wrefresh(menu);
	delwin(shadow);
	return menu;
}

int process_menu(std::vector<std::string> &items) {
	int item_number = items.size();
	int cur_item = 0;
	int height = item_number + 2;
	int width = 20;

	WINDOW* selection_menu = create_menu(height, width, true);

	int ch;

	do {
		for (int i = 0; i < item_number; i++) {
			if (i == cur_item) {
				wattron(selection_menu, COLOR_PAIR(4));
			}
			mvwprintw(selection_menu, i + 1, (width - items[i].size()) / 2, items[i].c_str());
			if (i == cur_item) {
				wattroff(selection_menu, COLOR_PAIR(4));
			}
		}

		wrefresh(selection_menu);

		switch (ch = getch()) {
			case KEY_DOWN:
				cur_item++;
				cur_item %= item_number;
				break;
			case KEY_UP:
				cur_item = (cur_item - 1 + item_number) % item_number;
				break;
			case KEY_RESIZE:
				return item_number;
			case '\n':
				delwin(selection_menu);
				return cur_item;
				break;
		} 
	}	while (true);
}

void process_main_m(GameState &state) {
	std::vector<std::string> items = {
		"Play",
		"Help",
		"Exit"
	};

	GameState gStatus[4] = {
		play_m,
		help_m,
		exit_g,
		main_m
	};

	int item_number = process_menu(items);

	state = gStatus[item_number];
}

void process_play_m(GameState &state) {
	std::vector<std::string> items = {
		"Solo",
		"Online",
		"Back"
	};

	GameState gStatus[4] = {
		local_m,
		online_m,
		main_m,
		play_m
	};

	int item_number = process_menu(items);

	state = gStatus[item_number];
}

void process_local_m(GameState &state) {
	std::vector<std::string> items = {
		"Easy",
		"Middle",
		"Hard",
		"Back"
	};

	GameState gStatus[5] = {
		easy_g,
		TODO_m,
		TODO_m,
		play_m,
		local_m
	};

	int item_number = process_menu(items);

	state = gStatus[item_number];
}

void process_online_m(GameState &state) {
	std::vector<std::string> items = {
		"Create game",
		"Join game",
		"Back"
	};

	GameState gStatus[4] = {
		TODO_m,
		TODO_m,
		play_m,
		online_m
	};

	int item_number = process_menu(items);

	state = gStatus[item_number];
}

void process_TODO_m(GameState &state) {
	std::vector<std::string> items = {
		"In future versions",
	};

	GameState gStatus[2] = {
		main_m,
		main_m
	};

	int item_number = process_menu(items);

	state = gStatus[item_number];
}

void process_help_m(GameState &state) {
	int row, col;
	getmaxyx(stdscr, row, col);
	int height = (row / 3) * 2;
	int width = 52;

	WINDOW* rules_page = create_menu(height, width, false);

	mvprintw(row - 1, 0, "Press F1 to exit");
	refresh();

	int ch;
	int cur_item = 0;

	std::string line;
	std::ifstream in("help.txt");
	std::vector<std::string> rules_text;

	if (in.is_open()) {
		while (std::getline(in, line)) {
			rules_text.push_back(line);
		}
	}

	in.close();
	
	int cur_page = 0;
	int page_number = 1;
	if (height != 2) {
		page_number = (rules_text.size() + height - 3) / (height - 2);
	}

	do {
		for (int i = 0; i < height - 2; i++) {
			if (cur_page * (height - 2) + i >= rules_text.size()) {
				for (int j = 0; j < 50; j++) {
					mvwprintw(rules_page, i + 1, j + 1, " ");
				}
			} else {
				mvwprintw(rules_page, i + 1, 1, rules_text[cur_page * (height - 2) + i].c_str());
			}
		}
		wrefresh(rules_page);

		switch (ch = getch()) {
			case KEY_DOWN:
				cur_page++;
				cur_page %= page_number;
				break;
			case KEY_UP:
				cur_page = (cur_page - 1 + page_number) % page_number;
				break;
			case KEY_F(1):
				delwin(rules_page);
				state = main_m;
				return;
		}
	} while	(true);
}

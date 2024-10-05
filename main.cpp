#include <ncurses.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <fstream>
#include "menu.h"
#include "game.h"

int main() {
  initscr();			
	curs_set(0);
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	if (has_colors() == FALSE) {
    endwin();
		printf("Your terminal does not support color\n");
		exit(1);
	}

	start_color();
	// init_pair(1, COLOR_WHITE, COLOR_CYAN);
	init_pair(1, COLOR_WHITE, COLOR_BLUE);
	init_pair(2, COLOR_GREEN, COLOR_WHITE);
	init_pair(3, COLOR_BLACK, COLOR_BLACK);
	init_pair(4, COLOR_RED, COLOR_WHITE);
	init_pair(5, COLOR_WHITE, COLOR_WHITE);


	init_pair(6, COLOR_WHITE, COLOR_GREEN);
	// init_pair(7, COLOR_WHITE, COLOR_BLUE);
	init_pair(7, COLOR_WHITE, COLOR_MAGENTA);

	init_pair(8, COLOR_WHITE, COLOR_YELLOW);
	init_pair(9, COLOR_WHITE, COLOR_BLACK);
	// init_pair(10, COLOR_WHITE, COLOR_CYAN);
	init_pair(10, COLOR_WHITE, COLOR_BLUE);
	init_pair(13, COLOR_YELLOW, COLOR_YELLOW);
	init_pair(14, COLOR_RED, COLOR_RED);


	init_color(COLOR_MAGENTA, 574, 769, 913);
	init_color(COLOR_YELLOW, 500, 500, 500);


	wbkgd(stdscr, COLOR_PAIR(1));


	GameState state = main_m;

	while (true) {
		switch (state) {
			case main_m:
				process_main_m(state);
				break;
			case play_m:
				process_play_m(state);
				break;
			case local_m:
				process_local_m(state);
				break;
			case online_m:
				process_online_m(state);
				break;
			case help_m:
				process_help_m(state);
				break;
			case easy_g:
				process_easy_g(state);
				break;
			case TODO_m:
				process_TODO_m(state);
				break;
				/*
			case middle_g:
				process_middle(state);
				break;
			case hard_g:
				process_hard(state);
				break;
			case create_g:
				process_create(state);
				break;
			case connect_g:
				process_connect(state);
				break;
				*/
			case exit_g:
				goto Exit;
		}
	}

Exit:
	
	endwin();
  return 0;
}

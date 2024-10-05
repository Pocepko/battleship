#pragma once
#include <ncurses.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <fstream>
#include "GameState.h"

const int title_h = 7;

void print_title(int y, int x);

void print_centered_title(int row, int col, int height);

int process_menu(std::vector<std::string> &items);

void process_main_m(GameState &state);

void process_play_m(GameState &state);

void process_local_m(GameState &state);

void process_online_m(GameState &state);

void process_TODO_m(GameState &state);

void process_help_m(GameState &state);

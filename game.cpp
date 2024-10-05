#include <ncurses.h>
#include "GameState.h"
#include "menu.h"
#include <random>

enum class ShotRes {
	hit,
	miss,
	sank,
	game_over
};

enum class GameRes {
	win,
	loss
};

struct Coord {
	int x, y;
};

struct AbstractPlayer {
	virtual void arrange_ships() = 0;

	virtual Coord take_shot() = 0;

	virtual ShotRes get_shot(Coord) = 0;

	virtual void get_res(ShotRes, Coord) = 0;

	virtual void game_res(GameRes) = 0;

	int field_m[10][10];
	int other_field_m[10][10];
	int health_points[10];
	int alive_ships_num = 10;
};

struct LocalPlayer : AbstractPlayer {
	LocalPlayer() {
		int row, col;
		getmaxyx(stdscr, row, col);
		int height = 14;
		int width = 25;
		int gap_size = 10;

		int field1x = (col - width * 2 - gap_size) / 2;
		field1 = newwin(height, width, (row - height) / 2, field1x);
		print_field(field1, true);

		shadow1 = newwin(height, width, (row - height) / 2 + 1, field1x + 1);
		wbkgd(shadow1, COLOR_PAIR(3));
		
		field2 = newwin(height, width, (row - height) / 2, field1x + width + gap_size);
		print_field(field2, true);

		shadow2 = newwin(height, width, (row - height) / 2 + 1, field1x + width + gap_size + 1);
		wbkgd(shadow2, COLOR_PAIR(3));

		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				mvprintw(i, j, " ");
			}
		}

		print_centered_title(row, col, height);

		refresh();
		wrefresh(shadow1);
		wrefresh(shadow2);
		wrefresh(field1);
		wrefresh(field2);
	}

	virtual void arrange_ships() override {
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				field_m[i][j] = 0;
				other_field_m[i][j] = 0;
			}
		}

		int ship_num = 1;
		for (int ship_len = 4; ship_len > 0; ship_len--) {
			for (int i = 0; i < 5 - ship_len; i++) {
				get_ship(ship_len, ship_num);
				health_points[ship_num - 1] = ship_len;
				ship_num++;
			}
		}
	}

	virtual Coord take_shot() override {
		int x = 4, y = 4, ch;

		do {
			print_cursor(field2, x, y);
			wrefresh(field2);

			switch (ch = getch()) {
				case KEY_DOWN:
					if (y < 9) {
						y++;
					}
					break;
				case KEY_UP:
					if (y > 0) {
						y--;
					}
					break;
				case KEY_RIGHT:
					if (x < 9) {
						x++;
					}
					break;
				case KEY_LEFT:
					if (x > 0) {
						x--;
					}
					break;
				case '\n':
					if (other_field_m[y][x] == 0) {
						return Coord{x, y};
					} 
					break;
			}
		} while (true);
	}

	ShotRes get_shot(Coord xy) override {
		ShotRes res;
		if (field_m[xy.y][xy.x] < 1 || 10 < field_m[xy.y][xy.x]) {
			field_m[xy.y][xy.x] = 12;
			res = ShotRes::miss;
		} else {
			int ship_num = field_m[xy.y][xy.x];

			health_points[ship_num - 1]--;

			field_m[xy.y][xy.x] = 13;

			if (health_points[ship_num - 1] > 0) {
				return ShotRes::hit;
			} else {
				alive_ships_num--;
				if (alive_ships_num > 0) {
					return ShotRes::sank;
				} else {
					return ShotRes::game_over;
				}
			}
		}

		print_ships(field1, field_m);

		return res;
	}

	virtual void get_res(ShotRes res, Coord shot) override {
		if (res == ShotRes::miss) {
			other_field_m[shot.y][shot.x] = 12;
		} else if (res == ShotRes::hit) {
			other_field_m[shot.y][shot.x] = 13;
		} else if (res == ShotRes::sank) {
			other_field_m[shot.y][shot.x] = 13;
			int dir[4][2] = {
				{1, 0},
				{0, 1},
				{-1, 0},
				{0, -1}
			};

			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					int x = shot.x + dir[i][0] * j;
					int y = shot.y + dir[i][1] * j;
					if (in_range(x) && in_range(y) && (other_field_m[y][x] == 13
							|| other_field_m[y][x] == 14)) {
						other_field_m[y][x] = 14;
						fill_empty_neighbors(12, x, y, other_field_m);
					} else {
						break;
					}
				}
			}
		}
		print_ships(field2, other_field_m);
	}


	virtual void game_res(GameRes res) {
		int row, col;
		getmaxyx(stdscr, row, col);

		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				mvprintw(i, j, " ");
			}
		}

		int height;
		int width;
		int color;

		std::string addr;
		std::string line;
		std::vector<std::string> text;

		if (res == GameRes::win) {
			height = 7 + 4;
			width = 59 + 6;
			color = 2;
			addr = "win.txt";
		} else {
			height = 7 + 4;
			width = 71 + 6;
			color = 4;
			addr = "lose.txt";
		}

		WINDOW* field = newwin(height, width, (row - height) / 2, (col - width) / 2);
		wbkgd(field, COLOR_PAIR(color)); 					
		box(field, 0, 0);

		WINDOW* shadow = newwin(height, width, (row - height) / 2 + 1, (col - width) / 2 + 1);
		wbkgd(shadow, COLOR_PAIR(3));

		print_centered_title(row, col, height);

		std::ifstream in(addr);

		if (in.is_open()) {
			while (std::getline(in, line)) {
				text.push_back(line);
			}
		}

		in.close();

		for (int i = 0; i < text.size(); i++) {
			mvwprintw(field, 2 + i, 3, text[i].c_str());
		}

		refresh();
		wrefresh(shadow);
		wrefresh(field);
		getch();
	}

 private:
	void fill_empty_neighbors(int val, int x, int y, int (& field_l)[10][10]) {
		int neighbours[8][2] = {
			{0, 1},
			{0, -1},
			{1, 0},
			{-1, 0},
			{1, 1},
			{1, -1},
			{-1, 1},
			{-1, -1}
		};

		for (int j = 0; j < 8; j++) {
			int dx = x + neighbours[j][0];
			int dy = y + neighbours[j][1];
			if (in_range(dy) && in_range(dx) && field_l[dy][dx] == 0) {
				field_l[dy][dx] = val;
			}
		}
	}

	void print_squares(WINDOW* &field) {
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				if ((i + j) % 2 == 0) {
					wattron(field, COLOR_PAIR(7));
				} else {
					wattron(field, COLOR_PAIR(10));
				}
				mvwprintw(field, i + 3, j * 2 + 4, "  ");
				if ((i + j) % 2 == 0) {
					wattroff(field, COLOR_PAIR(7));
				} else {
					wattroff(field, COLOR_PAIR(10));
				}
			}
		}
	}

	void print_cursor(WINDOW* &field, int x, int y) {
		print_squares(field);
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				if (other_field_m[i][j] == 12) {
					wattron(field, COLOR_PAIR(2));
				} else if (other_field_m[i][j] == 13) {
					wattron(field, COLOR_PAIR(13));
				}	else if (other_field_m[i][j] == 14) {
					wattron(field, COLOR_PAIR(14));
				}

				if (other_field_m[i][j] != 0) {
					mvwprintw(field, i + 3, j * 2 + 4, "  ");
				}
				if (other_field_m[i][j] == 12) {
					wattroff(field, COLOR_PAIR(2));
				} else if (other_field_m[i][j] == 13) {
					wattroff(field, COLOR_PAIR(13));
				} else if (other_field_m[i][j] == 14) {
					wattroff(field, COLOR_PAIR(14));
				}
			}
		}

		wattron(field, COLOR_PAIR(6));
		mvwprintw(field, y + 3, x * 2 + 4, "  ");
		wattroff(field, COLOR_PAIR(6));
	}


	void print_field(WINDOW* &field, bool squares) {
		// field color
		wbkgd(field, COLOR_PAIR(2)); 					
		box(field, 0, 0);

		// field lines 
		mvwvline(field, 3, 3, 0, 10);
		mvwhline(field, 2, 4, 0, 20);

		mvwaddch(field, 2, 3, ACS_PLUS);

		mvwaddch(field, 2, 0, ACS_LTEE);
		mvwaddch(field, 0, 3, ACS_TTEE);

		mvwaddch(field, 2, 1, ACS_HLINE);
		mvwaddch(field, 2, 2, ACS_HLINE);
		mvwaddch(field, 1, 3, ACS_VLINE);

		mvwaddch(field, 2, 24, ACS_RTEE);
		mvwaddch(field, 13, 3, ACS_BTEE);

		// field chars and numbers
		for (int i = 0; i < 10; i++) {
			mvwaddch(field, 1, i * 2 + 4, char(int('A') + i));
			if (i < 9) {
				mvwaddch(field, i + 3, 2, char(int('1') + i));
			}
		}
		mvwprintw(field, 12, 1, "10");


		// field squares
		if (squares) {
			print_squares(field);
		}
	}


	int max_y(int ship_len, int orientation) {
		return 10 - orientation * (ship_len - 1);
	}

	int max_x(int ship_len, int orientation) {
		return 10 - (1 - orientation) * (ship_len - 1);
	}

	bool in_range(int x) {
		return (0 <= x && x < 10);
	}

	bool occupied(int x, int y, int ship_len, int orientation) {
		bool flag = false;
		for (int i = 0; i < ship_len; i++) {
			int xx = x;
			int yy = y;
			if (orientation == 0) {
				xx += i;
			} else {
				yy += i;
			}
			if (field_m[yy][xx] != 0) {
				flag = true;
			}
		}
		return flag;
	}

	void print_ships(WINDOW* &fieldw, int x, int y, int ship_len, int orientation) {
		print_squares(fieldw);
		refresh();
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				if (0 < field_m[i][j] && field_m[i][j] < 11) {
					wattron(fieldw, COLOR_PAIR(13));
					mvwprintw(fieldw, i + 3, j * 2 + 4, "  ");
					wattroff(fieldw, COLOR_PAIR(13));
				}
			}
		}
		refresh();

		wattron(fieldw, COLOR_PAIR(13));
		for (int i = 0; i < ship_len; i++) {
			if (field_m[y + i * orientation][x + i * (1 - orientation)] != 0) {
				wattron(fieldw, COLOR_PAIR(14));
			}
			mvwprintw(fieldw, y + 3 + i * orientation, (x + i * (1 - orientation)) * 2 + 4, "  ");
			if (field_m[y + i * orientation][x + i * (1 - orientation)] != 0) {
				wattron(fieldw, COLOR_PAIR(13));
			}
		}
		wattroff(fieldw, COLOR_PAIR(13));
		refresh();
	}

	void print_ships(WINDOW* &fieldw, int (& field_l)[10][10]) {
		print_squares(fieldw);
		refresh();
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				if (0 < field_l[i][j] && field_l[i][j] < 11) {
					wattron(fieldw, COLOR_PAIR(13));
					mvwprintw(fieldw, i + 3, j * 2 + 4, "  ");
					wattroff(fieldw, COLOR_PAIR(13));
				} else if (field_l[i][j] == 12) {
					wattron(fieldw, COLOR_PAIR(5));
					mvwprintw(fieldw, i + 3, j * 2 + 4, "  ");
					wattroff(fieldw, COLOR_PAIR(5));
				} else if (field_l[i][j] == 13) {
					wattron(fieldw, COLOR_PAIR(14));
					mvwprintw(fieldw, i + 3, j * 2 + 4, "  ");
					wattroff(fieldw, COLOR_PAIR(14));
				}
			}
		}
		wrefresh(fieldw);
		refresh();
	}

	void get_ship(int ship_len, int ship_num) {
		int x = 3, y = 3, ch;

		// vertical = 1, horizontal = 0
		int orientation = 0; 			

		do {
			print_ships(field1, x, y, ship_len, orientation);
			wrefresh(field1);

			switch (ch = getch()) {
				case KEY_DOWN:
					if (y + 1 < max_y(ship_len, orientation)) {
						y++;
					}
					break;
				case KEY_UP:
					if (y > 0) {
						y--;
					}
					break;
				case KEY_RIGHT:
					if (x + 1 < max_x(ship_len, orientation)) {
						x++;
					}
					break;
				case KEY_LEFT:
					if (x > 0) {
						x--;
					}
					break;
				case 'r':
					orientation = 1 - orientation;
					if (x >= max_x(ship_len, orientation)) {
						x = max_x(ship_len, orientation) - 1;
					}
					if (y >= max_y(ship_len, orientation)) {
						y = max_y(ship_len, orientation) - 1;
					}
					break;
				case '\n':
					if (!occupied(x, y, ship_len, orientation)) {
						for (int i = 0; i < ship_len; i++) {
							int dx = x;
							int dy = y;
							if (orientation == 0) {
								dx += i;
							} else {
								dy += i;
							}
							fill_empty_neighbors(11, dx, dy, field_m);
							field_m[dy][dx] = ship_num;
						}
						goto ship_exit;
					} else {
					}
					break;
			}

		} while (true);
		ship_exit:
		return;
	}

	WINDOW* field1;
	WINDOW* field2;
	WINDOW* shadow1;
	WINDOW* shadow2;
};

struct EasyPlayer : AbstractPlayer {
	EasyPlayer() : rng(dev()) {}

	virtual void arrange_ships() override {
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				field_m[i][j] = 0;
				other_field_m[i][j] = 0;
			}
		}

		int ship_num = 1;
		for (int ship_len = 4; ship_len > 0; ship_len--) {
			for (int i = 0; i < 5 - ship_len; i++) {
				get_ship(ship_len, ship_num);
				health_points[ship_num - 1] = ship_len;
				ship_num++;
			}
		}

		refresh();
	}

	virtual Coord take_shot() override {
		int zero_num = 0;
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				if (other_field_m[i][j] == 0) {
					zero_num++;
				}
			}
		}
		int rand = distr(rng) % zero_num + 1;
		zero_num = 0;
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				if (other_field_m[i][j] == 0) {
					zero_num++;
					if (zero_num == rand) {
						return Coord{j, i};
					}
				}
			}
		}
	}

	virtual ShotRes get_shot(Coord xy) override {
		if (field_m[xy.y][xy.x] < 1 || 10 < field_m[xy.y][xy.x]) {
			return ShotRes::miss;
		} else {
			int ship_num = field_m[xy.y][xy.x];

			health_points[ship_num - 1]--;

			if (health_points[ship_num - 1] > 0) {
				return ShotRes::hit;
			} else {
				alive_ships_num--;
				if (alive_ships_num > 0) {
					return ShotRes::sank;
				} else {
					return ShotRes::game_over;
				}
			}
		}
	}

	virtual void get_res(ShotRes res, Coord shot) override {
		if (res == ShotRes::miss) {
			other_field_m[shot.y][shot.x] = 12;
		} else if (res == ShotRes::hit) {
			other_field_m[shot.y][shot.x] = 13;
		} else if (res == ShotRes::sank) {
			other_field_m[shot.y][shot.x] = 13;
		}
	}
  
	virtual void game_res(GameRes res) override {}

private:
	bool occupied(int x, int y, int ship_len, int orientation) {
		bool flag = false;
		for (int i = 0; i < ship_len; i++) {
			int xx = x;
			int yy = y;
			if (orientation == 0) {
				xx += i;
			} else {
				yy += i;
			}
			if (field_m[yy][xx] != 0) {
				flag = true;
			}
		}
		return flag;
	}

	bool in_range(int x) {
		return (0 <= x && x < 10);
	}

	void get_ship(int ship_len, int ship_num) {

		int neighbours[8][2] = {
			{0, 1},
			{0, -1},
			{1, 0},
			{-1, 0},
			{1, 1},
			{1, -1},
			{-1, 1},
			{-1, -1}
		};

		int free_num = 0;
		for (int i = 0; i < 10 - ship_len + 1; i++) {
			for (int j = 0; j < 10 - ship_len + 1; j++) {
				if (field_m[i][j] == 0 && !occupied(j, i, ship_len, 0)) {
					free_num++;
				} else if (field_m[i][j] == 0 && !occupied(j, i, ship_len, 1)) {
					free_num++;
				}
			}
		}

		int rand = distr(rng) % free_num + 1;
		int num = 0;
		for (int i = 0; i < 10 - ship_len + 1; i++) {
			for (int j = 0; j < 10 - ship_len + 1; j++) {
				int orientation = -1;
				if (field_m[i][j] == 0 && !occupied(j, i, ship_len, 0)) {
					num++;
					if (num == rand) {
						orientation = 0;
					}
				} else if (field_m[i][j] == 0 && !occupied(j, i, ship_len, 1)) {
					num++;
					if (num == rand) {
						orientation = 1;
					}
				}
				if (orientation != -1) {
					for (int k = 0; k < ship_len; k++) {
						int x = j + k * (1 - orientation);
						int y = i + k * orientation;
						field_m[y][x] = ship_num;
						for (int w = 0; w < 8; w++) {
							int xx = x + neighbours[w][0];
							int yy = y + neighbours[w][1];
							if (in_range(xx) && in_range(yy) && field_m[yy][xx] == 0) {
								field_m[yy][xx] = 11;
							}
						}
					}
				}
			}
		}
	}

	std::random_device dev;
  std::mt19937 rng;
  std::uniform_int_distribution<int> distr;
};

void process_g(AbstractPlayer &player1, AbstractPlayer &player2) {
	player1.arrange_ships();
	player2.arrange_ships();

	int cur_player = 1;
	ShotRes res;
	Coord shot;

	while (true) {
		if (cur_player == 1) {
			shot = player1.take_shot();
			res = player2.get_shot(shot);
			player1.get_res(res, shot);
			if (res == ShotRes::game_over) {
				player1.game_res(GameRes::win);
				break;
			}
		} else {
			shot = player2.take_shot();
			res = player1.get_shot(shot);
			player2.get_res(res, shot);
			if (res == ShotRes::game_over) {
				player1.game_res(GameRes::loss);
				break;
			}
		}
		if (res == ShotRes::miss) {
			cur_player = 3 - cur_player;
		}
	}
}

void process_easy_g(GameState &state) {
	LocalPlayer player1;
	EasyPlayer player2;
	
	process_g(player1, player2);

	state = main_m;
}

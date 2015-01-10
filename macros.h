#define B_INC(e) (sizeof(e) + 2)
#define CENTRE(e) ((COLS - B_INC(e)) / 2)
#define SM_CENTRE(width, len) (((len) - (width)) / 2 + 1)
#define SAME(a, b) (strcmp((a), (b)) == 0)
#define DISPLAY_ERROR(back_win, e) \
	{ \
	WINDOW *err_win; \
		{ \
		err_win = CreateWindow(back_win, 3, B_INC(e), 10, CENTRE(e)); \
		mvwaddstr(err_win, 1, 1, e); \
		wrefresh(err_win); \
		sleep(3); \
		RemoveWindow(back_win, err_win); \
		} \
	}
#define BANG_PRINT(w, c, b, v) (b[c->y][c->x] & S_CRASH ? \
	mvwaddch(w, c->y, c->x, S_BANG_CHAR) : \
	mvwaddch(w, c->y, c->x, v))

#define BAD_COORDS(y, x) (y < 0 || y >= Y_MAX || x < 0 || x >= X_MAX)

#define SETUP_THEM(board, t) \
	{ \
	board[t->y + Delta[S_NORTH].dy][t->x + Delta[S_NORTH].dx] |= S_TEMP; \
	board[t->y + Delta[S_SOUTH].dy][t->x + Delta[S_SOUTH].dx] |= S_TEMP; \
	board[t->y + Delta[S_EAST].dy][t->x + Delta[S_EAST].dx] |= S_TEMP; \
	board[t->y + Delta[S_WEST].dy][t->x + Delta[S_WEST].dx] |= S_TEMP; \
	}

#define REMOVE_THEM(board, t) \
	{ \
	board[t->y + Delta[S_NORTH].dy][t->x + Delta[S_NORTH].dx] &= ~S_TEMP; \
	board[t->y + Delta[S_SOUTH].dy][t->x + Delta[S_SOUTH].dx] &= ~S_TEMP; \
	board[t->y + Delta[S_EAST].dy][t->x + Delta[S_EAST].dx] &= ~S_TEMP; \
	board[t->y + Delta[S_WEST].dy][t->x + Delta[S_WEST].dx] &= ~S_TEMP; \
	}

#define MAINTAIN_COURSE(us) \
	{ \
	us->y += Delta[us->dir].dy; \
	us->x += Delta[us->dir].dx; \
	}

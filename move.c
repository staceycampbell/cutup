#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include <assert.h>
#include "consts.h"
#include "types.h"
#include "macros.h"
#include "funcs.h"

extern int IComputer;
extern int ComputerOpponent;
extern int StopIO;

int DeltaI[128];
delta_t Delta[S_DELTA_MAX];

static void
HumanMove(WINDOW * board_w, coords_t * us)
{
	int ch;
	int tmp_dir;

	wrefresh(board_w);
	ch = wgetch(board_w);
	if (ch != ERR)
		tmp_dir = DeltaI[ch];
	else
		tmp_dir = S_SAME;
	switch (tmp_dir)
	{
	case S_BOGUS:
		tmp_dir = us->dir;
		flash();
		break;
	case S_SAME:
		tmp_dir = us->dir;
		break;
	default:
		us->dir = tmp_dir;
		break;
	}
	us->y += Delta[tmp_dir].dy;
	us->x += Delta[tmp_dir].dx;
}

int
GetMoves(WINDOW * board_w, int board[Y_MAX][X_MAX], int pipe_r, int pipe_w, coords_t * us, coords_t * them)
{
	int status = S_NO_COLLISION;
        int pipe_status;

	if (IComputer)
		ComputeMove(board, us, them);
	else
		HumanMove(board_w, us);

	pipe_status = write(pipe_w, (char *)us, sizeof *us);
        assert(pipe_status >= 0);
	pipe_status = read(pipe_r, (char *)them, sizeof *them);
        assert(pipe_status >= 0);

	if (board[us->y][us->x] != S_FREE)
	{
		board[us->y][us->x] |= S_CRASH;
		status |= S_WE_COLLIDE;
	}
	board[us->y][us->x] |= S_ONE;
	if (board[them->y][them->x] != S_FREE)
	{
		board[them->y][them->x] |= S_CRASH;
		status |= S_THEY_COLLIDE;
	}
	board[them->y][them->x] |= S_OTHER;

	return status;
}

void
DeltaInits(void)
{
	int i;

	for (i = 1; i < 128; ++i)
		DeltaI[i] = S_BOGUS;

	DeltaI[S_RIGHT_CH] = S_EAST;
	DeltaI[S_RIGHT_ALT_CH] = S_EAST;
	DeltaI[S_LEFT_CH] = S_WEST;
	DeltaI[S_LEFT_ALT_CH] = S_WEST;
	DeltaI[S_UP_CH] = S_NORTH;
	DeltaI[S_UP_ALT_CH] = S_NORTH;
	DeltaI[S_DOWN_CH] = S_SOUTH;
	DeltaI[S_DOWN_ALT_CH] = S_SOUTH;
	DeltaI[0] = S_SAME;

	Delta[S_NORTH].dy = -1;
	Delta[S_NORTH].dx = 0;
	Delta[S_SOUTH].dy = 1;
	Delta[S_SOUTH].dx = 0;
	Delta[S_EAST].dy = 0;
	Delta[S_EAST].dx = 1;
	Delta[S_WEST].dy = 0;
	Delta[S_WEST].dx = -1;
}

void
DrawPositions(WINDOW * board_w, int board[Y_MAX][X_MAX], coords_t * us, coords_t * them)
{
	if (!StopIO)
	{
		BANG_PRINT(board_w, us, board, S_US_CHAR);
		BANG_PRINT(board_w, them, board, S_THEM_CHAR);
		wmove(board_w, us->y, us->x);
		wrefresh(board_w);
	}
	if (!IComputer)
		MillisecondSleep(140);
        else
		MillisecondSleep(10);
}

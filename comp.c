#include <curses.h>
#include "consts.h"
#include "types.h"
#include "macros.h"
#include "funcs.h"

extern int StopIO;
extern delta_t Delta[S_DELTA_MAX];
extern int SlowCPU;

static int Direct[S_DIR_MAX][C_MAX];
static int Order[LR_MAX][C_MAX];
static int Opposite[S_DIR_MAX];
static int RndMoveProb;

int CollisionImminent();
int CutupImminent();
int MakeRndMove();
int MakeMoveTo();
int DomainSize();
void AvoidCutup();
int CalcDomains();
void GenRndCoord();

void ComputeMove(board, us, them)

int board[Y_MAX][X_MAX];
coords_t *us;
coords_t *them;

	{
	int dom_right, dom_left;
	int flag;

	SETUP_THEM(board, them);
	if (CollisionImminent(board, us))
		{
		++RndMoveProb;
		dom_right = DomainSize(board, us, C_RIGHT);
		dom_left = DomainSize(board, us, C_LEFT);
		flag = 0;
		if (dom_right > dom_left)
			flag = MakeMoveTo(board, us, C_RIGHT);
		if (! flag)
			{
			if (! MakeMoveTo(board, us, C_LEFT))
				MAINTAIN_COURSE(us);
			}
		}
	else
		if (CutupImminent(board, us))
			AvoidCutup(board, us);
		else
			if (MakeRndMove())
				GenRndCoord(board, us);
			else
				MAINTAIN_COURSE(us);
	REMOVE_THEM(board, them);
	}

static void AvoidCutup(board, us)

int board[Y_MAX][X_MAX];
coords_t *us;

	{
	int left_dom, right_dom, forward_dom;
	int flag = 0;
	coords_t attempt;
	int board_spot;

	if (SlowCPU)
		{
		GenRndCoord(board, us);
		return;
		}

	attempt.dir = Direct[us->dir][C_FORWARD];
	attempt.y = us->y + Delta[attempt.dir].dy;
	attempt.x = us->x + Delta[attempt.dir].dx;
	board_spot = board[attempt.y][attempt.x];
	board[attempt.y][attempt.x] = S_ONE;
	left_dom = CalcDomains(board, us, C_LEFT);
	right_dom = CalcDomains(board, us, C_RIGHT);
	board[attempt.y][attempt.x] = board_spot;
	forward_dom = CalcDomains(board, us, C_FORWARD) + 2;

	if (forward_dom >= left_dom && forward_dom >= right_dom)
		flag = MakeMoveTo(board, us, C_FORWARD);
	else
		if (right_dom > left_dom)
			flag = MakeMoveTo(board, us, C_RIGHT);
		else
			flag = MakeMoveTo(board, us, C_LEFT);
	if (! flag)
		MAINTAIN_COURSE(us);
	}

static int CalcDomains(board, us, rel)

int board[Y_MAX][X_MAX];
coords_t *us;
int rel;

	{
	coords_t us_test;
	int domain_total = 0;
	int dom_left, dom_right;

	us_test.dir = Direct[us->dir][rel];
	us_test.y = us->y + Delta[us_test.dir].dy;
	us_test.x = us->x + Delta[us_test.dir].dx;
	if (rel == C_FORWARD)
		us_test.dir = Opposite[us_test.dir];
	if (board[us_test.y][us_test.x] == S_FREE)
		{
		board[us_test.y][us_test.x] |= S_ONE;
		dom_left = DomainSize(board, &us_test, C_LEFT);
		dom_right = DomainSize(board, &us_test, C_RIGHT);
		domain_total = dom_left > dom_right ? dom_left : dom_right;
		board[us_test.y][us_test.x] = S_FREE;
		}

	return domain_total;
	}

static void GenRndCoord(board, us)

int board[Y_MAX][X_MAX];
coords_t *us;

	{
	int dir, start_dir, flag;
	int y, x;

	dir = lrand48() % 4; 
	start_dir = dir;
	do	{
		y = us->y + Delta[dir].dy;
		x = us->x + Delta[dir].dx;
		flag = board[y][x] == S_FREE;
		if (! flag)
			dir = (dir + 1) % 4;
		} while (! flag && dir != start_dir);
	if (flag)
		{
		us->y = y;
		us->x = x;
		us->dir = dir;
		}
	else
		{
		us->y += Delta[us->dir].dy;
		us->x += Delta[us->dir].dx;
		}
	}

void CompGameInit()

	{
	RndMoveProb = lrand48() % 10 + 3;
	}

void CompInit()

	{
	Direct[S_NORTH][C_REVERSE] = S_SOUTH;
	Direct[S_NORTH][C_FORWARD] = S_NORTH;
	Direct[S_NORTH][C_LEFT] = S_WEST;
	Direct[S_NORTH][C_RIGHT] = S_EAST;
	Direct[S_SOUTH][C_REVERSE] = S_NORTH;
	Direct[S_SOUTH][C_FORWARD] = S_SOUTH;
	Direct[S_SOUTH][C_LEFT] = S_EAST;
	Direct[S_SOUTH][C_RIGHT] = S_WEST;
	Direct[S_EAST][C_REVERSE] = S_WEST;
	Direct[S_EAST][C_FORWARD] = S_EAST;
	Direct[S_EAST][C_LEFT] = S_NORTH;
	Direct[S_EAST][C_RIGHT] = S_SOUTH;
	Direct[S_WEST][C_REVERSE] = S_EAST;
	Direct[S_WEST][C_FORWARD] = S_WEST;
	Direct[S_WEST][C_LEFT] = S_SOUTH;
	Direct[S_WEST][C_RIGHT] = S_NORTH;
	Order[C_RIGHT][0] = C_RIGHT;
	Order[C_RIGHT][1] = C_FORWARD;
	Order[C_RIGHT][2] = C_LEFT;
	Order[C_RIGHT][3] = C_REVERSE;
	Order[C_LEFT][0] = C_LEFT;
	Order[C_LEFT][1] = C_FORWARD;
	Order[C_LEFT][2] = C_RIGHT;
	Order[C_LEFT][3] = C_REVERSE;
	Opposite[S_NORTH] = S_SOUTH;
	Opposite[S_SOUTH] = S_NORTH;
	Opposite[S_EAST] = S_WEST;
	Opposite[S_WEST] = S_EAST;
	}

static int CollisionImminent(board, us)

int board[Y_MAX][X_MAX];
coords_t *us;

	{
	int y, x;

	y = us->y + Delta[us->dir].dy;
	x = us->x + Delta[us->dir].dx;

	return board[y][x] != S_FREE;
	}

static int MakeRndMove()

	{
	return ! (lrand48() % RndMoveProb);
	}

static int CutupImminent(board, us)

int board[Y_MAX][X_MAX];
coords_t *us;

	{
	int y, x;
	int dir;
	int board_spot;
	int back_spot;
	int board_right, board_left;

	y = us->y + Delta[us->dir].dy;
	x = us->x + Delta[us->dir].dx;
	dir = Direct[us->dir][C_RIGHT];
	board_spot = board[y + Delta[dir].dy][x + Delta[dir].dx];
	back_spot = board[us->y + Delta[dir].dy][us->x + Delta[dir].dx];
	board_right = board_spot != S_FREE && !(board_spot & S_BORDER) &&
	    back_spot == S_FREE;
	if (! board_right)
		{
		dir = Direct[us->dir][C_LEFT];
		board_spot = board[y + Delta[dir].dy][x + Delta[dir].dx];
		back_spot = board[us->y + Delta[dir].dy][us->x + Delta[dir].dx];
		board_left = board_spot != S_FREE && !(board_spot & S_BORDER) &&
		    back_spot == S_FREE;
		}

	return board_left || board_right;
	}

static int MakeMoveTo(board, us, rel)

int board[Y_MAX][X_MAX];
coords_t *us;
int rel;

	{
	coords_t attempt;
	int illegal;

	attempt.dir = Direct[us->dir][rel];
	attempt.y = us->y + Delta[attempt.dir].dy;
	attempt.x = us->x + Delta[attempt.dir].dx;
	illegal = board[attempt.y][attempt.x] != S_FREE;
	if (! illegal)
		*us = attempt;

	return ! illegal;
	}

static int DomainSize(board, us, rel)

int board[Y_MAX][X_MAX];
coords_t *us;
int rel;

	{
	int dir;
	int y, x;
	int count;

	count = 0;
	dir = us->dir;
	y = us->y;
	x = us->x;
	do	{
		++count;
		dir = FindBestDir(board, y, x, dir, rel, &count);
		y += Delta[dir].dy;
		x += Delta[dir].dx;
		} while (count < COUNT_LIMIT && ! (y == us->y && x == us->x));

	return count;
	}

static int FindBestDir(board, y, x, dir, rel, count_p)

int board[Y_MAX][X_MAX];
int y;
int x;
int dir;
int rel;
int *count_p;

	{
	int i = 0;
	int y_new, x_new;
	int new_order;

	new_order = Order[rel][i];
	y_new = y + Delta[Direct[dir][new_order]].dy;
	x_new = x + Delta[Direct[dir][new_order]].dx;
	while (BAD_COORDS(y_new, x_new) || board[y_new][x_new] == S_FREE)
		{
		++i;
		new_order = Order[rel][i];
		y_new = y + Delta[Direct[dir][new_order]].dy;
		x_new = x + Delta[Direct[dir][new_order]].dx;
		}
	*count_p += i;

	return Direct[dir][new_order];
	}

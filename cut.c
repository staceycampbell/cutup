#include <errno.h>
#include <curses.h>
#include "consts.h"
#include "types.h"
#include "macros.h"
#include "funcs.h"

#define ARG_STRING "bcsd"

void GetNames();
void ErrorExit();
void EndGame();
void ProcessArgs();
void SwapNames();
WINDOW *InitBoard();
void DecisionScreen();

int ComputerOpponent;
int IComputer;
int StopIO;
int ChildPid;
int SlowCPU;
int DisplayPlay;

static int OurScore;
static int TheirScore;

int main(argc, argv)

int argc;
char *argv;

	{
	WINDOW *play_w, *board_w;
	char you[NAME_MAX + 1], hom[NAME_MAX + 1];
	int p_write, p_read;
	int status;
	int collision;
	int board[Y_MAX][X_MAX];
	coords_t us, them;
	coords_t c1, c2;
	int game_count = 0;

	SeedRnd();
	DeltaInits();
	play_w = InitScreen();
	ProcessArgs(play_w, argc, argv);
	Welcome(play_w);
	GetNames(play_w, you, hom);
	if (ComputerOpponent)
		if (! (ChildPid = status = fork()))
			{
			SwapNames(you, hom);
			IComputer = 1;
			StopIO = 1;
			BlockSignals();
			(void)nice(5);
			sleep(2);
			SeedRnd();
			}
		else
			if (status == -1)
				ErrorExit(play_w, errno);
	if (status = GetFIFOName(play_w, &p_write, &p_read, you, hom, &c1, &c2))
		ErrorExit(play_w, status);
	if (IComputer)
		CompInit();
	do	{
		++game_count;
		GetReady(play_w, p_read, p_write, game_count, hom);
		board_w = InitBoard(play_w, board, &us, &them, &c1, &c2);
		CompGameInit();
		DrawPositions(board_w, board, &us, &them);
		do	{
			collision = GetMoves(board_w, board, p_read, p_write,
			    &us, &them);
			DrawPositions(board_w, board, &us, &them);
			} while (! collision);
		DecisionScreen(play_w, board_w, collision, you, hom);
		} while (((OurScore < SCORE_MAX && TheirScore < SCORE_MAX)
		    || OurScore == TheirScore) || DisplayPlay);
	EndGame(play_w, you, hom);
	if (! StopIO)
		EndScreen();
	return 0;
	}

static void GetNames(old_w, you, hom)

WINDOW *old_w;
char *you;
char *hom;

	{
	WINDOW *namesW;
	int you_len, hom_len;
	char test1[FN_MAX];
	char test2[FN_MAX];
	static char opponent[] = "Opponent's name: ";
	static char you_str[] =  "      Your name: ";

	namesW = CreateWindow(old_w, 4, sizeof opponent + NAME_MAX + 2, 1, 1);
	mvwaddstr(namesW, 1, 1, you_str);
	mvwaddstr(namesW, 2, 1, opponent);
	wmove(namesW, 1, sizeof you_str);
	you_len = GetString(namesW, you, NAME_MAX);
	wmove(namesW, 2, sizeof opponent);
	hom_len = GetString(namesW, hom, NAME_MAX);
	if (you_len == 0 || hom_len == 0)
		{
		DISPLAY_ERROR(namesW, "Null name given - try again.");
		GetNames(old_w, you, hom);
		}
	else
		{
		sprintf(test1, "%s%s", you, hom);
		sprintf(test2, "%s%s", hom, you);
		if (SAME(test1, test2))
			{
			DISPLAY_ERROR(namesW,
			    "Concatenated names are bad - try again.");
			GetNames(old_w, you, hom);
			}
		}
	RemoveWindow(old_w, namesW);
	}

static void ErrorExit(old_w, status)

WINDOW *old_w;
int status;

	{
	extern char *sys_errlist[];
	extern int sys_nerr;

	EndFIFO();
	if (! StopIO)
		EndScreen();
	if (status > sys_nerr)
		fprintf(stderr, "Unknown error number: %d\n", status);
	else
		fprintf(stderr, "error no: %d - %s\n", status,
		    sys_errlist[status]);
	exit(1);
	}

static void EndGame(old_w, you, hom)

WINDOW *old_w;
char *you;
char *hom;

	{
	WINDOW *final_w;
	static char hdr[] = "Cutup - Final Score";
	char mesg[256];
	int max_len, len;

	if (! StopIO)
		{
		sprintf(mesg, "%s: %d - %s: %d ", you, OurScore, hom,
		    TheirScore);
		len = strlen(mesg);
		max_len = len < sizeof hdr ? sizeof hdr : len;
		max_len += 3;
		final_w = CreateWindow(old_w, 5, max_len, 9, SM_CENTRE(max_len,
		    COLS));
		wstandout(final_w);
		mvwaddstr(final_w, 1, SM_CENTRE(sizeof hdr, max_len), hdr);
		wstandend(final_w);
		mvwaddstr(final_w, 3, SM_CENTRE(len, max_len), mesg);
		wrefresh(final_w);
		}
	EndFIFO();
	}

static void ProcessArgs(old_w, argc, argv)

WINDOW *old_w;
int argc;
char **argv;

	{
	int found;
	char mess[23];
	extern int opterr;
	extern char *optarg;

	opterr = 1;
	while ((found = getopt(argc, argv, ARG_STRING)) != EOF)
		switch (found)
		    {
		    case 'b' :
			IComputer = 1;
			ComputerOpponent = 1;
			break;
		    case 'c' :
			ComputerOpponent = 1;
			IComputer = 0;
			break;
		    case 's' :
			SlowCPU = 1;
			break;
		    case 'd' :
			DisplayPlay = 1;
			IComputer = 1;
			ComputerOpponent = 1;
			break;
		    default :
			sprintf(mess, "Ignoring argument: %c", found);
			DISPLAY_ERROR(old_w, mess);
			break;
		    }
	}

static void SwapNames(you, hom)

char *you;
char *hom;

	{
	char temp[NAME_MAX + 1];

	strcpy(temp, you);
	strcpy(you, hom);
	strcpy(hom, temp);
	}

static WINDOW *InitBoard(play_w, board, us, them, c1, c2)

WINDOW *play_w;
int board[Y_MAX][X_MAX];
coords_t *us;
coords_t *them;
coords_t *c1;
coords_t *c2;

	{
	WINDOW *board_w;
	int i, j;

	*us = *c1;
	*them = *c2;
	for (i = 1; i < Y_MAX - 1; ++i)
		for (j = 1; j < X_MAX - 1; ++j)
			board[i][j] = S_FREE;
	for (i = 0; i < Y_MAX; ++i)
		{
		board[i][0] = S_BORDER;
		board[i][X_MAX - 1] = S_BORDER;
		}
	for (j = 0; j < X_MAX; ++j)
		{
		board[0][j] = S_BORDER;
		board[Y_MAX - 1][j] = S_BORDER;
		}
	board[us->y][us->x] = S_ONE;
	board[them->y][them->x] = S_OTHER;
	if (! StopIO)
		{
		board_w = CreateWindow(play_w, Y_MAX, X_MAX, 1, 1);
		mvwaddch(board_w, us->y, us->x, S_US_CHAR);
		mvwaddch(board_w, them->y, them->x, S_THEM_CHAR);
		nodelay(board_w, TRUE);
		wrefresh(board_w);
		}
	return board_w;
	}

static void DecisionScreen(old_w, board_w, col_mask, you, hom)

WINDOW *old_w;
WINDOW *board_w;
int col_mask;
char *you;
char *hom;

	{
	WINDOW *decisW;
	int they_collide, we_collide;
	char decis[256];
	char score[256];
	int len;
	int decis_len;
	int score_len;
	static char mesg[] = "** Collision **";
	static char clr_mesg[] = "               ";

	if (StopIO)
		return;
	nodelay(board_w, FALSE);
	crmode();
	flushinp();
	mvwaddstr(old_w, 22, CENTRE(mesg), mesg);
	wrefresh(old_w);
	sleep(2);
	flushinp();
	mvwaddstr(old_w, 22, CENTRE(mesg), clr_mesg);
	wrefresh(old_w);
	if (we_collide = col_mask & S_WE_COLLIDE)
		++TheirScore;
	if (they_collide = col_mask & S_THEY_COLLIDE)
		++OurScore;
	if (we_collide && they_collide)
		sprintf(decis, "%s and %s crashed simultaneously", you, hom);
	else
		if (we_collide)
			sprintf(decis, "%s crashed, %s lives", you, hom);
		else
			sprintf(decis, "%s crashed, %s lives!", hom, you);
	sprintf(score, "%s: %d   %s: %d", you, OurScore, hom, TheirScore);
	decis_len = strlen(decis);
	score_len = strlen(score);
	len = decis_len > score_len ? decis_len : score_len;
	decisW = CreateWindow(board_w, 4, len + 3, 7, SM_CENTRE(len, COLS));
	mvwaddstr(decisW, 2, SM_CENTRE(score_len, len), score);
	mvwaddstr(decisW, 1, SM_CENTRE(decis_len, len), decis);
	wrefresh(decisW);
	sleep(3);
	RemoveWindow(board_w, decisW);
	RemoveWindow(old_w, board_w);
	flushinp();
	}

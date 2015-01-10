#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <curses.h>
#include "consts.h"
#include "types.h"
#include "funcs.h"
#include "macros.h"

extern int StopIO;
extern int ChildPid;

void KillChild();
void FixNodelay();
void zap();
void dumpcore();

static int SavedFcntl;

WINDOW *CreateWindow(old_win, lines, cols, y, x)

WINDOW *old_win;
int lines, cols;
int y, x;

	{
	WINDOW *front_win;
	
	front_win = newwin(lines, cols, y, x);
	touchwin(front_win);
	box(front_win, 0, 0);
	old_win = old_win;

	return front_win;
	}

void RemoveWindow(old_win, new_win)

WINDOW *old_win;
WINDOW *new_win;

	{
	werase(new_win);
	wrefresh(new_win);
	delwin(new_win);
	touchwin(old_win);
	wrefresh(old_win);
	}

WINDOW *InitScreen()

	{
	WINDOW *basic_w;

	SavedFcntl = fcntl(fileno(stdin), F_GETFL, 0);
	signal(SIGINT, zap);
	signal(SIGQUIT, dumpcore);
	signal(SIGPIPE, zap);
	initscr();
	noecho();
	crmode();
	basic_w = stdscr;
	werase(basic_w);
	box(basic_w, 0, 0);
	wrefresh(basic_w);
	return basic_w;
	}

void EndScreen()

	{
	echo();
	nocrmode();
	endwin();
	FixNodelay();
	}

int GetString(win, str, max_len)

WINDOW *win;
char *str;
int max_len;

	{
	int y, x;
	int len = 0;
	int ch;

	noecho();
	getyx(win, y, x);
	wrefresh(win);
	while ((ch = wgetch(win)) != '\n')
		{
		switch (ch)
		    {
		    case ASCII_BS :
		    case ASCII_DEL :
			if (len > 0)
				{
				--len;
				mvwaddch(win, y, x + len, ' ');
				}
			else
				flash();
			break;
		    default :
			if (len < max_len && isalnum(ch))
				{
				str[len] = ch;
				mvwaddch(win, y, x + len, ch);
				++len;
				}
			else
				flash();
			break;
		    }
		wmove(win, y, x + len);
		wrefresh(win);
		}
	str[len] = 0;

	return len;
	}

void BlockSignals()

	{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	}

static void zap(signo)

int signo;

	{
	if (! StopIO)
		{
		endwin();
		if (signo == SIGINT)
			fprintf(stderr, "You aborted!\n");
		else
			fprintf(stderr, "Your opponent aborted!\n");
		FixNodelay();
		KillChild();
		}
	EndFIFO();
	exit(1);
	}

static void dumpcore()

	{
	if (StopIO)
		return;
	endwin();
	FixNodelay();
	signal(SIGQUIT, SIG_DFL);
	KillChild();
	kill(getpid(), SIGQUIT);
	pause();
	}

static void KillChild()

	{
	if (ChildPid <= 0)
		return;
	signal(SIGCLD, SIG_IGN);
	kill(ChildPid, SIGKILL);
	}

static void FixNodelay()

	{
	(void)fcntl(fileno(stdin), F_SETFL, SavedFcntl);
	}

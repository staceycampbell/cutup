#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "consts.h"
#include "types.h"
#include "funcs.h"
#include "macros.h"

static int SavedFcntl;

static void
FixNodelay(void)
{
	(void)fcntl(fileno(stdin), F_SETFL, SavedFcntl);
}

static void
KillChild(void)
{
	if (ChildPid <= 0)
		return;
	signal(SIGCLD, SIG_IGN);
	kill(ChildPid, SIGKILL);
}

static void
zap(int signo)
{
	if (!StopIO)
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

static void
dumpcore(void)
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

WINDOW *
CreateWindow(WINDOW * old_win, int lines, int cols, int y, int x)
{
	WINDOW *front_win;

	front_win = newwin(lines, cols, y, x);
	touchwin(front_win);
	box(front_win, 0, 0);
	old_win = old_win;

	return front_win;
}

void
RemoveWindow(WINDOW * old_win, WINDOW * new_win)
{
	werase(new_win);
	wrefresh(new_win);
	delwin(new_win);
	touchwin(old_win);
	wrefresh(old_win);
}

WINDOW *
InitScreen(void)
{
	WINDOW *basic_w;

	SavedFcntl = fcntl(fileno(stdin), F_GETFL, 0);
	signal(SIGINT, zap);
	signal(SIGQUIT, (void *)dumpcore);
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

void
EndScreen(void)
{
	echo();
	nocrmode();
	endwin();
	FixNodelay();
}

int
GetString(WINDOW * win, char *str, int max_len)
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
		case ASCII_BS:
		case ASCII_DEL:
			if (len > 0)
			{
				--len;
				mvwaddch(win, y, x + len, ' ');
			}
			else
				flash();
			break;
		default:
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

void
BlockSignals(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>
#include <assert.h>
#include "consts.h"
#include "types.h"
#include "funcs.h"
#include "macros.h"

extern int IComputer;
extern int StopIO;

void
Welcome(WINDOW * wel_w)
{
	static char header[] = "C U T U P";

	wstandout(wel_w);
	mvwaddstr(wel_w, 3, CENTRE(header), header);
	wstandend(wel_w);
	wmove(wel_w, 5, 5);
	wprintw(wel_w, "Control the %c character...", S_US_CHAR);
	wmove(wel_w, 7, 5);
	wprintw(wel_w, "Avoid other %c and %c characters...", S_US_CHAR, S_THEM_CHAR);
	mvwaddstr(wel_w, 9, 5, "Avoid hitting the border...");
	mvwaddstr(wel_w, 11, 5, "First to crash is the loser...");
	wrefresh(wel_w);
	wstandout(wel_w);
	if (!IComputer)
	{
		mvwaddstr(wel_w, 14, 30, "Controls");
		wstandend(wel_w);
		wmove(wel_w, 16, 5);
		wprintw(wel_w, "Left: '%c'    Right: '%c'", S_LEFT_ALT_CH, S_RIGHT_ALT_CH);
		wmove(wel_w, 18, 5);
		wprintw(wel_w, "Up: '%c'      Down: '%c'", S_UP_ALT_CH, S_DOWN_ALT_CH);
		mvwaddstr(wel_w, 20, 25, "--press space to start--");
		wrefresh(wel_w);
		while (wgetch(wel_w) != ' ');
	}
	else
		sleep(2);
	werase(wel_w);
	box(wel_w, 0, 0);
	wrefresh(wel_w);
}

void
SeedRnd(void)
{
	extern long time();

	srand48(time((long *)0));
}

void
GetReady(WINDOW * win, int p_read, int p_write, int game_count, char *hom)
{
	WINDOW *ready_w = 0;
	char mesg[256];
	char waiting[256];
	char ack;
	int len, wait_len;
        int pipe_status;

	if (!IComputer)
	{
		sprintf(mesg, "Game: %d - press space to begin...", game_count);
		sprintf(waiting, "waiting for %s...", hom);
		len = strlen(mesg) + 3;
		wait_len = strlen(waiting) + 3;
		if (wait_len > len)
			len = wait_len;
		ready_w = CreateWindow(win, 3, len, 10, (COLS - len) / 2);
		mvwaddstr(ready_w, 1, 1, mesg);
		wrefresh(ready_w);
		while (wgetch(ready_w) != ' ');
		werase(ready_w);
		box(ready_w, 0, 0);
		mvwaddstr(ready_w, 1, 1, waiting);
		wrefresh(ready_w);
	}
	pipe_status = write(p_write, &ack, sizeof ack);
        assert(pipe_status >= 0);
	pipe_status = read(p_read, &ack, sizeof ack);
        assert(pipe_status >= 0);
	if (!IComputer)
		RemoveWindow(win, ready_w);
}

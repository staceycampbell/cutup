#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <curses.h>
#include "consts.h"
#include "types.h"
#include "funcs.h"
#include "macros.h"

#define FIFO_MODE (S_IFIFO | S_IRWXU | S_IRWXG | S_IRWXO)

extern int StopIO;

static char Fifo1[FN_MAX];
static char Fifo2[FN_MAX];

static int
PipeOpen(WINDOW * old_w, char *fifo1, char *fifo2, int *pipe1, int *pipe2, int mode1, int mode2, coords_t * c1, coords_t * c2)
{
	int status;

	c2->y = 10;
	c2->x = 20;
	c2->dir = S_WEST;
	c1->y = 10;
	c1->x = 57;
	c1->dir = S_EAST;
	if ((*pipe1 = open(fifo1, mode1)) == -1)
	{
		status = errno;
		DISPLAY_ERROR(old_w, "Open failure 1.");
		return status;
	}
	if ((*pipe2 = open(fifo2, mode2)) == -1)
	{
		status = errno;
		DISPLAY_ERROR(old_w, "Open failure 2.");
		return status;
	}
	return 0;
}

int
GetFIFOName(WINDOW * old_w, int *pipe_w, int *pipe_r, char *you, char *hom, coords_t * us, coords_t * them)
{
	int err, cmask;
	WINDOW *fifo_w = 0;
	char mesg[256];
	int len;

	cmask = umask(0);
	if (!StopIO)
	{
		sprintf(mesg, "waiting for %s...", hom);
		len = strlen(mesg) + 3;
		fifo_w = CreateWindow(old_w, 3, len, 1, 1);
		mvwaddstr(fifo_w, 1, 1, mesg);
		wrefresh(fifo_w);
	}
	sprintf(Fifo1, "/tmp/%s%s", you, hom);
	sprintf(Fifo2, "/tmp/%s%s", hom, you);
	if (mknod(Fifo1, FIFO_MODE, 0))
		if (mknod(Fifo2, FIFO_MODE, 0))
		{
			if ((err = PipeOpen(fifo_w, Fifo1, Fifo2, pipe_r, pipe_w, O_RDONLY, O_WRONLY, us, them)) != 0)
				return err;
		}
		else
		{
			if ((err = PipeOpen(fifo_w, Fifo2, Fifo1, pipe_r, pipe_w, O_RDONLY, O_WRONLY, them, us)) != 0)
				return err;
		}
	else if (mknod(Fifo2, FIFO_MODE, 0))
	{
		if ((err = PipeOpen(fifo_w, Fifo1, Fifo2, pipe_w, pipe_r, O_WRONLY, O_RDONLY, us, them)) != 0)
			return err;
	}
	else
	{
		if ((err = PipeOpen(fifo_w, Fifo2, Fifo1, pipe_w, pipe_r, O_WRONLY, O_RDONLY, them, us)) != 0)
			return err;
	}
	(void)umask(cmask);
	if (!StopIO && fifo_w)
		RemoveWindow(old_w, fifo_w);
	return 0;
}

void
EndFIFO(void)
{
	(void)unlink(Fifo1);
	(void)unlink(Fifo2);
}

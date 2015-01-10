#include <curses.h>
#include "consts.h"
#include "types.h"
#include "funcs.h"

#ifdef M_XENIX
void MillisecondSleep(ms)

int ms;

	{
	(void)nap(ms);
	}
#else
#ifdef SYS5_3

#include <sys/poll.h>

void MillisecondSleep(ms)

int ms;

	{
	(void) poll((struct pollfd *)0, 0L, ms);

	return (1);
	}
#else

/* this must be the last resort, single user machines only! */

MAKE SURE YOU CHECK THIS WHILE YOU ARE PORTING

int MillisecondSleep(ms)

int ms;

	{
	int start = clock();
	int end = ms * 1000;

	while (clock() - start < end);
	}
#endif
#endif

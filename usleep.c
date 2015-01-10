#include <curses.h>
#include "consts.h"
#include "types.h"
#include "funcs.h"

#ifdef M_XENIX
void
MillisecondSleep(int ms)
{
	(void)nap(ms);
}
#else

#include <sys/poll.h>

void
MillisecondSleep(int ms)
{
	(void)poll((struct pollfd *)0, 0L, ms);
}
#endif

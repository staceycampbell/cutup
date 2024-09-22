#include <curses.h>
#include <unistd.h>
#include "consts.h"
#include "types.h"
#include "funcs.h"

void
MillisecondSleep(int ms)
{
	(void)usleep(ms * 1000);
}

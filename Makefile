# Makefile for cutup

CC= gcc

# CFLAGS= -O				# non-XENIX systems can do this
CFLAGS= -O -DM_TERMINFO -DSYS5_3

# LIBS= -lcurses				# non-XENIX systems should do this 
LIBS= -lcurses -lx			# non-XENIX systems should do this
# LIBS= -ltinfo -lx

OBJS= cut.o scr.o fifo.o move.o initend.o comp.o usleep.o
SRC= cut.c scr.c fifo.c move.c initend.c comp.c usleep.c
INCLUDES= consts.h funcs.h macros.h types.h
EXE= cutup
LDFLAGS= -O

$(EXE): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(EXE) $(LIBS)

lint:
	lint $(CFLAGS) $(SRC) > lint.out

clean:
	rm -f $(OBJS) $(EXE) lint.out

shar:
	xshar README Makefile $(SRC) $(INCLUDES) > $(EXE).shar

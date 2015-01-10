CFLAGS := -O2 -Wall

LIBS := -lcurses

OBJS := cut.o scr.o fifo.o move.o initend.o comp.o usleep.o
INCLUDES := consts.h funcs.h macros.h types.h
EXE := cutup

$(EXE): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(EXE) $(LIBS)

$(OBJS): $(INCLUDES)

clean:
	rm -f $(OBJS) $(EXE)

# objects for target "test"
TESTOBJS = ICalendarParser.o util.o Arduino_testing.o CalConnector.o testing.o LedsMng.o TaskMgr.o
# objects for target "simrun"
SIMOBJS = ICalendarParser.o util.o Arduino_testing.o CalConnector.o main.o LedsMng.o simrun.o TaskMgr.o
CC   = g++
CPPFLAGS = -DTESTING -DFOREVERTODAY
# if you don't want to run demos, comment this out
#CPPFLAGS +=  -DICALBUFPARSDMO
#CPPFLAGS += -DWFCLIENTDMO
CFLAGS = -fpermissive -g -fstack-protector-all

# -c : compile and assemble only (no linking)
.cpp.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<
.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<
%.cpp: %.ino
	cp $< $@

all: test simrun

test:  $(TESTOBJS)
	$(CC) $(TESTOBJS) -o test

simrun: $(SIMOBJS)
	$(CC) $(CPPFLAGS) $(SIMOBJS) -o simrun

clean:
	rm *.o
	rm test
	rm simrun

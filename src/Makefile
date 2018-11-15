NAME = test
OBJS = ICalendarParser.o util.o Arduino_testing.o CalConnector.o testing.o
CC   = g++
CPPFLAGS = -DTESTING
# if you don't want to run demos, comment this out
#CPPFLAGS +=  -DICALBUFPARSDMO
#CPPFLAGS += -DWFCLIENTDMO
CFLAGS = -fpermissive -g -fstack-protector-all

# -c : compile and assemble only (no linking)
.cpp.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<
.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME)
clean:
	rm *.o
	rm $(NAME)

NAME = test
OBJS = ICalendarParser.o util.o Arduino_testing.o testing.o
CC   = g++
CPPFLAGS = -DTESTING
CFLAGS = -fpermissive -g

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
	rm $(TEST)

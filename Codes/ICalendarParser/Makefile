all: test

test: ICalendarParser.h ICalendarParser.cpp testing.cpp
	g++ -g -fpermissive -DTESTING testing.cpp -o test
	gdb ./test
proto: proto.c
	gcc -g proto.c -o test
	gdb ./test

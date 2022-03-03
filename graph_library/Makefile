CFLAGS=-O2 -ftrapv -fsanitize=undefined -Wall -Werror -Wformat-security -Wignored-qualifiers -Winit-self -Wswitch-default -Wfloat-equal -Wshadow -Wpointer-arith -Wtype-limits -Wempty-body -Wlogical-op -Wstrict-prototypes -Wold-style-declaration -Wold-style-definition -Wmissing-parameter-type -Wmissing-field-initializers -Wnested-externs -Wno-pointer-sign -Wcast-qual -Wwrite-strings -std=gnu11 -lm
solution: main.o graph.o graphout.o
	gcc main.o graph.o graphout.o -o solution $(CFLAGS)

main.o: main.c graph.h graphout.h mytypes.h
	gcc main.c -c $(FLAGS)

graph.o: graph.c mytypes.h
	gcc graph.c -c $(FLAGS)

graphout.o: graphout.c graph.h mytypes.h
	gcc graphout.c -c $(FLAGS)

test1: input1 solution
	./solution <input1 >output1

test2: input2 solution
	./solution <input2 >output2

test3: input3 solution
	./solution <input3 >output3

test4: input4 solution
	./solution <input4 >output4

test5: input5 solution
	./solution <input5 >output5

test6: input6 solution
	./solution <input6 >output6

clean:
	rm -rf *.o output* solution

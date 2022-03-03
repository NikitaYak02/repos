CFLAGS=-O2 -ftrapv -fsanitize=undefined -Wall -Werror -Wformat-security -Wignored-qualifiers -Winit-self -Wswitch-default -Wfloat-equal -Wshadow -Wpointer-arith -Wtype-limits -Wempty-body -Wlogical-op -Wstrict-prototypes -Wold-style-declaration -Wold-style-definition -Wmissing-parameter-type -Wmissing-field-initializers -Wnested-externs -Wno-pointer-sign -Wcast-qual -Wwrite-strings -std=gnu11 -lm -g

solution: main.o lexem_analyze.o executing.o build_tree.o
	gcc main.o lexem_analyze.o build_tree.o executing.o -o solution $(CFLAGS)

main.o: main.c mytypes.h lexem_analyze.h
	gcc main.c -c $(CFLAGS)

lexem_analyze.o: lexem_analyze.c mytypes.h
	gcc lexem_analyze.c -c $(CFLAGS)

build_tree.o: build_tree.c mytypes.h
	gcc build_tree.c -c $(CFLAGS)

executing.o: executing.c mytypes.h
	gcc executing.c -c $(CFLAGS)

clean:
	rm -f *.o solution

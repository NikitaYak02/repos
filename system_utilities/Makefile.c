CFLAGS=-O2 -ftrapv -fsanitize=undefined -Wall -Werror -Wformat-security -Wignored-qualifiers -Winit-self -Wswitch-default -Wfloat-equal -Wshadow -Wpointer-arith -Wtype-limits -Wempty-body -Wlogical-op -Wstrict-prototypes -Wold-style-declaration -Wold-style-definition -Wmissing-parameter-type -Wmissing-field-initializers -Wnested-externs -Wno-pointer-sign -Wcast-qual -Wwrite-strings -std=gnu11 -lm -g
wc: my_wc.c
	gcc my_wc.c -o solution $(CFLAGS)

cat: my_cat.c
	gcc my_cat.c -o solution $(CFLAGS)

cp:
	gcc my_cp.c -o solution $(CFLAGS)

clean:
	rm -f solution

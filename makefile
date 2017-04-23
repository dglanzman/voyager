all: main

%.o: %.c
	cc -c --std=c99 -g -o $@ $<

main: main.o arithmetic.o alpha.o
	cc --std=c99 -g -o $@ $^

tests: arithmetic.o alpha.o tests.o
	cc --std=c99 -g -o $@ `pkg-config --cflags --libs cmocka` $^

clean:
	rm -f *.o
	rm -f main
	rm -f tests

.PHONY: clean

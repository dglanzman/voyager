all: main

%.o: %.c
	cc -c --std=c99 -g -o $@ $<

main: main.o arithmetic.o alpha.o
	cc --std=c99 -g -o $@ $^

clean:
	rm -f *.o
	rm -f main

.PHONY: clean

all: decoder encoder tests

%.o: %.c
	c99 -c -g -o $@ $<

encoder: encoder.o arithmetic.o alpha.o generator.o
	c99 -g -o $@ $^

decoder: decoder.o arithmetic.o alpha.o pgz_routines.o generator.o
	c99 -g -o $@ $^

tests: arithmetic.o alpha.o tests.o generator.o test_message.o test_codeword.o \
pgz_routines.o
	c99 -g -o $@ $^ `pkg-config --cflags --libs cmocka`

clean:
	rm -f *.o
	rm -f encoder
	rm -f decoder
	rm -f tests
	rm -f hamlet.rs

.PHONY: clean

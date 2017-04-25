all: encoder tests

%.o: %.c
	c99 -c -g -o $@ $<

encoder: encoder.o arithmetic.o alpha.o linear_algebra.o generator.o
	c99 -g -o $@ $^

tests: arithmetic.o alpha.o tests.o linear_algebra.o generator.o test_message.o test_codeword.o
	c99 -g -o $@ $^ `pkg-config --cflags --libs cmocka`

clean:
	rm -f *.o
	rm -f encoder
	rm -f tests

.PHONY: clean

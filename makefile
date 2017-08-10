warning_options =
debug_info = -g
optimizations =
CFLAGS = $(warning_options) $(optimizations) $(debug_info)

all: decoder encoder tests

%.o: %.c
	c99 $(CFLAGS) -c -o $@ $<

encoder: encoder.o arithmetic.o alpha.o generator.o
	c99 $(CFLAGS) -o $@ $^

decoder: decoder.o arithmetic.o alpha.o pgz_routines.o generator.o
	c99 $(CFLAGS) -o $@ $^

tests: arithmetic.o alpha.o tests.o generator.o test_message.o test_codeword.o \
pgz_routines.o
	c99 $(CFLAGS) -o $@ $^ `pkg-config --cflags --libs cmocka`

clean:
	rm -f *.o
	rm -f *.gcov
	rm -f *.gcda
	rm -f *.gcno
	rm -f encoder
	rm -f decoder
	rm -f tests
	rm -f hamlet.rs

.PHONY: clean

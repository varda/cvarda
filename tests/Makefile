TEST_SOURCES = $(sort $(shell find . -name '*.c'))
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)
TEST_TARGETS = $(TEST_OBJECTS:.o=.out)

CC       = gcc
CFLAGS   = -std=c99 -march=native -Wall -Wextra -Wpedantic \
           -Wformat=2 -Wshadow -Wwrite-strings -Wstrict-prototypes \
           -Wold-style-definition -Wredundant-decls -Wnested-externs \
           -Wmissing-include-dirs -O0 -ggdb3 -DDEBUG

.PHONY: all clean

all: clean $(TEST_TARGETS)
	./run_tests.sh "$(TEST_TARGETS)" $(MEMCHECK)
	rm -f $(TEST_TARGETS)

clean:
	rm -f $(TEST_TARGETS)

%.out: %.o
	$(CC) $(CFLAGS) -o $@ $< $(addprefix ../, $(filter-out src/main.o, $(OBJECTS)))

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

INC_DIR = include
SRC_DIR = src

SOURCES  = $(sort $(shell find $(SRC_DIR) -name '*.c'))
OBJECTS  = $(SOURCES:.c=.o)
DEPS     = $(OBJECTS:.o=.d)

TARGET   = a.out

CC       = gcc
CFLAGS   = -std=c99 -march=native -Wall -Wextra -Wpedantic \
           -Wformat=2 -Wshadow -Wwrite-strings -Wstrict-prototypes \
           -Wold-style-definition -Wredundant-decls -Wnested-externs \
           -Wmissing-include-dirs $(addprefix -D, $(OPTIONS))
CPPFLAGS = $(addprefix -I, $(INC_DIR))

.PHONY: all check clean debug docs release

debug: CFLAGS += -O0 -ggdb3 -DDEBUG
debug: all

release: CFLAGS += -O3 -DNDEBUG
release: all

all: $(TARGET)

export OBJECTS
check: $(OBJECTS)
	$(MAKE) -C tests

clean:
	rm -f $(OBJECTS) $(DEPS) $(TARGET)

docs:
	$(MAKE) html -C doc

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $(TARGET) $(OBJECTS)

-include $(DEPS)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -o $@ -c $<

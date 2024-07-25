CC=gcc
CFLAGS=-Wall -Wextra -pedantic -g `pkg-config --cflags cairo`
LIBS=`pkg-config --libs cairo`

all: build/presentation

.PHONY: objects
objects: $(patsubst src/%.c, build/%.o, $(wildcard src/*.c))

build/%.o: src/%.c
	mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@ $(LIBS)

build/presentation: objects
	${CC} build/*.o ${LIBS} -o $@

.PHONY: clean
clean:
	rm -rf build

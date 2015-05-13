TARGET = build/tinylisp
SRC = env.c eval.c internals.c lisp.c obj.c utils.c

SOURCES = $(addprefix src/,${SRC})
TESTS = $(subst .c,,$(shell ls tests/*.c))
OBJS = $(addprefix build/,$(subst .c,.o,${SRC}))
CFLAGS = -g -std=c99 -Wall -Wextra -Wno-unused-parameter -Wno-gnu-union-cast
LDFLAGS = -lm

all: ${TARGET}

${TARGET}: build/main.o ${OBJS}
	${CC} -o $@ $^ ${LDFLAGS}

build/%.o: src/%.c
	${CC} -c -o $@ ${CPPFLAGS} ${CFLAGS} $<

build/%.o: tests/%.c
	${CC} -c -o $@ -I./lighttest2/ -I./src/ ${CPPFLAGS} ${CFLAGS} $<

tests/test_%: build/test_%.o ${OBJS}
	${CC} -o $@ $^ ${LDFLAGS}

.PHONY: clean mrproper test memtest all

test: ${TESTS}
	for f in $^; do ./$$f; done

memtest: ${TESTS}
	for f in $^; do valgrind --leak-check=full ./$$f; done

clean:
	rm -f build/*.o

mrproper: clean
	rm -f ${TARGET} ${TESTS}

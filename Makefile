TARGET = build/tinylisp
SRC = env.c eval.c internals.c lisp.c obj.c utils.c expr.c

SOURCES = $(addprefix src/,${SRC})
TESTS = $(subst .c,,$(shell ls tests/*.c))
TOOLS = $(subst .c,,$(shell ls tools/*.c))
OBJS = $(addprefix build/,$(subst .c,.o,${SRC}))
CFLAGS = -g -std=c99 -Wall -Wextra -Wno-unused-parameter -Wno-gnu-union-cast -Werror
LDFLAGS = -lm
VGFLAGS = --error-exitcode=1 --leak-check=full 

all: ${TARGET} ${TOOLS}

${TARGET}: build/main.o ${OBJS}
	${CC} -o $@ $^ ${LDFLAGS}

build/%.o: src/%.c
	${CC} -c -o $@ ${CPPFLAGS} ${CFLAGS} $<

build/%.o: tests/%.c
	${CC} -c -o $@ -I./lighttest2/ -I./src/ ${CPPFLAGS} ${CFLAGS} $<

build/%.o: tools/%.c
	${CC} -c -o $@ -I./src/ ${CPPFLAGS} ${CFLAGS} $<

tests/test_%: build/test_%.o ${OBJS}
	${CC} -o $@ $^ ${LDFLAGS}

tools/%: build/%.o ${OBJS}
	${CC} -o $@ $^ ${LDFLAGS}

.PHONY: clean mrproper test memtest vtest all

# Run tests
test: ${TESTS}
	for f in $^; do ./$$f || exit 1; done

# Run tests in valgrind
memtest: ${TESTS}
	for f in $^; do valgrind ${VGFLAGS} ./$$f && echo || exit 1; done

# Run tests in verbose mode
vtest: ${TESTS}
	for f in $^; do ./$$f -v || exit 1; done

clean:
	rm -f build/*.o

mrproper: clean
	rm -f ${TARGET} ${TESTS} ${TOOLS}

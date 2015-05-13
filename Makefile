TARGET = tinylisp
OBJ = lisp.o env.o utils.o obj.o eval.o internals.o
TESTS = test_env test_utils test_obj test_eval

#CC = clang-3.5
CFLAGS = -g -std=c99 -Wall -Wextra -Wno-unused-parameter -Wno-gnu-union-cast
LDFLAGS = -lm

all: ${TARGET}

${TARGET}: main.o ${OBJ}
	${CC} -o $@ $^ ${LDFLAGS}

test_%: test_%.o ${OBJ}
	${CC} -o $@ $^ ${LDFLAGS}

.PHONY: clean mrproper test memtest all

test: ${TESTS}
	for f in $^; do ./$$f; done

memtest: ${TESTS}
	for f in $^; do valgrind --leak-check=full ./$$f; done

clean:
	rm -f *.o

mrproper: clean
	rm -f ${TARGET} ${TESTS}

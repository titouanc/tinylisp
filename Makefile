TARGET = bin/tinylisp


SRC = $(subst src/,,$(shell ls src/*.c | grep -v 'tinylisp.c'))
TESTS = $(subst tests/,bin/,$(subst .c,,$(shell ls tests/*.c)))
TOOLS = $(subst tools/,bin/,$(subst .c,,$(shell ls tools/*.c)))
SNIPPETS = $(shell ls snippets/*.lisp)

OBJS = $(addprefix build/,$(subst .c,.o,${SRC}))
OBJSPROFILE = $(subst build/,build/profile-,${OBJS})
OBJSTESTS = $(subst tests/,build/,$(subst .c,.o,${TESTS}))
OBJSTOOLS = $(subst tools/,build/,$(subst .c,.o,${TOOLS}))

CPPFLAGS = 
CFLAGS = -std=c99 -Wall -Wextra -Wno-unused-parameter -Wno-gnu-union-cast -Werror
LDFLAGS = -lm
VALGRINDFLAGS = --error-exitcode=1 --leak-check=full --show-leak-kinds=all
GPROFFLAGS = -b

ifeq (${BUILD},release)
	CFLAGS += -march=native -O2
	CPPFLAGS += -DNDEBUG
else
	CFLAGS += -g
endif

all: ${TARGET} ${TOOLS}
release:
	make "BUILD=release"

# Compile
build/%.o: src/%.c
	${CC} -c -o $@ ${CPPFLAGS} ${CFLAGS} $<

build/%.o: tests/%.c
	${CC} -c -o $@ -I./lighttest2/ -I./src/ ${CPPFLAGS} ${CFLAGS} $<

build/%.o: tools/%.c
	${CC} -c -o $@ -I./src/ ${CPPFLAGS} ${CFLAGS} $<

build/profile-%.o: src/%.c
	${CC} -c -o $@ -pg ${CPPFLAGS} ${CFLAGS} $<

build/profile-%.o: tests/%.c
	${CC} -c -o $@ -pg -I./lighttest2/ -I./src/ ${CPPFLAGS} ${CFLAGS} $<

build/profile-%.o: tools/%.c
	${CC} -c -o $@ -pg -I./src/ ${CPPFLAGS} ${CFLAGS} $<

# Link
bin/profile-%: build/profile-%.o ${OBJSPROFILE}
	${CC} -o $@ $^ -pg ${LDFLAGS}

bin/%: build/%.o ${OBJS}
	${CC} -o $@ $^ ${LDFLAGS}

# Profile
profile: $(addsuffix .report,$(subst bin/,,${TESTS})) PROFILE $(subst .lisp,.report,${SNIPPETS})

PROFILE: bin/profile-tinylisp $(subst .lisp,.profile,${SNIPPETS})
	gprof ${GPROFFLAGS} $^ > $@

snippets/%.profile: snippets/%.lisp bin/profile-tinylisp
	./bin/profile-tinylisp < $< && mv gmon.out $@

snippets/%.report: bin/profile-tinylisp snippets/%.profile
	gprof ${GPROFFLAGS} $^ > $@

%.profile: bin/profile-%
	./$< && mv gmon.out $@

%.report: bin/profile-% %.profile
	gprof ${GPROFFLAGS} $^ > $@

.PHONY: all clean mrproper test memtest vtest profile
.PRECIOUS: build/%.o build/profile-%.o bin/profile-% %.profile snippets/%.profile

# Run tests
test: ${TESTS}
	for f in $^; do ./$$f || exit 1; done

# Run tests in valgrind
memtest: ${TESTS}
	for f in $^; do valgrind ${VALGRINDFLAGS} ./$$f -v && echo || exit 1; done

# Run tests in verbose mode
vtest: ${TESTS}
	for f in $^; do ./$$f -v || exit 1; done

clean:
	rm -f build/*.o *.profile *.report snippets/*.profile snippets/*.report PROFILE

mrproper: clean
	rm -f bin/*

include config.mk

OBJ=bin/main.o

all: mkbin options slackboat

mkbin:
	@mkdir -p bin

options:
	@echo slackboat build options:
	@echo "CFLAGS  = ${CFLAGS}"
	@echo "LDFLAGS = ${LDFLAGS}"
	@echo "CC      = ${CC}"

bin/%.o: src/%.c
	@echo CC $<
	@${CC} -static -o $@ -c ${CFLAGS} $<

slackboat: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}
	@echo $@ finished compiling.

clean:
	@echo cleaning
	@rm -f bin/*.o slackboat

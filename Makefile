CC = gcc
CFLAGS = -Wall
LEX = flex
YACC = bison
OBJS = scanner.o parser.o eval.o util.o varset.o builtin.o primitive.o stdlib.o cc_machine.o ck_machine.o cek_machine.o
SCANNER_C = lex.yy.c
PARSER_H = y.tab.h
PARSER_C = y.tab.c
GENERATED = $(SCANNER_C) $(PARSER_H) $(PARSER_C)

all: main test clean

debug: CFLAGS += -DDEBUG -g
debug: main test clean

main: main.c $(OBJS)
	$(CC) $(CFLAGS) -o main main.c $(OBJS)

test: test.c $(OBJS)
	$(CC) $(CFLAGS) -o test test.c $(OBJS)

scanner.o: $(PARSER_H) $(SCANNER_C)
	$(CC) $(CFLAGS) -c -o scanner.o $(SCANNER_C)

$(SCANNER_C) : scanner.l
	$(LEX) -o $(SCANNER_C) scanner.l

parser.o: $(PARSER_C)
	$(CC) $(CFLAGS) -c -o parser.o $(PARSER_C)

$(PARSER_H) $(PARSER_C): parser.y
	$(YACC) --defines=$(PARSER_H) -o $(PARSER_C) parser.y

eval.o: eval.h eval.c
	$(CC) $(CFLAGS) -c eval.c

util.o: util.h util.c
	$(CC) $(CFLAGS) -c util.c

varset.o: varset.h varset.c
	$(CC) $(CFLAGS) -c varset.c

builtin.o: builtin.h builtin.c
	$(CC) $(CFLAGS) -c builtin.c

primitive.o: primitive.c primitive.h
	$(CC) $(CFLAGS) -c primitive.c

stdlib.o: stdlib.c stdlib.h
	$(CC) $(CFLAGS) -c stdlib.c

cc_machine.o: cc_machine.c cc_machine.h
	$(CC) $(CFLAGS) -c cc_machine.c

ck_machine.o: ck_machine.c ck_machine.h
	$(CC) $(CFLAGS) -c ck_machine.c

cek_machine.o: cek_machine.c cek_machine.h
	$(CC) $(CFLAGS) -c cek_machine.c

clean:
	rm $(OBJS)

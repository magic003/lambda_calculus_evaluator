CC = gcc
LEX = flex
YACC = bison
OBJS = scanner.o parser.o eval.o util.o varset.o
SCANNER_C = lex.yy.c
PARSER_H = y.tab.h
PARSER_C = y.tab.c
GENERATED = $(SCANNER_C) $(PARSER_H) $(PARSER_C)

all: main test clean

main: main.c $(OBJS)
	$(CC) -o main main.c $(OBJS)

test: test.c $(OBJS)
	$(CC) -o test test.c $(OBJS)

scanner.o: $(PARSER_H) $(SCANNER_C)
	$(CC) -c -o scanner.o $(SCANNER_C)

$(SCANNER_C) : scanner.l
	$(LEX) -o $(SCANNER_C) scanner.l

parser.o: $(PARSER_C)
	$(CC) -c -o parser.o $(PARSER_C)

$(PARSER_H) $(PARSER_C): parser.y
	$(YACC) --defines=$(PARSER_H) -o $(PARSER_C) parser.y

eval.o: eval.h eval.c
	$(CC) -c eval.c

util.o: util.h util.c
	$(CC) -c util.c

varset.o: varset.h varset.c
	$(CC) -c varset.c

clean:
	rm $(OBJS)

FLAGS := --std=c99
CC := gcc

all : cfeeny 

cfeeny : src/cfeeny.c src/utils.c src/ast.c src/interpreter.c
	${CC}  $^  -o $@ ${FLAGS}

check: cfeeny
	./run_tests

clean:
	

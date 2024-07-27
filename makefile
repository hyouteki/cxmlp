default: run

run: test
	./test
	rm test

test: test.c node.h lexer.h parser.h
	gcc test.c -o test

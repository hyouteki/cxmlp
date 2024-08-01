default: run

run: test
	./test
	rm test

test: test.c xml_node.h lexer.h parser.h
	gcc test.c -Wall -Wextra -Wno-format -Wno-format-extra-args \
		-Wno-unused-result -Oz -I./ -Isteel/ -o test

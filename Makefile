

all: proalg

parser.c: parser.y
	bison -o $@ -d $<

scanner.c: scanner.flex
	flex -o $@ $<

proalg: parser.c scanner.c
	gcc -o $@ $? -lfl 


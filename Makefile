

all: proalg

parser.c: parser.y
	bison --defines=parser.tab.h -o $@ $<

scanner.c: scanner.flex
	flex -o $@ $<

proalg: parser.c scanner.c
	gcc -o $@ $? -lfl 
	
clean: 
	rm -f proalg *.c *.h 


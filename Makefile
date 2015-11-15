flex_debug = no
debug = no

ifeq ($(debug),yes)
	flex_debug = yes
endif

ifeq ($(flex_debug),yes)
	scanner = scanner_debug.flex
else
	scanner = scanner.flex
endif

all: proalg
	rm -f scanner_debug.flex

parser.c: parser.y
	bison --defines=parser.tab.h -o $@ $<

scanner_debug.flex: scanner.flex 
	cat $< | sed 's/{\([^}]*\)} [ ]*{\([^}]*\)}/{\1} {printf(\"shift: %s(%s)\\n\", \"\1\", yytext); \2}/g' > $@
	
scanner.c: $(scanner)
	flex -o $@ $<
	
proalg: parser.c scanner.c
	gcc -o $@ $^ -lfl 
	
	
	
clean: 
	rm -f proalg *.c *.h 
 

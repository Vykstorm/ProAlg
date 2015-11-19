flex_debug = no
bison_debug = no
debug = no


ifeq ($(debug),yes)
	flex_debug = yes
	bison_debug = yes
endif

ifeq ($(flex_debug),yes)
	scanner = scanner_debug.flex
else
	scanner = scanner.flex
endif

ifeq ($(bison_debug),yes)
	parser = parser_debug.y
else
	parser = parser.y
endif

all: proalg

parser_debug.y: parser.y
	cat parser.y | grep -n '%%' | head -n 1 | sed 's/\([0-9]*\):%%/\1/g' | xargs head parser.y -n > parser_debug.y
	cat parser.y | grep -n '%%' | tail -n 1 | sed 's/\([0-9]*\):%%/\1/g' | xargs head parser.y -n | head -n -1 > parser_debug2.y
	cat parser_debug2.y | grep -n '%%' | head -n 1 | sed 's/\([0-9]*\):%%/+\1/g' | xargs tail parser_debug2.y -n | tail -n +2 |  sed 's/T_\([a-zA-Z_]*\)/Tr_\1/g' >> parser_debug.y && 	rm -f parser_debug2.y
	cat parser.y | grep %token | sed 's/%token[ ]*T_\([a-zA-Z_]*\)/T_\1/g' | sed 's/%token[ ]*<[^>]*>[ ]*T_\([a-zA-Z_]*\)/T_\1/g' | sed 's/T_\([A-Za-z_]*\)/Tr_\1:\n\tT_\1 { printf(\"shift: %s\\n\", \"T_\1\"); }/g' >> parser_debug.y 	
	cat parser.y | grep -n '%%' | tail -n 1 | sed 's/\([0-9]*\):%%/+\1/g' | xargs tail parser.y -n >> parser_debug.y
	
	
parser.c: $(parser)
	bison --defines=parser.tab.h -o $@ $< $(bison_options)

scanner_debug.flex: scanner.flex 
	cat $< | sed 's/{\([^}]*\)} [ ]*{\([^}]*\)}/{\1} {printf(\"read-token: %s(%s)\\n\", \"\1\", yytext); \2}/g' > $@
	
scanner.c: $(scanner)
	flex -o $@ $<
	
proalg: parser.c scanner.c
	gcc -o $@ $^ -lfl 
	
	
	
clean: 
	rm -f proalg *.c *.h 
	rm -f scanner_debug.flex parser_debug.y
 

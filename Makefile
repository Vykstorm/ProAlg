
debug = no
parser_warnings = on
bison_options = 
debug_flags = 

ifeq ($(debug), yes)
	ifeq ($(scanner_debug), )
		scanner_debug = yes
	endif
	ifeq ($(parser_debug), )
		parser_debug = yes
	endif
	ifeq ($(tr_debug), )
		tr_debug = yes
	endif
	ifeq ($(ts_debug), )
		ts_debug = yes
	endif
else
	ifeq ($(scanner_debug), )
		scanner_debug = no
	endif
	ifeq ($(parser_debug), )
		parser_debug = no
	endif
	ifeq ($(tr_debug), )
		tr_debug = no
	endif
	ifeq ($(ts_debug), )
		ts_debug = no
	endif
endif
	
ifeq ($(scanner_debug),yes)
	scanner = scanner_debug.flex
else
	scanner = scanner.flex
endif

ifeq ($(parser_debug),yes)
	parser = parser.y
	bison_options += -v
else
	parser = parser.y
endif

ifeq ($(parser_warnings),on)
	bison_options +=--warnings='other,conflicts-sr,conflicts-rr'
else
	bison_options +=--warnings='no-other,no-conflicts-sr,no-conflicts-rr'
endif

ifeq ($(ts_debug),yes)
	debug_flags += -DTS_DEBUG
endif

ifeq ($(tr_debug),yes)
	debug_flags += -DTR_DEBUG
endif


modulos = tabla_simbolos.c traducciones.c errores.c $(shell find util -name "*.c")

all: proalg

parser.c: $(parser)
	bison --defines=parser.tab.h -o $@ $< $(bison_options)

scanner_debug.flex: scanner.flex 
	cat $< | sed 's/{\([^}]*\)} [ ]*{\([^}]*\)}/{\1} {printf(\"read-token: %s(%s)\\n\", \"\1\", yytext); \2}/g' > $@
	
scanner.c: $(scanner)
	flex -o $@ $<
	
proalg: parser.c scanner.c $(modulos)
	gcc -o $@ $^ -lfl $(debug_flags)
	
	
	
clean: 
	rm -f proalg parser.c scanner.c parser.tab.h
	rm -f scanner_debug.flex parser.output
 

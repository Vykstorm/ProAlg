/**
 * @file
 * 	\brief Este fichero es la implementación de un parser en bison. 
 * Puedes compilarlo con la utilidad bison. Usa la opción -d para generar
 * la cabecera parser.tab.h que flex necesita
 * @author Silvia Popa y Víctor Ruiz Gómez
 */
%{
#include <stdio.h>
#include <stdlib.h>
%}

/* Definición de yystype. Contiene los campos con los que podemos suministrar
 * información de los tokens leídos desde flex
 */
%union {
	char C_id[128];	
	int C_literal_entero;
	float C_literal_real;
	char C_literal_caracter[3];
	char C_literal_string[256];
	int C_literal_booleano; 
	char C_tipo_base[16];
}

/* Asociamos identificadores de tokens de bison a campos de yystype */
%token <C_literal_entero> T_literal_entero
%token <C_literal_real> T_literal_real
%token <C_literal_booleano> T_literal_booleano
%token <C_literal_caracter> T_literal_caracter
%token <C_literal_string> T_literal_string
%token <C_id> T_id
%token T_creacion_tipo
%token T_tupla
%token T_ftupla
%token T_tabla
%token T_comp_secuencial
%token T_de
%token T_subrango
%token T_inic_array
%token T_fin_array
%token T_ref
%token T_def_tipo_variable
%token <C_tipo_base> T_tipo_base
%%
	/* Zona de declaración de producciones de la gramática */
sentencia:
	lista_de_cte
|lista_de_tipo

/* Declaración de constantes y variables */
lista_de_cte:
	lista_de_cte definicion_cte
	| definicion_cte
definicion_cte:
	T_id T_creacion_tipo constante T_comp_secuencial  {printf("%s\n",$1);} 
constante:
	literal
	| T_literal_booleano {printf("%d\n",$1);}
literal:
	T_literal_entero {printf("%d\n",$1);}
	| T_literal_real {printf("%f\n",$1);}
	| T_literal_caracter {printf("%s\n",$1);}
	| T_literal_string {printf("%s\n",$1);}
	

/* Declaraciones de tipos */	
lista_de_tipo:
	lista_de_tipo definicion_tipo
	| definicion_tipo
definicion_tipo:
	T_id T_creacion_tipo d_tipo T_comp_secuencial 

d_tipo:
	T_tupla lista_campos T_ftupla 
	| T_tabla T_inic_array expresion_t T_subrango expresion_t T_fin_array T_de d_tipo
	| T_id
	|  expresion_t T_subrango expresion_t
	| T_ref d_tipo 
	| T_tipo_base {printf("%s\n",$1);}

expresion_t:
	T_literal_caracter
lista_campos:
	lista_campos definicion_campos
	|definicion_campos
definicion_campos:
	T_id T_def_tipo_variable d_tipo T_comp_secuencial 

%%
	/* Definición de procedimientos auxiliares */
int yyerror(const char* s) /* Error de parseo */
{
	printf("error de parseo: %s\n",s);
}
/* Rutina principal */
int main(int argc,char ** argv)
{
	yyparse();
}


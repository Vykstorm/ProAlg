/**
 * @file
 * 	\brief Este fichero es la implementación de un scanner en flex. 
 * Para compilarlo, es necesario generar primero el fichero parser.tab.h con
 * bison, usando la opción -d en la línea de comandos
 * Luego, puede usar la herramienta flex para traducir el código fuente de
 * este fichero, a código C
 * @author Silvia Popa y Víctor Ruiz Gómez
 */

%{
 /* Necesaria la inclusión de esta cabecera para que los tokens (definidos
  * por bison), puedan sean visibles para flex)
  */
#include "parser.tab.h"	
#include "tabla_simbolos.h"					
#include <stdio.h>


%}

/* Definición de las expresiones regulares */

/* Algunas expresiones regulares básicas */
letra [A-Za-z]
digito [0-9]
delim [ \t\n]
id {letra}({letra}|{digito})*

/* Expresiones regulares para algunos operadores */
asignacion ":="
comp_secuencial ";"
separador ","
subrango ".."
def_tipo_variable ":"
entonces "->"
si_no_si "[]"
creacion_tipo "="
inicio_array "["
fin_array "]"


/* Definición de expresiones regulares asociadas a expresiones aritméticas */
op_suma "+"
op_resta "-"
op_mult "*"
op_div "/"
op_mod "%"
inicio_parentesis "("
fin_parentesis ")"
op_rel ">"|"<"|">="|"<="|"="|"<>"
op_ref "."

/* Definición de expresiones regulares para las palabras reservadas */
r_accion (?i:"accion")
r_de (?i:"de")
r_algoritmo (?i:"algoritmo")
r_const (?i:"const")
r_continuar (?i:"continuar")
r_dev (?i:"dev")
r_div (?i:"div")
r_ent (?i:"ent")
r_es (?i:"e\/s")
r_faccion (?i:"faccion")
r_falgoritmo (?i:"falgoritmo")
r_fconst (?i:"fconst")
r_ffuncion (?i:"ffuncion")
r_fmientras (?i:"fmientras")
r_fpara (?i:"fpara")
r_fsi (?i:"fsi")
r_ftipo (?i:"ftipo")
r_ftupla (?i:"ftupla")
r_funcion (?i:"funcion")
r_fvar (?i:"fvar")
r_hacer (?i:"hacer")
r_hasta (?i:"hasta")
r_mientras (?i:"mientras")
r_mod (?i:"mod")
r_no (?i:"no")
r_o (?i:"o")
r_para (?i:"para")
r_ref (?i:"ref")
r_sal (?i:"sal")
r_si (?i:"si")
r_tabla (?i:"tabla")
r_tipo (?i:"tipo")
r_tupla (?i:"tupla")
r_var (?i:"var")
r_y (?i:"y")

r_verdadero(?i:"verdadero")
r_falso(?i:"falso")
r_entero (?i:"entero")
r_booleano (?i:"booleano")
r_caracter (?i:"caracter")
r_real (?i:"real")
r_string (?i:"string")


/* Definición de expresiones regulares asociadas a literales */
literal_entero (\-)?{digito}{digito}*((e|E){digito}{digito})? 
literal_real (\-)?{digito}{digito}*(\.{digito}{digito}*)?((e|E){digito}{digito}*)?
literal_caracter \'{letra}\'
literal_string \"([^\"]|\/\")*\"

/* Definición de la expresión regular para comentarios */
comentario \{([^\}]|\/\})*\} 


%%
	/* Zona de definición de reglas */
{delim} {  } 

	/* Reglas para literales */
{literal_entero} { yylval.C_literal_entero=atoi(yytext);return T_literal_entero;}
{literal_real} {yylval.C_literal_entero=atof(yytext); return T_literal_real; }
{literal_caracter} {strcpy(yylval.C_literal_string, yytext); return T_literal_caracter;}
{literal_string} { strcpy(yylval.C_literal_string, yytext);  return T_literal_string; }
{comentario} {return T_comentario;}
	/* Tenemos un caso especial para literales booleanos */
{r_verdadero} { yylval.C_literal_booleano=1; return T_literal_booleano; }
{r_falso} { yylval.C_literal_booleano=0; return T_literal_booleano; }

{asignacion} { return T_asignacion; }
{comp_secuencial} {return T_comp_secuencial; }
{separador} { return T_separador; }
{subrango} { return T_subrango; }
{def_tipo_variable} { return T_def_tipo_variable; }
{entonces} { return T_entonces; }
{si_no_si} { return T_si_no_si; }
{creacion_tipo} {return T_creacion_tipo; }
{inicio_array} { return T_inic_array; }
{fin_array} { return T_fin_array; }

	/* Reglas para operadores aritméticos */
{op_suma} { return T_suma; }
{op_resta} { return T_resta; }
{op_mult} { return T_mult; }
{op_div} { return T_div; }
{op_mod} { return T_mod; }
{op_rel} { strcpy(yylval.C_oprel, yytext); return T_oprel; }
{inicio_parentesis} { return T_inic_parentesis; }
{fin_parentesis} { return T_fin_parentesis; }
	

	/* Reglas para palabras reservadas */
{r_accion} { return T_accion; }
{r_real} { yylval.C_tipo_base = TS_REAL; return T_tipo_base;}
{r_entero} {yylval.C_tipo_base = TS_ENTERO; return T_tipo_base;}
{r_caracter} { yylval.C_tipo_base = TS_CARACTER; return T_tipo_base;}
{r_booleano} { yylval.C_tipo_base = TS_BOOLEANO; return T_tipo_base;}
{r_string} {  yylval.C_tipo_base = TS_STRING; return T_tipo_base;}
{r_ref} {return T_ref;}
{r_de} {return T_de;}
{r_algoritmo} { return T_algoritmo; }
{r_const} { return T_const; }
{r_continuar} { return T_continuar; }
{r_dev} { return T_dev; } 
{r_div} { return T_div_entera; }
{r_ent} { return T_ent; }
{r_es} { return T_es; }
{r_faccion} { return T_faccion; } 
{r_falgoritmo} { return T_falgoritmo; }
{r_fconst} { return T_fconst; }
{r_ffuncion} { return T_ffuncion; }
{r_fmientras} { return T_fmientras; }
{r_fpara} { return T_fpara; }
{r_fsi} { return T_fsi; }
{r_ftipo} { return T_ftipo; }
{r_ftupla} { return T_ftupla; } 
{r_funcion} { return T_funcion; } 
{r_fvar} { return T_fvar; }
{r_hacer} { return T_hacer; }
{r_hasta} { return T_hasta;  }
{r_mientras} { return T_mientras; }
{r_mod} { return T_mod; }
{r_no} { return T_no; }
{r_o} { return T_o; }
{r_para} { return T_para; } 
{r_sal} { return T_sal; }
{r_si} { return T_si; }
{r_tabla}  { return T_tabla; }
{r_tipo} { return T_tipo; }
{r_tupla} { return T_tupla; }
{r_var} { return T_var; }
{r_y} { return T_y; }
{op_ref} { return T_referencia; }	
	
{id} { strcpy(yylval.C_id, yytext); return T_id;}

%%

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
	char C_oprel[3];
}

/* Asociamos identificadores de tokens de bison a campos de yystype */
%token <C_literal_entero> T_literal_entero
%token <C_literal_real> T_literal_real
%token <C_literal_booleano> T_literal_booleano
%token <C_literal_caracter> T_literal_caracter
%token <C_literal_string> T_literal_string

%token <C_id> T_id

%token T_asignacion
%token T_comp_secuencial
%token T_separador
%token T_subrango
%token T_def_tipo_variable
%token T_entonces
%token T_si_no_si
%token T_creacion_tipo
%token T_inic_array
%token T_fin_array

%token T_suma
%token T_resta
%token T_div
%token T_mult
%token T_mod
%token T_inic_parentesis
%token T_fin_parentesis
%token <C_oprel> T_oprel
%token T_referencia


%token T_accion
%token T_real
%token T_entero
%token T_caracter
%token T_booleano
%token T_string
%token T_ref
%token T_de
%token T_algoritmo
%token T_const
%token T_continuar
%token T_dev 
%token T_ent
%token T_es
%token T_faccion 
%token T_falgoritmo
%token T_fconst
%token T_ffuncion
%token T_fmientras
%token T_fpara
%token T_fsi
%token T_ftipo
%token T_ftupla 
%token T_funcion 
%token T_fvar
%token T_hacer
%token T_hasta
%token T_mientras
%token T_no
%token T_o
%token T_para 
%token T_sal
%token T_si
%token T_tabla
%token T_tipo
%token T_tupla
%token T_var
%token T_y
%token T_div_entera
%token T_comentario

%token <C_tipo_base> T_tipo_base



%%
	/* Zona de declaración de producciones de la gramática */
axioma:
	desc_algoritmo
/* Declaración para la estructura básica de un programa ProAlg */
desc_algoritmo:
	T_algoritmo T_id cabecera_alg bloque_alg T_falgoritmo

cabecera_alg:
	decl_globales decl_a_f decl_ent_sal T_comentario
bloque_alg:
	bloque T_comentario

decl_globales:
	decl_globales declaracion_tipo
	|decl_globales declaracion_cte
	|

decl_a_f:
	decl_a_f accion_d
	|decl_a_f funcion_d
	|

bloque:
	declaraciones instrucciones
	| instrucciones
	
declaraciones:
	declaraciones declaracion_tipo
	| declaraciones declaracion_cte
	| declaraciones declaracion_var
	|
	
	
/* Declaraciones para expresiones */
expresion:
	exp_a
	| exp_b

exp_a:
	exp_a T_suma exp_a 
	| exp_a T_resta exp_a 
	| exp_a T_mult exp_a 
	| exp_a T_div_entera exp_a 
	| exp_a T_div exp_a 
	| T_inic_parentesis exp_a T_fin_parentesis
	| operando
	| T_literal_entero
	| T_literal_real
	| T_resta exp_a
	| exp_a T_mod exp_a

exp_b:
	exp_b T_y exp_b
	| exp_b T_o exp_b 
	| T_no exp_b 
	| operando
	| T_literal_booleano 
	| expresion T_oprel expresion
	| T_inic_parentesis exp_b T_fin_parentesis 

operando:
	T_id
	| operando T_referencia operando 
	| operando T_inic_array expresion T_fin_array 
	| operando T_ref



/* Declaración para instrucciones */
instrucciones:
	instrucciones T_comp_secuencial instruccion 
	| instruccion 

instruccion:
	T_continuar  { printf("instruccion\n"); }
	| asignacion { printf("instruccion\n"); }
	| alternativa { printf("instruccion\n"); }
	| iteracion { printf("instruccion\n"); }

asignacion:
	operando T_asignacion expresion 

alternativa:
	T_si expresion T_entonces instrucciones lista_opciones T_fsi 

lista_opciones:
	T_si_no_si expresion T_entonces instrucciones lista_opciones 
	|

iteracion:
	it_cota_fija
	| it_cota_exp

it_cota_exp:
	T_mientras expresion T_hacer instrucciones T_fmientras 

it_cota_fija:
	T_para T_id T_asignacion expresion T_hasta expresion T_hacer instrucciones T_fpara 
	


/* Declaraciones */
declaracion_tipo:
	T_tipo lista_de_tipo T_ftipo T_comp_secuencial
declaracion_cte:
	T_const lista_de_cte T_fconst T_comp_secuencial
declaracion_var:
	T_var lista_de_var T_fvar T_comp_secuencial

/* Declaraciones de tipos */	
lista_de_tipo:
	T_id T_creacion_tipo d_tipo T_comp_secuencial lista_de_tipo
	|

d_tipo:
	T_tupla lista_campos T_ftupla 
	| T_tabla T_inic_array expresion_t T_subrango expresion_t T_fin_array T_de d_tipo { }
	| T_id
	|  expresion_t T_subrango expresion_t
	| T_ref d_tipo 
	| T_tipo_base { }	

expresion_t:
	expresion 
	| T_literal_caracter
lista_campos:
	T_id T_def_tipo_variable d_tipo T_comp_secuencial lista_campos
	|


/* Declaración de constantes y variables */
lista_de_cte:
	T_id T_creacion_tipo constante T_comp_secuencial lista_de_cte
	|

constante:
	literal
	| T_literal_booleano {}
literal:
	T_literal_entero {}
	| T_literal_real {}
	| T_literal_caracter {}
	| T_literal_string {}

lista_de_var:
	lista_id T_def_tipo_variable T_id T_comp_secuencial lista_de_var
	| lista_id T_def_tipo_variable d_tipo T_comp_secuencial lista_de_var
	|
	
lista_id:
	T_id T_separador lista_id 
	| T_id
	| 

decl_ent_sal: 
	decl_ent
	| decl_ent decl_sal 
	| decl_sal 
decl_ent: 
	T_ent lista_de_var
decl_sal:
	T_sal lista_de_var


/* Acciones y funciones */
accion_d:
	T_accion a_cabecera bloque T_faccion 

funcion_d:
	T_funcion f_cabecera bloque T_dev expresion T_ffuncion

a_cabecera:
	T_id T_inic_parentesis d_par_form T_fin_parentesis T_comp_secuencial 

f_cabecera:
	T_id T_inic_parentesis lista_de_var T_fin_parentesis T_dev d_tipo T_comp_secuencial

d_par_form:
	d_p_form T_comp_secuencial d_par_form
	| d_p_form 
	|
d_p_form:
	T_ent lista_id T_def_tipo_variable d_tipo
	| T_sal lista_id T_def_tipo_variable d_tipo
	| T_es lista_id T_def_tipo_variable d_tipo

accion_ll:
	T_id T_inic_parentesis l_ll T_fin_parentesis 
	
funcion_ll:
	T_id T_inic_parentesis l_ll T_fin_parentesis

l_ll:
	expresion T_separador expresion
	| expresion
	|

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


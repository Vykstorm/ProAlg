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
#include <string.h>
#include "tabla_simbolos.h"

%}

%code requires
{
	#include "util/pila.h"
	#include "tabla_simbolos.h"
	#include "traducciones.h"
	
	/* estructura auxiliar para valores semánticos de ctes */
	typedef struct C_cte_t
	{
		int tipo;  /* tipo de cte */
		TS_cte_val val; /* el valor de la cte */
	} C_cte_t;
	
	/* estructura auxiliar para valores semánticos de exp aritméticas */
	typedef struct C_exp_a_t
	{
		int tipo; 
		int place; /* apuntador a la tabla de símbolos */
	} C_exp_a_t;

	/* estructura auxiliar para valores semánticos de exp booleanas */
	typedef struct C_exp_b_t
	{
		lista true,false;
	} C_exp_b_t;
}

/* Definición de yystype. Contiene los campos con los que podemos suministrar
 * información de los tokens leídos desde flex
 */
%union {
	char C_id[128];	
	int C_literal_entero;
	float C_literal_real;
	char C_literal_caracter;
	char C_literal_string[256];
	int C_literal_booleano; 
	int C_tipo_base;
	char C_oprel[3];
	
	
	pila C_lista_id; /* para listados de ids */
	C_cte_t C_cte;	/* para la declaración de ctes */
	TS_tipo C_registro_tipo;  /* para la declaración de tipos */
	C_exp_a_t C_exp_a; /* para expresiones aritméticas */
	C_exp_b_t C_exp_b; /* para expresiones booleanas */
	
	int M_b; /*para reducciones por cadena vacia para las expresiones booleanas*/
	
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



/* Asociamos no terminales a campos de la unión */
%type <C_lista_id> lista_id
%type <C_tipo_base> dd_tipo
%type <C_cte> literal
%type <C_cte> constante

%type <C_exp_a> operando
%type <C_exp_a> exp_a
%type <M_b> M_b
%type <C_exp_b> exp_b
%type <C_exp_b> operando_b


/* Indicamos la asociatividad y prioridad de los operadores */
%left T_o
%left T_y
%right T_no

%left T_suma T_resta
%left T_mult T_div T_div_entera
%left T_mod



%right T_referencia
%left T_ref

%nonassoc T_oprel

%left T_comp_secuencial
%left T_separador

%%
	/* Zona de declaración de producciones de la gramática */
axioma:
	declaracion_var exp_b
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
	|funcion_ll

exp_a:
	exp_a T_suma exp_a { 
		int T=TS_newtempvar();
		$$.place = T;
		if($1.tipo == TS_ENTERO)
		{
			if($3.tipo == TS_ENTERO)
			{
				TS_modificar_simbolo(T,TS_VAR|TS_ENTERO);
				gen_asig_binaria(TR_OP_SUMA, $1.place, $3.place, T);
			}
			else
			{
				TS_modificar_simbolo(T,TS_VAR|TS_REAL);
				gen_asig_unaria(TR_OP_INT_TO_REAL, $1.place, T);
				gen_asig_binaria(TR_OP_SUMA_REAL, T, $3.place, T);
			}
		} 
		else 
		{	
			if($3.tipo == TS_ENTERO)
			{
				TS_modificar_simbolo(T,TS_VAR|TS_REAL);
				gen_asig_unaria(TR_OP_INT_TO_REAL, $3.place, T);
				gen_asig_binaria(TR_OP_SUMA_REAL, $1.place, T, T);
			}
			else
			{
				TS_modificar_simbolo(T,TS_VAR|TS_REAL);
				gen_asig_binaria(TR_OP_SUMA_REAL, $1.place, $3.place, T);
			}
		}

	}
	| exp_a T_resta exp_a { 
		int T=TS_newtempvar();
		$$.place = T;
		if($1.tipo == TS_ENTERO)
		{
			if($3.tipo == TS_ENTERO)
			{
				TS_modificar_simbolo(T,TS_VAR|TS_ENTERO);
				gen_asig_binaria(TR_OP_RESTA, $1.place, $3.place, T);
			}
			else
			{
				TS_modificar_simbolo(T,TS_VAR|TS_REAL);
				gen_asig_unaria(TR_OP_INT_TO_REAL, $1.place, T);
				gen_asig_binaria(TR_OP_RESTA_REAL, T, $3.place, T);
			}
		} 
		else 
		{	
			if($3.tipo == TS_ENTERO)
			{
				TS_modificar_simbolo(T,TS_VAR|TS_REAL);
				gen_asig_unaria(TR_OP_INT_TO_REAL, $3.place, T);
				gen_asig_binaria(TR_OP_RESTA_REAL, $1.place, T, T);
			}
			else
			{
				TS_modificar_simbolo(T,TS_VAR|TS_REAL);
				gen_asig_binaria(TR_OP_RESTA_REAL, $1.place, $3.place, T);
			}
		}

	}
	| exp_a T_mult exp_a { 
		int T=TS_newtempvar();
		$$.place = T;
		if($1.tipo == TS_ENTERO)
		{
			if($3.tipo == TS_ENTERO)
			{
				/* si es una multiplicación en la que uno de los operandos es
				 * una cte(sabemos su valor) y es una potencia de 2, en vez de usar la instrucción producto,
				 * podemos hacer un shift*/
				TS_modificar_simbolo(T,TS_VAR|TS_ENTERO);
				if(((TS_consultar_tipo($1.place)&TS_CTE) == TS_CTE) && es_potencia_2(TS_consultar_cte($1.place).entero))
				{
					/* creo una nueva cte en la tabla de símbolos, con el valor del logaritmo en base 2 del
					 * operando que es potencia de  2 */
					int L = TS_newliteral();
					TS_modificar_simbolo(L, TS_CTE|TS_ENTERO);
					TS_cte_val val; val.entero = log_base_2(TS_consultar_cte($1.place).entero);
					TS_modificar_cte(L,val);
					
					
					gen_asig_binaria(TR_OP_SHIFT_LEFT, $3.place, L, T);
				}
				else if(((TS_consultar_tipo($3.place)&TS_CTE) == TS_CTE) && es_potencia_2(TS_consultar_cte($3.place).entero))
				{
					int L = TS_newliteral();
					TS_modificar_simbolo(L,TS_CTE|TS_ENTERO);
					TS_cte_val val; val.entero = log_base_2(TS_consultar_cte($3.place).entero);
					TS_modificar_cte(L,val);
					
					gen_asig_binaria(TR_OP_SHIFT_LEFT, $1.place, L, T);
				}
				else
					gen_asig_binaria(TR_OP_MULT, $1.place, $3.place, T);
			}
			else
			{
				TS_modificar_simbolo(T,TS_VAR|TS_REAL);
				gen_asig_unaria(TR_OP_INT_TO_REAL, $1.place, T);
				gen_asig_binaria(TR_OP_MULT_REAL, T, $3.place, T);
			}
		} 
		else 
		{	
			if($3.tipo == TS_ENTERO)
			{
				TS_modificar_simbolo(T,TS_VAR|TS_REAL);
				gen_asig_unaria(TR_OP_INT_TO_REAL, $3.place, T);
				gen_asig_binaria(TR_OP_MULT_REAL, $1.place, T, T);
			}
			else
			{
				TS_modificar_simbolo(T,TS_VAR|TS_REAL);
				gen_asig_binaria(TR_OP_MULT_REAL, $1.place, $3.place, T);
			}
		}
}
	| exp_a T_div_entera exp_a { 
		int T=TS_newtempvar();
		$$.place = T;
		if(($1.tipo == TS_ENTERO) && ($3.tipo == TS_ENTERO))
		{
			TS_modificar_simbolo(T,TS_VAR|TS_ENTERO);
			gen_asig_binaria(TR_OP_DIV, $1.place, $3.place, T);
		}
		else
		{
			/* error */
		}
	} 
	| exp_a T_div exp_a { 
		int T=TS_newtempvar();
		$$.place = T;
		if($1.tipo == TS_ENTERO)
		{
			if($3.tipo == TS_ENTERO)
			{
				int S=TS_newtempvar();
				TS_modificar_simbolo(S,TS_VAR|TS_REAL);
				TS_modificar_simbolo(T,TS_VAR|TS_REAL);
				gen_asig_unaria(TR_OP_INT_TO_REAL, $1.place, T);
				gen_asig_unaria(TR_OP_INT_TO_REAL, $3.place, S);
				gen_asig_binaria(TR_OP_DIV, T, S, T);
			}
			else
			{
				TS_modificar_simbolo(T,TS_VAR|TS_REAL);
				gen_asig_unaria(TR_OP_INT_TO_REAL, $1.place, T);
				gen_asig_binaria(TR_OP_DIV_REAL, T, $3.place, T);
			}
		} 
		else 
		{	
			if($3.tipo == TS_ENTERO)
			{
				TS_modificar_simbolo(T,TS_VAR|TS_REAL);
				gen_asig_unaria(TR_OP_INT_TO_REAL, $3.place, T);
				gen_asig_binaria(TR_OP_DIV_REAL, $1.place, T, T);
			}
			else
			{
				TS_modificar_simbolo(T,TS_VAR|TS_REAL);
				gen_asig_binaria(TR_OP_DIV_REAL, $1.place, $3.place, T);
			}
		}
	}
	| T_inic_parentesis exp_a T_fin_parentesis { $$ = $2; }
	| operando { $$ = $1; }
	| T_literal_entero { int L=TS_newliteral(); TS_modificar_simbolo(L, TS_CTE|TS_ENTERO); TS_cte_val val; val.entero=$1; TS_modificar_cte(L, val); $$.place = L; $$.tipo = TS_ENTERO;  }
	| T_literal_real { int L=TS_newliteral(); TS_modificar_simbolo(L, TS_CTE|TS_REAL); TS_cte_val val;  val.real=$1; TS_modificar_cte(L, val); $$.place = L; $$.tipo = TS_REAL;  }
	| T_resta exp_a { int T=TS_newtempvar(); TS_modificar_simbolo(T,TS_VAR|$2.tipo); $$.place = T; $$.tipo = $2.tipo; if($2.tipo == TS_REAL) { gen_asig_unaria($2.place, TR_OP_NEG_REAL , T); } else { gen_asig_unaria($2.place, TR_OP_NEG , T); }  }
	| exp_a T_mod exp_a


//Expresiones booleanas
exp_b:
	exp_b T_y M_b exp_b { backpatch($$.true, $3); $$.false = merge($1.false, $4.false); $$.true = $4.true;  }
	| exp_b T_o M_b exp_b { backpatch($$.false, $3); $$.true = merge($1.true, $4.true); $$.false = $4.false;  } 
	| T_no exp_b { $$.true = $2.false; $$.false = $$.true; } 
	| operando_b { $$ = $1;  }
	| T_literal_booleano {$$.true = makelist(nextquad()); $$.false = makelist(nextquad()+1); gen_salto_incondicional(-1); gen_salto_incondicional(-1); if(!$1) { lista aux = $$.false; $$.false = $$.true; $$.true = aux; }  }
	| expresion T_oprel expresion {  }
	| T_inic_parentesis exp_b T_fin_parentesis { $$ = $2; }
	
	// debe estar en operando_b
	| T_id { int id; if(((id=TS_buscar_simbolo($1)) == -1) || !((TS_consultar_tipo(id)&TS_BOOLEANO)==TS_BOOLEANO)) { /* error */  } else{ $$.true = makelist(nextquad()); $$.false = makelist(nextquad()+1);  gen_salto_condicional2(id,-1); gen_salto_incondicional(-1); }}

M_b:
	| {$$=nextquad();}

operando:
	T_id  { int id; if((id=TS_buscar_simbolo($1)) == -1) { /* error */ } else {  $$.place = id; int tipo=TS_consultar_tipo(id); if(((tipo&0x00FF) == TS_VAR) && (((tipo&0xFF00) == TS_REAL) || ((tipo&0xFF00) == TS_ENTERO)) ) { $$.tipo = tipo&0xFF00; } else { /* error */ }  }  }
	// T_id  { int id =TS_insertar_simbolo($1); TS_modificar_simbolo(id, TS_VAR|TS_REAL); $$.place = id; int tipo=TS_consultar_tipo(id); if(((tipo&0x00FF) == TS_VAR) && (((tipo&0xFF00) == TS_REAL) || ((tipo&0xFF00) == TS_ENTERO)) ) { $$.tipo = tipo&0xFF00; } else { /* error */  }    }
	| operando T_referencia operando 
	| operando T_inic_array expresion T_fin_array 
	| operando T_ref

operando_b:
	// T_id { printf("hola!!!\n"); int id; if(((id=TS_buscar_simbolo($1)) == -1) || !((TS_consultar_tipo(id)&TS_BOOLEANO)==TS_BOOLEANO)) { /* error */  } else{ $$.true = makelist(nextquad()); $$.false = makelist(nextquad()+1);  gen_salto_condicional2(id,-1); gen_salto_incondicional(-1); }}
/*	| operando_b T_referencia operando_b 
	| operando_b T_inic_array expresion T_fin_array 
	| operando_b T_ref*/


/* Declaración para instrucciones */
instrucciones:
	instrucciones T_comp_secuencial instruccion 
	| instruccion 

instruccion:
	T_continuar
	| asignacion
	| alternativa 
	| iteracion
	|accion_ll

asignacion:
	operando T_asignacion expresion { }

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
	T_id T_creacion_tipo dd_tipo T_comp_secuencial lista_de_tipo
	| T_id T_creacion_tipo T_id T_comp_secuencial lista_de_tipo

d_tipo:
	dd_tipo 
	| T_id 
	
dd_tipo:
	T_tupla lista_campos T_ftupla { $$ = TS_TUPLA; }
	| T_tabla T_inic_array expresion_t T_subrango expresion_t T_fin_array T_de d_tipo { $$ = TS_TABLA; }
	| T_id { $$ = TS_UNKNOWN; } 
	|  expresion_t T_subrango expresion_t { $$ = TS_UNKNOWN; }
	| T_ref d_tipo { $$ = TS_PUNTERO; }
	| T_tipo_base { $$ = $1; }	

expresion_t:
	expresion 
	| T_literal_caracter
lista_campos:
	T_id T_def_tipo_variable d_tipo T_comp_secuencial lista_campos
	|


/* Declaración de constantes y variables */
lista_de_cte:
	T_id T_creacion_tipo constante T_comp_secuencial lista_de_cte { int id=TS_insertar_simbolo($1); TS_modificar_simbolo(id, TS_CTE | $3.tipo); TS_modificar_cte(id,$3.val); }
	|

constante:
	literal { $$ = $1; } 
	| T_literal_booleano { $$.tipo = TS_BOOLEANO; $$.val.booleano = $1; }
literal:
	T_literal_entero { $$.tipo = TS_ENTERO; $$.val.entero = $1;  }
	| T_literal_real { $$.tipo = TS_REAL; $$.val.real = $1; }
	| T_literal_caracter { $$.tipo = TS_CARACTER; $$.val.caracter = $1; }
	| T_literal_string { $$.tipo = TS_STRING; strcpy($$.val.string, $1); }

lista_de_var:
	lista_id T_def_tipo_variable T_id T_comp_secuencial lista_de_var { while(!pila_vacia($1)) { TS_modificar_var(desapilar($1), $3); } }
	| lista_id T_def_tipo_variable dd_tipo T_comp_secuencial lista_de_var { while(!pila_vacia($1)) { TS_modificar_simbolo(desapilar($1), TS_VAR | $3); } } 
	|
lista_id:
	T_id T_separador lista_id { apilar($3, TS_insertar_simbolo($1)); $$ = $3; }
	| T_id { $$ = crear_pila(); apilar($$, TS_insertar_simbolo($1)); }

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
	/* Métodos auxiliares */
	int es_potencia_2(int n)
	{
		if(n < 0)
			n = -n;
		else if(n == 0)
			return 0;
		while((n > 1) && ((n % 2) == 0))
			n /= 2;
		return n == 1;
	}
	
	int log_base_2(int n)
	{
		// n debe ser potencia de 2 y mayor que 0
		int m = 0;
		while(n > 1)
		{
			n /= 2;
			m++;
		}
		return m;
	}
	
	/* Definición de procedimientos auxiliares */
int yyerror(const char* s) /* Error de parseo */
{
	printf("error de parseo: %s\n",s);
}
/* Rutina principal */
int main(int argc,char ** argv)
{
	TS_inicializar();
	yyparse();
	TS_printdebug();
	TS_liberar();
}


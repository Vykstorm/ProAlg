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
#include "errores.h"

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
	
	/* estructura auxiliar para valores semánticos de exp aritméticas y booleanas */
	typedef struct C_exp_t
	{
		int tipo; /* es TS_REAL, TS_ENTERO, TS_BOOLEANO, ... */
		union
		{
			int place; /* apuntador a la tabla de símbolos  */
			struct
			{
				lista true,false; /* son listas de cuadruplas (se rellenan solo cuando es booleano) */
			};
		};
	} C_exp_t;

	/* estructura auxiliar para valores semánticos de las instrucciones */
	typedef struct C_instr_t
	{
		lista next; 
	} C_instr_t;
	
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
	int C_oprel; 
	
	
	pila C_lista_id; /* para listados de ids */
	C_cte_t C_cte;	/* para la declaración de ctes */
	int C_tipo;  /* para la declaración de tipos */
	C_exp_t C_exp; /* para expresiones aritméticas/lógicas/llamadas a funciones */
	C_instr_t C_instr; /* para instrucciones */
	int E; /* para reducciones por cadena vacia (tienen como valor semántico nextquad()) */
	int V; /* se usa en la estructura iterativa de cota fija, para almacenar la id de la variable de control */
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
%type <C_tipo> d_tipo
%type <C_tipo> dd_tipo
%type <C_cte> literal
%type <C_cte> constante

%type <C_exp> operando
%type <C_exp> operando_a
%type <C_exp> operando_b
%type <C_exp> exp_a
%type <C_exp> exp_b
%type <C_exp> expresion
%type <C_exp> condicion
%type <C_exp> expresion_f

%type <C_instr> instrucciones
%type <C_instr> instruccion
%type <C_instr> asignacion
%type <C_instr> alternativa
%type <C_instr> iteracion
%type <C_instr> it_cota_exp
%type <C_instr> it_cota_fija
%type <V> it_cota_fija_c
%type <C_instr> lista_opciones
%type <C_instr> P

%type <E> M
%type <E> N


/* Indicamos la asociatividad y prioridad de los operadores */

// Asociatividad en op. de referencia 
%left T_referencia
%left T_inic_array
%left T_ref

// Prioridad en op. booleanos
%left T_y
%left T_o
%left T_no

// Prioridad en op. aritméticos. 
%left T_suma T_resta
%left T_mult T_div T_div_entera
%left T_mod


%%
	/* Zona de declaración de producciones de la gramática */
axioma:
	desc_algoritmo { CHECK_PARSE_ERRORS; }
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
	| %empty

decl_a_f:
	decl_a_f accion_d
	|decl_a_f funcion_d
	| %empty

bloque:
	declaraciones instrucciones
	
declaraciones:
	declaraciones declaracion_tipo
	| declaraciones declaracion_cte
	| declaraciones declaracion_var
	| %empty
	
	
/* Declaraciones para expresiones */
expresion: 
	exp_a  { $$.place = $1.place; $$.tipo = TS_consultar_tipo($1.place)&0xFF00; }
	| exp_b { $$.true = $1.true; $$.false = $1.false; $$.tipo = TS_BOOLEANO; }
	
// Expresiones aritméticas
exp_a:
	operando_a T_suma operando_a { 
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
	| operando_a T_resta operando_a { 
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
	| operando_a T_mult operando_a { 
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
	| operando_a T_div_entera operando_a { 
		int T=TS_newtempvar();
		$$.place = T;
		if(($1.tipo == TS_ENTERO) && ($3.tipo == TS_ENTERO))
		{
			TS_modificar_simbolo(T,TS_VAR|TS_ENTERO);
			gen_asig_binaria(TR_OP_DIV, $1.place, $3.place, T);
		}
		else
		{
			if($1.tipo != TS_ENTERO)
			{
				PARSE_ERROR(E_OPERANDO_NO_VALIDO, $1.place);
			}
			else
			{
				PARSE_ERROR(E_OPERANDO_NO_VALIDO, $3.place);
			}	
		}
	} 
	| operando_a T_div operando_a { 
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
	| T_literal_entero { int L=TS_newliteral(); TS_modificar_simbolo(L, TS_CTE|TS_ENTERO); TS_cte_val val; val.entero=$1; TS_modificar_cte(L, val); $$.place = L; $$.tipo = TS_ENTERO;  }
	| T_literal_real { int L=TS_newliteral(); TS_modificar_simbolo(L, TS_CTE|TS_REAL); TS_cte_val val;  val.real=$1; TS_modificar_cte(L, val); $$.place = L; $$.tipo = TS_REAL;  }
	| T_resta operando_a { int T=TS_newtempvar(); TS_modificar_simbolo(T,TS_VAR|$2.tipo); $$.place = T; $$.tipo = $2.tipo; if($2.tipo == TS_REAL) { gen_asig_unaria($2.place, TR_OP_NEG_REAL , T); } else { gen_asig_unaria($2.place, TR_OP_NEG , T); }  }
	| operando_a T_mod operando_a {
		if(($1.tipo == TS_ENTERO) && ($3.tipo == TS_ENTERO))
		{
			int T = TS_newtempvar();
			TS_modificar_simbolo(T,TS_VAR|TS_ENTERO);
			gen_asig_binaria(TR_OP_MOD, $1.place, $3.place, T);
		}
		else
		{
			if($1.tipo != TS_ENTERO)
			{
				PARSE_ERROR(E_OPERANDO_NO_VALIDO, $1.place);
			}
			else
			{
				PARSE_ERROR(E_OPERANDO_NO_VALIDO, $3.place);
			}
		}
	}


//Expresiones booleanas
exp_b:
	operando_b T_y M operando_b {  backpatch($$.true, $3); $$.false = merge($1.false, $4.false); $$.true = $4.true;  }
	| operando_b T_o M operando_b { backpatch($$.false, $3); $$.true = merge($1.true, $4.true); $$.false = $4.false;  } 
	| T_no operando_b { $$.true = $2.false; $$.false = $$.true; } 
	| T_literal_booleano {$$.true = makelist(nextquad()); $$.false = makelist(nextquad()+1); gen_salto_incondicional(-1); gen_salto_incondicional(-1); if(!$1) { lista aux = $$.false; $$.false = $$.true; $$.true = aux; }  }
	| operando_a T_oprel operando_a { $$.true = makelist(nextquad()); $$.false = makelist(nextquad()+1); gen_salto_condicional($2, $1.place, $3.place, -1); gen_salto_incondicional(-1);   }
	| T_inic_parentesis exp_b T_fin_parentesis { $$ = $2; }
	
	
operando_b:
	exp_b { $$ = $1; }
	| operando { 
		if($1.tipo == TS_BOOLEANO) 
		{ 
			$$.tipo = $1.tipo; 					
			$$.true = makelist(nextquad()); 
			$$.false = makelist(nextquad()+1);  
			gen_salto_condicional(TR_OP_EQUAL, $1.place, TS_cte_verdadero(), -1); 
			gen_salto_incondicional(-1); 
		} else 
		{ 
			/* error */ 
			PARSE_ERROR(E_OPERANDO_NO_VALIDO, $1.place);
		} 
	}
operando_a:
	exp_a { $$ = $1; }
	| operando { if(($1.tipo == TS_REAL) || ($1.tipo == TS_ENTERO)) { $$ = $1; } else { PARSE_ERROR(E_OPERANDO_NO_VALIDO, $1.place);  } }
operando:
	T_id  { 
		int id; 
		if((id=TS_buscar_simbolo($1)) == -1) 
		{ 
			/* error */ 
			PARSE_ERROR(E_SIMBOLO_NO_DEFINIDO, $1);
		} 
		else 
		{  
			/* también hay que comprobar si es una cte o una variable */
			int tipo=TS_consultar_tipo(id); 
			if(((tipo&0x00FF) != TS_VAR) && ((tipo&0x00FF) != TS_CTE))
			{
				/* error */
				PARSE_ERROR(E_OPERANDO_NO_VALIDO, id);
			}
			$$.place = id;
			$$.tipo = tipo&0xFF00; 
		}  
	}
	| operando T_referencia operando 
	| operando T_inic_array expresion T_fin_array 
	| operando T_ref
	| funcion_ll {
		/* guardar el tipo de valor de retorno en la traducción .tipo. Generar una variable temporal
		 * donde se almacenará dicho valor */
		}

// Rutinas semánticas vacías.
M:
	%empty {$$=nextquad();}
N:
	%empty {$$=nextquad();}
P:
	%empty { $$.next = makelist(nextquad()); gen_salto_incondicional(-1); }
	

/* Declaración para instrucciones */
instrucciones:
	instrucciones T_comp_secuencial M instruccion { 
		backpatch($1.next, $3);
		$$ = $4;
	}
	| instruccion { 
		$$ = $1; 
	}
	
	| instrucciones T_comp_secuencial error {
		$$ = $1;
		REPORT_PARSE_ERROR;
	}
	| error { 
		$$.next = makelistempty();
		REPORT_PARSE_ERROR;
	}

instruccion:
	asignacion {  $$ = $1; }
	| alternativa { $$ = $1; }
	| iteracion { $$ = $1; }
	//| T_continuar
	//|accion_ll

asignacion:
	operando T_asignacion expresion {
		$$.next = makelistempty();
		
		/* la parte izquierda de la asignación debe ser una variable */
		if((TS_consultar_tipo($1.place)&0x00FF) != TS_VAR)
		{
			PARSE_ERROR(E_OPERANDO_NO_VALIDO, $1.place);
		}
		
		/* los tipos deben coincidir, o el tipo expresión expresion sea convertible
		 * al tipo operando. */
		if($1.tipo == $3.tipo)
		{
			if($3.tipo == TS_BOOLEANO)
			{
				backpatch($3.true, nextquad());
				backpatch($3.false, nextquad()+1);
				gen_copia(TS_cte_verdadero(), $1.place);
				gen_copia(TS_cte_falso(), $1.place);
			}
			else
			{
				gen_copia($3.place, $1.place);
			}
		}
		else
		{
			/* puede que los tipos de ambos operandos sean compatibles, en tal
			 * caso hacemos una conversión de tipos implicita */
			if((($1.tipo == TS_ENTERO) && ($3.tipo == TS_REAL)) || (($1.tipo == TS_REAL) && ($3.tipo == TS_ENTERO)))
			{
				/* reales y enteros compatibles */
				int T;
				if($1.tipo == TS_ENTERO)
				{
					gen_asig_unaria(TR_OP_REAL_TO_INT, $3.place, $1.place);
				}
				else
				{
					gen_asig_unaria(TR_OP_INT_TO_REAL, $3.place, $1.place);
				}
			}
			else 
			{
				PARSE_ERROR(E_CONVERSION_NO_VALIDA);
			}
		}
	}
	| operando T_asignacion operando {
		$$.next = makelistempty();
			
		/* la parte izquierda de la asignación debe ser una variable */
		if((TS_consultar_tipo($1.place)&0x00FF) != TS_VAR)
		{
			PARSE_ERROR(E_MODIFICANDO_CTE,$1.place);
		}
			
		/* comprobar si tipos coinciden (Si son tipos estructurados, hay que 
		 * llevar a cabo comprobaciones adicionales) */
		if($1.tipo == $3.tipo)
		{
			gen_copia($3.place, $1.place);
		}
		else
		{
			if((($1.tipo == TS_ENTERO) && ($3.tipo == TS_REAL)) || (($1.tipo == TS_REAL) && ($3.tipo == TS_ENTERO)))
			{
				int T;
				if($1.tipo == TS_ENTERO)
				{
					gen_asig_unaria(TR_OP_REAL_TO_INT, $3.place, $1.place);
				}
				else
				{
					gen_asig_unaria(TR_OP_INT_TO_REAL, $3.place, $1.place);
				}
			}
			else
			{
				PARSE_ERROR(E_CONVERSION_NO_VALIDA);
			}
		}
	}
		
/* este no terminal representa una condición */
condicion:
	expresion {
		if($1.tipo == TS_BOOLEANO)
		{
			$$ = $1;
		}
		else if($1.tipo == TS_ENTERO)
		{
			$$.true = makelist(nextquad()); 
			$$.false = makelist(nextquad()+1);  
			gen_salto_condicional(TR_OP_NOT_EQUAL, $1.place, TS_cte_0(), -1); 
			gen_salto_incondicional(-1); 
		}
	}
	| operando {
		if($1.tipo == TS_BOOLEANO)
		{
			$$ = $1;
		}
		else if($1.tipo == TS_ENTERO)
		{				
			$$.true = makelist(nextquad()); 
			$$.false = makelist(nextquad()+1);  
			gen_salto_condicional(TR_OP_NOT_EQUAL, $1.place, TS_cte_0(), -1); 
			gen_salto_incondicional(-1); 
		}
		else
		{
			PARSE_ERROR(E_OPERANDO_NO_VALIDO, $1.place);
		}
	}
	
alternativa:
	T_si condicion T_entonces M instrucciones P N lista_opciones T_fsi { 
		backpatch($2.true, $4);
		backpatch($2.false, $7);
		$$.next = merge($5.next, merge($6.next, $8.next));
	}
	| T_si condicion T_entonces M instrucciones T_fsi {
		backpatch($2.true, $4);
		if(!empty($5.next))
			$$.next = merge($2.false, $5.next);
		else
		{
			$$.next = merge($2.false, makelist(nextquad()));
			gen_salto_incondicional(-1);
		} 
	}


lista_opciones:
	T_si_no_si condicion T_entonces M instrucciones P N lista_opciones {
		backpatch($2.true, $4);
		backpatch($2.false, $7);
		$$.next = merge($5.next, merge($6.next, $8.next));
	}
	| T_si_no_si condicion T_entonces M instrucciones {
		backpatch($2.true, $4);
		if(!empty($5.next))
			$$.next = merge($2.false, $5.next);
		else 
		{
			$$.next = merge($2.false, makelist(nextquad()));
			gen_salto_incondicional(-1);
		}
	}
iteracion:
	it_cota_fija { $$ = $1; }
	| it_cota_exp { $$ = $1; }

it_cota_exp:
	T_mientras M expresion T_hacer N instrucciones T_fmientras {
		backpatch($3.true, $5);
		if(!empty($6.next))
			backpatch($6.next, $2);
		else
			gen_salto_incondicional($2);
		$$.next = $3.false;
	}
	
	
expresion_f:
	expresion { 
		if($1.tipo == TS_ENTERO)
			$$ = $1;
		else
		{
			PARSE_ERROR(E_OPERANDO_NO_VALIDO, $1.place);
		}
	}
	| operando {
		if($1.tipo == TS_ENTERO)
			$$ = $1;
		else
		{
			PARSE_ERROR(E_OPERANDO_NO_VALIDO, $1.place);
		}
	}
	

it_cota_fija_c:
	T_para T_id T_asignacion expresion_f T_hasta expresion_f T_hacer { 
		/* inicializar variable de control & comparar variable de control con cota */
		
		/* comprobar variable de control */
		int v;
		if((v=TS_buscar_simbolo($2)) == -1)
		{
			PARSE_ERROR(E_SIMBOLO_NO_DEFINIDO, $2);
		}
		else if(TS_consultar_tipo(v) != (TS_ENTERO | TS_VAR))
		{
			PARSE_ERROR(E_OPERANDO_NO_VALIDO, v);
		}
		
		gen_copia($4.place, v);
		gen_salto_condicional(TR_OP_GREATER, v, $6.place, -1);
		$$ = v;
	}

it_cota_fija:
	M it_cota_fija_c instrucciones T_fpara {
		backpatch($3.next, nextquad());
		gen_asig_binaria(TR_OP_SUMA, $2, TS_cte_1(), $2);
		gen_salto_incondicional($1+1);
		$$.next = makelist($1+1);
	}
	

/* Declaraciones */
declaracion_tipo:
	T_tipo lista_de_tipo T_ftipo T_comp_secuencial
declaracion_cte:
	T_const lista_de_cte T_fconst T_comp_secuencial
declaracion_var:
	T_var lista_de_var T_fvar T_comp_secuencial

/* Declaraciones de tipos */	
lista_de_tipo:
	T_id T_creacion_tipo d_tipo T_comp_secuencial lista_de_tipo {
		
	}

d_tipo:
	T_id { 
		/* comprobar que existe este tipo en la TS */	
	
	}
	| dd_tipo {
		$$ = $1;
	}

dd_tipo:
	T_tupla lista_campos T_ftupla { 
		/* insertar nuevo tipo tupla en TS*/
	}
	| T_tabla T_inic_array expresion_t T_subrango expresion_t T_fin_array T_de d_tipo {
		/* insertar nuevo tipo tabla en TS */
	} 
	|  expresion_t T_subrango expresion_t {
		/* insertar nuevo tipo subrango en TS */
	}
	| T_ref d_tipo {
		/* insertar nuevo tipo ptro en TS */
	}
	| T_tipo_base {
		/* insertar nuevo tipo base en TS */

		$$ = $1;
	}	

expresion_t:
	expresion 
	| T_literal_caracter
lista_campos:
	T_id T_def_tipo_variable d_tipo T_comp_secuencial lista_campos
	| %empty


/* Declaración de constantes y variables */
lista_de_cte:
	T_id T_creacion_tipo constante T_comp_secuencial lista_de_cte { int id=TS_insertar_simbolo($1); TS_modificar_simbolo(id, TS_CTE | $3.tipo); TS_modificar_cte(id,$3.val); }
	| %empty

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
	| %empty
lista_id:
	T_id T_separador lista_id { apilar($3, TS_insertar_simbolo($1)); $$ = $3; }
	| T_id { $$ = crear_pila(); apilar($$, TS_insertar_simbolo($1)); }

decl_ent_sal: 
	decl_ent
	| decl_ent decl_sal 
	| decl_sal 
	| %empty
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
	| %empty
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
	| %empty

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
		
	/* Rutina principal */
	int main(int argc,char ** argv)
	{
		TS_inicializar();
		printf("%d\n", yyparse());
		TS_liberar();
	}
	
	int yyerror(const char* s)
	{
		printf("error de parseo(linea %d): %s\n", line_number, s);
	}

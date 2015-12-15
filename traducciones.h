/**
	\file
	@brief Cabecera que define las estructuras de datos que luego se convertirán en instrucciones en lenguaje
	de código intermedio. Se definen una serie de rutinas para generar estas estructuras de forma sencilla.
	@author Víctor Ruiz Gómez, Silvia Popa
*/

#ifndef __TRADUCCIONES_H
#	define __TRADUCCIONES_H


/* Define todos los posibles valores para los operadores en las cuadruplas */
typedef enum TR_operador 
{
	TR_OP_ASIG = 0x0001,  	   /* para copias */
	TR_OP_GOTO, 		   /* para saltos incondicionales */
	TR_OP_PARAM,		   /* definición de parametros para procedimientos */
	TR_OP_CALL,			   /* llamadas a procedimientos */
	TR_OP_RETURN,		   /* retornos de procedimientos */
	TR_OP_REF,			   /* para asignar un puntero a una variable a otra (A = ref B) */
	TR_OP_DEREF,		   /* para asignar el valor apuntado por un puntero a otra, derreferencia (A = ^ B) */
	TR_OP_ASIG_DEREF,	   /* para asignar al valor de un puntero, una variabe dada (^ A = B) */
	
	/* matrices */
	TR_OP_MATRIX, 			/* para asignaciones matriciales de tipo x = y[i] */
	TR_OP_MATRIX_ASIG,     /* para asignaciones matriciales del tipo x[i] = y */ 
		
	
	/* aritmética */
	TR_OP_SUMA = 0x0010 | 1<<15,			  
	TR_OP_RESTA,
	TR_OP_MULT,
	TR_OP_DIV,
	TR_OP_SHIFT_LEFT,	   /* operacion para move bits hacia la izquierda. Es equivalente a multiplicar por potencias 2 */
	TR_OP_SHIFT_RIGHT,	   /* igual que la anterior, pero a la derecha. Igual que dividir por potencias de 2 */

	/* aritmética en coma flotante */
	TR_OP_SUMA_REAL = 0x0020 | 1<<15,
	TR_OP_RESTA_REAL,
	TR_OP_MULT_REAL,
	TR_OP_DIV_REAL,
	
	/* operadores lógicos */
	TR_OP_AND = 0x0030 | 1<<15,
	TR_OP_OR = 0x0031 | 1<<15,
	TR_OP_NOT = 0x0032,
	
	/* conversiones */
	TR_OP_INT_TO_REAL = 0x0040,		/* conversión entero -> real */
	TR_OP_REAL_TO_INT = 0x0041,		/* conversión real -> entero */
	
	/* operadores relacionales */
	TR_OP_EQUAL = 0x0100 | 1<<15,
	TR_OP_NOT_EQUAL = 0x0200,
	TR_OP_LOWER = 0x0300 | 1<<15,
	TR_OP_GREATER = 0x0400 | 1<<15,
	TR_OP_GREATER_EQUAL = 0x0500 | 1<<15,	/* mayor o igual */
	TR_OP_LOWER_EQUAL = 0x0600 | 1<<15,		/* menor o igual */
	
	/* se pueden combinar los bits de los operadores relaciones con el id para
	 * el operador de salto incodicional goto, para formar un nuevo operador de salto
	 * condicional.
	 * TR_OP_EQUAL | TR_OP_GOTO = Saltar si los operandos son iguales
	 * TR_OP_LOWER_EQUAL | TR_OP_GOTO = Saltar si el primer operando es menor o igual que el segundo.
	 */
	TR_OP_GOTO_EQUAL = TR_OP_GOTO | TR_OP_EQUAL,
	TR_OP_GOTO_NOT_EQUAL = TR_OP_GOTO | TR_OP_NOT_EQUAL,
	TR_OP_GOTO_LOWER = TR_OP_GOTO | TR_OP_LOWER,
	TR_OP_GOTO_GREATER = TR_OP_GOTO | TR_OP_GREATER,
	TR_OP_GOTO_GREATER_EQUAL = TR_OP_GOTO | TR_OP_GREATER_EQUAL,
	TR_OP_GOTO_LOWER_EQUAL = TR_OP_GOTO | TR_OP_LOWER_EQUAL
	
} TR_operador;


/* Esta estructura es la cuadrupla. Posteriormente puede
 * convertirse a una instrucción de código intermedio */
typedef struct TR_cuadrupla
{
	int op; /* es el operador */
	int ops[2]; /* son los operandos. se utiliza uno para operaciones unarias y
		ambos para operaciones binarias */
	int res; /* es el lugar donde se guardará el resultado (si lo tiene) */
} TR_cuadrupla;


/** Guarda una cuadrupla en el fichero de cuadruplas. El fichero de cuadruplas las guarda
 * en notación postfija (se guarda al final del fichero)
 */
void gen(TR_cuadrupla q);

/* Los siguientes métodos crean cuadruplas correspondientes a cada uno de los tipos de instrucciones,
 * y llaman a gen, para guardarla en el fichero de cuadruplas */


void gen_asig_binaria(int op_binario, int op1, int op2, int res); /* A = B op C */

void gen_asig_unaria(int op_unario, int op, int res); /* A = op B */

void gen_copia(int src, int dest); /* A:=B */

void gen_salto_incondicional(int direccion); /* goto A */

void gen_salto_condicional(int op_rel, int op1, int op2, int direccion); /* if A op B then goto C */

void gen_parametro(int param); /* param A */

void gen_call(int f, int n); /* call F,n */

void gen_return(int y); /* return y */

void gen_acceso_matricial(int b, int offset, int a); /* A = B[i] */ 

void gen_asig_matricial(int b, int offset, int a);  /* A[i] := B */


/* operaciones con punteros */
void gen_ptr_ref(int y, int x); /* x = ref y */

void gen_ptr_deref(int y, int x); /* x = ^y */

void gen_ptr_asig_deref(int y, int x); /* ^x = y */


#endif

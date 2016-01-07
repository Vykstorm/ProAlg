
#include "traducciones.h"
#include "tabla_simbolos.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "util/lista.h"
#include "util/cola.h"

/////// Métodos y variables auxiliares 
int cont=0;
lista cuadruplas = NULL;
void print_quad(TR_cuadrupla q);


/////// Implementación de la interfaz
void gen(TR_cuadrupla q)
{
	cont++;
	/* guardar la cuadrupla en el fichero de cuadruplas */
	if(cuadruplas == NULL)
		cuadruplas = crear_lista(sizeof(TR_cuadrupla));
	lista_insertar_cola(cuadruplas, (const void*)&q);
	
	/* para depurar la generación de código */
#ifdef TR_DEBUG
	printf("gen(%d):  ", nextquad()-1);
	print_quad(q);
#endif
}

void completar_salto(int salto, int quad)
{
	TR_cuadrupla* q = ((TR_cuadrupla*)lista_consultar(cuadruplas, salto)); 
	assert(q != NULL); 
	q->res = quad;
	
	/* imprimir información de depuración */
#ifdef TR_DEBUG
	printf("salto completado(%d):  ",salto);
	print_quad(*q);
#endif
}

void backpatch(lista L, int quad)
{
	assert(L != NULL);
	void insertar(const void* el) { completar_salto((*(int*)el), quad); }
	lista_recorrer(L, insertar);
}

lista makelist(int quad)
{
	cola A = crear_cola();
	pedir_turno(A, quad);
	return A;
}

lista makelistempty()
{
	return crear_cola();
}

int empty(lista L)
{
	return cola_vacia(L);
}


lista merge(lista L1,lista L2)
{
	cola B = crear_cola();
	void insertar(const void* el) { int val = (*(int*)el);pedir_turno(B, val); }
	lista_recorrer(L1, insertar);
	lista_recorrer(L2, insertar);
	
	return B;
}

//Devuelve la siguiente cuadrupla
int nextquad()
{
	return (cuadruplas!=NULL) ? lista_len(cuadruplas) : 0;
}


void print_quads()
{
	void _print_quad(const void* quad)
	{
		print_quad(*(TR_cuadrupla*)quad);
	}
	printf("\n\n______________ Instrucciones generadas _____________\n");
	if(cuadruplas != NULL) /* no imprimir nada si no hay ninguna cuadrupla */
	{
		printf("num instrucciones: %d\n", lista_len(cuadruplas));
		lista_recorrer(cuadruplas, _print_quad);
	}
}

void gen_asig_binaria(int op_binario, int op1, int op2, int res)
{
	assert((op_binario & 0x8000) != 0); /* es op binario ? */
	/* creamos la cuadrupla */
	TR_cuadrupla q;
	q.op = op_binario;
	q.ops[0] = op1; q.ops[1] = op2;
	q.res = res;
	gen(q);
}

void gen_asig_unaria(int op_unario, int op, int res)
{
	assert((op_unario & 0x8000) == 0); /* es op unario ? */
	
	TR_cuadrupla q;
	q.op = op_unario;
	q.ops[0] = op;
	q.res = res;
	gen(q);
}

void gen_copia(int src, int dest)
{
	TR_cuadrupla q;
	q.op = TR_OP_ASIG;
	q.ops[0] = src;
	q.res = dest;
	gen(q);
}

void gen_salto_incondicional(int direccion)
{
	TR_cuadrupla q;
	q.op = TR_OP_GOTO;
	q.res = direccion;
	gen(q);
}

void gen_salto_condicional(int op_rel, int op1, int op2, int direccion)
{
	assert((op_rel & 0x8000) != 0); /*comprobación: op es binario (op relacionales son binarios) */
	TR_cuadrupla q;
	q.op = TR_OP_GOTO | op_rel;
	q.ops[0] = op1;
	q.ops[1] = op2;
	q.res = direccion;
	gen(q);
}

void gen_salto_condicional2(int op, int direccion)
{
	gen_salto_condicional(TR_OP_EQUAL, op, TS_cte_verdadero(), direccion);
}

void gen_parametro(int param)
{
	TR_cuadrupla q;
	q.op = TR_OP_PARAM;
	q.ops[0] = param;
	gen(q);
}

void gen_call(int f, int n)
{
	assert(n >= 0);
	TR_cuadrupla q;
	q.op = TR_OP_CALL;
	q.ops[0] = f;
	q.ops[1] = n;
	gen(q);
}

void gen_return(int y)
{
	TR_cuadrupla q;
	q.op = TR_OP_RETURN;
	q.ops[0] = y;
	gen(q);
}

void gen_acceso_matricial(int b, int offset, int a)
{
	assert(offset >= 0);
	TR_cuadrupla q;
	q.op = TR_OP_MATRIX;
	q.ops[0] = b;
	q.ops[1] = offset;
	q.res = a;
	gen(q);
}

void gen_asig_matricial(int b, int offset, int a)
{
	assert(offset >= 0);
	TR_cuadrupla q;
	q.op = TR_OP_MATRIX_ASIG;
	q.ops[0] = b;
	q.ops[1] = offset;
	q.res = a; 
	gen(q);
}

void gen_ptr_ref(int y, int x)
{
	gen_asig_unaria(TR_OP_REF, y, x);
}

void gen_ptr_deref(int y, int x)
{
	gen_asig_unaria(TR_OP_DEREF, y, x);
}	

void gen_ptr_asig_deref(int y, int x)
{
	gen_asig_unaria(TR_OP_ASIG_DEREF, y, x);
}



///// Implementación de métodos auxiliares 
char huecos[3][16];
int hueco_actual = 0;
const char* print_var(int id)
{
	char* hueco = huecos[hueco_actual];
	/* si es una variable, imprimimos su id. Si es una cte, imprimimos su valor */
	int tipo;
	if(((tipo = TS_consultar_tipo(id))&TS_CTE) == TS_CTE)
	{
		TS_cte_val val = TS_consultar_cte(id);
		switch(tipo & 0xFF00)
		{
			case TS_BOOLEANO:
				sprintf(hueco, "%d", val.booleano);
			break;
			case TS_REAL:
				sprintf(hueco, "%.3f", val.real);
			break;
			case TS_ENTERO:
				sprintf(hueco, "%d", val.entero);
			break;
			case TS_CARACTER:
				sprintf(hueco, "%c", val.caracter);
			break;
			case TS_STRING:
			{
				char aux[13];
				strncpy(aux, val.string, 10);
				if(strlen(val.string) > 10)
					strcat(aux, "...");
				sprintf(hueco, "\"%s\"", aux);
			}
			break;
		}
	}
	else
	{
		sprintf(hueco, "v%d", id);
	}

	if(++hueco_actual == 3)
		hueco_actual=0;
	return hueco;
}

char* print_jump(int quad)
{
	char* hueco = huecos[hueco_actual];
	
	if(quad == -1)
		strcpy(hueco, "?");
	else
		sprintf(hueco, "%d", quad);
	
	if(++hueco_actual == 3)
		hueco_actual=0;
	return hueco;
}


void print_quad(TR_cuadrupla q)
{
	switch(q.op)
	{
		case TR_OP_ASIG:
			printf("%s := %s", print_var(q.res), print_var(q.ops[0]));
		break;
		case TR_OP_GOTO:
			printf("goto %s", print_jump(q.res));
		break;
		case TR_OP_PARAM:
			printf("param %s", print_var(q.ops[0]));
		break;
		case TR_OP_CALL:
			printf("call %s, %s", print_var(q.ops[0]), print_var(q.ops[1]));
		break;
		case TR_OP_RETURN:
			printf("return %s", print_var(q.ops[0]));
		break;
		case TR_OP_REF:
			printf("%s := ref %s", print_var(q.res), print_var(q.ops[0]));
		break;
		case TR_OP_DEREF:
			printf("%s := ^ %s", print_var(q.res), print_var(q.ops[0]));
		break;
		case TR_OP_ASIG_DEREF:
			printf("^ %s := %s", print_var(q.res), print_var(q.ops[0]));
		break;
		case TR_OP_MATRIX:
			printf("%s := %s[%s]", print_var(q.res), print_var(q.ops[0]), print_var(q.ops[1]));
		break;
		case TR_OP_MATRIX_ASIG:
			printf("%s[%s] := %s", print_var(q.res), print_var(q.ops[1]), print_var(q.ops[0]));
		break;
		case TR_OP_SUMA:
			printf("%s := %s + %s (entera)", print_var(q.res), print_var(q.ops[0]), print_var(q.ops[1]));
		break;
		case TR_OP_RESTA:
			printf("%s := %s - %s (entera)", print_var(q.res), print_var(q.ops[0]), print_var(q.ops[1]));
		break;
		case TR_OP_MULT: 
			printf("%s := %s * %s (entera)", print_var(q.res), print_var(q.ops[0]), print_var(q.ops[1]));
		break;
		case TR_OP_DIV:
			printf("%s := %s div %s (entera)", print_var(q.res), print_var(q.ops[0]), print_var(q.ops[1]));
		break;
		case TR_OP_MOD:
			printf("%s := %s %% %s", print_var(q.res), print_var(q.ops[0]), print_var(q.ops[1]));
		break;
		case TR_OP_SHIFT_LEFT:
			printf("%s := %s << %s", print_var(q.res), print_var(q.ops[0]), print_var(q.ops[1]));
		break;
		case TR_OP_SHIFT_RIGHT:
			printf("%s := %s >> %s", print_var(q.res), print_var(q.ops[0]), print_var(q.ops[1]));
		break;
		case TR_OP_NEG:
			printf("%s := - %s (entera)", print_var(q.res), print_var(q.ops[0]));
		break;
		case TR_OP_SUMA_REAL:
			printf("%s := %s + %s (entera)", print_var(q.res), print_var(q.ops[0]), print_var(q.ops[1]));
		break;
		case TR_OP_RESTA_REAL:
			printf("%s := %s - %s (real)", print_var(q.res), print_var(q.ops[0]), print_var(q.ops[1]));
		break;
		case TR_OP_MULT_REAL: 
			printf("%s := %s * %s (real)", print_var(q.res), print_var(q.ops[0]), print_var(q.ops[1]));
		break;
		case TR_OP_DIV_REAL:
			printf("%s := %s div %s (real)", print_var(q.res), print_var(q.ops[0]), print_var(q.ops[1]));
		break;
		case TR_OP_NEG_REAL:
			printf("%s := - %s (real)", print_var(q.res), print_var(q.ops[0]));
		break;
		
		case TR_OP_NOT:
			printf("%s := not %s", print_var(q.res), print_var(q.ops[0]));
		break;
		case TR_OP_AND:
			printf("%s := %s and %s", print_var(q.res), print_var(q.ops[0]), print_var(q.ops[1]));
		break;
		case TR_OP_OR:
			printf("%s := %s or %s", print_var(q.res), print_var(q.ops[0]), print_var(q.ops[1]));		
		break;
		
		case TR_OP_INT_TO_REAL:
			printf("%s := toreal %s", print_var(q.res), print_var(q.ops[0]));
		break;
		case TR_OP_REAL_TO_INT:
			printf("%s := toint %s",  print_var(q.res), print_var(q.ops[0]));
		break;
		
		
		case TR_OP_EQUAL:
			printf("%s := %s = %s", print_var(q.res), print_var(q.ops[0]), print_var(q.ops[1])); 
		break;
		case TR_OP_NOT_EQUAL:
			printf("%s := %s <> %s", print_var(q.res), print_var(q.ops[0]), print_var(q.ops[1]));
		break;
		case TR_OP_LOWER:
			printf("%s := %s < %s", print_var(q.res), print_var(q.ops[0]), print_var(q.ops[1]));
		break;
		case TR_OP_GREATER:
			printf("%s := %s > %s", print_var(q.res), print_var(q.ops[0]), print_var(q.ops[1]));
		break;
		case TR_OP_GREATER_EQUAL:
			printf("%s := %s >= %s", print_var(q.res), print_var(q.ops[0]), print_var(q.ops[1]));
		break;
		case TR_OP_LOWER_EQUAL:
			printf("%s := %s <= %s", print_var(q.res), print_var(q.ops[0]), print_var(q.ops[1]));
		break;
		
		default:
			if((TR_OP_GOTO & q.op) == TR_OP_GOTO)
			{
				switch(q.op & 0xFF00)
				{
					case TR_OP_EQUAL:
						printf("if %s = %s goto %s", print_var(q.ops[0]), print_var(q.ops[1]), print_jump(q.res));
					break;
					case TR_OP_NOT_EQUAL:
						printf("if %s <> %s goto %s", print_var(q.ops[0]), print_var(q.ops[1]), print_jump(q.res));
					break;
					case TR_OP_LOWER:
						printf("if %s < %s goto %s", print_var(q.ops[0]), print_var(q.ops[1]), print_jump(q.res));
					break;
					case TR_OP_GREATER:
						printf("if %s > %s goto %s", print_var(q.ops[0]), print_var(q.ops[1]), print_jump(q.res));
					break;
					case TR_OP_GREATER_EQUAL:
						printf("if %s >= %s goto %s", print_var(q.ops[0]), print_var(q.ops[1]),print_jump(q.res));
					break;
					case TR_OP_LOWER_EQUAL:
						printf("if %s <= %s goto %s", print_var(q.ops[0]), print_var(q.ops[1]), print_jump(q.res));
					break;				
				}
			}
	}
	
	putchar('\n');
}

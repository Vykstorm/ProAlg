
#include "traducciones.h"
#include "tabla_simbolos.h"
#include <stdio.h>
#include <assert.h>
#include "util/lista.h"
#include "util/cola.h"


int cont=0;
lista cuadruplas = NULL;
void gen(TR_cuadrupla q)
{
	printf("cuadrupla: %d %d %d %d\n", q.op, q.ops[0], q.ops[1], q.res);
	cont++;
	/* guardar la cuadrupla en el fichero de cuadruplas */
	if(cuadruplas == NULL)
		cuadruplas = crear_lista(sizeof(TR_cuadrupla));
	lista_insertar_cola(cuadruplas, (const void*)&q);
}


void backpatch(lista L, int quad)
{
	assert(L != NULL);
	void insertar(const void* el) { int val = (*(int*)el); TR_cuadrupla* q = ((TR_cuadrupla*)lista_consultar(cuadruplas, val)); assert(q != NULL); q->res=quad;}
	lista_recorrer(L, insertar);
}

lista makelist(int quad)
{
	cola A = crear_cola();
	pedir_turno(A, quad);
	return A;
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
	assert(cuadruplas != NULL);
	return lista_len(cuadruplas);
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

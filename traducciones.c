
#include "traducciones.h"
#include <stdio.h>
#include <assert.h>

void gen(TR_cuadrupla q)
{
	printf("cuadrupla: %d %d %d %d\n", q.op, q.ops[0], q.ops[1], q.res);
	
	/* guardar la cuadrupla en el fichero de cuadruplas */
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


int main(int argc, char** argv)
{
	
}

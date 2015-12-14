#include "pila.h"
#include <stdio.h>
#include <assert.h>

pila crear_pila()
{
	return crear_lista(sizeof(int));
}

void liberar_pila(pila p)
{
	assert(p != NULL);
	liberar_lista(p);
}

void apilar(pila p, int el)
{
	assert(p != NULL);
	lista_insertar_cabeza(p, (void*)&el);
}

int desapilar(pila p)
{
	assert((p != NULL) && !pila_vacia(p));
	int el = cima(p);
	lista_eliminar_cabeza(p);
	return el;
}

int cima(pila p)
{
	assert((p != NULL) && !pila_vacia(p));
	return *((int*)lista_cabeza(p));
}

int pila_vacia(pila p)
{
	assert(p != NULL);
	return lista_vacia(p);
}


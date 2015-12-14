
#include "cola.h"
#include <assert.h>
#include <stdlib.h>

cola crear_cola()
{
	return crear_lista(sizeof(int));
}

void liberar_cola(cola c)
{
	assert(c != NULL);
	liberar_lista(c);
}

void pedir_turno(cola c, int el)
{
	assert(c != NULL);
	lista_insertar_cola(c, (void*)&el);
}

int avanzar(cola c)
{
	assert((c != NULL) && !cola_vacia(c));
	int p = primero(c);
	lista_eliminar_cabeza(c);
	return p;
}

int primero(cola c)
{
	assert((c != NULL) && !cola_vacia(c));
	return *((int*)lista_cabeza(c));
}

int cola_vacia(cola c)
{
	assert(c != NULL);
	return lista_vacia(c);
}

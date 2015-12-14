/**
	\file
	@brief Esta cabecera define la estructura de datos "cola" para enteros.
	@author Víctor Ruiz Gómez, Silvia Popa
*/


#ifndef __COLA_H
#	define __COLA_H

#	include "lista.h"

///// Estructuras de datos
typedef lista cola;

///// Interfaz de cola
/** Crea una cola vacía */
cola crear_cola();
/** Elimina una cola */
void liberar_cola(cola c);
/** Inserta un nuevo elemento al final de la cola */
void pedir_turno(cola c, int el);
/** Elimina el primer elemento de la cola. Devuelve el elemento eliminado */
int avanzar(cola c);
/** Devuelve el primer elemento de la cola */
int primero(cola c);
/** Comprueba si la cola está vacía */
int cola_vacia(cola c);

#endif 

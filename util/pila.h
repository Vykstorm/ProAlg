/**
	\file
	@brief Esta cabecera define la estructura de datos "pila" para enteros.
	@author Víctor Ruiz Gómez, Silvia Popa
*/

#ifndef __PILA_H
#	define __PILA_H
#	include "lista.h"

//// Estructuras de datos.
typedef lista pila;

//// Interfaz de pila.
/** Crea una pila vacía */
pila crear_pila();
/** Elimina una pila */
void liberar_pila(pila p);
/** Apila un elemento en una pila */
void apilar(pila p, int el);
/** Desapila un elemento de una pila. Devuelve el elemento desapilado */
int desapilar(pila p);
/** Devuelve la cima de la pila */
int cima(pila p);
/** Comprueba si la pila está vacía */
int pila_vacia(pila p);




#endif

/**
	\file
	@brief Esta cabecera define la estructura de datos "lista".
	Esta lista permite iterar por los elementos de la misma, del primero al último,
	obtener el primer elemento y el último con coste O(1), o cualquier elemento de la secuencia
	con coste O(N), buscar elementos e insertar y eliminar elementos en cualquier posición.
	@author Víctor Ruiz Gómez, Silvia Popa
*/

#ifndef __LISTA_H
#   define __LISTA_H


////////// Estructuras de datos para almacenar una lista

/* Define el nodo de una lista */
typedef struct lista_nodo
{
    struct lista_nodo* siguiente;
    // lista_elemento el;
} lista_nodo;

/* Define la lista */
struct lista
{
    lista_nodo* primero, *ultimo;
    int bpel;
};
typedef struct lista* lista;




///////////// Interfaz lista


/** Crea una lista vacía y la devuelve */
lista crear_lista(int bpel);

/** Elimina la lista y la memoria usada por esta */
void liberar_lista(lista l);

/** Devuelve el valor del primer elemento de la lista */
void* lista_cabeza(const lista l);

/** Devuelve el valor del último elemento de la lista */
void* lista_cola(const lista l);

/** Devuelve el elemento en la posición i-esima. */
void* lista_consultar(const lista l, int i);

/** Devuelve el tamaño de la lista */
int lista_len(const lista l);

/** Comprueba si una lista está vacía */
int lista_vacia(const lista l);

/** Busca un elemento que satisfaga una condición en la lista.
Devuelve la posición del primer elemento que satisfaga la condición, o -1 si no
se ha encontrado ninguno */
int lista_buscar(const lista l, int(const void*));


/** Inserta un elemento en la cabeza de la lista */
void lista_insertar_cabeza(lista l, const void* el);

/** Inserta un elemento en la cola de la lista */
void lista_insertar_cola(lista l, const void* el);

/** Inserta un elemento en una posición de la lista */
void lista_insertar(lista l, int pos, const void* el);

/** Elimina la cabeza de la lista */
void lista_eliminar_cabeza(lista l);

/** Elimina la cola de la lista */
void lista_eliminar_cola(lista l);

/** Elimina un elemento en una posición de la lista */
void lista_eliminar(lista l, int pos);


/** LLeva a cabo una acción para cada elemento de la lista */
void lista_recorrer(const lista l, void(void*));

/** Muestra la lista por pantalla */
void lista_mostrar(const lista l, void(const void*,char*), char separador);

#endif


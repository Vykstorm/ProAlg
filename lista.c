
#include "lista.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

////////// Implementación de lista


lista* crear_lista()
{
    lista* l = (lista*)malloc(sizeof(lista));
    l->primero = NULL;
    return l;
}

lista_nodo* crear_nodo()
{
    lista_nodo* n = (lista_nodo*)malloc(sizeof(lista_nodo));
    return n;
}

void liberar_nodo(lista_nodo* n)
{
    free((void*)n);
}

void liberar_lista(lista* l)
{
    assert(l != NULL);
    if(!lista_vacia(l))
    {
        lista_nodo *curr, *next;
        curr = l->primero;
        while(curr->siguiente != NULL)
        {
            next = curr->siguiente;
            liberar_nodo(curr);
            curr = next;
        }
        liberar_nodo(curr);
    }
    free((void*)l);
}

void* lista_cabeza(const lista* l)
{
    assert((l != NULL) && !lista_vacia(l));
    return l->primero->el.value;
}

void* lista_cola(const lista* l)
{
    assert((l != NULL) && !lista_vacia(l));
    return l->ultimo->el.value;
}

int lista_vacia(const lista* l)
{
    assert(l != NULL);
    return l->primero == NULL;
}

void* lista_consultar(const lista* l, int i)
{
    assert((l != NULL) && (i >= 0) && !lista_vacia(l));

    int j = 0;
    const lista_nodo* curr = l->primero;
    while((j < i) && (curr->siguiente != NULL))
    {
        curr = curr->siguiente;
        j++;
    }
    assert(i == j);
    return curr->el.value;
}

void lista_modificar(lista* l, int i, void* el)
{
    assert((l != NULL) && (i >= 0) && !lista_vacia(l));
    int j = 0;
    lista_nodo* curr = l->primero;
    while((j < i) && (curr->siguiente != NULL))
    {
        curr = curr->siguiente;
        j++;
    }
    assert(i == j);
    curr->el.value = el;
}

int lista_len(const lista* l)
{
    assert(l != NULL);
    int len = 0;
    lista_nodo* curr = l->primero;
    while(curr != NULL)
    {
        curr = curr->siguiente;
        len++;
    }
    return len;
}

int lista_buscar_val(const lista* l, const void* val)
{
    assert(l != NULL);

    if(lista_vacia(l))
        return -1;
    int j = 0;
    const lista_nodo* curr = l->primero;
    while((curr->siguiente != NULL) && (val != curr->el.value))
    {
        curr = curr->siguiente;
        j++;
    }
    return (val == curr->el.value) ? j : -1;
}

int lista_buscar(const lista* l, int cond(const void*))
{
    assert(l != NULL);

    if(lista_vacia(l))
        return -1;
    int j = 0;
    const lista_nodo* curr = l->primero;
    while((curr->siguiente != NULL) && !cond(curr->el.value))
    {
        curr = curr->siguiente;
        j++;
    }
    return cond(curr->el.value) ? j : -1;
}

void lista_insertar_cabeza(lista* l, void* el)
{
    assert(l != NULL);

    lista_nodo* nuevo = crear_nodo();
    nuevo->el.value = el;
    nuevo->siguiente = l->primero;

    l->primero = nuevo;
    if(l->primero->siguiente == NULL)
        l->ultimo = l->primero;
}

void lista_insertar_cola(lista* l, void* el)
{
    assert(l != NULL);

    lista_nodo* nuevo = crear_nodo();
    nuevo->el.value = el;
    nuevo->siguiente = NULL;
    if(l->primero != NULL)
    {
        l->ultimo->siguiente = nuevo;
        l->ultimo = nuevo;
    }
    else
    {
        l->primero = l->ultimo = nuevo;
    }
}

void lista_insertar(lista* l, int pos, void* el)
{
    assert((l != NULL) && (pos >= 0) && !lista_vacia(l));
    if(pos == 0)
        lista_insertar_cabeza(l, el);
    else
    {
        lista_nodo* curr = l->primero;
        int i = 0;
        while((i < (pos-1)) && (curr->siguiente != NULL))
        {
            curr = curr->siguiente;
            i++;
        }
        assert(curr->siguiente != NULL);
        lista_nodo* nuevo = crear_nodo();
        nuevo->siguiente = curr->siguiente;
        nuevo->el.value = el;

        curr->siguiente = nuevo;
    }
}

void lista_eliminar_cabeza(lista* l)
{
    assert((l != NULL) && !lista_vacia(l));

    lista_nodo* primero = l->primero;
    l->primero = l->primero->siguiente;
    liberar_nodo(primero);
}

void lista_eliminar_cola(lista* l)
{
    assert((l != NULL) && !lista_vacia(l));
    if(l->primero->siguiente == NULL)
    {
        lista_nodo* primero = l->primero;
        l->primero = NULL;
        liberar_nodo(primero);
    }
    else
    {
        lista_nodo* curr = l->primero;
        while(curr->siguiente->siguiente != NULL)
        {
            curr = curr->siguiente;
        }
        lista_nodo* aux = curr->siguiente;
        curr->siguiente = NULL;
        l->ultimo = curr;
        liberar_nodo(aux);
    }
}

void lista_eliminar(lista* l, int pos)
{
    assert((l != NULL) && (pos >= 0) && !lista_vacia(l));
    if(pos == 0)
        lista_eliminar_cabeza(l);
    else
    {
        lista_nodo* curr = l->primero;
        int i = 0;
        while((i < (pos-1)) && (curr->siguiente != NULL))
        {
            curr = curr->siguiente;
            i++;
        }
        assert(curr->siguiente != NULL);
        lista_nodo* aux = curr->siguiente;
        curr->siguiente = curr->siguiente->siguiente;
        liberar_nodo(aux);
        if(curr->siguiente == NULL)
            l->ultimo = curr;
    }
}


void lista_recorrer(const lista* l, void accion(void*))
{
    assert(l != NULL);

    lista_nodo* curr = l->primero;
    while(curr != NULL)
    {
        accion(curr->el.value);
        curr = curr->siguiente;
    }
}

void lista_mostrar(const lista* l, void to_string(const void*,char*), char separador)
{
    assert(l != NULL);

    if(!lista_vacia(l))
    {
        lista_nodo* curr = l->primero;
        char el[256];
        while(curr->siguiente != NULL)
        {
            memset(el, 0, sizeof(el));
            to_string(curr->el.value, el);
            printf("%s%c", el, separador);
            curr = curr->siguiente;
        }
        memset(el, 0, sizeof(el));
        to_string(curr->el.value, el);
        printf("%s", el);
    }
}

void mostrar_valor(void* valor, char* buffer)
{
    sprintf(buffer, "%i", *((int*)valor));
}

void liberar_elementos(void* valor)
{
    free(valor);
}

int contador = 0;
void* crear_entero()
{
    void* el = malloc(sizeof(int));
    *((int*)el) = contador++;
    return el;
}

void modificar_entero(void* el, int val)
{
    *((int*)el) = val;
}

int ver_entero(const void* el)
{
    return *((int*)el);
}


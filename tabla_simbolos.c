/**
	\file
	@brief Fichero de código que implementa las funciones de la interfaz de la tabla de símbolos
	@author Víctor Ruiz Gómez, Silvia Popa
*/
#include "tabla_simbolos.h"
#include "lista.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

// Macro que se usará para poder lanzar asercciones en la tabla de símbolos
#define TS_assert(condicion) assert(condicion)

// Macro que facilita la reserva de memoria dinámica
#define TS_malloc(tipo,n) ((tipo*)malloc(sizeof(tipo)*n))
#define TS_malloc1(tipo) TS_malloc(tipo,1)
#define TS_free(ptr) free((void*)ptr)


///// Estructuras de datos adicionales
/* Aqui se almacenan todos los símbolos que se declaran en un mismo ámbito (Una lista de símbolos) */
typedef struct lista* TS_subtabla_simbolos;

/* Es la estructura principal. Almacena subtablas (lista de subtablas de símbolos) */
typedef struct lista* TS_tabla_simbolos;

///// Variables y métodos auxiliares
int TS_contador_id = 0;
TS_tabla_simbolos tabla_simbolos;

/* métodos de creación y eliminación de la tabla de símbolos y las subtablas */
TS_tabla_simbolos    TS_crear_tabla();
TS_subtabla_simbolos TS_crear_subtabla();

void            TS_eliminar_subtabla(TS_subtabla_simbolos subtabla);
void            TS_eliminar_tabla();

/* métodos de busquedas y consultas en la tabla */
TS_simbolo* TS_buscar_simbolo(const char* nombre);

/* métodos de creación y eliminación de estructuras de datos auxiliares  */
TS_simbolo*     TS_crear_simbolo(const char* nombre);
TS_var*         TS_crear_var(int tipo);
TS_tipo*        TS_crear_tipo(int tipo);
TS_cte*         TS_crear_cte(int tipo);
TS_accion*      TS_crear_accion();
TS_funcion*     TS_crear_funcion();

void            TS_eliminar_simbolo(TS_simbolo* simbolo);
void            TS_eliminar_registro_simbolo(TS_simbolo* simbolo);
void            TS_eliminar_var(TS_var* var);
void            TS_eliminar_tipo(TS_tipo* tipo);
void            TS_eliminar_cte(TS_cte* cte);
void            TS_eliminar_accion(TS_accion* accion);
void            TS_eliminar_funcion(TS_funcion* funcion);



///// Implementación de los métodos de la interfaz de la tabla de símbolos
void TS_inicializar()
{
    tabla_simbolos = TS_crear_tabla();
    TS_set();
}

void TS_set()
{
    /* añadir una nueva subtabla */
    lista_insertar_cabeza(tabla_simbolos, (void*)TS_crear_subtabla());
}

void TS_reset()
{
    TS_assert(lista_len(tabla_simbolos) >= 2);

    /* eliminar la última subtabla creada */
    TS_subtabla_simbolos aux = (TS_subtabla_simbolos)lista_cabeza(tabla_simbolos);
    lista_eliminar_cabeza(tabla_simbolos);
    TS_eliminar_subtabla(aux);
}

void TS_liberar()
{
    /* eliminamos todas las subtablas */
    while(!lista_vacia(tabla_simbolos))
    {
        TS_subtabla_simbolos aux = (TS_subtabla_simbolos)lista_cabeza(tabla_simbolos);
        lista_eliminar_cabeza(tabla_simbolos);
        TS_eliminar_subtabla(aux);
    }
    /* eliminamos la tabla de simbolos */
    TS_eliminar_tabla();
}

void TS_insertar_simbolo(const char* nombre)
{
    /* obtenemos la subtabla creada más recientemente */
    TS_subtabla_simbolos subtabla = (TS_subtabla_simbolos)lista_cabeza(tabla_simbolos);

    /* comprobamos si ya está insertado en la subtabla creada más reciente */
    int buscar_simbolo(const void* sim) { return strcmp(((const TS_simbolo*)sim)->nombre, nombre) == 0; }
    if(lista_buscar(subtabla, buscar_simbolo)!=-1)
        return;

    /* insertamos el símbolo en la subtabla */
    lista_insertar_cabeza(subtabla, (void*)TS_crear_simbolo(nombre));
}

void TS_modificar_tipo(const char* nombre, int tipo)
{
    /* buscamos el símbolo por nombre */
    TS_simbolo* simbolo = TS_buscar_simbolo(nombre);
    TS_assert(simbolo != NULL);

    /* modificamos la información */
    int subtipo = tipo & 0xFF00;
    tipo &= 0x00FF;
    simbolo->tipo = tipo;
    switch(tipo)
    {
        case TS_VAR:
            simbolo->registro = (void*)TS_crear_var(subtipo);
        break;
        case TS_TIPO:
            simbolo->registro = (void*)TS_crear_tipo(subtipo);
        break;
        case TS_CTE:
            simbolo->registro = (void*)TS_crear_cte(subtipo);
        break;
        case TS_ACCION:
            simbolo->registro = (void*)TS_crear_accion();
        break;
        case TS_FUNCION:
            simbolo->registro = (void*)TS_crear_funcion();
        break;
    }
}



int main(int argc, char** argv)
{
    TS_inicializar();

    TS_insertar_simbolo("vector");
    TS_modificar_tipo(TS_TIPO | TS_TUPLA);
    
    TS_liberar();
}

///// Implementación de métodos auxiliares

TS_tabla_simbolos TS_crear_tabla()
{
    tabla_simbolos = crear_lista();
}

TS_subtabla_simbolos TS_crear_subtabla()
{
    return crear_lista();
}

void TS_eliminar_registro_simbolo(TS_simbolo* simbolo)
{
    switch(simbolo->tipo)
    {
        case TS_VAR:
            TS_eliminar_var((TS_var*)simbolo->registro);
        break;
        case TS_TIPO:
            TS_eliminar_tipo((TS_tipo*)simbolo->registro);
        break;
        case TS_CTE:
            TS_eliminar_cte((TS_cte*)simbolo->registro);
        break;
        case TS_FUNCION:
            TS_eliminar_funcion((TS_funcion*)simbolo->registro);
        break;
        case TS_ACCION:
            TS_eliminar_accion((TS_accion*)simbolo->registro);
        break;
    }
}

void TS_eliminar_simbolo(TS_simbolo* simbolo)
{
    TS_eliminar_registro_simbolo(simbolo);
    TS_free(simbolo);
}


void TS_eliminar_subtabla(TS_subtabla_simbolos subtabla)
{
    while(!lista_vacia(subtabla))
    {
        TS_eliminar_simbolo((TS_simbolo*) lista_cabeza(subtabla));
        lista_eliminar_cabeza(subtabla);
    }
}

void TS_eliminar_tabla()
{
    liberar_lista(tabla_simbolos);
}

TS_simbolo* TS_buscar_simbolo(const char* nombre)
{
    TS_simbolo* simbolo;

    int buscar_subtabla_con_simbolo(const void* subtabla)
    {
        int buscar_simbolo(const void* sim)
        {
            if(strcmp(nombre, ((const TS_simbolo*)sim)->nombre) == 0)
            {
                simbolo = (TS_simbolo*)sim;
                return 1;
            }
            return 0;
        }

        return lista_buscar((const TS_subtabla_simbolos)subtabla, buscar_simbolo)!=-1;

    }

    return lista_buscar(tabla_simbolos, buscar_subtabla_con_simbolo)!=-1 ? simbolo : NULL;
}

TS_simbolo* TS_crear_simbolo(const char* nombre)
{
    TS_simbolo* simbolo = TS_malloc1(TS_simbolo);
    simbolo->nombre = nombre;
    simbolo->id = TS_contador_id++;

    simbolo->registro = NULL;
    simbolo->tipo = ST_DESCONOCIDO;
    return simbolo;

}



TS_var* TS_crear_var(int tipo)
{
    TS_var* var = TS_malloc1(TS_var);
    var->tipo = TS_crear_tipo(tipo);
    return var;
}

TS_tipo* TS_crear_tipo(int tipo)
{
    TS_tipo* t = TS_malloc1(TS_tipo);
    t->tipo = tipo;
    return t;
}

TS_cte* TS_crear_cte(int tipo)
{
    TS_cte* cte = TS_malloc1(TS_cte);
    cte->tipo = tipo;
    return cte;
}

TS_accion* TS_crear_accion()
{
    TS_accion* accion = TS_malloc1(TS_accion);
    return accion;
}
TS_funcion* TS_crear_funcion()
{
    TS_funcion* accion = TS_malloc1(TS_funcion);
    return accion;
}




void TS_eliminar_var(TS_var* var)
{
    TS_eliminar_tipo(var->tipo);
    TS_free(var);
}
void TS_eliminar_accion(TS_accion* accion)
{
    TS_free(accion);
}
void TS_eliminar_funcion(TS_funcion* funcion)
{
    TS_free(funcion);
}
void TS_eliminar_tipo(TS_tipo* tipo)
{
    TS_free(tipo);
}
void TS_eliminar_cte(TS_cte* cte)
{
    TS_free(cte);
}

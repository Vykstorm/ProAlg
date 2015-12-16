#include "tabla_simbolos.h"
#include "util/lista.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

///// Variables y funciones auxiliares 
lista simbolos;
int TS_contador_id = 1;

void 	TS_modificar_registro(TS_simbolo* sim, int tipo);
void 	TS_eliminar_registro(TS_simbolo* sim);
void 	TS_eliminar_simbolo(TS_simbolo* sim);
TS_simbolo* TS_buscar_simbolo(int id);

///// Implementación de los métodos de la interfaz
void TS_inicializar()
{
	simbolos = crear_lista(sizeof(TS_simbolo));
}

void TS_liberar()
{
	/* eliminamos todos los símbolos */
	while(!lista_vacia(simbolos))
	{
		TS_eliminar_simbolo((TS_simbolo*)lista_cabeza(simbolos));
		lista_eliminar_cabeza(simbolos);
	}
	liberar_lista(simbolos);
}

int TS_insertar_simbolo(const char* nombre)
{
	assert(nombre != NULL);
	
	/* creamos el símbolo */
	TS_simbolo sim;
	sim.id = TS_contador_id++;
	sim.tipo = TS_UNKNOWN;
	strcpy(sim.nombre, nombre);
	
	/* metemos símbolo en la lista */
	lista_insertar_cola(simbolos, (void*)&sim);
	
	return sim.id;
}

void TS_modificar_tipo(int id, int tipo)
{
	printf("modificando tipo: %d, %d\n", id, tipo);
	
	/* Buscar símbolo por ID */
	TS_simbolo* sim;
	sim = TS_buscar_simbolo(id);
	assert(sim != NULL);
	
	/* Modificamos el tipo de símbolo */
	TS_modificar_registro(sim,tipo);
}

int TS_newtemp()
{
	/* creamos el símbolo */
	TS_simbolo sim;
	sim.id = TS_contador_id++;
	sim.tipo = TS_UNKNOWN;
	sprintf(sim.nombre, "tempvar_%d", sim.id);
	
	lista_insertar_cola(simbolos, (void*)&sim);
	return sim.id;
}


////// Implementación de los métodos auxiliares
void TS_eliminar_registro(TS_simbolo* sim)
{
	switch(sim->tipo)
	{
		case TS_CTE:
		case TS_FUNCION:
		case TS_ACCION:
		{
			free((void*)sim->registro);
		}
		break;
		case TS_VAR:
		{
			free((void*)(((TS_var*)sim->registro)->tipo));
			free((void*)sim->registro);
		}
		break;
		case TS_PUNTERO:
		{ 
			free((void*)(((TS_puntero*)sim->registro)->tipo_apuntado));
			free((void*)sim->registro);
		}
		break;
	}
}

void TS_modificar_registro(TS_simbolo* sim, int tipo)
{
	if(sim->tipo != (tipo & 0x00FF))
	{
		if(sim->tipo != TS_UNKNOWN)
			TS_eliminar_registro(sim);
		switch(tipo & 0x00FF)
		{
			case TS_CTE:
			{
				TS_cte* cte = (TS_cte*)malloc(sizeof(TS_cte));
				assert((tipo & 0x8000) != 0);
				cte->tipo = tipo & 0xFF00;
				sim->registro = (void*)cte;
			}
			break;
			case TS_FUNCION:
			{
				TS_funcion* funcion = (TS_funcion*)malloc(sizeof(TS_funcion));
				sim->registro = (void*)funcion;
			}
			break;
			case TS_ACCION:
			{
				TS_accion* accion = (TS_accion*)malloc(sizeof(TS_accion));
				sim->registro = (void*)accion;
			}
			break;
			case TS_VAR:
			{
				TS_var* var = (TS_var*)malloc(sizeof(TS_var));
				TS_tipo* var_t = (TS_tipo*)malloc(sizeof(TS_tipo));
				var_t->tipo = tipo & 0xFF00;
				
				var->tipo = var_t;
				sim->registro = (void*)var;
			}
			break;
			case TS_PUNTERO:
			{
				TS_puntero* ptr = (TS_puntero*)malloc(sizeof(TS_puntero));
				TS_tipo* ptr_t = (TS_tipo*)malloc(sizeof(TS_tipo));
				ptr_t->tipo = tipo & 0xFF00;
				ptr->tipo_apuntado = ptr_t;
				sim->registro = (void*)ptr;
			}
			break;
			case TS_TIPO:
			{
				TS_tipo* tipo_t = (TS_tipo*)malloc(sizeof(TS_tipo));
				tipo_t->tipo = tipo & 0xFF00;
				sim->registro = (void*)tipo_t;
			}
			break;
		}
		sim->tipo = tipo & 0x00FF;
	}
	
}

void TS_eliminar_simbolo(TS_simbolo* sim)
{
	TS_eliminar_registro(sim);
}

TS_simbolo* TS_buscar_simbolo(int id)
{
	int busqueda(const void* sim) { return ((TS_simbolo*)sim)->id == id; }
	int i;
	if( (i=lista_buscar(simbolos, busqueda)) != -1)
		return (TS_simbolo*)lista_consultar(simbolos,i);
	return NULL;
}

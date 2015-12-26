#include "tabla_simbolos.h"
#include "util/lista.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

///// Variables y funciones auxiliares 
lista simbolos;
int TS_contador_id = 1;
int TS_cte_verdadero_id=0, TS_cte_falso_id=0, TS_cte_0_id=0;

void 	TS_modificar_registro(TS_simbolo* sim, int tipo);
void 	TS_eliminar_registro(TS_simbolo* sim);
void 	TS_eliminar_simbolo(TS_simbolo* sim);
TS_simbolo* TS_buscar_simbolo_por_id(int id);
TS_simbolo* TS_buscar_simbolo_por_nombre(const char* nombre);

///// Implementación de los métodos de la interfaz
void TS_inicializar()
{
	simbolos = crear_lista(sizeof(TS_simbolo));
}

void TS_liberar()
{
	/* mostramos la tabla de símbolos */
#ifdef TS_DEBUG
	TS_printdebug();
#endif
	
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

void TS_modificar_simbolo(int id, int tipo)
{	
	/* Buscar símbolo por ID */
	TS_simbolo* sim;
	sim = TS_buscar_simbolo_por_id(id);
	assert(sim != NULL);
	
	/* Modificamos el tipo de símbolo */
	TS_modificar_registro(sim,tipo);
}

void TS_modificar_cte(int id, TS_cte_val val)
{
	/* Buscamos el símbolo por ID y comprobamos si es una cte */
	TS_simbolo* sim;
	sim = TS_buscar_simbolo_por_id(id);
	assert((sim != NULL) && (sim->tipo == TS_CTE));
	TS_cte* cte = (TS_cte*)sim->registro;
	switch(cte->tipo)
	{
		case TS_ENTERO:
			cte->val.entero = val.entero;
		break;
		case TS_REAL:
			cte->val.real = val.real;
		break;
		case TS_BOOLEANO:
			cte->val.booleano = val.booleano;
		break;
		case TS_CARACTER:
			cte->val.caracter = val.caracter;
		break;
		case TS_STRING:
			strcpy(cte->val.string, val.string);
		break;
	}
}

void TS_modificar_var(int id_var, const char* nombre_tipo)
{
	TS_simbolo *sim_var, *sim_tipo;
	sim_var=TS_buscar_simbolo_por_id(id_var);
	sim_tipo=TS_buscar_simbolo_por_nombre(nombre_tipo);
	assert((sim_var != NULL) && (sim_tipo != NULL) && (sim_var->tipo == TS_VAR) && (sim_tipo->tipo == TS_TIPO));
	
	/* Modificamos la variable */
	((TS_var*)sim_var->registro)->tipo = (TS_tipo*)sim_tipo->registro;
}

int TS_buscar_simbolo(const char* nombre)
{
	TS_simbolo* sim = TS_buscar_simbolo_por_nombre(nombre);
	return (sim != NULL) ? sim->id : -1;
}

int TS_consultar_tipo(int id)
{
	TS_simbolo* sim;
	sim = TS_buscar_simbolo_por_id(id);
	assert(sim != NULL);
	
	int tipo = 0;
	tipo |= sim->tipo;
	switch(sim->tipo)
	{
		case TS_VAR:
			tipo |= (((TS_var*)sim->registro)->tipo)->tipo; 
		break;
		case TS_CTE:
			tipo |= ((TS_cte*)sim->registro)->tipo;
		break;
		case TS_TIPO:
			tipo |= ((TS_tipo*)sim->registro)->tipo;
		break;
	}
	
	return tipo;
}

TS_cte_val TS_consultar_cte(int id)
{
	TS_simbolo* sim;
	sim = TS_buscar_simbolo_por_id(id);
	assert((sim != NULL) && (sim->tipo == TS_CTE));
	return ((TS_cte*)sim->registro)->val;
}

int TS_newtempvar()
{
	/* creamos el símbolo */
	TS_simbolo sim;
	sim.id = TS_contador_id++;
	sim.tipo = TS_UNKNOWN;
	sprintf(sim.nombre, "tempvar_%d", sim.id);
	lista_insertar_cola(simbolos, (void*)&sim);
	TS_modificar_registro((TS_simbolo*)lista_cola(simbolos), TS_VAR);
	return sim.id;
}

int TS_newliteral()
{
	/* creamos el símbolo */
	TS_simbolo sim;
	sim.id = TS_contador_id++;
	sim.tipo = TS_UNKNOWN;
	sprintf(sim.nombre, "templiteral_%d", sim.id);
	lista_insertar_cola(simbolos, (void*)&sim);
	TS_modificar_registro((TS_simbolo*)lista_cola(simbolos), TS_CTE);
	return sim.id;
}

int TS_cte_verdadero()
{
	if(!TS_cte_verdadero_id)
	{
		TS_cte_verdadero_id = TS_newliteral();
		TS_modificar_simbolo(TS_cte_verdadero_id, TS_CTE|TS_BOOLEANO); 
		TS_cte_val val; val.booleano = 1;
		TS_modificar_cte(TS_cte_verdadero_id,val);
	}
	return TS_cte_verdadero_id;
}

int TS_cte_falso()
{
	if(!TS_cte_falso_id)
	{
		TS_cte_falso_id = TS_newliteral();
		TS_modificar_simbolo(TS_cte_falso_id, TS_CTE|TS_BOOLEANO); 
		TS_cte_val val; val.booleano = 0;
		TS_modificar_cte(TS_cte_falso_id,val);
	}
	return TS_cte_falso_id;
}

int TS_cte_0()
{
	if(!TS_cte_0_id)
	{
		TS_cte_0_id = TS_newliteral();
		TS_modificar_simbolo(TS_cte_0_id, TS_CTE|TS_ENTERO); 
		TS_cte_val val; val.entero = 0;
		TS_modificar_cte(TS_cte_0_id,val);
	}
	return TS_cte_0_id;
}


void info_tipo(char* str, int tipo)
{
	switch(tipo)
	{
		case TS_TABLA:
			strcpy(str, "tabla");
		break;
		case TS_TUPLA:
			strcpy(str, "tupla");
		break;
		case TS_PUNTERO:
			strcpy(str, "puntero");
		break;
		case TS_ENTERO:
			strcpy(str, "entero");
		break;
		case TS_REAL:
			strcpy(str, "real");
		break;
		case TS_BOOLEANO:
			strcpy(str, "booleano");
		break;
		case TS_CARACTER:
			strcpy(str, "caracter");
		break;
		case TS_STRING:
			strcpy(str, "string");
		break;
	}
}



void mostrar_simbolo(const void* s)
{
	TS_simbolo* sim =(TS_simbolo*)s;
	char tipo[128];
	switch(sim->tipo)
	{
		case TS_ACCION:
		strcpy(tipo, "accion");
		break;
		case TS_FUNCION:
		strcpy(tipo, "funcion");
		break;
		case TS_CTE:
		{
			TS_cte* cte = (TS_cte*)sim->registro;
			if(cte->tipo != TS_UNKNOWN)
			{
				char tipo_cte[64];
				char valor_cte[256];
				info_tipo(tipo_cte, cte->tipo);
				switch(cte->tipo)
				{
					case TS_ENTERO:
						sprintf(valor_cte, "%d", cte->val.entero);
					break;
					case TS_REAL:
						sprintf(valor_cte, "%.2f", cte->val.real);
					break;
					case TS_BOOLEANO:
						strcpy(valor_cte, cte->val.booleano ? "verdadero" : "falso");
					break;
					case TS_CARACTER:
						sprintf(valor_cte, "%c", cte->val.caracter);
					break;
					case TS_STRING:
						strcpy(valor_cte, cte->val.string); 
					break;
				}
				sprintf(tipo, "constante de tipo %s (valor: %s)", tipo_cte, valor_cte);
			}
			else 
				strcpy(tipo, "constante");
		}
		break;
		case TS_VAR:
		{
			TS_tipo* var_t = ((TS_var*)sim->registro)->tipo;
			if(var_t->tipo != TS_UNKNOWN)
			{
				char tipo_var[64];
				info_tipo(tipo_var, var_t->tipo);
				sprintf(tipo, "variable de tipo %s", tipo_var);
			}
			else
				strcpy(tipo, "variable");
		}
		break;
		case TS_TIPO:
		{
			TS_tipo* tipo_t = (TS_tipo*)sim->registro;
			if(tipo_t->tipo != TS_UNKNOWN)
				info_tipo(tipo, tipo_t->tipo);
			else
				strcpy(tipo, "tipo");
		}
		break;
		case TS_UNKNOWN:
			sprintf(tipo, "---");
		break;
	}
	printf("%5d %10s %25s\n", sim->id, sim->nombre, tipo);
}

void TS_printdebug()
{
	puts("---------------------------");
	printf("nº simbolos: %d\n", lista_len(simbolos));
	printf("%5s %10s %25s\n", "ID", "NOMBRE", "TIPO");
	lista_recorrer(simbolos, mostrar_simbolo);
	puts("---------------------------");
}


////// Implementación de los métodos auxiliares
void TS_eliminar_registro(TS_simbolo* sim)
{
	switch(sim->tipo)
	{
		case TS_CTE:
		case TS_FUNCION:
		case TS_ACCION:
		case TS_TIPO:
		{
			free((void*)sim->registro);
		}
		break;
		case TS_VAR:
		{
			int buscar_registro(const void* sim2) { return ((const TS_simbolo*)sim2)->registro == (const void*)((((TS_var*)sim->registro)->tipo)); }
			if(lista_buscar(simbolos, buscar_registro) != -1)
				free((void*)(((TS_var*)sim->registro)->tipo));
			free((void*)sim->registro);
			
		}
		break;
	}
}

void TS_modificar_registro(TS_simbolo* sim, int tipo)
{

	if(sim->tipo != TS_UNKNOWN)
		TS_eliminar_registro(sim);
	switch(tipo & 0x00FF)
	{
		case TS_CTE:
		{
			TS_cte* cte = (TS_cte*)malloc(sizeof(TS_cte));
			//assert((tipo & 0x8000) != 0);
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

void TS_eliminar_simbolo(TS_simbolo* sim)
{
	TS_eliminar_registro(sim);
}

TS_simbolo* TS_buscar_simbolo_por_id(int id)
{
	int busqueda(const void* sim) { return ((TS_simbolo*)sim)->id == id; }
	int i;
	if( (i=lista_buscar(simbolos, busqueda)) != -1)
		return (TS_simbolo*)lista_consultar(simbolos,i);
	return NULL;
}

TS_simbolo* TS_buscar_simbolo_por_nombre(const char* nombre)
{
	int busqueda(const void* sim) { return strcmp(((TS_simbolo*)sim)->nombre, nombre) == 0; }
	int i;
	if( (i=lista_buscar(simbolos, busqueda)) != -1)
		return (TS_simbolo*)lista_consultar(simbolos,i);
	return NULL;
}

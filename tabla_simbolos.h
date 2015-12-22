/**
	\file
	@brief Cabecera que define las estructuras de datos de la tabla de símbolos y define una
	interfaz (funciones) para poder acceder/modificar esta.
	@author Víctor Ruiz Gómez, Silvia Popa
*/

#ifndef _TABLA_SIMBOLOS_H
#	define _TABLA_SIMBOLOS_H

///////// MACROS Y ENUMERACIONES

/* Tipos de simbolos */

enum TS_tipo_simbolo
{
    TS_VAR = 1,
    TS_TIPO,
    TS_CTE,
    TS_FUNCION,
    TS_ACCION
};

/* Tipos de tipos de datos */
enum TS_tipo_dato
{
    TS_TABLA = 1<<8,
    TS_TUPLA = 2<<8,
    TS_PUNTERO = 3<<8,
    TS_ENTERO = 4<<8 | 1<<15,
    TS_REAL = 5<<8 | 1<<15, 
    TS_BOOLEANO = 6<<8 | 1<<15,
    TS_CARACTER = 7<<8 | 1<<15,
    TS_STRING = 8<<8 | 1<<15
};

/* Se usa para indicar que un campo se desconoce */
#define TS_UNKNOWN 0


///////// DEFINICIÓN DE ESTRUCTURAS DE DATOS




typedef struct TS_simbolo
{
    int tipo; /* el tipo tendrá el valor TS_UNKNOWN si no se sabe que tipo de símbolo es */
    int id; /* la dirección del símbolo */
    char nombre[128];
    void* registro; /* es un apuntador a otra estructura que contiene información adicional para este símbolo.
        la estructura apuntada dependerá del tipo de símbolo */

} TS_simbolo;


/* Estructura que contiene información adicional asociada un símbolo (si es un tipo) */
struct TS_tipo;
typedef struct TS_tipo TS_tipo;


/* Esta estructura indica información adicional si el tipo de dato es una tupla */
typedef struct TS_tupla
{

} TS_tupla;

/* Esta estructura indica información adicional si el tipo de dato es una tabla */
typedef struct TS_tabla
{

} TS_tabla;

/* Esta estructura indica información adicional si el tipo de dato es un puntero a un tipo */
typedef struct TS_puntero
{
    TS_tipo* tipo_apuntado; /* es el tipo de dato apuntado */
} TS_puntero;


typedef struct TS_tipo
{
    int tipo; /* tipo de dato: puede ser un tipo de dato básico o estructurado. Si es un tipo estructurado, se podrá
    acceder a información adicional sobre este, usando el campo adecuado en la unión. Tiene el valor  TS_UNKNOWN si
    se desconoce que tipo de dato es este. */
    union
    {
        TS_tabla tabla;
        TS_tupla tupla;
        TS_puntero ptro;
    };
} TS_tipo;


/* Estructura que contiene información adicional asociada un símbolo (si es una variable) */
typedef struct TS_var
{
    TS_tipo* tipo; /* tipo de dato de la variable */
} TS_var;

typedef struct TS_cte_val
{
    union /* en función del tipo, se podrá acceder a uno de estos campos para obtener el valor de la cte */
    {
        int entero;
        double real;
        int booleano;
        char caracter;
        char string[256];
    };
} TS_cte_val;

typedef struct TS_cte
{
    int tipo; /* indica el tipo de dato básico de la cte. Es ST_DESCONOCIDO si se desconoce el tipo de cte */
    TS_cte_val val; /* el valor de la cte */
} TS_cte;

/* Estructura que contiene información añadida de un símbolo (si es una función) */
typedef struct TS_funcion
{

} TS_funcion;

/* Estructura que contiene información adicional asociada un símbolo (si es una acción) */
typedef struct TS_accion
{

} TS_accion;



///////// INTERFAZ DE LA TABLA DE SIMBOLOS

/// Inicialización/Eliminación de la tabla de símbolos.

/**
    Debe invocarse antes de usar la tabla de símbolos
*/
void TS_inicializar();

/**
    Debe invocarse para eliminar los recursos asociados a la tabla
    de simbolos
*/
void TS_liberar();

/// Consultas y búsquedas en la tabla de símbolos
/**
 * @return Devuelve información sobre el tipo de símbolo
 * if(TS_consultar_tipo(id) & TS_VAR) -> variable
 * if((TS_consultar_tipo(id) & TS_VAR) && (TS_consultar_tipo(id) & TS_TUPLA)) -> variable cuyo tipo es una tupla
 */
int TS_consultar_tipo(int id);


/**
 * @return Devuelve el valor de una cte que está en la tabla de símbolos.
 */
TS_cte_val TS_consultar_cte(int id);


/**
 * @return Devuelve una cte creada por la tabla de símbolos cuyo valor es verdadero. Esto se usa para generar
 * las instrucciones de salto condicionales, en las que se salta en función de si una variable booleana es cierta
 * o falsa.
 */
int TS_cte_verdadero();


/// Insercciones y eliminaciones en la tabla de símbolos.


/**
	Inserta un nuevo símbolo en la tabla, con el nombre indicado.
	@param nombre El nombre del símbolo
	El símbolo se inserta en la TS si aún no se ha insertado,
	después de una invocación a la función TS_set(). En caso contrario,
	el símbolo ya esta insertado en la TS y no se hace nada.
	@return Devuelve la id asociada al nuevo simbolo creado, o la 
	del símbolo ya existente con ese nombre 
*/
int TS_intertar_simbolo(const char* nombre);

/// Modificaciones en la tabla de símbolos

/**
	Añade información del tipo de símbolo a un símbolo de la tabla.
	@param nombre Es el nombre del símbolo.
	@param tipo Es el tipo de símbolo. El byte menos significativo indica el
	tipo de símbolo (variable, tipo, constante, accion, funcion, ...)
	El byte más significativo especifíca mas información sobre el tipo
    e.g:
    TS_VAR | TS_ENTERO  -> símbolo que es una variable de tipo entero
    TS_VAR -> símbolo que es una variable de la que se desconoce su tipo.
    TS_TIPO | TS_TUPLA -> símbolo que es un tipo y cuya estructura es en forma de tupla.
    TS_CTE | TS_REAL -> cte de tipo real
    ...
*/
void TS_modificar_simbolo(int id, int tipo);


/**
 * Establece el valor de una cte de la tabla de símbolos.
 * @param id Es la id de la cte
 * @param val Es el valor de la cte
 */
void TS_modificar_cte(int id, TS_cte_val val);

/**
 * Modifica el tipo de una variable existente para que tenga el tipo de un símbolo
 * también existente 
 * @param id_var Es la id de la variable
 * @param nombre_tipo Es el nombre del tipo.
 */
void TS_modificar_var(int id_var, const char* nombre_tipo);



/**
 * Crea una nueva variable temporal en la tabla de símbolos.
 * @return Devuelve la id de la nueva variable temporal.
 */
int TS_newtempvar();

/**
* Crea una cte temporal en la tabla de símbolos (permite trabajar
* con literales en expresiones aritméticas y booleanas)
*/
int TS_newliteral();

/**
 * Busca un símbolo por nombre.
 * @return Devuelve -1 si no hay ningún símbolo con ese nombre, o la id del símbolo si existe.
 */
int TS_buscar_simbolo(const char* nombre);

/**
 * Imprime información en la salida estándar de la tabla de símbolos 
 */
void TS_printdebug();

#endif


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
    TS_ENTERO = 4<<8,
    TS_REAL = 5<<8,
    TS_BOOLEANO = 6<<8,
    TS_CARACTER = 7<<8,
    TS_STRING = 8<<8
};

/* Se usa para indicar que un campo se desconoce */
#define ST_DESCONOCIDO 0


///////// DEFINICIÓN DE ESTRUCTURAS DE DATOS


typedef struct TS_simbolo
{
    int tipo; /* el tipo tendrá el valor ST_DESCONOCIDO si no se sabe que tipo de símbolo es */
    int id;
    const char* nombre;
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
    acceder a información adicional sobre este, usando el campo adecuado en la unión. Tiene el valor ST_DESCONOCIDO si
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

typedef struct TS_cte
{
    int tipo; /* indica el tipo de dato básico de la cte. Es ST_DESCONOCIDO si se desconoce el tipo de cte */
    union   /* en función del tipo, se podrá acceder a uno de estos campos para obtener el valor de la cte */
    {
        int entero;
        double real;
        int booleano;
        char caracter;
        char* string;
    };
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



/// Insercciones y eliminaciones en la tabla de símbolos.


/**
	Inserta un nuevo símbolo en la tabla, con el nombre indicado.
	@param nombre El nombre del símbolo
	El símbolo se inserta en la TS si aún no se ha insertado,
	después de una invocación a la función TS_set(). En caso contrario,
	el símbolo ya esta insertado en la TS y no se hace nada.
*/
void TS_intertar_simbolo(const char* nombre);

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
void TS_modificar_tipo(const char* nombre, int tipo);


/**
    Crea una nueva subtabla dentro de la tabla de símbolos. Los nuevos símbolos que se
    inserten en la TS, se guardarán en esa subtabla. La idea es que en una misma subtabla,
    estén todos los símbolos que se hayan declarado en un mismo ámbito.
    Además, al buscar un símbolo en la TS, se comenzará a buscar primero por las subtablas que
    se hayan creado más recientemente.

    @note Debe invocarse este método cada vez que se lee un código que cree un nuevo ámbito
    de declaración de variables. Por ejemplo, al leer la cabecera de una acción o función.
*/
void TS_set();

/**
    Elimina la subtabla creada más recientemente, junto con los símbolos que hay en ella almacenados.
    @note Este método debe invocarse cuando se lee código que cierre un ámbito de delcaración de variables.
    @note Debe invocarse antes de cualquier llamada a esta función, TS_set()
*/
void TS_reset();


#endif


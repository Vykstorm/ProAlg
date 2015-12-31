/**
 * @file
 * 	\brief Es una cabecera que define una serie de procedimientos para manejar los errores
 * que puedan cometerse en el parser.
 * @author Silvia Popa y Víctor Ruiz Gómez
 */
#ifndef __ERRORES_H
#	define __ERRORES_H


///////////// Gestión de errores del parser

/** Tipos de errores del parser */
typedef enum errores_parser
{
	E_SIMBOLO_NO_DEFINIDO = 1, /* error que se produce cuando en una expresión se reconoce
			un operando que no ha sido previamente declarado, por ejemplo */
	E_OPERANDO_NO_VALIDO, /*error que se produce en expres. aritméticas/booleanas... cuando 
			se espera tener un entero o un real, y el operando es de otro tipo */
	E_CONVERSION_NO_VALIDA, /* error que se produce en las asignaciones cuando la conversión de tipos
			no es válida */
	E_MODIFICANDO_CTE, /* error que se produce cuando se intenta modificar el valor de una cte */
} errores_parser;


/** Es como yyerror, pero acepta formato, como printf  */
int __yyerror(const char* fmt, ...);

/** Es una rutina que imprime información de un error del parser.
 * @param err Es el tipo de error que se ha cometido
 * ... Un conjunto de parámetros que añaden más información al mensaje de error,
 * en función del tipo de error.
 */
int parse_error(int err, ...);


/** Es una macro que imprime un mensaje de error del parser, y le dice a bison que 
 * hay un error en la rutina semántica que ha invocado esta macro.
 * Por ello, solo puede usarse las rutinas semánticas del parser.
 */
#define PARSE_ERROR(...) parse_error(__VA_ARGS__); YYERROR;


/**
 * Es una variable global, gestionada por flex, que indica el nº línea actual que
 * se está procesando.
 */
extern int line_number;


#endif

////////////// Autores

Silvia Popa
Víctor Ruiz Gómez 


//////////////  ¿Que es lo que hemos conseguido con nuestro compilador? Caracteristicas del compilador

- En la tabla de símbolos:

Solo se almacenan en la TS las constantes y las variables. Además, las variables no pueden ser de tipos estructurados, solo pueden ser tipos básicos.
Los literales que aparecen en el código fuente son almacenados en la TS, al ser analizados por el parser, como constantes de un tipo básico específico.

- En la generación de código intermedio:

Nuestro parser es capaz de generar código tanto para expresiones booleanas como aritméticas.
Con respecto a las aritméticas, se permite además para determinadas operaciones, usar un operando real y otro aritmético, realizándose una conversión implícita de tipos (suma, resta, multiplicación, división no entera).

También permite generar código intermedio para los siguientes tipos de instrucciones: asignaciones, instrucciones alternativas e iterativas (con cota fija o con cota por expresión).

Las asignaciones pueden realizar conversiones implícitas si los tipos son compatibles (Puede asignarse una expresión real a una variable entera y viceversa)

Además escogimos el código de cuatro direcciones como forma de representación de las instrucciones de código intermedio.

- En la gestión de errores:

Este parser tiene en cuenta los errores semánticos del código fuente: 
- Conversiones de tipos que no son compatibles
- Modificación de constantes.
- Símbolos que se usan en expresiones no declarados previamente en la zona de declaraciones (variables y constantes)
- Operandos en expresiones aritméticas/booleanas no válidos

Además permite acotar los errores y continuar con el proceso de parseo en busca de otros errores
(error recovery). Pero solo puede identificar y acotar un error máximo por instrucción.

De forma adicional, muestra el lugar donde se han cometido los errores en cualquier fase de la compilación (indica la línea donde está el error)



///////////////// ¿Que pasos se puedan dar para probar este compilador?

//// 1º Instrucciones para construir el compilador. 

Se proporciona un Makefile para esto. 

Para construir el compilador por defecto
$ make 

//// 2º Como ejecutar el compilador

El programa tomará siempre el código fuente por la entrada estandar y mostrará por la salida estandar,
las instrucciones en código intermedio generadas.
Puedes usar el siguiente comando para compilar el ejemplo después de haber construido el compilador.

$ cat ejemplo.proalg | ./proalg

También tienes más ejemplos en el directorio ejemplos/

//// 3º Como depurar el compilador

De forma adicional, a la hora de construir el compilador, pueden añadirse las siguientes opciones para
obtener más información de depuración.

$ make tr_debug=yes/no  
Con esta opción, se van mostrando las instrucciones en código intermedio que se van generando y
también indica cuando se completan los saltos al hacer backpatches.

$ make ts_debug=yes/no 
Muestra el contenido de la tabla de símbolos al final de la compilación.

$ make scanner_debug=yes/no 
Se añade información de depuración al leer los tokens en el scanner. Imprime un mensaje cuando el parser
solicita la lectura de un nuevo token y este se lee.

$ make parser_debug=yes/no
Está opción añade el comando -v a bison para generar el fichero parser.output con información sobre los estados
del parser.

$ make debug=yes
Para activar todas las anteriores al mismo tiempo. También se puede desactivar alguna en particular..

$ make debug=yes scanner_debug=no


Para recompilar el proyecto, hay que limpiar previamente los ficheros de la compilación anterior..

$ make clean

///////////////// Imputaciones de trabajo

Silvia Popa 30%
Víctor Ruiz Gómez 70%

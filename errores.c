#include "errores.h"
#include "tabla_simbolos.h"
#include <stdarg.h>


int __yyerror(const char* fmt, ...)
{
	va_list args;
	char buffer[1024];
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);
	int r=yyerror(buffer);
	va_end(args);
	return r;
}

int parse_error(int err, ...)
{
	va_list args;
	va_start(args,err);
	
	switch(err)
	{
		case E_SIMBOLO_NO_DEFINIDO:
			__yyerror("simbolo \"%s\" no definido", va_arg(args,const char*));
		break;
		case E_OPERANDO_NO_VALIDO:
			__yyerror("operando \"%s\" no valido", TS_consultar_nombre(va_arg(args,int)));
		break;
		case E_CONVERSION_NO_VALIDA:
			__yyerror("conversion de tipos no valida");
		break;
		case E_MODIFICANDO_CTE:
			__yyerror("no se puede modificar el valor de la cte \"%s\"", TS_consultar_nombre(va_arg(args,int)));
		break;
	}
	va_end(args);
}

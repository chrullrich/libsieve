#ifndef HEADERINC_H
#define HEADERINC_H

/* Structures are here now */
#include "sv_parser/parser.h"

#define HEADERHASHSIZE 1019
#define YY_DECL int libsieve_headerlex(YYSTYPE *yylval_param, struct sieve2_context *context, void *yyscanner)

#endif /* HEADERINC_H */

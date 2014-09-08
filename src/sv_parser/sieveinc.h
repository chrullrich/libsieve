#ifndef SIEVEINC_H
#define SIEVEINC_H

#include "sv_interface/tree.h"
#include "sv_interface/script.h"
#include "sv_interface/context2.h"

#undef YY_DECL
#define YY_DECL int libsieve_sievelex (YYSTYPE *yylval_param, struct sieve2_context *context, void *yyscanner)
int libsieve_sieveerror(struct sieve2_context *context, void *yyscanner, const char *);

#endif /* SIEVEINC_H */

/* context2.c -- manages the libSieve opaque context and callbacks.
 * Aaron Stone
 * $Id$
 */
/* * * *
 * Copyright 2005 by Aaron Stone
 *
 * Licensed under the GNU Lesser General Public License (LGPL)
 * version 2.1, and other versions at the author's discretion.
 * * * */

#include "msvc/msvc.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* CMU portions. */
#include "sv_interface/tree.h"
#include "sv_interface/script.h"
#include "sv_interface/message.h"

/* libSieve additions. */
#include "sv_include/sieve2.h"
#include "sv_include/sieve2_error.h"
#include "sv_interface/context2.h"
#include "sv_interface/message2.h"
#include "sv_parser/parser.h"
#include "sv_util/util.h"


/* Indicate that we're starting a new callback. */
int libsieve_callback_begin(
    struct sieve2_context *context,
    sieve2_values_t callback)
{
    int i;

    /* We're clear to begin if:
     * begin and end are both 0
     * begin and end are both 1
     * */
    if (context->cur_call.begin != context->cur_call.end) {
        /* This means a programming error. */
        return SIEVE2_ERROR_FAIL;
    }

    context->cur_call.begin = TRUE;
    context->cur_call.end = FALSE;
    context->cur_call.code = callback;

    for (i = 0; i < MAX_VALUES; i++) {
        context->cur_call.values[i].name = NULL;
        context->cur_call.values[i].value.s = NULL;
    }

    return SIEVE2_OK;
}

/* Make the callback to the user app. */
int libsieve_callback_do(
    struct sieve2_context *c,
    sieve2_values_t callback)
{
        switch(callback)
          {
#define   CBCALL(VAL, CB) \
          case VAL: \
              if (c->callbacks.CB) \
                  ((sieve2_callback_func)c->callbacks.CB)(c, c->user_data); \
	      else \
		  return SIEVE2_ERROR_UNSUPPORTED;\
              break
          CBCALL(SIEVE2_ACTION_REDIRECT,       redirect);
          CBCALL(SIEVE2_ACTION_REJECT,         reject);
          CBCALL(SIEVE2_ACTION_DISCARD,        discard);
          CBCALL(SIEVE2_ACTION_FILEINTO,       fileinto);
          CBCALL(SIEVE2_ACTION_KEEP,           keep);
          CBCALL(SIEVE2_ACTION_NOTIFY,         notify);
          CBCALL(SIEVE2_ACTION_DENOTIFY,       denotify);
          CBCALL(SIEVE2_ACTION_VACATION,       vacation);

          CBCALL(SIEVE2_ERRCALL_RUNTIME,       err_runtime);
          CBCALL(SIEVE2_ERRCALL_PARSE,         err_parse);
          CBCALL(SIEVE2_ERRCALL_HEADER,        err_header);
          CBCALL(SIEVE2_ERRCALL_ADDRESS,       err_address);

          CBCALL(SIEVE2_DEBUG_TRACE,           debug_trace);

          CBCALL(SIEVE2_SCRIPT_GETSCRIPT,      getscript);

          CBCALL(SIEVE2_MESSAGE_GETHEADER,     getheader);
          CBCALL(SIEVE2_MESSAGE_GETALLHEADERS, getallheaders);
          CBCALL(SIEVE2_MESSAGE_GETSUBADDRESS, getsubaddress);
          CBCALL(SIEVE2_MESSAGE_GETENVELOPE,   getenvelope);
          CBCALL(SIEVE2_MESSAGE_GETSIZE,       getsize);
          CBCALL(SIEVE2_MESSAGE_GETBODY,       getbody);
	  default:
              // FIXME: Also put useful error text into the context.
              return SIEVE2_ERROR_UNSUPPORTED;
	  }

    return SIEVE2_OK;
}

/* Indicate that we're done with this callback. */
int libsieve_callback_end(
    struct sieve2_context *context,
    sieve2_values_t callback)
{
    int i;

    /* We're clear to clean up if:
     * begin is true.
     * end is false.
     * callback matches current callback
     * */
    if (!(context->cur_call.begin && !context->cur_call.end)
    || context->cur_call.code != callback) {
        /* Again, this means some programming error. */
        return SIEVE2_ERROR_FAIL;
    }

    context->cur_call.end = TRUE;
    context->cur_call.code = SIEVE2_VALUE_FIRST;

    for (i = 0; i < MAX_VALUES; i++) {
        if (context->cur_call.values[i].name)
            libsieve_free(context->cur_call.values[i].name);
/*        switch (context->cur_call.values[i].type) {
        case VAL_INT:
            // Nothing doing.
            break;
        case VAL_STRING:
            if (context->cur_call.values[i].value.s)
                libsieve_free(context->cur_call.values[i].value.s);
            break;
        case VAL_STRINGLIST:
            // Crap.
            break;
        }*/
    }

    return SIEVE2_OK;
}

/* The functions below are publicly accessible for working
 * with callbacks. Fancy this, we're going to use the same
 * API on the inside, just in reverse! */

/* libSieve will free this memory for you, don't worry about it. */
VISIBLE const char * sieve2_getvalue_string(
    sieve2_context_t *c,
    const char * const name)
{
    int i;

    for (i = 0; i < MAX_VALUES; i++) {
        if (c->cur_call.values[i].type == VAL_STRING
        && c->cur_call.values[i].name && name
        && strcasecmp(c->cur_call.values[i].name, name) == 0) {
            return c->cur_call.values[i].value.s;
        }
    }

    return NULL;
}

/* libSieve will free this memory for you, don't worry about it. */
VISIBLE char * * sieve2_getvalue_stringlist(
    sieve2_context_t *c,
    const char * const name)
{
    int i;

    for (i = 0; i < MAX_VALUES; i++) {
        if (c->cur_call.values[i].type == VAL_STRINGLIST
        && c->cur_call.values[i].name && name
        && strcasecmp(c->cur_call.values[i].name, name) == 0) {
            return c->cur_call.values[i].value.sl;
        }
    }

    return NULL;
}

VISIBLE int sieve2_getvalue_int(
    sieve2_context_t *c,
    const char * const name)
{
    int i;

    for (i = 0; i < MAX_VALUES; i++) {
        if (c->cur_call.values[i].type == VAL_INT
        && c->cur_call.values[i].name && name
        && strcasecmp(c->cur_call.values[i].name, name) == 0) {
            return c->cur_call.values[i].value.i;
        }
    }

    return -1;
}

/* If you allocated the memory, you have to free it. */
VISIBLE int sieve2_setvalue_string(
    sieve2_context_t *c,
    const char * const name, const char * const value)
{
    int i;

    if (!name || !value)
        return SIEVE2_ERROR_FAIL;

    for (i = 0; i < MAX_VALUES; i++) {
        if (c->cur_call.values[i].name == NULL) {
            c->cur_call.values[i].name = libsieve_strdup(name);
            c->cur_call.values[i].type = VAL_STRING;
            c->cur_call.values[i].value.s = value;
            return SIEVE2_OK;
        }
    }

    /* This was caused by programming error. */
    return SIEVE2_ERROR_FAIL;
}

VISIBLE int sieve2_setvalue_stringlist(
    sieve2_context_t *c,
    const char * const name, char ** const value)
{
    int i;

    if (!name || !value)
        return SIEVE2_ERROR_FAIL;

    for (i = 0; i < MAX_VALUES; i++) {
        if (c->cur_call.values[i].name == NULL) {
            c->cur_call.values[i].name = libsieve_strdup(name);
            c->cur_call.values[i].type = VAL_STRINGLIST;
            c->cur_call.values[i].value.sl = value;
            return SIEVE2_OK;
        }
    }

    /* This was caused by programming error. */
    return SIEVE2_ERROR_FAIL;
}

VISIBLE int sieve2_setvalue_int(
    sieve2_context_t *c,
    const char * const name, const int value)
{
    int i;

    if (!name)
        return SIEVE2_ERROR_FAIL;

    for (i = 0; i < MAX_VALUES; i++) {
        if (c->cur_call.values[i].name == NULL) {
            c->cur_call.values[i].name = libsieve_strdup(name);
            c->cur_call.values[i].type = VAL_INT;
            c->cur_call.values[i].value.i = value;
            return SIEVE2_OK;
        }
    }

    /* This was caused by programming error. */
    return SIEVE2_ERROR_FAIL;
}

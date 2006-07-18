/* comparator.c -- comparator functions
 * Larry Greenfield
 * $Id$
 */
/***********************************************************
        Copyright 1999 by Carnegie Mellon University

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Carnegie Mellon
University not be used in advertising or publicity pertaining to
distribution of the software without specific, written prior
permission.

CARNEGIE MELLON UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO
THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS, IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR
ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
******************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "comparator.h"
#include "tree.h"
#include "sieve.h"

/* --- i;octet comparators --- */

/* just compare the two; these should be NULL terminated */
static int octet_is(const char *pat, const char *text)
{
    size_t sl;
    sl = strlen(pat);

    return (sl == strlen(text)) && !memcmp(pat, text, sl);
}

/* we do a brute force attack */
static int octet_contains(const char *pat, const char *text)
{
    return (strstr(text, pat) != NULL);
}

static int octet_matches_(const char *pat, const char *text, int casemap)
{
    const char *p;
    const char *t;
    char c;

    t = text;
    p = pat;
    for (;;) {
	if (*p == '\0') {
	    /* ran out of pattern */
	    return (*t == '\0');
	}
	c = *p++;
	switch (c) {
	case '?':
	    if (*t == '\0') {
		return 0;
	    }
	    t++;
	    break;
	case '*':
	    while (*p == '*' || *p == '?') {
		if (*p == '?') {
		    /* eat the character now */
		    if (*t == '\0') {
			return 0;
		    }
		    t++;
		}
		/* coalesce into a single wildcard */
		p++;
	    }
	    if (*p == '\0') {
		/* wildcard at end of string, any remaining text is ok */
		return 1;
	    }

	    while (*t != '\0') {
		/* recurse */
		if (octet_matches_(p, t, casemap)) return 1;
		t++;
	    }
	case '\\':
	    p++;
	    /* falls through */
	default:
	    if (casemap && (toupper((int)(unsigned char)c) ==
			    toupper((int)(unsigned char)*t))) {
		t++;
	    } else if (!casemap && (c == *t)) {
		t++;
	    } else {
		/* literal char doesn't match */
		return 0;
	    }
	}
    }
    /* never reaches */
    abort();
}

static int octet_matches(const char *pat, const char *text)
{
    return octet_matches_(pat, text, 0);
}

static int octet_regex(const char *pat, const char *text)
{
    return (!libsieve_regexec((const regex_t *)pat, text, 0, NULL, 0));
}


/* --- i;ascii-casemap comparators --- */

static int ascii_casemap_is(const char *pat, const char *text)
{
    size_t sl;
    sl = strlen(pat);

    return (sl == strlen(text)) && !strncasecmp(pat, text, sl);
}

/* sheer brute force */
static int ascii_casemap_contains(const char *pat, const char *text)
{
    int N, M, i, j;

    N = strlen(text);
    M = strlen(pat);
    i = 0;
    j = 0;
    while ((j < M) && (i < N)) {
	if (toupper((int)(unsigned char)text[i]) ==
	    toupper((int)(unsigned char)pat[j])) {
	    i++; j++;
	} else {
	    i = i - j + 1;
	    j = 0;
	}
    }
    return (j == M); /* we found a match! */
}

static int ascii_casemap_matches(const char *pat, const char *text)
{
    return octet_matches_(pat, text, 1);
}

/* i;ascii-numeric; only supports "is"
 equality: numerically equal, or both not numbers */
static int ascii_numeric_is(const char *pat, const char *text)
{
    if (isdigit((int)(unsigned char)*pat)) {
	if (isdigit((int)(unsigned char)*text)) {
	    return (atoi(pat) == atoi(text));
	} else {
	    return 0;
	}
    } else if (isdigit((int)(unsigned char)*text)) return 0;
    else return 1; /* both not digits */
}

comparator_t *libsieve_comparator_lookup(const char *comp, int mode)
{
    comparator_t *ret;

    ret = NULL;
    if (!strcmp(comp, "i;octet")) {
	switch (mode) {
	case IS:
	    ret = &octet_is;
	    break;
	case CONTAINS:
	    ret = &octet_contains;
	    break;
	case MATCHES:
	    ret = &octet_matches;
	    break;
	case REGEX:
	    ret = &octet_regex;
	    break;
	}
    } else if (!strcmp(comp, "i;ascii-casemap")) {
	switch (mode) {
	case IS:
	    ret = &ascii_casemap_is;
	    break;
	case CONTAINS:
	    ret = &ascii_casemap_contains;
	    break;
	case MATCHES:
	    ret = &ascii_casemap_matches;
	    break;
	case REGEX:
	    /* the ascii-casemap destinction is made during
	       the compilation of the regex in verify_regex() */
	    ret = &octet_regex;
	    break;
	}
    } else if (!strcmp(comp, "i;ascii-numeric")) {
	switch (mode) {
	case IS:
	    ret = &ascii_numeric_is;
	    break;
	}
    }
    return ret;
}

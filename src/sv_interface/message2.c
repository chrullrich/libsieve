/* message2.c -- Sieve 2 API Message Parsing Functions
 * Aaron Stone
 * $Id$
 */
/**********************************************************
 *      FIXME: Copyright needed                           *
 **********************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

/* sv_include */
#include "sieve2_interface.h"

/* sv_parser */
#include "headerinc.h"

/* sv_interface */
#include "message2.h"

/* sv_util */
#include "util.h"

/* These functions interact with the Sieve 2 API's 
 * message struct. In the Sieve 1 API, these are all
 * callbacks and the client program is free to define
 * the structure any which way. Here, the structure is
 * defined and the functions are internal to libSieve. */

/* Plaes the size of the entire message in sz */
int message2_getsize(sieve2_message *m, int *sz)
{
    return m->size;
    return SIEVE2_OK;
}

/* Places the contents of the header specified by chead into body */
int message2_getheader(sieve2_message *m, const char *chead, const char ***body)
{
    int c, cl;
    char *head = NULL;

    /* Make sure there's nothing in the way */
    *body = NULL;

    /* Make a non-const copy of the header */
    head = sv_strdup(chead, strlen(chead));
    if( head == NULL )
        return SIEVE2_ERROR_NOMEM;

    head = sv_strtolower(head, strlen(head));

    /* check the hash */
    cl = c = message2_hashheader(head, m->hashsize);
    while (m->hash[c] != NULL) {
        if (strcmp(head, m->hash[c]->name) == 0) {
            *body = (const char **) m->hash[c]->contents;
            break;
        }
        c++; /* try next hash bin */
        c %= m->hashsize;
        if (c == cl) break; /* gone all the way around */
    }

    sv_free(head);

    if (*body) {
        return SIEVE2_OK;
    } else {
        return SIEVE2_ERROR_FAIL;
    }
}

/* Get the envelope sender from the message struct and put it into body */
int message2_getenvelope(sieve2_message *m, const char *chead, const char ***body)
{
    (const char **)body = m->envelope;
    return SIEVE2_OK;
}

int message2_freecache(sieve2_message *m)
{
    int i;

    /* Free the header hash hash entries */
    for (i = 0; i < m->hashsize; i++) {
        if (m->hash[i]) {
            for (; m->hash[i]->space > 0; m->hash[i]->space--)
                sv_free(m->hash[i]->contents[m->hash[i]->space]);
            sv_free(m->hash[i]->contents);
            sv_free(m->hash[i]->name);
        }
        sv_free(m->hash[i]);
    }
    sv_free(m->hash);
    sv_free(m);

    return SIEVE2_OK;
}

/* This function takes the header in m->message and 
 * then uses the header parser to work at filling
 * the header hash in m->hash
 */
int message2_headercache(sieve2_message *m)
{
    size_t c, cl;
    char *err = NULL;
    header_list_t *hl = NULL, *hlfree;

    if ((hl = header_parse_buffer(&hl, &m->header, &err)) == NULL) {
        /* That's a shame, we didn't find anything, or worse! */
        return SIEVE2_ERROR_EXEC;
    }

    while(hl != NULL) {
        /* Get a hash number of the header name */
        cl = c = message2_hashheader(hl->h->name, m->hashsize);
        while (m->hash[c] != NULL && strcmp(hl->h->name, m->hash[c]->name) != 0) {
            c++;
            c %= m->hashsize;
            /* If we've skipped back to the beginning,
             * give up and we'll just realloc a larger space */
            if (c == cl) break;
        }

        if (m->hash[c]) {
            /* Looks like someone's already home */
            if(m->hash[c]->count >= m->hash[c]->space)
                m->hash[c]->contents[m->hash[c]->count++] = hl->h->contents[0];
            else {
                /* Need to make some more space in here */
                char **tmp;
                tmp = sv_realloc(m->hash[c]->contents, sizeof(char *) * (m->hash[c]->space+=8));
                if(tmp == NULL)
                    return SIEVE2_ERROR_NOMEM;
                else
                    m->hash[c]->contents = tmp;
                /* OK, now we can put that body in here */
                m->hash[c]->contents[m->hash[c]->count++] = hl->h->contents[0];
            }
        } else {
            /* Make of copy of the pointer */
            m->hash[c] = hl->h;
        }

	/* Advance the cursor and cleanup as we go */
	hlfree = hl;
        hl = hl->next;
	sv_free(hlfree);
    }

    m->hashfull = 1;
    return SIEVE2_OK;
}

static int message2_hashheader(char *header, int hashsize)
{
    int x = 0;

    /* Any char except ' ', :, or a ctrl char */
    for (; !iscntrl(*header) && (*header != ' ') && (*header != ':'); header++) {
        x *= 256;
        x += *header;
        x %= hashsize;
    }

    return x;
}

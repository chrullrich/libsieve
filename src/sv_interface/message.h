/* message.h
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

#ifndef MESSAGE_H
#define MESSAGE_H

#include "sv_interface/tree.h"		/* for stringlist_t */
#include "sv_parser/addrinc.h"		/* for struct address */

typedef enum {
    ACTION_NULL = -1,
    ACTION_NONE = 0,
    ACTION_REJECT,
    ACTION_FILEINTO,
    ACTION_KEEP,
    ACTION_REDIRECT,
    ACTION_DISCARD,
    ACTION_VACATION,
    ACTION_SETFLAG,
    ACTION_ADDFLAG,
    ACTION_REMOVEFLAG,
    ACTION_MARK,
    ACTION_NOTIFY,
} action_t;

/* header parsing */
typedef enum {
    ADDRESS_ALL,
    ADDRESS_LOCALPART,
    ADDRESS_DOMAIN,
    ADDRESS_USER,
    ADDRESS_DETAIL
} address_part_t;

int libsieve_parse_address(struct sieve2_context *context, const char *header, struct address **data, struct addr_marker **marker);
char *libsieve_get_address(struct sieve2_context *context, address_part_t addrpart, struct addr_marker **marker, int canon_domain);
int libsieve_free_address(struct address **data, struct addr_marker **marker);


#endif /* MESSAGE_H */

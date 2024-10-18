/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#ifndef HC_CPT_H
#define HC_CPT_H

#include <stdio.h>
#include <errno.h>
#include <time.h>

int  cpt_ctx_init    (supercrack_ctx_t *supercrack_ctx);
void cpt_ctx_destroy (supercrack_ctx_t *supercrack_ctx);
void cpt_ctx_reset   (supercrack_ctx_t *supercrack_ctx);

#endif // HC_CPT_H

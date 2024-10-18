/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#ifndef HC_STRAIGHT_H
#define HC_STRAIGHT_H

#include <string.h>

#define INCR_DICTS 1000

int  straight_ctx_update_loop (supercrack_ctx_t *supercrack_ctx);
int  straight_ctx_init        (supercrack_ctx_t *supercrack_ctx);
void straight_ctx_destroy     (supercrack_ctx_t *supercrack_ctx);

#endif // HC_STRAIGHT_H

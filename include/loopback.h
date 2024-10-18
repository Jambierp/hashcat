/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#ifndef HC_LOOPBACK_H
#define HC_LOOPBACK_H

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

static const char LOOPBACK_FILE[] = "supercrack.loopback";

int  loopback_init          (supercrack_ctx_t *supercrack_ctx);
void loopback_destroy       (supercrack_ctx_t *supercrack_ctx);
int  loopback_write_open    (supercrack_ctx_t *supercrack_ctx);
void loopback_write_close   (supercrack_ctx_t *supercrack_ctx);
void loopback_write_append  (supercrack_ctx_t *supercrack_ctx, const u8 *plain_ptr, const unsigned int plain_len);
void loopback_write_unlink  (supercrack_ctx_t *supercrack_ctx);

#endif // HC_LOOPBACK_H

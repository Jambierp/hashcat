/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#ifndef HC_DICTSTAT_H
#define HC_DICTSTAT_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <search.h>

#define MAX_DICTSTAT 100000

#define DICTSTAT_FILENAME "supercrack.dictstat2"
#define DICTSTAT_VERSION  (0x6863646963743200 | 0x03)

int sort_by_dictstat (const void *s1, const void *s2);

int  dictstat_init    (supercrack_ctx_t *supercrack_ctx);
void dictstat_destroy (supercrack_ctx_t *supercrack_ctx);
void dictstat_read    (supercrack_ctx_t *supercrack_ctx);
int  dictstat_write   (supercrack_ctx_t *supercrack_ctx);
u64  dictstat_find    (supercrack_ctx_t *supercrack_ctx, dictstat_t *d);
void dictstat_append  (supercrack_ctx_t *supercrack_ctx, dictstat_t *d);

#endif // HC_DICTSTAT_H

/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#ifndef HC_HLFMT_H
#define HC_HLFMT_H

#include <stdio.h>

#define HLFMTS_CNT 11

const char *strhlfmt (const u32 hashfile_format);

void hlfmt_hash (supercrack_ctx_t *supercrack_ctx, u32 hashfile_format, char *line_buf, const int line_len, char **hashbuf_pos, int *hashbuf_len);
void hlfmt_user (supercrack_ctx_t *supercrack_ctx, u32 hashfile_format, char *line_buf, const int line_len, char **userbuf_pos, int *userbuf_len);

u32 hlfmt_detect (supercrack_ctx_t *supercrack_ctx, HCFILE *fp, u32 max_check);

#endif // HC_HLFMT_H

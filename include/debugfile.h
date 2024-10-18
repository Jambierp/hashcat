/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#ifndef HC_DEBUGFILE_H
#define HC_DEBUGFILE_H

#include <stdio.h>

int  debugfile_init         (supercrack_ctx_t *supercrack_ctx);
void debugfile_destroy      (supercrack_ctx_t *supercrack_ctx);
void debugfile_write_append (supercrack_ctx_t *supercrack_ctx, const u8 *rule_buf, const u32 rule_len, const u8 *mod_plain_ptr, const u32 mod_plain_len, const u8 *orig_plain_ptr, const u32 orig_plain_len);

#endif // HC_DEBUGFILE_H

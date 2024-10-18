/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#ifndef HC_OUTFILE_H
#define HC_OUTFILE_H

#include <stdio.h>
#include <time.h>
#include <inttypes.h>

int  build_plain     (supercrack_ctx_t *supercrack_ctx, hc_device_param_t *device_param, plain_t *plain, u32 *plain_buf, int *out_len);
int  build_crackpos  (supercrack_ctx_t *supercrack_ctx, hc_device_param_t *device_param, plain_t *plain, u64 *out_pos);
int  build_debugdata (supercrack_ctx_t *supercrack_ctx, hc_device_param_t *device_param, plain_t *plain, u8 *debug_rule_buf, int *debug_rule_len, u8 *debug_plain_ptr, int *debug_plain_len);

u32 outfile_format_parse (const char *format_string);

int  outfile_init           (supercrack_ctx_t *supercrack_ctx);
void outfile_destroy        (supercrack_ctx_t *supercrack_ctx);
int  outfile_write_open     (supercrack_ctx_t *supercrack_ctx);
void outfile_write_close    (supercrack_ctx_t *supercrack_ctx);
int  outfile_write          (supercrack_ctx_t *supercrack_ctx, const char *out_buf, const int out_len, const unsigned char *plain_ptr, const u32 plain_len, const u64 crackpos, const unsigned char *username, const u32 user_len, const bool print_eol, char *tmp_buf);

#endif // HC_OUTFILE_H

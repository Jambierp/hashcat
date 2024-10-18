/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#ifndef HC_USER_OPTIONS_H
#define HC_USER_OPTIONS_H

#include <getopt.h>

int user_options_init (supercrack_ctx_t *supercrack_ctx);

void user_options_destroy (supercrack_ctx_t *supercrack_ctx);

int user_options_getopt (supercrack_ctx_t *supercrack_ctx, int argc, char **argv);

int user_options_sanity (supercrack_ctx_t *supercrack_ctx);

void user_options_session_auto (supercrack_ctx_t *supercrack_ctx);

void user_options_preprocess (supercrack_ctx_t *supercrack_ctx);

void user_options_postprocess (supercrack_ctx_t *supercrack_ctx);

void user_options_extra_init (supercrack_ctx_t *supercrack_ctx);

void user_options_extra_destroy (supercrack_ctx_t *supercrack_ctx);

u64 user_options_extra_amplifier (supercrack_ctx_t *supercrack_ctx);

void user_options_logger (supercrack_ctx_t *supercrack_ctx);

int user_options_check_files (supercrack_ctx_t *supercrack_ctx);

void user_options_info (supercrack_ctx_t *supercrack_ctx);

#endif // HC_USER_OPTIONS_H

/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#ifndef HC_PIDFILE_H
#define HC_PIDFILE_H

#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#if defined (_WIN)
#include <windows.h>
#include <psapi.h>
#endif // _WIN

int pidfile_ctx_init (supercrack_ctx_t *supercrack_ctx);

void pidfile_ctx_destroy (supercrack_ctx_t *supercrack_ctx);

#endif // HC_PIDFILE_H

/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#ifndef HC_TERMINAL_H
#define HC_TERMINAL_H

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#if defined (_WIN)
#include <windows.h>
#else
#include <termios.h>
#if defined (__APPLE__)
#include <sys/ioctl.h>
#endif // __APPLE__
#endif // _WIN

#if !defined (_WIN) && !defined (__CYGWIN__) && !defined (__MSYS__)
#include <sys/utsname.h>
#if !defined (__linux__)
#include <sys/sysctl.h>
#endif // ! __linux__
#endif // ! _WIN && | __CYGWIN__ && ! __MSYS__

void welcome_screen (supercrack_ctx_t *supercrack_ctx, const char *version_tag);
void goodbye_screen (supercrack_ctx_t *supercrack_ctx, const time_t proc_start, const time_t proc_stop);

int setup_console (void);

void send_prompt  (supercrack_ctx_t *supercrack_ctx);
void clear_prompt (supercrack_ctx_t *supercrack_ctx);

HC_API_CALL void *thread_keypress (void *p);

#if defined (_WIN)
void SetConsoleWindowSize (const int x);
#endif

int tty_break (void);
int tty_getchar (void);
int tty_fix (void);

bool is_stdout_terminal (void);

void compress_terminal_line_length (char *out_buf, const size_t keep_from_beginning, const size_t keep_from_end);

void hash_info                          (supercrack_ctx_t *supercrack_ctx);

void backend_info                       (supercrack_ctx_t *supercrack_ctx);
void backend_info_compact               (supercrack_ctx_t *supercrack_ctx);

void status_progress_machine_readable   (supercrack_ctx_t *supercrack_ctx);
void status_progress                    (supercrack_ctx_t *supercrack_ctx);
void status_speed_machine_readable      (supercrack_ctx_t *supercrack_ctx);
void status_speed                       (supercrack_ctx_t *supercrack_ctx);
void status_display_machine_readable    (supercrack_ctx_t *supercrack_ctx);
void status_display                     (supercrack_ctx_t *supercrack_ctx);
void status_benchmark_machine_readable  (supercrack_ctx_t *supercrack_ctx);
void status_benchmark                   (supercrack_ctx_t *supercrack_ctx);

#endif // HC_TERMINAL_H

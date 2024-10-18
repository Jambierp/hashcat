/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#ifndef HC_EVENT_H
#define HC_EVENT_H

#include <stdio.h>
#include <stdarg.h>

void event_call (const u32 id, supercrack_ctx_t *supercrack_ctx, const void *buf, const size_t len);

#define EVENT(id)              event_call ((id), supercrack_ctx, NULL,  0)
#define EVENT_DATA(id,buf,len) event_call ((id), supercrack_ctx, (buf), (len))

__attribute__ ((format (printf, 2, 3))) size_t event_log_advice_nn  (supercrack_ctx_t *supercrack_ctx, const char *fmt, ...);
__attribute__ ((format (printf, 2, 3))) size_t event_log_info_nn    (supercrack_ctx_t *supercrack_ctx, const char *fmt, ...);
__attribute__ ((format (printf, 2, 3))) size_t event_log_warning_nn (supercrack_ctx_t *supercrack_ctx, const char *fmt, ...);
__attribute__ ((format (printf, 2, 3))) size_t event_log_error_nn   (supercrack_ctx_t *supercrack_ctx, const char *fmt, ...);

__attribute__ ((format (printf, 2, 3))) size_t event_log_advice     (supercrack_ctx_t *supercrack_ctx, const char *fmt, ...);
__attribute__ ((format (printf, 2, 3))) size_t event_log_info       (supercrack_ctx_t *supercrack_ctx, const char *fmt, ...);
__attribute__ ((format (printf, 2, 3))) size_t event_log_warning    (supercrack_ctx_t *supercrack_ctx, const char *fmt, ...);
__attribute__ ((format (printf, 2, 3))) size_t event_log_error      (supercrack_ctx_t *supercrack_ctx, const char *fmt, ...);

int  event_ctx_init         (supercrack_ctx_t *supercrack_ctx);
void event_ctx_destroy      (supercrack_ctx_t *supercrack_ctx);

#endif // HC_EVENT_H

/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#include "common.h"
#include "types.h"
#include "thread.h"
#include "event.h"

void event_call (const u32 id, supercrack_ctx_t *supercrack_ctx, const void *buf, const size_t len)
{
  event_ctx_t *event_ctx = supercrack_ctx->event_ctx;

  bool is_log = false;

  switch (id)
  {
    case EVENT_LOG_INFO:    is_log = true; break;
    case EVENT_LOG_WARNING: is_log = true; break;
    case EVENT_LOG_ERROR:   is_log = true; break;
    case EVENT_LOG_ADVICE:  is_log = true; break;
  }

  if (is_log == false)
  {
    hc_thread_mutex_lock (event_ctx->mux_event);
  }

  supercrack_ctx->event (id, supercrack_ctx, buf, len);

  if (is_log == false)
  {
    hc_thread_mutex_unlock (event_ctx->mux_event);
  }

  // add more back logs in case user wants to access them

  if (is_log == false)
  {
    for (int i = MAX_OLD_EVENTS - 1; i >= 1; i--)
    {
      memcpy (event_ctx->old_buf[i], event_ctx->old_buf[i - 1], event_ctx->old_len[i - 1]);

      event_ctx->old_len[i] = event_ctx->old_len[i - 1];
    }

    size_t copy_len = 0;

    if (buf)
    {
      // truncate the whole buffer if needed (such that it fits into the old_buf):

      const size_t max_buf_len = sizeof (event_ctx->old_buf[0]);

      copy_len = MIN (len, max_buf_len - 1);

      memcpy (event_ctx->old_buf[0], buf, copy_len);
    }

    event_ctx->old_len[0] = copy_len;
  }
}

__attribute__ ((format (printf, 1, 0)))
static int event_log (const char *fmt, va_list ap, char *s, const size_t sz)
{
  size_t length;

  length = vsnprintf (s, sz, fmt, ap);
  length = MIN (length, sz);

  s[length] = 0;

  return (int) length;
}

size_t event_log_advice_nn (supercrack_ctx_t *supercrack_ctx, const char *fmt, ...)
{
  event_ctx_t *event_ctx = supercrack_ctx->event_ctx;

  if (fmt == NULL)
  {
    event_ctx->msg_buf[0] = 0;

    event_ctx->msg_len = 0;
  }
  else
  {
    va_list ap;

    va_start (ap, fmt);

    event_ctx->msg_len = event_log (fmt, ap, event_ctx->msg_buf, HCBUFSIZ_SMALL - 1);

    va_end (ap);
  }

  event_ctx->msg_newline = false;

  event_call (EVENT_LOG_ADVICE, supercrack_ctx, NULL, 0);

  return event_ctx->msg_len;
}

size_t event_log_info_nn (supercrack_ctx_t *supercrack_ctx, const char *fmt, ...)
{
  event_ctx_t *event_ctx = supercrack_ctx->event_ctx;

  if (fmt == NULL)
  {
    event_ctx->msg_buf[0] = 0;

    event_ctx->msg_len = 0;
  }
  else
  {
    va_list ap;

    va_start (ap, fmt);

    event_ctx->msg_len = event_log (fmt, ap, event_ctx->msg_buf, HCBUFSIZ_SMALL - 1);

    va_end (ap);
  }

  event_ctx->msg_newline = false;

  event_call (EVENT_LOG_INFO, supercrack_ctx, NULL, 0);

  return event_ctx->msg_len;
}

size_t event_log_warning_nn (supercrack_ctx_t *supercrack_ctx, const char *fmt, ...)
{
  event_ctx_t *event_ctx = supercrack_ctx->event_ctx;

  if (fmt == NULL)
  {
    event_ctx->msg_buf[0] = 0;

    event_ctx->msg_len = 0;
  }
  else
  {
    va_list ap;

    va_start (ap, fmt);

    event_ctx->msg_len = event_log (fmt, ap, event_ctx->msg_buf, HCBUFSIZ_SMALL - 1);

    va_end (ap);
  }

  event_ctx->msg_newline = false;

  event_call (EVENT_LOG_WARNING, supercrack_ctx, NULL, 0);

  return event_ctx->msg_len;
}

size_t event_log_error_nn (supercrack_ctx_t *supercrack_ctx, const char *fmt, ...)
{
  event_ctx_t *event_ctx = supercrack_ctx->event_ctx;

  if (fmt == NULL)
  {
    event_ctx->msg_buf[0] = 0;

    event_ctx->msg_len = 0;
  }
  else
  {
    va_list ap;

    va_start (ap, fmt);

    event_ctx->msg_len = event_log (fmt, ap, event_ctx->msg_buf, HCBUFSIZ_SMALL - 1);

    va_end (ap);
  }

  event_ctx->msg_newline = false;

  event_call (EVENT_LOG_ERROR, supercrack_ctx, NULL, 0);

  return event_ctx->msg_len;
}

size_t event_log_advice (supercrack_ctx_t *supercrack_ctx, const char *fmt, ...)
{
  event_ctx_t *event_ctx = supercrack_ctx->event_ctx;

  if (fmt == NULL)
  {
    event_ctx->msg_buf[0] = 0;

    event_ctx->msg_len = 0;
  }
  else
  {
    va_list ap;

    va_start (ap, fmt);

    event_ctx->msg_len = event_log (fmt, ap, event_ctx->msg_buf, HCBUFSIZ_SMALL - 1);

    va_end (ap);
  }

  event_ctx->msg_newline = true;

  event_call (EVENT_LOG_ADVICE, supercrack_ctx, NULL, 0);

  return event_ctx->msg_len;
}

size_t event_log_info (supercrack_ctx_t *supercrack_ctx, const char *fmt, ...)
{
  event_ctx_t *event_ctx = supercrack_ctx->event_ctx;

  if (fmt == NULL)
  {
    event_ctx->msg_buf[0] = 0;

    event_ctx->msg_len = 0;
  }
  else
  {
    va_list ap;

    va_start (ap, fmt);

    event_ctx->msg_len = event_log (fmt, ap, event_ctx->msg_buf, HCBUFSIZ_LARGE - 1);

    va_end (ap);
  }

  event_ctx->msg_newline = true;

  event_call (EVENT_LOG_INFO, supercrack_ctx, NULL, 0);

  return event_ctx->msg_len;
}

size_t event_log_warning (supercrack_ctx_t *supercrack_ctx, const char *fmt, ...)
{
  event_ctx_t *event_ctx = supercrack_ctx->event_ctx;

  if (fmt == NULL)
  {
    event_ctx->msg_buf[0] = 0;

    event_ctx->msg_len = 0;
  }
  else
  {
    va_list ap;

    va_start (ap, fmt);

    event_ctx->msg_len = event_log (fmt, ap, event_ctx->msg_buf, HCBUFSIZ_SMALL - 1);

    va_end (ap);
  }

  event_ctx->msg_newline = true;

  event_call (EVENT_LOG_WARNING, supercrack_ctx, NULL, 0);

  return event_ctx->msg_len;
}

size_t event_log_error (supercrack_ctx_t *supercrack_ctx, const char *fmt, ...)
{
  event_ctx_t *event_ctx = supercrack_ctx->event_ctx;

  if (fmt == NULL)
  {
    event_ctx->msg_buf[0] = 0;

    event_ctx->msg_len = 0;
  }
  else
  {
    va_list ap;

    va_start (ap, fmt);

    event_ctx->msg_len = event_log (fmt, ap, event_ctx->msg_buf, HCBUFSIZ_SMALL - 1);

    va_end (ap);
  }

  event_ctx->msg_newline = true;

  event_call (EVENT_LOG_ERROR, supercrack_ctx, NULL, 0);

  return event_ctx->msg_len;
}

int event_ctx_init (supercrack_ctx_t *supercrack_ctx)
{
  event_ctx_t *event_ctx = supercrack_ctx->event_ctx;

  memset (event_ctx, 0, sizeof (event_ctx_t));

  hc_thread_mutex_init (event_ctx->mux_event);

  return 0;
}

void event_ctx_destroy (supercrack_ctx_t *supercrack_ctx)
{
  event_ctx_t *event_ctx = supercrack_ctx->event_ctx;

  hc_thread_mutex_delete (event_ctx->mux_event);
}

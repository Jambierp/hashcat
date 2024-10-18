/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "types.h"
#include "user_options.h"
#include "usage.h"
#include "memory.h"
#include "supercrack.h"
#include "terminal.h"
#include "thread.h"
#include "status.h"
#include "shared.h"
#include "event.h"

#ifdef WITH_BRAIN
#include "brain.h"
#endif

#if defined (__MINGW64__) || defined (__MINGW32__)
int _dowildcard = -1;
#endif

static void main_log_clear_line (MAYBE_UNUSED const size_t prev_len, MAYBE_UNUSED FILE *fp)
{
  if (!is_stdout_terminal ()) return;

  #if defined (_WIN)

  fputc ('\r', fp);

  for (size_t i = 0; i < prev_len; i++) fputc (' ', fp);

  fputc ('\r', fp);

  #else

  fputs ("\033[2K\r", fp);

  #endif
}

static void main_log (supercrack_ctx_t *supercrack_ctx, FILE *fp, const int loglevel)
{
  event_ctx_t *event_ctx = supercrack_ctx->event_ctx;

  const char  *msg_buf     = event_ctx->msg_buf;
  const size_t msg_len     = event_ctx->msg_len;
  const bool   msg_newline = event_ctx->msg_newline;

  // handle last_len

  const size_t prev_len = event_ctx->prev_len;

  if (prev_len) main_log_clear_line (prev_len, fp);

  if (msg_newline == true)
  {
    event_ctx->prev_len = 0;
  }
  else
  {
    event_ctx->prev_len = msg_len;
  }

  #if defined (_WIN)
  HANDLE hConsole = GetStdHandle (STD_OUTPUT_HANDLE);

  CONSOLE_SCREEN_BUFFER_INFO con_info;

  GetConsoleScreenBufferInfo (hConsole, &con_info);

  const int orig = con_info.wAttributes;
  #endif

  // color stuff pre
  if (is_stdout_terminal ())
  {
  #if defined (_WIN)
    switch (loglevel)
    {
      case LOGLEVEL_INFO:
        break;
      case LOGLEVEL_WARNING: SetConsoleTextAttribute (hConsole, 6);
        break;
      case LOGLEVEL_ERROR:   SetConsoleTextAttribute (hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
        break;
      case LOGLEVEL_ADVICE:  SetConsoleTextAttribute (hConsole, 6);
        break;
    }

  #else
    switch (loglevel)
    {
      case LOGLEVEL_INFO:                                   break;
      case LOGLEVEL_WARNING: fwrite ("\033[33m", 5, 1, fp); break;
      case LOGLEVEL_ERROR:   fwrite ("\033[31m", 5, 1, fp); break;
      case LOGLEVEL_ADVICE:  fwrite ("\033[33m", 5, 1, fp); break;
    }
  #endif
  }

  // finally, print

  fwrite (msg_buf, msg_len, 1, fp);

  // color stuff post
  if (is_stdout_terminal ())
  {
  #if defined (_WIN)
    switch (loglevel)
    {
      case LOGLEVEL_INFO:                                              break;
      case LOGLEVEL_WARNING: SetConsoleTextAttribute (hConsole, orig); break;
      case LOGLEVEL_ERROR:   SetConsoleTextAttribute (hConsole, orig); break;
      case LOGLEVEL_ADVICE:  SetConsoleTextAttribute (hConsole, orig); break;
    }
  #else
    switch (loglevel)
    {
      case LOGLEVEL_INFO:                                  break;
      case LOGLEVEL_WARNING: fwrite ("\033[0m", 4, 1, fp); break;
      case LOGLEVEL_ERROR:   fwrite ("\033[0m", 4, 1, fp); break;
      case LOGLEVEL_ADVICE:  fwrite ("\033[0m", 4, 1, fp); break;
    }
  #endif
  }

  // eventual newline

  if (msg_newline == true)
  {
    fwrite (EOL, strlen (EOL), 1, fp);

    // on error, add another newline

    if (loglevel == LOGLEVEL_ERROR)
    {
      fwrite (EOL, strlen (EOL), 1, fp);
    }
  }

  fflush (fp);
}

static void main_log_advice (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->advice == false) return;

  main_log (supercrack_ctx, stdout, LOGLEVEL_ADVICE);
}

static void main_log_info (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  main_log (supercrack_ctx, stdout, LOGLEVEL_INFO);
}

static void main_log_warning (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  main_log (supercrack_ctx, stdout, LOGLEVEL_WARNING);
}

static void main_log_error (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  main_log (supercrack_ctx, stderr, LOGLEVEL_ERROR);
}

static void main_outerloop_starting (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t       *user_options       = supercrack_ctx->user_options;
  const user_options_extra_t *user_options_extra = supercrack_ctx->user_options_extra;

  supercrack_user_t *supercrack_user = supercrack_ctx->supercrack_user;
  status_ctx_t   *status_ctx   = supercrack_ctx->status_ctx;

  /**
   * keypress thread
   */

  supercrack_user->outer_threads_cnt = 0;

  supercrack_user->outer_threads = (hc_thread_t *) hccalloc (2, sizeof (hc_thread_t)); if (supercrack_user->outer_threads == NULL) return;

  status_ctx->shutdown_outer = false;

  if (user_options->hash_info    == true) return;
  if (user_options->keyspace     == true) return;
  if (user_options->stdout_flag  == true) return;
  if (user_options->speed_only   == true) return;
  if (user_options->identify     == true) return;
  if (user_options->backend_info  > 0)    return;

  if ((user_options_extra->wordlist_mode == WL_MODE_FILE) || (user_options_extra->wordlist_mode == WL_MODE_MASK))
  {
    // see thread_keypress() how to access status information

    hc_thread_create (supercrack_user->outer_threads[supercrack_user->outer_threads_cnt], thread_keypress, supercrack_ctx);

    supercrack_user->outer_threads_cnt++;
  }
}

static void main_outerloop_finished (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  supercrack_user_t *supercrack_user = supercrack_ctx->supercrack_user;
  status_ctx_t   *status_ctx   = supercrack_ctx->status_ctx;

  // we should never stop supercrack with STATUS_INIT:
  // keypress thread blocks on STATUS_INIT forever!

  if (status_ctx->devices_status == STATUS_INIT)
  {
    status_ctx->devices_status = STATUS_ERROR;
  }

  // wait for outer threads

  status_ctx->shutdown_outer = true;

  for (int thread_idx = 0; thread_idx < supercrack_user->outer_threads_cnt; thread_idx++)
  {
    hc_thread_wait (1, &supercrack_user->outer_threads[thread_idx]);
  }

  hcfree (supercrack_user->outer_threads);

  supercrack_user->outer_threads_cnt = 0;
}

static void main_cracker_starting (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t       *user_options       = supercrack_ctx->user_options;
  const user_options_extra_t *user_options_extra = supercrack_ctx->user_options_extra;

  if (user_options->quiet == true) return;

  // Tell the user we're about to start

  if ((user_options_extra->wordlist_mode == WL_MODE_FILE) || (user_options_extra->wordlist_mode == WL_MODE_MASK))
  {
    if ((user_options->quiet == false) && (user_options->speed_only == false))
    {
      event_log_info_nn (supercrack_ctx, NULL);

      clear_prompt (supercrack_ctx);

      send_prompt (supercrack_ctx);
    }
  }
  else if (user_options_extra->wordlist_mode == WL_MODE_STDIN)
  {
    event_log_info (supercrack_ctx, "Starting attack in stdin mode");
    event_log_info (supercrack_ctx, NULL);
  }
}

static void main_cracker_finished (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const hashes_t             *hashes             = supercrack_ctx->hashes;
  const user_options_t       *user_options       = supercrack_ctx->user_options;
  const user_options_extra_t *user_options_extra = supercrack_ctx->user_options_extra;

  if (user_options->hash_info    == true) return;
  if (user_options->keyspace     == true) return;
  if (user_options->stdout_flag  == true) return;
  if (user_options->backend_info  > 0)    return;

  // if we had a prompt, clear it

  if ((user_options_extra->wordlist_mode == WL_MODE_FILE) || (user_options_extra->wordlist_mode == WL_MODE_MASK))
  {
    if ((user_options->speed_only == false) && (user_options->quiet == false))
    {
      clear_prompt (supercrack_ctx);
    }
  }

  // print final status

  if (user_options->benchmark == true)
  {
    status_benchmark (supercrack_ctx);

    if (user_options->machine_readable == false)
    {
      event_log_info (supercrack_ctx, NULL);
    }
  }
  else if (user_options->progress_only == true)
  {
    status_progress (supercrack_ctx);

    if (user_options->machine_readable == false)
    {
      event_log_info (supercrack_ctx, NULL);
    }
  }
  else if (user_options->speed_only == true)
  {
    status_speed (supercrack_ctx);

    if (user_options->machine_readable == false)
    {
      event_log_info (supercrack_ctx, NULL);
    }
  }
  else if (user_options->machine_readable == true)
  {
    status_display (supercrack_ctx);
  }
  else if (user_options->status == true)
  {
    status_display (supercrack_ctx);
  }
  else if (user_options->status_json == true)
  {
    status_display (supercrack_ctx);
  }
  else
  {
    if (user_options->quiet == false)
    {
      if (hashes->digests_saved != hashes->digests_done) event_log_info (supercrack_ctx, NULL);

      status_display (supercrack_ctx);

      event_log_info (supercrack_ctx, NULL);
    }
  }
}

static void main_cracker_hash_cracked (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  outfile_ctx_t         *outfile_ctx        = supercrack_ctx->outfile_ctx;
  status_ctx_t          *status_ctx         = supercrack_ctx->status_ctx;
  user_options_t        *user_options       = supercrack_ctx->user_options;
  user_options_extra_t  *user_options_extra = supercrack_ctx->user_options_extra;

  if (outfile_ctx->fp.pfp != NULL) return; // cracked hash was not written to an outfile

  if ((user_options_extra->wordlist_mode == WL_MODE_FILE) || (user_options_extra->wordlist_mode == WL_MODE_MASK))
  {
    if (outfile_ctx->filename == NULL) if (user_options->quiet == false) clear_prompt (supercrack_ctx);
  }

  fwrite (buf, len,          1, stdout);
  fwrite (EOL, strlen (EOL), 1, stdout);

  if ((user_options_extra->wordlist_mode == WL_MODE_FILE) || (user_options_extra->wordlist_mode == WL_MODE_MASK))
  {
    if (status_ctx->devices_status != STATUS_CRACKED)
    {
      if (outfile_ctx->filename == NULL) if (user_options->quiet == false) send_prompt (supercrack_ctx);
    }
  }
}

static void main_calculated_words_base (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const status_ctx_t   *status_ctx   = supercrack_ctx->status_ctx;
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->keyspace == false) return;

  event_log_info (supercrack_ctx, "%" PRIu64 "", status_ctx->words_base);
}

static void main_potfile_remove_parse_pre (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_info_nn (supercrack_ctx, "Comparing hashes with potfile entries. Please be patient...");
}

static void main_potfile_remove_parse_post (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_info_nn (supercrack_ctx, "Compared hashes with potfile entries");
}

static void main_rulesfiles_parse_pre (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_info_nn (supercrack_ctx, "Loading rules. Please be patient...");
}

static void main_rulesfiles_parse_post (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_info_nn (supercrack_ctx, "Loading rules finished");
}

static void main_potfile_hash_show (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  outfile_ctx_t *outfile_ctx = supercrack_ctx->outfile_ctx;

  if (outfile_ctx->fp.pfp != NULL) return; // cracked hash was not written to an outfile

  fwrite (buf, len,          1, stdout);
  fwrite (EOL, strlen (EOL), 1, stdout);
}

static void main_potfile_hash_left (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  outfile_ctx_t *outfile_ctx = supercrack_ctx->outfile_ctx;

  if (outfile_ctx->fp.pfp != NULL) return; // cracked hash was not written to an outfile

  fwrite (buf, len, 1, stdout);
}

static void main_potfile_num_cracked (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;
  hashes_t             *hashes       = supercrack_ctx->hashes;

  if (user_options->quiet == true) return;

  if (hashes->digests_done_zero == 1)
  {
    event_log_info (supercrack_ctx, "INFO: Removed hash found as empty hash.");
    event_log_info (supercrack_ctx, NULL);
  }
  else if (hashes->digests_done_zero > 1)
  {
    event_log_info (supercrack_ctx, "INFO: Removed %d hashes found as empty hashes.", hashes->digests_done_zero);
    event_log_info (supercrack_ctx, NULL);
  }

  if (hashes->digests_done_pot == 1)
  {
    event_log_info (supercrack_ctx, "INFO: Removed hash found as potfile entry.");
    event_log_info (supercrack_ctx, NULL);
  }
  else if (hashes->digests_done_pot > 1)
  {
    event_log_info (supercrack_ctx, "INFO: Removed %d hashes found as potfile entries.", hashes->digests_done_pot);
    event_log_info (supercrack_ctx, NULL);
  }
}

static void main_potfile_all_cracked (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_info (supercrack_ctx, "INFO: All hashes found as potfile and/or empty entries! Use --show to display them.");
  event_log_info (supercrack_ctx, NULL);
}

static void main_outerloop_mainscreen (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const bitmap_ctx_t   *bitmap_ctx   = supercrack_ctx->bitmap_ctx;
  const hashconfig_t   *hashconfig   = supercrack_ctx->hashconfig;
  const hashes_t       *hashes       = supercrack_ctx->hashes;
  const hwmon_ctx_t    *hwmon_ctx    = supercrack_ctx->hwmon_ctx;
  const straight_ctx_t *straight_ctx = supercrack_ctx->straight_ctx;
  const user_options_t *user_options = supercrack_ctx->user_options;

  /**
   * In benchmark-mode, inform user which algorithm is checked
   */

  if (user_options->benchmark == true)
  {
    if (user_options->machine_readable == false)
    {
      char buf[HCBUFSIZ_TINY] = { 0 };

      size_t len = 0;

      if ((hashconfig->attack_exec == ATTACK_EXEC_OUTSIDE_KERNEL) && (hashconfig->is_salted == true))
      {
        len = snprintf (buf, sizeof (buf), "* Hash-Mode %d (%s) [Iterations: %d]", hashconfig->hash_mode, hashconfig->hash_name, hashes[0].salts_buf[0].salt_iter);
      }
      else
      {
        len = snprintf (buf, sizeof (buf), "* Hash-Mode %d (%s)", hashconfig->hash_mode, hashconfig->hash_name);
      }

      char line[HCBUFSIZ_TINY] = { 0 };

      memset (line, '-', len);

      line[len] = 0;

      event_log_info (supercrack_ctx, "%s", line);
      event_log_info (supercrack_ctx, "%s", buf);
      event_log_info (supercrack_ctx, "%s", line);
      event_log_info (supercrack_ctx, NULL);
    }
  }

  if (user_options->quiet == true) return;

  event_log_info (supercrack_ctx, "Hashes: %u digests; %u unique digests, %u unique salts", hashes->hashes_cnt_orig, hashes->digests_cnt, hashes->salts_cnt);
  event_log_info (supercrack_ctx, "Bitmaps: %u bits, %u entries, 0x%08x mask, %u bytes, %u/%u rotates", bitmap_ctx->bitmap_bits, bitmap_ctx->bitmap_nums, bitmap_ctx->bitmap_mask, bitmap_ctx->bitmap_size, bitmap_ctx->bitmap_shift1, bitmap_ctx->bitmap_shift2);

  if ((user_options->attack_mode == ATTACK_MODE_STRAIGHT) || (user_options->attack_mode == ATTACK_MODE_ASSOCIATION))
  {
    event_log_info (supercrack_ctx, "Rules: %u", straight_ctx->kernel_rules_cnt);
  }

  if (user_options->quiet == false) event_log_info (supercrack_ctx, NULL);

  if (hashconfig->opti_type)
  {
    event_log_info (supercrack_ctx, "Optimizers applied:");

    for (u32 i = 0; i < 32; i++)
    {
      const u32 opti_bit = 1U << i;

      if (hashconfig->opti_type & opti_bit) event_log_info (supercrack_ctx, "* %s", stroptitype (opti_bit));
    }
  }

  event_log_info (supercrack_ctx, NULL);

  if ((hashconfig->opti_type & OPTI_TYPE_OPTIMIZED_KERNEL) == 0)
  {
    if (hashconfig->has_optimized_kernel == true)
    {
      event_log_advice (supercrack_ctx, "ATTENTION! Pure (unoptimized) backend kernels selected.");
      event_log_advice (supercrack_ctx, "Pure kernels can crack longer passwords, but drastically reduce performance.");
      event_log_advice (supercrack_ctx, "If you want to switch to optimized kernels, append -O to your commandline.");
      event_log_advice (supercrack_ctx, "See the above message to find out about the exact limits.");
      event_log_advice (supercrack_ctx, NULL);
    }
  }

  if (user_options->keep_guessing == true)
  {
    event_log_advice (supercrack_ctx, "ATTENTION! --keep-guessing mode is enabled.");
    event_log_advice (supercrack_ctx, "This tells supercrack to continue attacking all target hashes until exhaustion.");
    event_log_advice (supercrack_ctx, "supercrack will NOT check for or remove targets present in the potfile, and");
    event_log_advice (supercrack_ctx, "will add ALL plains/collisions found, even duplicates, to the potfile.");
    event_log_advice (supercrack_ctx, NULL);
  }

  if (hashconfig->potfile_disable == true && user_options->attack_mode != ATTACK_MODE_ASSOCIATION)
  {
    event_log_advice (supercrack_ctx, "ATTENTION! Potfile storage is disabled for this hash mode.");
    event_log_advice (supercrack_ctx, "Passwords cracked during this session will NOT be stored to the potfile.");

    if(user_options->outfile_chgd == false)
    {
      event_log_advice (supercrack_ctx, "Consider using -o to save cracked passwords.");
    }

    event_log_advice (supercrack_ctx, NULL);
  }

  if (user_options->attack_mode == ATTACK_MODE_ASSOCIATION)
  {
    event_log_advice (supercrack_ctx, "ATTENTION! Potfile read/write is disabled for this attack mode.");
    event_log_advice (supercrack_ctx, "Passwords cracked during this session will NOT be stored to the potfile.");

    if(user_options->outfile_chgd == false)
    {
      event_log_advice (supercrack_ctx, "Consider using -o to save cracked passwords.");
    }

    event_log_advice (supercrack_ctx, NULL);
  }
  /**
   * Watchdog and Temperature balance
   */

  if (hwmon_ctx->enabled == false)
  {
    event_log_info (supercrack_ctx, "Watchdog: Hardware monitoring interface not found on your system.");
  }

  if (hwmon_ctx->enabled == true && user_options->hwmon_temp_abort > 0)
  {
    event_log_info (supercrack_ctx, "Watchdog: Temperature abort trigger set to %uc", user_options->hwmon_temp_abort);
  }
  else
  {
    event_log_info (supercrack_ctx, "Watchdog: Temperature abort trigger disabled.");
  }

  event_log_info (supercrack_ctx, NULL);
}

static void main_backend_session_pre (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_info_nn (supercrack_ctx, "Initializing device kernels and memory. Please be patient...");
}

static void main_backend_session_post (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_info_nn (supercrack_ctx, "Initialized device kernels and memory");
}

static void main_backend_session_hostmem (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  const u64 *hostmem = (const u64 *) buf;

  event_log_info (supercrack_ctx, "Host memory required for this attack: %" PRIu64 " MB", *hostmem / (1024 * 1024));
  event_log_info (supercrack_ctx, NULL);
}

static void main_backend_device_init_pre (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  const u32 *device_id = (const u32 *) buf;

  event_log_info_nn (supercrack_ctx, "Initializing backend runtime for device #%u. Please be patient...", *device_id + 1);
}

static void main_backend_device_init_post (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  const u32 *device_id = (const u32 *) buf;

  event_log_info_nn (supercrack_ctx, "Initialized backend runtime for device #%u", *device_id + 1);
}

static void main_bitmap_init_pre (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_info_nn (supercrack_ctx, "Generating bitmap tables...");
}

static void main_bitmap_init_post (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_info_nn (supercrack_ctx, "Generated bitmap tables");
}

static void main_bitmap_final_overflow (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_advice (supercrack_ctx, "Bitmap table overflowed at %d bits.", user_options->bitmap_max);
  event_log_advice (supercrack_ctx, "This typically happens with too many hashes and reduces your performance.");
  event_log_advice (supercrack_ctx, "You can increase the bitmap table size with --bitmap-max, but");
  event_log_advice (supercrack_ctx, "this creates a trade-off between L2-cache and bitmap efficiency.");
  event_log_advice (supercrack_ctx, "It is therefore not guaranteed to restore full performance.");
  event_log_advice (supercrack_ctx, NULL);
}

static void main_set_kernel_power_final (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  clear_prompt (supercrack_ctx);

  event_log_advice (supercrack_ctx, "Approaching final keyspace - workload adjusted.");
  event_log_advice (supercrack_ctx, NULL);

  send_prompt (supercrack_ctx);
}

static void main_monitor_throttle1 (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t       *user_options       = supercrack_ctx->user_options;
  const user_options_extra_t *user_options_extra = supercrack_ctx->user_options_extra;

  if (user_options->quiet == true) return;

  if ((user_options_extra->wordlist_mode == WL_MODE_FILE) || (user_options_extra->wordlist_mode == WL_MODE_MASK))
  {
    clear_prompt (supercrack_ctx);
  }

  const u32 *device_id = (const u32 *) buf;

  event_log_warning (supercrack_ctx, "Driver temperature threshold met on GPU #%u. Expect reduced performance.", *device_id + 1);

  if ((user_options_extra->wordlist_mode == WL_MODE_FILE) || (user_options_extra->wordlist_mode == WL_MODE_MASK))
  {
    send_prompt (supercrack_ctx);
  }
}

static void main_monitor_throttle2 (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t       *user_options       = supercrack_ctx->user_options;
  const user_options_extra_t *user_options_extra = supercrack_ctx->user_options_extra;

  if (user_options->quiet == true) return;

  if ((user_options_extra->wordlist_mode == WL_MODE_FILE) || (user_options_extra->wordlist_mode == WL_MODE_MASK))
  {
    clear_prompt (supercrack_ctx);
  }

  const u32 *device_id = (const u32 *) buf;

  event_log_warning (supercrack_ctx, "Driver temperature threshold met on GPU #%u. Expect reduced performance.", *device_id + 1);

  if ((user_options_extra->wordlist_mode == WL_MODE_FILE) || (user_options_extra->wordlist_mode == WL_MODE_MASK))
  {
    send_prompt (supercrack_ctx);
  }
}

static void main_monitor_throttle3 (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t       *user_options       = supercrack_ctx->user_options;
  const user_options_extra_t *user_options_extra = supercrack_ctx->user_options_extra;

  if (user_options->quiet == true) return;

  if ((user_options_extra->wordlist_mode == WL_MODE_FILE) || (user_options_extra->wordlist_mode == WL_MODE_MASK))
  {
    clear_prompt (supercrack_ctx);
  }

  const u32 *device_id = (const u32 *) buf;

  event_log_warning (supercrack_ctx, "Driver temperature threshold met on GPU #%u. Expect reduced performance.", *device_id + 1);
  event_log_warning (supercrack_ctx, NULL);

  if ((user_options_extra->wordlist_mode == WL_MODE_FILE) || (user_options_extra->wordlist_mode == WL_MODE_MASK))
  {
    send_prompt (supercrack_ctx);
  }
}

static void main_monitor_performance_hint (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const hashconfig_t         *hashconfig         = supercrack_ctx->hashconfig;
  const user_options_t       *user_options       = supercrack_ctx->user_options;
  const user_options_extra_t *user_options_extra = supercrack_ctx->user_options_extra;

  if (user_options->quiet == true) return;

  if ((user_options_extra->wordlist_mode == WL_MODE_FILE) || (user_options_extra->wordlist_mode == WL_MODE_MASK))
  {
    clear_prompt (supercrack_ctx);
  }

  event_log_advice (supercrack_ctx, "Cracking performance lower than expected?");
  event_log_advice (supercrack_ctx, NULL);

  if (user_options->optimized_kernel == false)
  {
    if ((hashconfig->opti_type & OPTI_TYPE_OPTIMIZED_KERNEL) == 0)
    {
      if (hashconfig->has_optimized_kernel == true)
      {
        event_log_advice (supercrack_ctx, "* Append -O to the commandline.");
        event_log_advice (supercrack_ctx, "  This lowers the maximum supported password/salt length (usually down to 32).");
        event_log_advice (supercrack_ctx, NULL);
      }
    }
  }

  if (user_options->workload_profile < 3)
  {
    event_log_advice (supercrack_ctx, "* Append -w 3 to the commandline.");
    event_log_advice (supercrack_ctx, "  This can cause your screen to lag.");
    event_log_advice (supercrack_ctx, NULL);
  }

  if (user_options->slow_candidates == false)
  {
    if ((user_options_extra->wordlist_mode == WL_MODE_FILE) || (user_options_extra->wordlist_mode == WL_MODE_MASK))
    {
      if ((user_options->attack_mode != ATTACK_MODE_HYBRID1) && (user_options->attack_mode != ATTACK_MODE_HYBRID2) && (user_options->attack_mode != ATTACK_MODE_ASSOCIATION))
      {
        event_log_advice (supercrack_ctx, "* Append -S to the commandline.");
        event_log_advice (supercrack_ctx, "  This has a drastic speed impact but can be better for specific attacks.");
        event_log_advice (supercrack_ctx, "  Typical scenarios are a small wordlist but a large ruleset.");
        event_log_advice (supercrack_ctx, NULL);
      }
    }
  }

  event_log_advice (supercrack_ctx, "* Update your backend API runtime / driver the right way:");
  event_log_advice (supercrack_ctx, "  https://supercrack.net/faq/wrongdriver");
  event_log_advice (supercrack_ctx, NULL);
  event_log_advice (supercrack_ctx, "* Create more work items to make use of your parallelization power:");
  event_log_advice (supercrack_ctx, "  https://supercrack.net/faq/morework");
  event_log_advice (supercrack_ctx, NULL);


  if ((user_options_extra->wordlist_mode == WL_MODE_FILE) || (user_options_extra->wordlist_mode == WL_MODE_MASK))
  {
    send_prompt (supercrack_ctx);
  }
}

static void main_monitor_noinput_hint (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_advice (supercrack_ctx, "ATTENTION! Read timeout in stdin mode. Password candidates input is too slow:");
  event_log_advice (supercrack_ctx, "* Are you sure you are using the correct attack mode (--attack-mode or -a)?");
  event_log_advice (supercrack_ctx, "* Are you sure you want to use input from standard input (stdin)?");
  event_log_advice (supercrack_ctx, "* If using stdin, are you sure it is working correctly, and is fast enough?");
  event_log_advice (supercrack_ctx, NULL);
}

static void main_monitor_noinput_abort (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  event_log_error (supercrack_ctx, "No password candidates received in stdin mode, aborting");
}

static void main_monitor_temp_abort (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t       *user_options       = supercrack_ctx->user_options;
  const user_options_extra_t *user_options_extra = supercrack_ctx->user_options_extra;


  if (((user_options_extra->wordlist_mode == WL_MODE_FILE) || (user_options_extra->wordlist_mode == WL_MODE_MASK)) && user_options->quiet == false)
  {
    clear_prompt (supercrack_ctx);
  }

  const u32 *device_id = (const u32 *) buf;

  event_log_error (supercrack_ctx, "Temperature limit on GPU #%u reached, aborting", *device_id + 1);
}

static void main_monitor_runtime_limit (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t       *user_options       = supercrack_ctx->user_options;
  const user_options_extra_t *user_options_extra = supercrack_ctx->user_options_extra;

  if (user_options->quiet == true) return;

  if ((user_options_extra->wordlist_mode == WL_MODE_FILE) || (user_options_extra->wordlist_mode == WL_MODE_MASK))
  {
    clear_prompt (supercrack_ctx);
  }

  event_log_warning (supercrack_ctx, "Runtime limit reached, aborting");
}

static void main_monitor_status_refresh (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t       *user_options       = supercrack_ctx->user_options;
  const user_options_extra_t *user_options_extra = supercrack_ctx->user_options_extra;
  const status_ctx_t         *status_ctx         = supercrack_ctx->status_ctx;

  if (status_ctx->accessible == false) return;

  if ((user_options_extra->wordlist_mode == WL_MODE_FILE) || (user_options_extra->wordlist_mode == WL_MODE_MASK))
  {
    if (user_options->quiet == false)
    {
      //clear_prompt (supercrack_ctx);

      event_log_info (supercrack_ctx, NULL);
      event_log_info (supercrack_ctx, NULL);
    }
  }

  status_display (supercrack_ctx);

  if ((user_options_extra->wordlist_mode == WL_MODE_FILE) || (user_options_extra->wordlist_mode == WL_MODE_MASK))
  {
    if (user_options->quiet == false)
    {
      event_log_info (supercrack_ctx, NULL);

      send_prompt (supercrack_ctx);
    }
  }

  if (user_options_extra->wordlist_mode == WL_MODE_STDIN)
  {
    if (user_options->quiet == false)
    {
      event_log_info (supercrack_ctx, NULL);
    }
  }
}

static void main_wordlist_cache_hit (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  const cache_hit_t *cache_hit = (const cache_hit_t *) buf;

  event_log_info (supercrack_ctx, "Dictionary cache hit:");
  event_log_info (supercrack_ctx, "* Filename..: %s", cache_hit->dictfile);
  event_log_info (supercrack_ctx, "* Passwords.: %" PRIu64, cache_hit->cached_cnt);
  event_log_info (supercrack_ctx, "* Bytes.....: %" PRId64, cache_hit->stat.st_size);
  event_log_info (supercrack_ctx, "* Keyspace..: %" PRIu64, cache_hit->keyspace);
  event_log_info (supercrack_ctx, NULL);
}

static void main_wordlist_cache_generate (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  const cache_generate_t *cache_generate = (const cache_generate_t *) buf;

  if (cache_generate->percent < 100)
  {
    event_log_info_nn (supercrack_ctx, "Dictionary cache building %s: %" PRIu64 " bytes (%.2f%%)", cache_generate->dictfile, cache_generate->comp, cache_generate->percent);
  }
  else
  {
    char *runtime = (char *) hcmalloc (HCBUFSIZ_TINY);

    const time_t runtime_sec = cache_generate->runtime;

    struct tm *tmp;
    struct tm  tm;

    tmp = gmtime_r (&runtime_sec, &tm);

    format_timer_display (tmp, runtime, HCBUFSIZ_TINY);

    event_log_info (supercrack_ctx, "Dictionary cache built:");
    event_log_info (supercrack_ctx, "* Filename..: %s", cache_generate->dictfile);
    event_log_info (supercrack_ctx, "* Passwords.: %" PRIu64, cache_generate->cnt2);
    event_log_info (supercrack_ctx, "* Bytes.....: %" PRId64, cache_generate->comp);
    event_log_info (supercrack_ctx, "* Keyspace..: %" PRIu64, cache_generate->cnt);
    event_log_info (supercrack_ctx, "* Runtime...: %s", runtime);
    event_log_info (supercrack_ctx, NULL);

    hcfree (runtime);
  }
}

static void main_hashconfig_pre (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
}

static void main_hashconfig_post (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const hashconfig_t   *hashconfig   = supercrack_ctx->hashconfig;
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  /**
   * Optimizer constraints
   */

  event_log_info (supercrack_ctx, "Minimum password length supported by kernel: %u", hashconfig->pw_min);
  event_log_info (supercrack_ctx, "Maximum password length supported by kernel: %u", hashconfig->pw_max);

  if (hashconfig->is_salted == true)
  {
    if (hashconfig->opti_type & OPTI_TYPE_RAW_HASH)
    {
      event_log_info (supercrack_ctx, "Minimum salt length supported by kernel: %u", hashconfig->salt_min);
      event_log_info (supercrack_ctx, "Maximum salt length supported by kernel: %u", hashconfig->salt_max);
    }
  }

  event_log_info (supercrack_ctx, NULL);
}

static void main_hashlist_count_lines_pre (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  const char *hashfile = (const char *) buf;

  event_log_info_nn (supercrack_ctx, "Counting lines in %s. Please be patient...", hashfile);
}

static void main_hashlist_count_lines_post (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  const char *hashfile = (const char *) buf;

  event_log_info_nn (supercrack_ctx, "Counted lines in %s", hashfile);
}

static void main_hashlist_parse_hash (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  const hashlist_parse_t *hashlist_parse = (const hashlist_parse_t *) buf;

  const u64 hashes_cnt   = hashlist_parse->hashes_cnt;
  const u64 hashes_avail = hashlist_parse->hashes_avail;

  if (hashes_cnt < hashes_avail)
  {
    event_log_info_nn (supercrack_ctx, "Parsing Hashes: %" PRIu64 "/%" PRIu64 " (%0.2f%%)...", hashes_cnt, hashes_avail, ((double) hashes_cnt / hashes_avail) * 100.0);
  }
  else
  {
    event_log_info_nn (supercrack_ctx, "Parsed Hashes: %" PRIu64 "/%" PRIu64 " (%0.2f%%)", hashes_cnt, hashes_avail, 100.0);
  }
}

static void main_hashlist_sort_hash_pre (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_info_nn (supercrack_ctx, "Sorting hashes. Please be patient...");
}

static void main_hashlist_sort_hash_post (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_info_nn (supercrack_ctx, "Sorted hashes");
}

static void main_hashlist_unique_hash_pre (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_info_nn (supercrack_ctx, "Removing duplicate hashes. Please be patient...");
}

static void main_hashlist_unique_hash_post (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_info_nn (supercrack_ctx, "Removed duplicate hashes");
}

static void main_hashlist_sort_salt_pre (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_info_nn (supercrack_ctx, "Sorting salts. Please be patient...");
}

static void main_hashlist_sort_salt_post (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_info_nn (supercrack_ctx, "Sorted salts");
}

static void main_autodetect_starting (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_info_nn (supercrack_ctx, "Autodetecting hash-modes. Please be patient...");
}

static void main_autodetect_finished (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_info_nn (supercrack_ctx, "Autodetected hash-modes");
}

static void main_selftest_starting (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_info_nn (supercrack_ctx, "Starting self-test. Please be patient...");
}

static void main_selftest_finished (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_info_nn (supercrack_ctx, "Finished self-test");
}

static void main_autotune_starting (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_info_nn (supercrack_ctx, "Starting autotune. Please be patient...");
}

static void main_autotune_finished (MAYBE_UNUSED supercrack_ctx_t *supercrack_ctx, MAYBE_UNUSED const void *buf, MAYBE_UNUSED const size_t len)
{
  const user_options_t *user_options = supercrack_ctx->user_options;

  if (user_options->quiet == true) return;

  event_log_info_nn (supercrack_ctx, "Finished autotune");
}

static void event (const u32 id, supercrack_ctx_t *supercrack_ctx, const void *buf, const size_t len)
{
  switch (id)
  {
    case EVENT_AUTOTUNE_FINISHED:         main_autotune_finished         (supercrack_ctx, buf, len); break;
    case EVENT_AUTOTUNE_STARTING:         main_autotune_starting         (supercrack_ctx, buf, len); break;
    case EVENT_SELFTEST_FINISHED:         main_selftest_finished         (supercrack_ctx, buf, len); break;
    case EVENT_SELFTEST_STARTING:         main_selftest_starting         (supercrack_ctx, buf, len); break;
    case EVENT_AUTODETECT_FINISHED:       main_autodetect_finished       (supercrack_ctx, buf, len); break;
    case EVENT_AUTODETECT_STARTING:       main_autodetect_starting       (supercrack_ctx, buf, len); break;
    case EVENT_BITMAP_INIT_POST:          main_bitmap_init_post          (supercrack_ctx, buf, len); break;
    case EVENT_BITMAP_INIT_PRE:           main_bitmap_init_pre           (supercrack_ctx, buf, len); break;
    case EVENT_BITMAP_FINAL_OVERFLOW:     main_bitmap_final_overflow     (supercrack_ctx, buf, len); break;
    case EVENT_CALCULATED_WORDS_BASE:     main_calculated_words_base     (supercrack_ctx, buf, len); break;
    case EVENT_CRACKER_FINISHED:          main_cracker_finished          (supercrack_ctx, buf, len); break;
    case EVENT_CRACKER_HASH_CRACKED:      main_cracker_hash_cracked      (supercrack_ctx, buf, len); break;
    case EVENT_CRACKER_STARTING:          main_cracker_starting          (supercrack_ctx, buf, len); break;
    case EVENT_HASHCONFIG_PRE:            main_hashconfig_pre            (supercrack_ctx, buf, len); break;
    case EVENT_HASHCONFIG_POST:           main_hashconfig_post           (supercrack_ctx, buf, len); break;
    case EVENT_HASHLIST_COUNT_LINES_POST: main_hashlist_count_lines_post (supercrack_ctx, buf, len); break;
    case EVENT_HASHLIST_COUNT_LINES_PRE:  main_hashlist_count_lines_pre  (supercrack_ctx, buf, len); break;
    case EVENT_HASHLIST_PARSE_HASH:       main_hashlist_parse_hash       (supercrack_ctx, buf, len); break;
    case EVENT_HASHLIST_SORT_HASH_POST:   main_hashlist_sort_hash_post   (supercrack_ctx, buf, len); break;
    case EVENT_HASHLIST_SORT_HASH_PRE:    main_hashlist_sort_hash_pre    (supercrack_ctx, buf, len); break;
    case EVENT_HASHLIST_SORT_SALT_POST:   main_hashlist_sort_salt_post   (supercrack_ctx, buf, len); break;
    case EVENT_HASHLIST_SORT_SALT_PRE:    main_hashlist_sort_salt_pre    (supercrack_ctx, buf, len); break;
    case EVENT_HASHLIST_UNIQUE_HASH_POST: main_hashlist_unique_hash_post (supercrack_ctx, buf, len); break;
    case EVENT_HASHLIST_UNIQUE_HASH_PRE:  main_hashlist_unique_hash_pre  (supercrack_ctx, buf, len); break;
    case EVENT_LOG_ERROR:                 main_log_error                 (supercrack_ctx, buf, len); break;
    case EVENT_LOG_INFO:                  main_log_info                  (supercrack_ctx, buf, len); break;
    case EVENT_LOG_WARNING:               main_log_warning               (supercrack_ctx, buf, len); break;
    case EVENT_LOG_ADVICE:                main_log_advice                (supercrack_ctx, buf, len); break;
    case EVENT_MONITOR_RUNTIME_LIMIT:     main_monitor_runtime_limit     (supercrack_ctx, buf, len); break;
    case EVENT_MONITOR_STATUS_REFRESH:    main_monitor_status_refresh    (supercrack_ctx, buf, len); break;
    case EVENT_MONITOR_TEMP_ABORT:        main_monitor_temp_abort        (supercrack_ctx, buf, len); break;
    case EVENT_MONITOR_THROTTLE1:         main_monitor_throttle1         (supercrack_ctx, buf, len); break;
    case EVENT_MONITOR_THROTTLE2:         main_monitor_throttle2         (supercrack_ctx, buf, len); break;
    case EVENT_MONITOR_THROTTLE3:         main_monitor_throttle3         (supercrack_ctx, buf, len); break;
    case EVENT_MONITOR_PERFORMANCE_HINT:  main_monitor_performance_hint  (supercrack_ctx, buf, len); break;
    case EVENT_MONITOR_NOINPUT_HINT:      main_monitor_noinput_hint      (supercrack_ctx, buf, len); break;
    case EVENT_MONITOR_NOINPUT_ABORT:     main_monitor_noinput_abort     (supercrack_ctx, buf, len); break;
    case EVENT_BACKEND_SESSION_POST:      main_backend_session_post      (supercrack_ctx, buf, len); break;
    case EVENT_BACKEND_SESSION_PRE:       main_backend_session_pre       (supercrack_ctx, buf, len); break;
    case EVENT_BACKEND_SESSION_HOSTMEM:   main_backend_session_hostmem   (supercrack_ctx, buf, len); break;
    case EVENT_BACKEND_DEVICE_INIT_POST:  main_backend_device_init_post  (supercrack_ctx, buf, len); break;
    case EVENT_BACKEND_DEVICE_INIT_PRE:   main_backend_device_init_pre   (supercrack_ctx, buf, len); break;
    case EVENT_OUTERLOOP_FINISHED:        main_outerloop_finished        (supercrack_ctx, buf, len); break;
    case EVENT_OUTERLOOP_MAINSCREEN:      main_outerloop_mainscreen      (supercrack_ctx, buf, len); break;
    case EVENT_OUTERLOOP_STARTING:        main_outerloop_starting        (supercrack_ctx, buf, len); break;
    case EVENT_POTFILE_ALL_CRACKED:       main_potfile_all_cracked       (supercrack_ctx, buf, len); break;
    case EVENT_POTFILE_HASH_LEFT:         main_potfile_hash_left         (supercrack_ctx, buf, len); break;
    case EVENT_POTFILE_HASH_SHOW:         main_potfile_hash_show         (supercrack_ctx, buf, len); break;
    case EVENT_POTFILE_NUM_CRACKED:       main_potfile_num_cracked       (supercrack_ctx, buf, len); break;
    case EVENT_POTFILE_REMOVE_PARSE_POST: main_potfile_remove_parse_post (supercrack_ctx, buf, len); break;
    case EVENT_POTFILE_REMOVE_PARSE_PRE:  main_potfile_remove_parse_pre  (supercrack_ctx, buf, len); break;
    case EVENT_RULESFILES_PARSE_POST:     main_rulesfiles_parse_post     (supercrack_ctx, buf, len); break;
    case EVENT_RULESFILES_PARSE_PRE:      main_rulesfiles_parse_pre      (supercrack_ctx, buf, len); break;
    case EVENT_SET_KERNEL_POWER_FINAL:    main_set_kernel_power_final    (supercrack_ctx, buf, len); break;
    case EVENT_WORDLIST_CACHE_GENERATE:   main_wordlist_cache_generate   (supercrack_ctx, buf, len); break;
    case EVENT_WORDLIST_CACHE_HIT:        main_wordlist_cache_hit        (supercrack_ctx, buf, len); break;
  }
}

int main (int argc, char **argv)
{
  // this increases the size on windows dos boxes

  setup_console ();

  const time_t proc_start = time (NULL);

  // supercrack main context

  supercrack_ctx_t *supercrack_ctx = (supercrack_ctx_t *) hcmalloc (sizeof (supercrack_ctx_t));

  if (supercrack_init (supercrack_ctx, event) == -1)
  {
    hcfree (supercrack_ctx);

    return -1;
  }
  // install and shared folder need to be set to recognize "make install" use

  const char *install_folder = NULL;
  const char *shared_folder  = NULL;

  #if defined (INSTALL_FOLDER)
  install_folder = INSTALL_FOLDER;
  #endif

  #if defined (SHARED_FOLDER)
  shared_folder = SHARED_FOLDER;
  #endif

  // initialize the user options with some defaults (you can override them later)

  if (user_options_init (supercrack_ctx) == -1)
  {
    supercrack_destroy (supercrack_ctx);

    hcfree (supercrack_ctx);

    return -1;
  }

  // parse commandline parameters and check them

  if (user_options_getopt (supercrack_ctx, argc, argv) == -1)
  {
    user_options_destroy (supercrack_ctx);

    supercrack_destroy (supercrack_ctx);

    hcfree (supercrack_ctx);

    return -1;
  }

  if (user_options_sanity (supercrack_ctx) == -1)
  {
    user_options_destroy (supercrack_ctx);

    supercrack_destroy (supercrack_ctx);

    hcfree (supercrack_ctx);

    return -1;
  }

  // some early exits

  user_options_t *user_options = supercrack_ctx->user_options;

  #ifdef WITH_BRAIN
  if (user_options->brain_server == true)
  {
    const int rc = brain_server (user_options->brain_host, user_options->brain_port, user_options->brain_password, user_options->brain_session_whitelist, user_options->brain_server_timer);

    hcfree (supercrack_ctx);

    return rc;
  }
  #endif

  if (user_options->version == true)
  {
    printf ("%s\n", VERSION_TAG);

    user_options_destroy (supercrack_ctx);

    supercrack_destroy (supercrack_ctx);

    hcfree (supercrack_ctx);

    return 0;
  }

  // init a supercrack session; this initializes backend devices, hwmon, etc

  welcome_screen (supercrack_ctx, VERSION_TAG);

  int rc_final = -1;

  if (supercrack_session_init (supercrack_ctx, install_folder, shared_folder, argc, argv, COMPTIME) == 0)
  {
    if (user_options->usage > 0)
    {
      usage_big_print (supercrack_ctx);

      rc_final = 0;
    }
    else if (user_options->hash_info == true)
    {
      hash_info (supercrack_ctx);

      rc_final = 0;
    }
    else if (user_options->backend_info > 0)
    {
      // if this is just backend_info, no need to execute some real cracking session

      backend_info (supercrack_ctx);

      rc_final = 0;
    }
    else
    {
      // now execute supercrack

      backend_info_compact (supercrack_ctx);

      user_options_info (supercrack_ctx);

      rc_final = supercrack_session_execute (supercrack_ctx);
    }
  }

  // finish the supercrack session, this shuts down backend devices, hwmon, etc

  supercrack_session_destroy (supercrack_ctx);

  // finished with supercrack, clean up

  const time_t proc_stop = time (NULL);

  goodbye_screen (supercrack_ctx, proc_start, proc_stop);

  supercrack_destroy (supercrack_ctx);

  hcfree (supercrack_ctx);

  return rc_final;
}

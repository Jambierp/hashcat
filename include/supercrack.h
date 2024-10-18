/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#ifndef HC_SUPERCRACK_H
#define HC_SUPERCRACK_H

int   supercrack_init               (supercrack_ctx_t *supercrack_ctx, void (*event) (const u32, struct supercrack_ctx *, const void *, const size_t));
void  supercrack_destroy            (supercrack_ctx_t *supercrack_ctx);

int   supercrack_session_init       (supercrack_ctx_t *supercrack_ctx, const char *install_folder, const char *shared_folder, int argc, char **argv, const int comptime);
int   supercrack_session_execute    (supercrack_ctx_t *supercrack_ctx);
int   supercrack_session_pause      (supercrack_ctx_t *supercrack_ctx);
int   supercrack_session_resume     (supercrack_ctx_t *supercrack_ctx);
int   supercrack_session_bypass     (supercrack_ctx_t *supercrack_ctx);
int   supercrack_session_checkpoint (supercrack_ctx_t *supercrack_ctx);
int   supercrack_session_finish     (supercrack_ctx_t *supercrack_ctx);
int   supercrack_session_quit       (supercrack_ctx_t *supercrack_ctx);
int   supercrack_session_destroy    (supercrack_ctx_t *supercrack_ctx);

char *supercrack_get_log            (supercrack_ctx_t *supercrack_ctx);
int   supercrack_get_status         (supercrack_ctx_t *supercrack_ctx, supercrack_status_t *supercrack_status);

#endif // HC_SUPERCRACK_H

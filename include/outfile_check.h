/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#ifndef HC_OUTFILE_CHECK_H
#define HC_OUTFILE_CHECK_H

#include <unistd.h>
#include <errno.h>

#define OUTFILES_DIR "outfiles"

HC_API_CALL void *thread_outfile_remove (void *p);

int  outcheck_ctx_init    (supercrack_ctx_t *supercrack_ctx);
void outcheck_ctx_destroy (supercrack_ctx_t *supercrack_ctx);

#endif // HC_OUTFILE_CHECK_H

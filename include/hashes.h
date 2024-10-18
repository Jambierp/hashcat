/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#ifndef HC_HASHES_H
#define HC_HASHES_H

int sort_by_digest_p0p1  (const void *v1, const void *v2, void *v3);
int sort_by_salt         (const void *v1, const void *v2);
int sort_by_hash         (const void *v1, const void *v2, void *v3);
int sort_by_hash_no_salt (const void *v1, const void *v2, void *v3);

int hash_encode (const hashconfig_t *hashconfig, const hashes_t *hashes, const module_ctx_t *module_ctx, char *out_buf, const int out_size, const u32 salt_pos, const u32 digest_pos);

int save_hash (supercrack_ctx_t *supercrack_ctx);

int check_hash (supercrack_ctx_t *supercrack_ctx, hc_device_param_t *device_param, plain_t *plain);

//int check_cracked (supercrack_ctx_t *supercrack_ctx, hc_device_param_t *device_param, const u32 salt_pos);
int check_cracked (supercrack_ctx_t *supercrack_ctx, hc_device_param_t *device_param);

int hashes_init_filename  (supercrack_ctx_t *supercrack_ctx);
int hashes_init_stage1    (supercrack_ctx_t *supercrack_ctx);
int hashes_init_stage2    (supercrack_ctx_t *supercrack_ctx);
int hashes_init_stage3    (supercrack_ctx_t *supercrack_ctx);
int hashes_init_stage4    (supercrack_ctx_t *supercrack_ctx);
int hashes_init_selftest  (supercrack_ctx_t *supercrack_ctx);
int hashes_init_benchmark (supercrack_ctx_t *supercrack_ctx);
int hashes_init_zerohash  (supercrack_ctx_t *supercrack_ctx);

void hashes_destroy (supercrack_ctx_t *supercrack_ctx);

void hashes_logger (supercrack_ctx_t *supercrack_ctx);

#endif // HC_HASHES_H

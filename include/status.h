/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#ifndef HC_STATUS_H
#define HC_STATUS_H

#include <stdio.h>
#include <time.h>
#include <inttypes.h>

double get_avg_exec_time (hc_device_param_t *device_param, const int last_num_entries);

// should be static after refactoring
void format_timer_display    (struct tm *tm, char *buf, size_t len);
void format_speed_display    (double val,    char *buf, size_t len);
void format_speed_display_1k (double val,    char *buf, size_t len);

int         status_get_device_info_cnt                (const supercrack_ctx_t *supercrack_ctx);
int         status_get_device_info_active             (const supercrack_ctx_t *supercrack_ctx);
bool        status_get_skipped_dev                    (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
bool        status_get_skipped_warning_dev            (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
char       *status_get_session                        (const supercrack_ctx_t *supercrack_ctx);
const char *status_get_status_string                  (const supercrack_ctx_t *supercrack_ctx);
int         status_get_status_number                  (const supercrack_ctx_t *supercrack_ctx);
int         status_get_guess_mode                     (const supercrack_ctx_t *supercrack_ctx);
char       *status_get_guess_base                     (const supercrack_ctx_t *supercrack_ctx);
int         status_get_guess_base_offset              (const supercrack_ctx_t *supercrack_ctx);
int         status_get_guess_base_count               (const supercrack_ctx_t *supercrack_ctx);
double      status_get_guess_base_percent             (const supercrack_ctx_t *supercrack_ctx);
char       *status_get_guess_mod                      (const supercrack_ctx_t *supercrack_ctx);
int         status_get_guess_mod_offset               (const supercrack_ctx_t *supercrack_ctx);
int         status_get_guess_mod_count                (const supercrack_ctx_t *supercrack_ctx);
double      status_get_guess_mod_percent              (const supercrack_ctx_t *supercrack_ctx);
char       *status_get_guess_charset                  (const supercrack_ctx_t *supercrack_ctx);
int         status_get_guess_mask_length              (const supercrack_ctx_t *supercrack_ctx);
char       *status_get_guess_candidates_dev           (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
char       *status_get_hash_name                      (const supercrack_ctx_t *supercrack_ctx);
char       *status_get_hash_target                    (const supercrack_ctx_t *supercrack_ctx);
int         status_get_digests_done                   (const supercrack_ctx_t *supercrack_ctx);
int         status_get_digests_done_pot               (const supercrack_ctx_t *supercrack_ctx);
int         status_get_digests_done_zero              (const supercrack_ctx_t *supercrack_ctx);
int         status_get_digests_done_new               (const supercrack_ctx_t *supercrack_ctx);
int         status_get_digests_cnt                    (const supercrack_ctx_t *supercrack_ctx);
double      status_get_digests_percent                (const supercrack_ctx_t *supercrack_ctx);
double      status_get_digests_percent_new            (const supercrack_ctx_t *supercrack_ctx);
int         status_get_salts_done                     (const supercrack_ctx_t *supercrack_ctx);
int         status_get_salts_cnt                      (const supercrack_ctx_t *supercrack_ctx);
double      status_get_salts_percent                  (const supercrack_ctx_t *supercrack_ctx);
double      status_get_msec_running                   (const supercrack_ctx_t *supercrack_ctx);
double      status_get_msec_paused                    (const supercrack_ctx_t *supercrack_ctx);
double      status_get_msec_real                      (const supercrack_ctx_t *supercrack_ctx);
char       *status_get_time_started_absolute          (const supercrack_ctx_t *supercrack_ctx);
char       *status_get_time_started_relative          (const supercrack_ctx_t *supercrack_ctx);
time_t      status_get_sec_etc                        (const supercrack_ctx_t *supercrack_ctx);
char       *status_get_time_estimated_absolute        (const supercrack_ctx_t *supercrack_ctx);
char       *status_get_time_estimated_relative        (const supercrack_ctx_t *supercrack_ctx);
u64         status_get_restore_point                  (const supercrack_ctx_t *supercrack_ctx);
u64         status_get_restore_total                  (const supercrack_ctx_t *supercrack_ctx);
double      status_get_restore_percent                (const supercrack_ctx_t *supercrack_ctx);
int         status_get_progress_mode                  (const supercrack_ctx_t *supercrack_ctx);
double      status_get_progress_finished_percent      (const supercrack_ctx_t *supercrack_ctx);
u64         status_get_progress_done                  (const supercrack_ctx_t *supercrack_ctx);
u64         status_get_progress_rejected              (const supercrack_ctx_t *supercrack_ctx);
double      status_get_progress_rejected_percent      (const supercrack_ctx_t *supercrack_ctx);
u64         status_get_progress_restored              (const supercrack_ctx_t *supercrack_ctx);
u64         status_get_progress_cur                   (const supercrack_ctx_t *supercrack_ctx);
u64         status_get_progress_end                   (const supercrack_ctx_t *supercrack_ctx);
u64         status_get_progress_ignore                (const supercrack_ctx_t *supercrack_ctx);
u64         status_get_progress_skip                  (const supercrack_ctx_t *supercrack_ctx);
u64         status_get_progress_cur_relative_skip     (const supercrack_ctx_t *supercrack_ctx);
u64         status_get_progress_end_relative_skip     (const supercrack_ctx_t *supercrack_ctx);
double      status_get_hashes_msec_all                (const supercrack_ctx_t *supercrack_ctx);
double      status_get_hashes_msec_dev                (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
double      status_get_hashes_msec_dev_benchmark      (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
double      status_get_exec_msec_all                  (const supercrack_ctx_t *supercrack_ctx);
double      status_get_exec_msec_dev                  (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
char       *status_get_speed_sec_all                  (const supercrack_ctx_t *supercrack_ctx);
char       *status_get_speed_sec_dev                  (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
int         status_get_cpt_cur_min                    (const supercrack_ctx_t *supercrack_ctx);
int         status_get_cpt_cur_hour                   (const supercrack_ctx_t *supercrack_ctx);
int         status_get_cpt_cur_day                    (const supercrack_ctx_t *supercrack_ctx);
double      status_get_cpt_avg_min                    (const supercrack_ctx_t *supercrack_ctx);
double      status_get_cpt_avg_hour                   (const supercrack_ctx_t *supercrack_ctx);
double      status_get_cpt_avg_day                    (const supercrack_ctx_t *supercrack_ctx);
char       *status_get_cpt                            (const supercrack_ctx_t *supercrack_ctx);
int         status_get_salt_pos_dev                   (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
int         status_get_innerloop_pos_dev              (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
int         status_get_innerloop_left_dev             (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
int         status_get_iteration_pos_dev              (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
int         status_get_iteration_left_dev             (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
char       *status_get_device_name                    (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
cl_device_type  status_get_device_type                (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
#ifdef WITH_BRAIN
int         status_get_brain_session                  (const supercrack_ctx_t *supercrack_ctx);
int         status_get_brain_attack                   (const supercrack_ctx_t *supercrack_ctx);
int         status_get_brain_link_client_id_dev       (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
int         status_get_brain_link_status_dev          (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
char       *status_get_brain_link_recv_bytes_dev      (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
char       *status_get_brain_link_send_bytes_dev      (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
char       *status_get_brain_link_recv_bytes_sec_dev  (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
char       *status_get_brain_link_send_bytes_sec_dev  (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
char       *status_get_brain_rx_all                   (const supercrack_ctx_t *supercrack_ctx);
char       *status_get_brain_tx_all                   (const supercrack_ctx_t *supercrack_ctx);
#endif
#if defined(__APPLE__)
char       *status_get_hwmon_fan_dev                  (const supercrack_ctx_t *supercrack_ctx);
#endif
char       *status_get_hwmon_dev                      (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
int         status_get_corespeed_dev                  (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
int         status_get_memoryspeed_dev                (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
u64         status_get_progress_dev                   (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
double      status_get_runtime_msec_dev               (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
int         status_get_kernel_accel_dev               (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
int         status_get_kernel_loops_dev               (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
int         status_get_kernel_threads_dev             (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);
int         status_get_vector_width_dev               (const supercrack_ctx_t *supercrack_ctx, const int backend_devices_idx);

int         status_progress_init                      (supercrack_ctx_t *supercrack_ctx);
void        status_progress_destroy                   (supercrack_ctx_t *supercrack_ctx);
void        status_progress_reset                     (supercrack_ctx_t *supercrack_ctx);

int         status_ctx_init                           (supercrack_ctx_t *supercrack_ctx);
void        status_ctx_destroy                        (supercrack_ctx_t *supercrack_ctx);

void        status_status_destroy                     (supercrack_ctx_t *supercrack_ctx, supercrack_status_t *supercrack_status);

#endif // HC_STATUS_H

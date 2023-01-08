// SPDX-License-Identifier: GPL-2.0-only
/* vendor_hook.c
 *
 * Android Vendor Hook Support
 *
 * Copyright 2020 Google LLC
 */

#define CREATE_TRACE_POINTS
#include <trace/hooks/vendor_hooks.h>
#include <linux/tracepoint.h>
#include <trace/hooks/sched.h>
#include <trace/hooks/fpsimd.h>
#include <trace/hooks/binder.h>
#include <trace/hooks/cpuidle.h>
#include <trace/hooks/mpam.h>
#include <trace/hooks/wqlockup.h>
#include <trace/hooks/debug.h>
#include <trace/hooks/sysrqcrash.h>
#include <trace/hooks/printk.h>
#include <trace/hooks/epoch.h>
#include <trace/hooks/cpufreq.h>
#include <trace/hooks/preemptirq.h>
#include <trace/hooks/ftrace_dump.h>
#include <trace/hooks/ufshcd.h>
#include <trace/hooks/cgroup.h>
#include <trace/hooks/sys.h>
#include <trace/hooks/iommu.h>
#include <trace/hooks/net.h>
#include <trace/hooks/pm_domain.h>
#include <trace/hooks/cpuidle_psci.h>
#include <trace/hooks/vmscan.h>
#include <trace/hooks/avc.h>
#include <trace/hooks/creds.h>
#include <trace/hooks/selinux.h>
#include <trace/hooks/syscall_check.h>
#include <trace/hooks/remoteproc.h>
#include <trace/hooks/hung_task.h>

/*
 * Export tracepoints that act as a bare tracehook (ie: have no trace event
 * associated with them) to allow external modules to probe them.
 */
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_select_task_rq_fair);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_select_task_rq_rt);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_select_fallback_rq);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_scheduler_tick);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_enqueue_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_dequeue_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_can_migrate_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_find_lowest_rq);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_rtmutex_prepare_setprio);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_prepare_prio_fork);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_finish_prio_fork);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_set_user_nice);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_setscheduler);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_is_fpsimd_save);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_transaction_init);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_set_priority);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_restore_priority);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_wakeup_ilocked);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpu_idle_enter);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpu_idle_exit);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mpam_set);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_find_busiest_group);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_wq_lockup_pool);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ipi_stop);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_sysrq_crash);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_dump_throttled_rt_tasks);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_printk_hotplug);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_jiffies_update);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_show_suspend_epoch_val);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_show_resume_epoch_val);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_freq_table_limits);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_sched_newidle_balance);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_sched_nohz_balancer_kick);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_sched_rebalance_domains);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_find_busiest_queue);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_migrate_queued_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_cpu_overutilized);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_sched_setaffinity);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_update_cpus_allowed);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_preempt_disable);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_preempt_enable);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_irqs_disable);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_irqs_enable);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_cpu_cgroup_attach);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_cpu_cgroup_can_attach);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_cpu_cgroup_online);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ftrace_oops_enter);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ftrace_oops_exit);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ftrace_size_check);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ftrace_format_check);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ftrace_dump_buffer);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_fill_prdt);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_prepare_command);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_update_sysfs);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_send_command);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_compl_command);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cgroup_set_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_syscall_prctl_finished);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_send_uic_command);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_send_tm_command);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_check_int_errors);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cgroup_attach);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_iommu_setup_dma_ops);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_iommu_iovad_alloc_iova);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_iommu_iovad_free_iova);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ptype_head);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_allow_domain_state);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpuidle_psci_enter);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpuidle_psci_exit);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_check_preempt_tick);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_check_preempt_wakeup_ignore);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_replace_next_task_fair);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_wait_for_work);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_sync_txn_recvd);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_build_sched_domains);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_sched_balance_rt);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_pick_next_entity);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_check_preempt_wakeup);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_cpufreq_transition);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_set_balance_anon_file_reclaim);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_show_max_freq);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_free_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_after_enqueue_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_after_dequeue_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_enqueue_entity);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_dequeue_entity);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_entity_tick);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_enqueue_task_fair);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_dequeue_task_fair);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_selinux_avc_insert);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_selinux_avc_node_delete);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_selinux_avc_node_replace);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_selinux_avc_lookup);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_commit_creds);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_exit_creds);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_override_creds);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_revert_creds);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_selinux_is_initialized);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_check_mmap_file);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_check_file_open);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_check_bpf_syscall);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_set_cpus_allowed_by_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_post_init_entity_util_avg);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_uclamp_eff_get);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_util_est_update);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_setscheduler_uclamp);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rproc_recovery);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rproc_recovery_set);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_check_uninterruptible_tasks);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_check_uninterruptible_tasks_dn);

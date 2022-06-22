#pragma once

#include <linux/version.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include "losttime.h"

#define PROCFS_NAME "goldeneye"
#define VERSION 5 // Defines the proc payload serialization version, increment if you change the format
#define NUM_HISTO_BINS 25 // How many histogram bins to serialize

// struct file_operations was replaced with struct proc_ops in Kernel v5.6
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

extern struct LostTimes g_lostTimes;
extern _u64 g_cyclesPerSec;
extern int secondsToRun;
extern int reportToHost;
extern int prettyPrint;

void format_bin(_u64* bin, char** unit);

int proc_create_file(void);
void proc_remove_file(void);
int proc_open_file(struct inode *inode, struct  file *file);
int proc_serialize_file(struct seq_file *m, void *v);
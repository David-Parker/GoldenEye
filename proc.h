#pragma once

#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include "losttime.h"

#define PROCFS_NAME "goldeneye"

extern struct LostTimes lostTimes;
extern const struct file_operations proc_fops;

int create_proc(void);
void remove_proc(void);
int proc_show(struct seq_file *m, void *v);
int proc_open(struct inode *inode, struct  file *file);
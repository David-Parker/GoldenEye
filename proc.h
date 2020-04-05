#pragma once

#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#define PROCFS_MAX_SIZE		1024
#define PROCFS_NAME 		"goldeneye"

extern char buff[1024];

int create_proc(void);
void remove_proc(void);
int proc_show(struct seq_file *m, void *v);
int proc_open(struct inode *inode, struct  file *file);

extern const struct file_operations proc_fops;
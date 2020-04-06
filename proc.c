#include "proc.h"

int create_proc()
{
    struct proc_dir_entry* proc = proc_create(PROCFS_NAME, 0, NULL, &proc_fops);

    if (proc == NULL)
    {
        remove_proc();
        return -1;
    }

    return 0;
}

void remove_proc()
{
    remove_proc_entry(PROCFS_NAME, NULL);
}

// Report lost times in a text serialized format to user space
int proc_show(struct seq_file *m, void *v) {
    int i;
    int j;

    seq_printf(m, "Cores: %d\n", lostTimes.Cores);
    seq_printf(m, "StartTime: %llu\n", lostTimes.StartTimeNs);

    for (i = 0; i < lostTimes.Cores; ++i)
    {
        seq_printf(m, "BEGIN Core: %d\n", i);

        for (j = 0; j < lostTimes.Times[i].Count; ++j)
        {
            seq_printf(m, "%llu\n", lostTimes.Times[i].Lost[j]);
        }

        seq_printf(m, "END\n");
    }

    return 0;
}

int proc_open(struct inode *inode, struct  file *file) {
    return single_open(file, proc_show, NULL);
}

const struct file_operations proc_fops = {
    .owner = THIS_MODULE,
    .open = proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};
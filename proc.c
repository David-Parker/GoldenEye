#include "proc.h"
#include "json.h"

char histogram_units[64][64];

#ifdef HAVE_PROC_OPS
static const struct proc_ops proc_fops = {
    .proc_open = proc_open_file,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};
#else
static const struct file_operations proc_fops = {
    .owner = THIS_MODULE,
    .open = proc_open_file,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};
#endif

int proc_create_file()
{
    struct proc_dir_entry* proc = proc_create(PROCFS_NAME, 0, NULL, &proc_fops);

    if (proc == NULL)
    {
        proc_remove_file();
        return -1;
    }

    return 0;
}

void proc_remove_file()
{
    remove_proc_entry(PROCFS_NAME, NULL);
}

int proc_open_file(struct inode* inode, struct  file* file)
{
    return single_open(file, proc_serialize_file, NULL);
}

// Report lost times in a json format to user space
int proc_serialize_file(struct seq_file *m, void *v) 
{
    int i;
    int j;
    char* cstr;
    bool pretty_print;
    json_serializer_t json_serializer;
    json_element_t* parent;
    json_element_t* curr;

    json_serializer_init(&json_serializer);

    parent = json_serializer.root;
    json_serializer_add_number(&json_serializer, parent, "Version", VERSION);
    json_serializer_add_number(&json_serializer, parent, "Cores", g_lostTimes.Cores);
    json_serializer_add_number(&json_serializer, parent, "SecondsToRun", secondsToRun);
    json_serializer_add_number(&json_serializer, parent, "StartTime", g_lostTimes.StartTimeNs);
    json_serializer_add_number(&json_serializer, parent, "TscFrequency", g_cyclesPerSec);
    
    parent = json_serializer_add_array(&json_serializer, parent, "Timeline");

    // Lost times
    for (i = 0; i < g_lostTimes.Cores; ++i)
    {
        curr = json_serializer_add_object(&json_serializer, parent, "");
        json_serializer_add_number(&json_serializer, curr, "Core", i);
        json_serializer_add_number(&json_serializer, curr, "TscOverhead", g_lostTimes.Times[i].TscOverhead);
        json_serializer_add_number(&json_serializer, curr, "Drift", -g_lostTimes.Times[i].Drift);
        curr = json_serializer_add_array(&json_serializer, curr, "LostTimes");

        for (j = 0; j < g_lostTimes.Times[i].Count; ++j)
        {
            json_serializer_add_number(&json_serializer, curr, "", g_lostTimes.Times[i].Lost[j]);
        }
    }

    parent = parent->parent;

    // Histogram
    parent = json_serializer_add_array(&json_serializer, parent, "Histogram");

    for (i = 0; i < NUM_HISTO_BINS; ++i)
    {
        _u64 count = 0;
        _u64 sum = 0;
        _u64 bin = 0;
        char* unit = "us";
        char* buf = &histogram_units[i][0];

        for (j = 0; j < g_lostTimes.Cores; ++j)
        {
            count += g_lostTimes.Times[j].Histogram[i];
            sum += g_lostTimes.Times[j].HistogramSum[i];
        }

        bin = 1ULL << i;

        format_bin(&bin, &unit);
        snprintf(buf, 64, "%llu%s", bin, unit);

        curr = json_serializer_add_object(&json_serializer, parent, "");
        json_serializer_add_string(&json_serializer, curr, "Bin", buf);
        json_serializer_add_number(&json_serializer, curr, "Count", count);
        json_serializer_add_number(&json_serializer, curr, "Sum", sum);
    }

    pretty_print = prettyPrint == 0 ? false : true;
    cstr = json_serializer_to_string(&json_serializer, pretty_print);
    seq_printf(m, "%s", cstr);

    vfree(cstr);

    json_serializer_deallocate(&json_serializer);

    return 0;
}

void format_bin(_u64* bin, char** unit)
{
    _u64 milli_bin = 1000ULL;
    _u64 second_bin = milli_bin * 1000ULL;
    _u64 minute_bin = second_bin * 60ULL;
    _u64 hour_bin = minute_bin * 60ULL;

    if (*bin > hour_bin)
    {
        *bin /= hour_bin;
        *unit = "h";
    }
    else if (*bin > minute_bin)
    {
        *bin /= minute_bin;
        *unit = "m";
    }
    else if (*bin > second_bin)
    {
        *bin /= second_bin;
        *unit = "s";
    }
    else if (*bin > milli_bin)
    {
        *bin /= milli_bin;
        *unit = "ms";
    }
}
# Overview
`GoldenEye` is a tool to detect missing CPU cycles. It works in a similar way to ftrace's hwlat_detector (https://docs.kernel.org/trace/hwlat_detector.html). It currently supports x86_64 and aarch64 (ARM64) architectures.

1) On bare metal hardware this usually means detecting and quantifying SMIs.
2) On virtual machines this will measure, with a high precision, CPU steal time.

`GoldenEye` runs as a ring-0 Linux Kernel Module. It runs a routine on each processor with interrupts disabled. This routine spins in a tight loop querying the TSC register and looking for gaps of at least 1us.
Sums per second for each processor, as well as histogram distributions of gap sizes are buffered into memory. Once complete, this buffer is serialized into a json file and is available via 

```
/proc/goldeneye
```

<b>WARNING</b>: GoldenEye will occupy all processors while in operation and will starve your kernel. Please do not run this tool unless you are aware of the associated risks.
<br><br>
# How to use
Tested on Debian/Ubuntu 18.04+ based distros:
1) git clone GoldenEye.git
2) Ensure required GNU compilation toolchain is installed on your machine. "sudo apt-get install build-essential" should be sufficient.
3) In git directory, build GoldenEye against your machine's Linux headers with "make".
4) To run, modify the MAKEFILE test recipe parameter "secondsToRun=10" to your desired run time (default is 10s).
5) "make test"
6) Examine contents of goldeneye.json. This file is in the following JSON format:
```json
{
  "Version": 5, // The current version of the JSON schema
  "Cores": 4, // Total number of cpu processors
  "SecondsToRun": 10, // Total number of seconds to run GoldenEye
  "StartTime": 1655876137938306409, // Timestamp of run start time (ktime_get_real_ns)
  "TscFrequency": 3491914000, // Frequency in hz of the time stamp clock (TSC for x86 and CNTVCT for aarch64)
  "Timeline": // Timeline of the measured data
  [
    {
      "Core": 0, // Processor number
      "TscOverhead": 34, // Overhead of the rdtsc instruction in clock cycles
      "Drift": 0, // Number of cycles that the TSC was detected going backwards (should be 0 unless there is a HW bug)
      "LostTimes": [ // Each line represents the accumulated TSC gaps (of size >=1us), in microseconds, for each second in the run
        100,
        ...
      ]
    },
    ...
  ],
  "Histogram": // Histogram of the distribution of individual TSC gaps during the run, in buckets of powers of 2
  [
    {
      "Bin": "1us",
      "Count": 100,
      "Sum": 100
    },
    ...
  ]
}
```
<br><br>
# License
`GoldenEye` is dual licensed under GPL 2.0 & MIT. Refer to the LICENSES folder for details.
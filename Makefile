obj-m += goldeneye.o
goldeneye-objs += main.o cpufreq.o proc.o losttime.o util.o vector.o string.o json.o ring.o
ccflags-y += -O2 -D ENABLE_BENCHMARK=0

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

test:
	sudo dmesg -C
	sudo insmod goldeneye.ko secondsToRun=10 prettyPrint=1
	cat /proc/goldeneye > goldeneye.json
	sudo rmmod goldeneye.ko
	sudo dmesg -T | grep "GoldenEye"
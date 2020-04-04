obj-m += goldeneye.o
goldeneye-objs += main.o cpufreq.o hostreporting.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

test:
	sudo dmesg -C
	sudo insmod goldeneye.ko secondsToRun=1
	sudo rmmod goldeneye.ko
	dmesg | grep "GoldenEye"
https://github.com/lemonsqueeze/uncached-ram-lkm

make

insmod uncached_ram.ko

tail /var/log/kern.log   (to get the major device num

mknod dev c 249 0

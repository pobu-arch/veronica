https://github.com/lemonsqueeze/uncached-ram-lkm

make

sudo insmod uncached_ram.ko

sudo tail /var/log/kern.log   (to get the major device num)

cd /home/bowen/; sudo mknod uncached_mem_dev c 236 0     (this is a fixed path in veronica)

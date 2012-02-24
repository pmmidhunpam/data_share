#/bin/bash

rmmod read_mem.ko
insmod read_mem.ko

hex_addr=`echo c0763a00 |  tr '[a-z]' '[A-Z]'`;
int_addr=`echo "obase=10; ibase=16; ${hex_addr}" | bc`



echo "${int_addr} 100 abc" > /sys/read_mem/cmd_attr
cat /sys/read_mem/cmd_attr


#!/bin/bash

args_num=$#
if  [ $args_num -ne 1 ];then
        echo "use format:  $0 <remote_ip>"
        echo "   example:  $0 192.168.0.136"
        exit 1
fi
remote_ip=$1


kernel_version=`cat include/generated/utsrelease.h | awk '{print $3}' | sed 's/"//g'`

echo "remote ip = "$remote_ip
echo "kernel version = "$kernel_version


#make install
echo scp System.map root@${remote_ip}:/boot/System.map-${kernel_version}
scp System.map root@${remote_ip}:/boot/System.map-${kernel_version}

echo scp ./arch/x86/boot/bzImage root@${remote_ip}:/boot/vmlinuz-${kernel_version}
scp ./arch/x86/boot/bzImage root@${remote_ip}:/boot/vmlinuz-${kernel_version}

echo scp .config root@${remote_ip}:/boot/config-${kernel_version}
scp .config root@${remote_ip}:/boot/config-${kernel_version}


#make modules_install
sudo make modules_install
ssh root@${remote_ip} "mkdir -p /lib/modules/${kernel_version}/kernel/"
ssh root@${remote_ip} "cd /lib/modules/${kernel_version}/;ln -s /home/work/linux-2.6.38 build"
ssh root@${remote_ip} "cd /lib/modules/${kernel_version}/;ln -s /home/work/linux-2.6.38 source"

echo scp  -r /lib/modules/${kernel_version}/kernel/ root@${remote_ip}:/lib/modules/${kernel_version}/
scp  -r /lib/modules/${kernel_version}/kernel/ root@${remote_ip}:/lib/modules/${kernel_version}/
echo scp  /lib/modules/${kernel_version}/modules* root@${remote_ip}:/lib/modules/${kernel_version}/
scp  /lib/modules/${kernel_version}/modules* root@${remote_ip}:/lib/modules/${kernel_version}/

#mkinitramfs
sudo mkinitramfs -o /boot/initrd.img-${kernel_version} ${kernel_version}
echo scp /boot/initrd.img-${kernel_version} root@${remote_ip}:/boot/initrd.img-${kernel_version}
scp /boot/initrd.img-${kernel_version} root@${remote_ip}:/boot/initrd.img-${kernel_version}


#update grub.cfg
ssh root@${remote_ip} "sed -i 's/vmlinuz-2.6.38[^ ]* /vmlinuz-${kernel_version} /g' /boot/grub/grub.cfg"
ssh root@${remote_ip} "sed -i 's/initrd.img-2.6.38[^ ]* /initrd.img-${kernel_version} /g' /boot/grub/grub.cfg"

ssh root@${remote_ip} "sync"

make -C linux-4.5.4/ M=/home/wittmann/git/kernel_course/monte_cassino/time || exit
sudo mount -o loop hdd.img hdd || exit
sudo cp monte_cassino/time/*.ko hdd/home/ || exit
sudo umount hdd
qemu-system-i386 -m 256M -kernel linux-4.5.4/arch/i386/boot/bzImage -append "root=/dev/sda" -hda hdd.img

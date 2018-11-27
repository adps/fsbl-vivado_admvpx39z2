echo "Making emmc.img"
dd if=/dev/zero of=emmc.img bs=512 count=128k
sleep 1
echo "Creating DOS partition"
sudo parted emmc.img mktable msdos
sleep 1
echo "Creating DOS partition"
sudo parted emmc.img mkpart p fat32 0% 100%
sleep 1
echo "kpartx"
sudo kpartx -a emmc.img
sleep 1
echo "mkfs"
sudo mkfs.vfat /dev/mapper/loop0p1
sleep 1
echo "Mounting"
sudo mount /dev/mapper/loop0p1 /mnt/
echo "Copying"
sudo cp BOOT.bin /mnt/BOOT.bin
sleep 1
sync
echo "Unmounting"
#sudo umount /dev/mapper/loop0p1
echo "kpartx again"
#sudo kpartx -d emmc.img
echo "Done"


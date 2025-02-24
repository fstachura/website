# riscv-pk

```
./buildroot/output/host/environment-setup
git clone https://github.com/riscv-software-src/riscv-pk
git checkout ac2c910b18c3e36cfd85080472e78ad2fe484325
mkdir build
cd build
../configure --host=riscv64-unknown-elf --prefix=/home/plates/projects/website/buildroot/output/host/
CFLAGS="-fno-stack-protector" make -j8
riscv64-buildroot-linux-gnu-objcopy -O binary bbl bbl.bin
```

# buildroot

e2fsck -f ./buildroot/output/images/rootfs.ext2
resize2fs -M ./buildroot/output/images/rootfs.ext2

# x11

* fbdev module - not required?
* xterm - mount devpts /dev/pts -t devpts -o mode=620



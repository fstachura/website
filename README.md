TinyEMU with patches to work with (patched) Linux 6.10.11 and Buildroot 2024.02.06

TinyEMU code by Fabrice Bellard taken from https://bellard.org/tinyemu/  
riscv-pk patches by Fabrice Bellard, taken from https://bellard.org/tinyemu/diskimage-linux-riscv-2018-09-23.tar.gz (see patches/riscv-pk.diff)

Some changes might have been also taken from the following repositories:
* https://github.com/drorgl/buildroot-tinyemu, https://github.com/drorgl/esp32-tinyemu (https://blog.drorgluska.com/2022/07/risc-v-linux-on-esp32.html)
* https://github.com/fernandotcl/TinyEMU
* https://github.com/michaelforney/tinyemu

# fixed issues

* kernel hang during boot - see notes/linux-bisect-unaligned-access, unaligned_access.patch, check_unaligned_access_all_cpus is either too slow, or causes a hang
* kernel hang after boot - see tinyemu/riscv_machine.c:649, notes/linux-bisect-mm-hang, linux probably started overwriting firmware, fixed by adding more memory to reserved-memory in dtb
* system hang after boot - some applications were waiting for higher entropy, fixed after adding haveged to system from buildroot
* sleep hangs, time does not progress - fixed by implementing rdtime csr read in emulator. probably should've been handled by bbl/pk, but for some reason isn't. clock is still too slow, but at least it progresses
* emscripten issues - `-O0` adds asserts, js functions called from c needed `__sig` entries

# setup notes (WIP)

## buildroot

Place contents of `buildroot` in unpacked buildroot-2024.02.06. Then run:

```
make -j8
. ./output/host/environment-setup
```

To make the kernel image compatible with TinyEMU:

```
riscv64-buildroot-linux-gnu-objcopy ./output/images/vmlinux  -O binary ./kernel-riscv64.bin
```

The following may be required if you change filesystem size:

```
e2fsck -f ./buildroot/output/images/rootfs.ext2
resize2fs -M ./buildroot/output/images/rootfs.ext2
```

## riscv-pk

Version in this repo was patched with patches from https://bellard.org/tinyemu/diskimage-linux-riscv-2018-09-23.tar.gz
(see patches/riscv-pk.diff)

```
git clone https://github.com/riscv-software-src/riscv-pk
git checkout ac2c910b18c3e36cfd85080472e78ad2fe484325
```

(now apply patches written by Fabrice Bellard on the repository)

Buildroot should build or download RISC-V toolchain after `make`, the environment-setup script adds it to PATH.
Assuming buildroot is in `../buildroot` and PWD is riscv-pk:

```
. ../buildroot/output/host/environment-setup
mkdir build
cd build
../configure --host=riscv64-unknown-elf --prefix=`realpath ../buildroot/output/host/`/
CFLAGS="-fno-stack-protector" make -j8
```

To make bbl image compatible with TinyEMU:

```
riscv64-buildroot-linux-gnu-objcopy -O binary bbl bbl.bin
```

## tinyemu

To convert rootfs created by buildroot to format compatible with TinyEMU:

```
./splitimg ../../buildroot-2024.02.6/output/images/rootfs.ext2 ./root-riscv64
```

In TinyEMU config, drive0.file should be set to `root-riscv64/blk.txt` (same directory as above).

## x11

* fbdev module - not required?
* xterm - mount devpts /dev/pts -t devpts -o mode=620


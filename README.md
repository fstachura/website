TinyEMU with patches to work with (patched) Linux 6.10.11.

TinyEMU code by Fabrice Bellard taken from https://bellard.org/tinyemu/

Some changes might have been also taken from the following repositories:
* https://github.com/drorgl/buildroot-tinyemu, https://github.com/drorgl/esp32-tinyemu (https://blog.drorgluska.com/2022/07/risc-v-linux-on-esp32.html)
* https://github.com/fernandotcl/TinyEMU
* https://github.com/michaelforney/tinyemu

# fixed issues

* kernel hang during boot - see notes/linux-bisect-unaligned-access, unaligned_access.patch, check_unaligned_access_all_cpus is either too slow, or causes a hang
* kernel hang after boot - see tinyemu/riscv_machine.c:649, notes/linux-bisect-mm-hang, linux probably started overwriting firmware, fixed by adding more memory to reserved-memory in dtb
* system hang after boot - some applications were waiting for higher entropy, fixed after adding haveged to system from buildroot
* sleep hangs, time does not progress - fixed by implementing rdtime csr read in emulator. probably should've been handled by bbl/pk, but for some reason isn't
* emscripten issues - `-O0` adds asserts, js functions called from c needed `__sig` entries

# setup notes (WIP)

## riscv-pk

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

## buildroot

e2fsck -f ./buildroot/output/images/rootfs.ext2
resize2fs -M ./buildroot/output/images/rootfs.ext2

## x11

* fbdev module - not required?
* xterm - mount devpts /dev/pts -t devpts -o mode=620


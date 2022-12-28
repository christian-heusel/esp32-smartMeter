# Debugging

## remote debugging on the ESP

> TODO(Chris): Write how to attach a debugger to the ESP

## with `qemu`

There is a fork of Qemu which supports ESP32 hardware: https://github.com/espressif/qemu
It can be installed via the AUR on Archlinux for example: https://aur.archlinux.org/packages/qemu-esp-xtensa-git

```
$ idf.py build
$ (cd build; esptool.py --chip esp32 merge_bin --fill-flash-size 4MB -o flash_image.bin @flash_args)
$ qemu-esp-xtensa -nographic -s -S -machine esp32 -drive file=./build/flash_image.bin,if=mtd,format=raw
$ idf.py gdbtui --gdbinit ./qemu_gdbinit
```

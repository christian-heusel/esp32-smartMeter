# Debugging

## remote debugging on the ESP

> TODO(Chris): Write how to attach a debugger to the ESP

## with `qemu`

```
$ idf.py build
$ (cd build; esptool.py --chip esp32 merge_bin --fill-flash-size 4MB -o flash_image.bin @flash_args)
$ qemu-esp-xtensa -nographic -s -S -machine esp32 -drive file=./build/flash_image.bin,if=mtd,format=raw
$ idf.py gdbtui --gdbinit ./qemu_gdbinit
```

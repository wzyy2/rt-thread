

esptool.py elf2image rtthread.elf -fs 16m -fm qio -ff 40m

# download
esptool.py -p /dev/tty.SLAB_USBtoUART --baud 115200 write_flash  -fs 16m -fm qio -ff 40m 0x20000 rtthread.elf-0x20000.bin 0x00000 rtthread.elf-0x00000.bin
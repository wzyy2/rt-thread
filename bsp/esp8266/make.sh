# build sdklib
cd sdklib  ; 
make all; 
cd ..;


# build rtt
scons

# xtensa-lx106-elf-gcc  -O2 -nostdlib  -Wl,--no-check-sections -Wl,-gc-sections -Wl,-static -Wl,-Map=rtthread-esp.map,-cref,-u,call_user_start \
#                      -Wl,--start-group -L ./build/sdklib/ -L ./ -L ./libc/xtensa-lx106-elf/lib/ \
#                      -Wl,-whole-archive -lrtthread  -Wl,-no-whole-archive -lc -lm -lhal -lmain -lnet80211 -lphy -lpp -lwpa -lgcc -Wl,--end-group \
#                      -T esp_nonota.ld  -T esp_rom.ld  -T esp_common.ld -o rtthread.elf

 # -Wl,--whole-archive
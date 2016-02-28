ROOT ?= ../
BUILD_DIR ?= $(ROOT)build/

Q := @

CROSS ?= xtensa-lx106-elf-

AR = $(CROSS)ar
CC = $(CROSS)gcc
CPP = $(CROSS)cpp
LD = $(CROSS)gcc
NM = $(CROSS)nm
C++ = $(CROSS)g++
SIZE = $(CROSS)size
OBJCOPY = $(CROSS)objcopy
OBJDUMP = $(CROSS)objdump

SDK_LIBS ?= main net80211 phy pp wpa
SDK_PROCESSED_LIBS = $(addsuffix .a,$(addprefix $(BUILD_DIR)sdklib/lib,$(SDK_LIBS)))

all: $(SDK_PROCESSED_LIBS)	
	
$(BUILD_DIR)sdklib:
	$(Q) mkdir -p $@

# Remove comment lines from <libname>.remove files
$(BUILD_DIR)sdklib/%.remove: $(ROOT)sdklib/%.remove | $(BUILD_DIR)sdklib
	$(Q) grep -v "^#" $< | cat > $@

# Stage 1: remove unwanted object files listed in <libname>.remove alongside each library
$(BUILD_DIR)sdklib/%_stage1.a: $(ROOT)sdklib/%.a $(BUILD_DIR)sdklib/%.remove | $(BUILD_DIR)sdklib
	@echo "SDK processing stage 1: Removing unwanted objects from $<"
	$(Q) cat $< > $@
	$(Q) $(AR) d $@ @$(word 2,$^)

# Stage 2: Redefine all SDK symbols as sdk_, weaken all symbols.
$(BUILD_DIR)sdklib/%.a: $(BUILD_DIR)sdklib/%_stage1.a $(ROOT)sdklib/allsymbols.rename
	@echo "SDK processing stage 2: Renaming symbols in SDK library $< -> $@"
	$(Q) $(OBJCOPY) --redefine-syms $(word 2,$^) --weaken $< $@


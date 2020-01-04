# MAKEFILE

PROJECT_NAME					=	GERANIUM

TOOLCHAIN_PREFIX				=	riscv64-unknown-elf-
#riscv-none-embed-

GCC								=	$(TOOLCHAIN_PREFIX)gcc
OBJCOPY							=	$(TOOLCHAIN_PREFIX)objcopy
VERILOG_CONV					=	../../../Tools/TABLEGEN/BIN/x64/Release/TABLEGEN.EXE

PROJECT_FILES					=	main.c \
									config.h \
									geranium.c \
									geranium.h \
									irq.c \
									./CONSOLE/help.c \
									./CONSOLE/command.c \
									./CONSOLE/console.c \
									./CONSOLE/console.h \
									./CONSOLE/error.c \
									./SYS/tiny-malloc.c \
									./SYS/disk.c \
									./SYS/start.S \
									./SYS/initial.c \
									./SYS/block_io.c \
									./SYS/pff.c \
									./SYS/atomic.c \
									./SYS/div.S \
									./SYS/muldi3.S \
									./SYS/save-restore.S \
									./SYS/custom_ops.S \
									./SYS/diskio.h \
									./SYS/integer.h \
									./SYS/pff.h \
									./SYS/pffconf.h \
									./SYS/riscv-asm.h \
									./SYS/sfp-machine.h

# set to 4 for simulation 18 for real
FIRMWARE_COUNTER_BITS			=	18

CFLAGS += -W
CFLAGS += -Wall
CFLAGS += --std=gnu99
CFLAGS += -O1
CFLAGS += -lgcc
CFLAGS += -mabi=ilp32
CFLAGS += -march=rv32im
CFLAGS += -fdata-sections
CFLAGS += -ffunction-sections
CFLAGS += -funsigned-char
CFLAGS += -funsigned-bitfields
CFLAGS += -fno-diagnostics-show-caret
CFLAGS += -static
CFLAGS += -mstrict-align
#CFLAGS += -v
CFLAGS += -ffreestanding
CFLAGS += -nostartfiles
#CFLAGS += -Wl,--script=firmware.lds
#CFLAGS += -flto
#CFLAGS += -nostdlib
#CFLAGS += -nodefaultlibs
CFLAGS += -lgloss

CFLAGS += \
  -DF_CPU=150000000



all: $(PROJECT_NAME).bin $(PROJECT_NAME).hex $(PROJECT_NAME).ROM $(PROJECT_NAME).v Makefile

## -------------------
## firmware generation
#--specs=nano.specs

$(PROJECT_NAME).elf: $(PROJECT_FILES) Makefile
	$(GCC) \
		-DSHIFT_COUNTER_BITS=$(FIRMWARE_COUNTER_BITS) $(CFLAGS)  --specs=nosys.specs \
		-o $@ $(PROJECT_FILES) \
		-Wl,-Bstatic,-T,linkscript.lds,-Map,$(PROJECT_NAME).map,--strip-debug

$(PROJECT_NAME).ROM: $(PROJECT_NAME).bin
	cp $(PROJECT_NAME).bin $(PROJECT_NAME).ROM

$(PROJECT_NAME).v: $(PROJECT_NAME).bin
	$(VERILOG_CONV) -e -i $(PROJECT_NAME).bin -o $(PROJECT_NAME).v -v

$(PROJECT_NAME).bin: $(PROJECT_NAME).elf
	$(OBJCOPY) -O binary $< $@ --remove-section=.comment --remove-section=.note
	chmod -x $@

$(PROJECT_NAME).hex: $(PROJECT_NAME).elf
	$(OBJCOPY) -O ihex $(PROJECT_NAME).elf $(PROJECT_NAME).hex

clean:
	rm -f $(PROJECT_NAME).elf $(PROJECT_NAME).map $(PROJECT_NAME).bin $(PROJECT_NAME).hex $(PROJECT_NAME).ROM $(PROJECT_NAME).v

.PHONY: all clean

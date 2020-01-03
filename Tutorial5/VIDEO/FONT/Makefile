NAME	= FONT

OUTPUT	=	../$(NAME).mif
BUILD_DIR	=	fontbuild

CC      = riscv64-unknown-elf-gcc
CXX     = riscv64-unknown-elf-g++
LD		= riscv64-unknown-elf-ld

all: $(BUILD_DIR) $(OUTPUT) 

$(OUTPUT) : $(BUILD_DIR)/$(NAME).mif
	cp -f --remove-destination $(BUILD_DIR)/$(NAME).mif $(OUTPUT)

$(BUILD_DIR)/$(NAME).mif : $(BUILD_DIR)/$(NAME).S68
	srec_cat $< -o $@ -mif 8

GRNM_CFLAGS = -c -O0
GRNM_LDFLAGS = --oformat=srec -nostartfiles -nostdlib

$(BUILD_DIR)/font.o : font.s
	$(CC) $(GRNM_CFLAGS) $< -o $@

GRNM_OBJECTS = \
	$(BUILD_DIR)/font.o

$(BUILD_DIR)/$(NAME).S68: $(GRNM_OBJECTS)
	$(LD) -Tlinkscript.ld $(GRNM_LDFLAGS) -o $@ $(GRNM_OBJECTS)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

# This clean rule can only be used with a properly setup Cygwin environment. Sorry.
.PHONY : clean
clean:
	rm -f *.x *.o *.map core *.d *.BIN $(BUILD_DIR)/*.*
	rmdir $(BUILD_DIR)
	rm -f $(OUTPUT)
	

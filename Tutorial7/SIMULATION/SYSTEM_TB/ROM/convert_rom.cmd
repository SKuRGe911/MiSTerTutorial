@echo off
srec_cat GERANIUM.elf -o GERANIUM.mif -mif 16
srec_cat GERANIUM.elf -o GERANIUM.hex -Intel
srec_cat GERANIUM.elf -o GERANIUM.BIN -Binary
pause
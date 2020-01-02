OUTPUT_FORMAT(srec)

MEMORY
{
  ram1 (r) : ORIGIN = 0x0, LENGTH = 2K
}

SECTIONS
{
	.text : { fontbuild/font.o } > ram1
}

/**/
MEMORY
{
  mem   (rwx)	: ORIGIN = 0x0000000, LENGTH = 0x1000000-0x0000100
  heap  (rw)	: ORIGIN = 0x1000000, LENGTH = 0x8000000-0x1000000
}

__top_mem				=	ORIGIN(mem) + LENGTH(mem);
__heap_mem				=	ORIGIN(heap);
__heap_top				=	ORIGIN(heap) + LENGTH(heap);
__builtin_frame_address	=	ORIGIN(heap) + LENGTH(heap);

ENTRY(_start)

SECTIONS
{
	
	.text : ALIGN(4)
	{
		*(.init*)
		*(.text.entry)
		*(.text*)
		. = ALIGN(4);
	} > mem

	.rodata : ALIGN(4)
	{
		*(.rodata)
		*(.rodata.*)
		. = ALIGN(4);
	} > mem

	. = ALIGN(4);
	_etext = .;

	.data : ALIGN(4)
	{
		*(.data)
		*(.data.*)
		*(.sdata)
		*(.sdata.*)
		. = ALIGN(4);
	} > mem

	.bss : ALIGN(4)
	{
		_bss = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		_ebss = .;
		PROVIDE(_end = .);
	} > mem

  . = ALIGN(32 / 8);

	PROVIDE(_stack_top = __top_mem);

  . = ALIGN(32 / 8);

	PROVIDE(__malloc_start = __heap_mem);

  . = ALIGN(32 / 8);

}


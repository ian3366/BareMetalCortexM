ENTRY(Reset_Handler)

MEMORY
{
	FLASH(rx):ORIGIN = 0x08000000,LENGTH = 1024K
	SRAM(rwx):ORIGIN = 0x20000000,LENGTH = 128K
}

__max_heap_size = 0x400; /*A symbol declaration. Not a Variable */
__max_stack_size = 0x200;/*A symbol declaration. Not a Variable */

SECTIONS
{
	.text :
	{
		*(.isr_vector)
		*(.text)
		*(.rodata)
		. = ALIGN(4); /*align the value of location counter*/
		_etext = .;/* location counter will store address end of section */
	}> FLASH
	.data :
	{
		_sdata = .;/* location counter always track vma -> SRAM */
		*(.data)
		. = ALIGN(4); /*align the value of location counter*/
		_edata = .;
	}> SRAM AT> FLASH
	.bss :
	{	
		_sbss = .;
		*(.bss)
		*(COMMON)
		. = ALIGN(4); /*align the value of location counter*/
		_ebss = .;
	}>SRAM
}
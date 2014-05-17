ENTRY (_start)

SECTIONS
{
   . = 0x00014000;

   .text ALIGN (0x1000) :
   {
      *(.text)
   }

   .rodata ALIGN (0x1000) :
   {
       *(.rodata*)
   }

   .data ALIGN (0x1000) :
   {
        *(.data)
   }

   .bss :
   {
        sbss = .;
        *(COMMON)
        *(.bss)
        ebss = .;
   }
}

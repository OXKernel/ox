ENTRY (_start)

SECTIONS
{
   . = 0x00100000;

   .text ALIGN (0x1000) :
   {
      *(.text)
   }

}

ENTRY (atom_bit_set)

SECTIONS
{
   . = 0x00100000;

   .text ALIGN (0x1000) :
   {
      *(.text)
   }

}

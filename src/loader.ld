OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)
ENTRY(start)
phys = 0x00090100;
dataseg = data >> 4;
SECTIONS
{
  .text phys : AT(phys) {
    code = .;
    *(.text)
    *(.rodata)
  }
  .data : AT(phys + (data - code))
  {
    data = .;
    *(.data)
  }
  .bss : AT(phys + (bss - code))
  {
    bss = .;
    *(.bss)
  }
  end = .;
}
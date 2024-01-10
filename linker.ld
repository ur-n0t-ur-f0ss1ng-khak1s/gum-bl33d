/* This is going to be the linker script for describing where our sections will end up in memory */
/* In our first two lines we tell the linker that we want an x86_64 ELF64 output file */
OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)

/* the symbol '_start' will be our entry point */
ENTRY(_start)

/* here we define the program headers so the bootloader will give us the right MMU permissions */
PHDRS
{
  text  PT_LOAD FLAGS((1 << 0) | (1 << 2)) ; /* execute + read */
  rodata  PT_LOAD FLAGS((1 << 2)) ; /* read only */
  data  PT_LOAD FLAGS((1 << 1) | (1 << 2)) ; /* write + read */
  dynamic  PT_DYNAMIC FLAGS((1 << 1) | (1 << 2)) ; /* dynamic PHDR for relocations */
}

SECTIONS
{
  /* we want to be placed in the topmost 2GiB of the address space, for optimisations and because that is what the Limine specification mandates. */
  /* Any address in this reason works, but we use 0xffffffff80000000 because it is the beginning of the region. */
  . = 0xffffffff80000000;

  .text : {
    *(.text .text.*)
  } :text

  /* move to the next memory page for .rodata */
  . += CONSTANT(MAXPAGESIZE);

  .rodata : {
    *(.rodata .rodata.*)
  } :rodata

  /* move to the next memory page for .data */
  . += CONSTANT(MAXPAGESIZE);

  .data : {
    *(.data .data.*)
  } :data

  /* dynamic section for relocations, both in its own PHDR and inside data PHDR */
  .dynamic : {
    *(.dynamic)
  } :data :dynamic

  /* Important: .bss needs to be the last thing mapped to :data, otherwise lots of unnecessary zeros will be written to the binary. */
  /* place new maps above this */
  .bss : {
    *(.bss .bss.*)
    *(COMMON)
  } :data

  /* discard .note.* and .eh_frame because they may cause issues on some hosts. */
  /DISCARD/ : {
    *(.eh_frame)
    *(.note .note.*)
  }
}

OUTPUT_ARCH (mips)
SECTIONS
{
   __romPos = 0;
   _bootSegmentStart = ADDR(.boot); _bootSegmentRomStart = __romPos; .boot 0x04000000 : AT(__romPos)
   {
      build/asm/rom_header.o(.text);
      build/boot.6102.o(.data);
   }
   _bootSegmentEnd = ADDR(.boot) + SIZEOF(.boot); _bootSegmentRomEnd = __romPos + SIZEOF(.boot); __romPos += SIZEOF(.boot);
   _codeSegmentStart = ADDR(.code); _codeSegmentRomStart = __romPos; .code 0x80025c00 : AT(__romPos) SUBALIGN(16)
   {
  build/asm/entry.o(.start);
      /usr/lib/n64/nusys/nusys_rom.o(.text);
      build/src/main*.o(.text);
      */libultra_d.a:*.o(.text);
      */libnusys_d.a:*.o(.text);
      */libnustd.a:*.o(.text);
      */libgcc.a:*.o(.text);
      /usr/lib/n64/PR/rspboot.o(.text);
      /usr/lib/n64/PR/gspF3DEX2.fifo.o(.text);
      /usr/lib/n64/PR/gspL3DEX2.fifo.o(.text);
      /usr/lib/n64/PR/gspF3DEX2.Rej.fifo.o(.text);
      /usr/lib/n64/PR/gspF3DEX2.NoN.fifo.o(.text);
      /usr/lib/n64/PR/gspF3DLX2.Rej.fifo.o(.text);
      /usr/lib/n64/PR/gspS2DEX2.fifo.o(.text);
      /usr/lib/n64/nusys/nusys_rom.o(.data*);
      build/src/main*.o(.data*);
      */libultra_d.a:*.o(.data*);
      */libnusys_d.a:*.o(.data*);
      /usr/lib/n64/PR/rspboot.o(.data*);
      /usr/lib/n64/PR/gspF3DEX2.fifo.o(.data*);
      /usr/lib/n64/PR/gspL3DEX2.fifo.o(.data*);
      /usr/lib/n64/PR/gspF3DEX2.Rej.fifo.o(.data*);
      /usr/lib/n64/PR/gspF3DEX2.NoN.fifo.o(.data*);
      /usr/lib/n64/PR/gspF3DLX2.Rej.fifo.o(.data*);
      /usr/lib/n64/PR/gspS2DEX2.fifo.o(.data*);
      /usr/lib/n64/nusys/nusys_rom.o(.*rodata*);
      build/src/main*.o(.*rodata*);
      */libultra_d.a:*.o(.*rodata*);
      */libnusys_d.a:*.o(.*rodata*);
      */libnustd.a:*.o(.*rodata*);
      */libgcc.a:*.o(.*rodata*);
   }
   _codeSegmentEnd = ADDR(.code) + SIZEOF(.code); _codeSegmentRomEnd = __romPos + SIZEOF(.code); __romPos += SIZEOF(.code);
   _codeSegmentBssStart = ADDR(.code.noload); .code.noload (NOLOAD) :
   {
      /usr/lib/n64/nusys/nusys_rom.o(COMMON);
      /usr/lib/n64/nusys/nusys_rom.o(.scommon);
      /usr/lib/n64/nusys/nusys_rom.o(.*bss*);
      build/src/main*.o(.*bss*);
      */libultra_d.a:*.o(COMMON);
      */libultra_d.a:*.o(.scommon);
      */libultra_d.a:*.o(.*bss*);
      */libnusys_d.a:*.o(COMMON);
      */libnusys_d.a:*.o(.scommon);
      */libnusys_d.a:*.o(.*bss*);
   }
   _codeSegmentBssEnd = ADDR(.code.noload) + SIZEOF(.code.noload); _codeSegmentBssSize = SIZEOF(.code.noload);
   _tomiSegmentStart = ADDR(.tomi); _tomiSegmentRomStart = __romPos; .tomi 0x01000000 : AT(__romPos)
   {
      build/data/keiki/keiki.o(.data);
   }
   _tomiSegmentEnd = ADDR(.tomi) + SIZEOF(.tomi); _tomiSegmentRomEnd = __romPos + SIZEOF(.tomi); __romPos += SIZEOF(.tomi);
   _vajssitiSegmentStart = ADDR(.vajssiti); _vajssitiSegmentRomStart = __romPos; .vajssiti 0x02000000 : AT(__romPos)
   {
      build/data/higu.o(.data);
   }
   _vajssitiSegmentEnd = ADDR(.vajssiti) + SIZEOF(.vajssiti); _vajssitiSegmentRomEnd = __romPos + SIZEOF(.vajssiti); __romPos += SIZEOF(.vajssiti);
   /DISCARD/ :
   {
      *(*);
   }
}

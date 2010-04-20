
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY {
      ram   : ORIGIN = 0x00000000, LENGTH =  0x8000000
}

SECTIONS {
      .text : {
	    _ramstart = .;
            _stext = . ;
	/* crt0.o contains the _start routine, which calls main() */
	    crt0.o(.text)
            *(.text)
            _etext = . ;
            . = ALIGN (4);
      } > ram
      .data : {
            _sdata = . ;
            *(.data)
	    *(.rodata)
            _edata = . ;
      } > ram
      .bss : {
            _sbss = . ;
            *(.bss)
            *(COMMON)
            _ebss = . ;
            _ramend = .;
      } > ram
}

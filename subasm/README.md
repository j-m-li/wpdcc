## What is sasm?

    Small Assembler (SASM) is a very small assembeler for the i80x
    line of processors.

## License

    All source code is Public Domain.

## Obtain the source code

    git clone https://git.candlhat.org/sasm.git

## Building

    BSD:
    
        Make sure you have gcc and gmake installed then run gmake -f Makefile.unix.
    
    Linux:
    
        Make sure you have gcc and make installed then run make -f Makefile.unix.
    
    macOS:
    
        Make sure you have xcode command line tools installed then run
        make -f Makefile.unix.
    
    Windows:
    
        Make sure you have mingw installed and the location within your PATH variable
        then run mingw32-make.exe -f Makefile.w32.

## Usage

    Example (comments and pre-process directives):
    
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; This is a comment.
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        %define     SOMETHING       5
        
        %if SOMETHING == 5
            ...
        %endif

    Code Directives:
    
        .8086                           -   Only allow 8086 instructions.
        .186                            -   Allow 186 and below instructions.
        .286/.286p                      -   Allow 286 and below instructions.
        .386/.386p                      -   Allow 386 and below instructions.
        .486/.480p                      -   Allow 486 and below instructions.
        .586                            -   Allow 586 and below instructions.
        .686                            -   Allow 686 and below instructions.
        
        .code/text                      -   Output bytes to the code/text section/segment.
        .data                           -   Output bytes to the data section/segment.
        .bss                            -   Output bytes to the bss section/segment.
        
        align                           -   Align the code to a specific boundary.
        extern/extrn                    -   Define a symbol that's in a different file.
        global/public                   -   Make the specified symbol available
        
        db                              -   Output a byte to the object file.
        dw                              -   Output a word to the object file.
        dd                              -   Output a dword to the object file.
    
    If you want to pad the file, you need the following:
    
        db [padding amount] dup [byte to pad with]

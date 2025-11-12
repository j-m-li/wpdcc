#******************************************************************************
# @file             Makefile.w32
#******************************************************************************
SRCDIR              ?=  $(CURDIR)
VPATH               :=  $(SRCDIR)

SRC                 :=   as.c coff.c cstr.c elks.c expr.c fixup.c frag.c hashtab.c intel.c kwd.c lex.c lib.c list.c listing.c ll.c macro.c process.c report.c section.c symbol.c vector.c

all: sasm.exe

sasm.exe: $(SRC)

	wcl -DNO_LONG_LONG -fe=$@ $^

clean:

	for /r %%f in (*.obj) do ( if exist %%f ( del /q %%f ) )
	if exist sasm.exe ( del /q sasm.exe )

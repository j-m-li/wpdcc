
warning:
	@echo please run ./configure.cmd

SOURCEDIR=../src
BUILDDIR=.
PATH:=$(PATH):.


SOURCES= \
	../src/as.c \
	../src/coff.c \
	../src/cstr.c \
	../src/elks.c \
	../src/expr.c \
	../src/hashtab.c \
	../src/lex.c \
	../src/list.c \
	../src/listing.c \
	../src/ll.c \
	../src/macro.c \
	../src/process.c \
	../src/report.c \
	../src/section.c \
	../src/symbol.c \
	../src/vector.c \
	../src/intel.c \

TESTS= \
	../tests/00000.c \
	../tests/00001.c \
	../tests/00002.c

OUTS=$(subst ../tests,$(BUILDDIR),$(TESTS:.c=.S))

ifeq ($(CC), wpdcc)
CFLAGS=-DNO_LONG_LONG
endif

ifeq ($(CC), clang)
CFLAGS=-Wall -ansi -D_POSIX_C_SOURCE=200809L -D_CRT_NONSTDC_NO_DEPRECATE=1 \
      -D_CRT_SECURE_NO_WARNINGS=1 -g -D_DEFAULT_SOURCE=1 -D_BSD_SOURCE=1
endif

ifeq ($(CC), gcc)
CFLAGS=-Wall  -D_POSIX_C_SOURCE=200809L -D_CRT_NONSTDC_NO_DEPRECATE=1 \
      -D_CRT_SECURE_NO_WARNINGS=1 -g -D_DEFAULT_SOURCE=1 -D_BSD_SOURCE=1
endif

ifeq ($(CC), cl)
CFLAGS= /nologo /D_CRT_NONSTDC_NO_DEPRECATE=1 \
      /D_CRT_SECURE_NO_WARNINGS=1
endif


ifeq ($(CC), cl)
OBJS=$(subst $(SOURCEDIR),$(BUILDDIR),$(SOURCES:.c=.obj))
else
OBJS=$(subst $(SOURCEDIR),$(BUILDDIR),$(SOURCES:.c=.o))
endif

top: build/xmake.exe
	(cd build && xmake.exe all)

build/xmake.exe:
	configure.cmd

all: pdas.exe 
	echo done

ifeq ($(CC), cl)
pdas.exe: $(OBJS)
        $(CC) $(CFLAGS) /Fe:$@ $^
        echo $^

./%.obj: ../src/%.c
        @$(CC) $(CFLAGS) /c /Fo:$@ $<
else
ifeq ($(CC), wpdcc)
pdas.exe: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

./%.o: ../src/%.c
	@rm -f $*.s
	@$(CC) -S $(CFLAGS) -o $*.s $<
	@$(AS) -m32 -o $@ $*.s
	@rm -f $*.s
else
pdas.exe: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

./%.o: ../src/%.c
	@$(CC) -S $(CFLAGS) -o $*.s $< 
	@$(AS) -o $@ $*.s
	@rm -f $*.s
endif
endif

test: cleartest ac90.exe diff.exe foreach.exe $(OUTS)
	echo xmake  'noquote' "double\"_ _" "\\ backslash"

./%.S: ../tests/%.c
	@./ac90.exe $< $@ > $*.log
	@gcc -m32 $@
	@./a.out > $*.res
	@./diff.exe $<.expected $*.res

cleartest:
	rm -f *.S
	rm -f *.log
	rm -f *.res

install:
	echo ...

flash:
	echo ...

sym:
	echo ...

clean:
	rm -f *.o 
	rm -f *.obj
	rm -f xmake.exe pdas.exe
	rm -rf build/



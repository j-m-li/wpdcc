MV=4
NV=9
EN=4
VENDOR=XOS

.M86.OBJ:
	XMAC $(@DPN)

.C.OBJ:
	owxc /hc /d2 /i=xosinc:\owxc\ /zl /zld /zq /5s /w3 /we /fp3 /s \
		/zl /zld /mf /zc /ze /of /okhs \
		/dMV=$(MV) /dNV=$(NV) /dEN=$(EN) /nm=$(@DPN) $(@DPN).c
#	WCC386 /D1 /I=XOSINC:\WCX\ /ZQ /3S /J /W3 /WE /FP3 /S /Od \
#		/DMV=$(MV) /DNV=$(NV) /DEN=$(EN) /NM=$(@DPN) $(@DPN)

all:	xxlink.run # exe2run.run run2exe.run rundmp.run symdmp.run
	@ECHO All done.

CLEAN:
	-DELETE *.BAK
	-DELETE *.DMP
	-DELETE *.ERR
	-DELETE *.EXE
	-DELETE *.LKE
	-DELETE *.LST
	-DELETE *.MAP
	-DELETE *.OBJ
	-DELETE *.RST
	-DELETE *.RUN
	-DELETE *.SYM
	-DELETE *.TMP

xxlink.run:	xlink.obj passone.obj fileone.obj map.obj \
		passtwo.obj filetwo.obj output.obj io.obj symtbl.obj \
		utility.obj
#	gecko \xossrcold\xlink-4-9\xxlink xoslib:\xos\defsegs xlink passone fileone map \
	xlink xoslib:\xos\defsegs xlink passone fileone map \
		passtwo filetwo output io symtbl utility \
		xoslib:\xos\libx01 xoslib:\xos\libc01 \
#		/maj=$(MV) /min=$(NV) /edi=$(EN) /ven=$(VENDOR) \
		/out=xxlink /map=xxlink /sym=xxlink
	copy /over xxlink.run newcmd:xlink.run

xlink.obj:	xlink.c
passone.obj:	passone.c
fileone.obj:	fileone.c
map.obj:	map.c
passtwo.obj:	passtwo.c
filetwo.obj:	filetwo.c
output.obj:	output.c
io.obj:		io.c
symtbl.obj:	symtbl.c
utility.obj:	utility.c

rundmp.run:	rundmp.obj
	xlink xoslib:\xos\defsegs rundmp \
		xoslib:\xos\libx01 xoslib:\xos\libc01 \
		/out=rundmp /map=rundmp /sym=rundmp
	copy /over rundmp.run newcmd:rundmp.run

rundmp.obj:	rundmp.c

symdmp.run:	symdmp.obj
	xlink xoslib:\xos\defsegs symdmp \
		xoslib:\xos\libx01 xoslib:\xos\libc01 \
		/out=symdmp /map=symdmp /sym=symdmp
	copy /over symdmp.run newcmd:symdmp.run

symdmp.obj:	symdmp.c

EXE2RUN.RUN:	EXE2RUN.OBJ
	XLINK XOSLIB:\XOS\DEFSEGS EXE2RUN \
		XOSLIB:\XOS\LIBX01 XOSLIB:\XOS\LIBC01 \
		/OUT=EXE2RUN /MAP=EXE2RUN /SYM=EXE2RUN
	COPY /OVER EXE2RUN.RUN NEWCMD:EXE2RUN.RUN

EXE2RUN.OBJ:	EXE2RUN.C

RUN2EXE.RUN:	RUN2EXE.OBJ
	XLINK XOSLIB:\XOS\DEFSEGS RUN2EXE \
		XOSLIB:\XOS\LIBX01 XOSLIB:\XOS\LIBC01 \
		/OUT=RUN2EXE /MAP=RUN2EXE /SYM=RUN2EXE
	COPY /OVER RUN2EXE.RUN NEWCMD:RUN2EXE.RUN

RUN2EXE.OBJ:	RUN2EXE.C

RUN2BIN.RUN:	RUN2BIN.OBJ
	XLINK XOSLIB:\XOS\DEFSEGS RUN2BIN \
		XOSLIB:\XOS\LIBX01 XOSLIB:\XOS\LIBC01 \
		/OUT=RUN2BIN /MAP=RUN2BIN /SYM=RUN2BIN
	COPY /OVER RUN2BIN.RUN newCMD:RUN2BIN.RUN

RUN2BIN.OBJ:	RUN2BIN.C

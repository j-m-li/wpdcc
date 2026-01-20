:<<"::CMDGOTO"
@echo off
goto :CMDENTRY
rem https://stackoverflow.com/questions/17510688/single-script-to-run-in-both-windows-batch-and-linux-bash
::CMDGOTO

echo "========== wpasm configure ${SHELL} ================="
DIR=$(dirname "$0")
(mkdir -p ${DIR}/build;)
rm -f ${DIR}/build/*.exe
rm -f ${DIR}/build/*.log
(cd ${DIR}/build;clang -D_FILE_OFFSET_BITS=64 ../tools/xmake.c -o xmake.exe >>build.log 2>&1)
if [ -e build/xmake.exe ]; then
	echo "CC=wpdcc" > ${DIR}/build/Makefile
	echo "AS=clang -c" >> ${DIR}/build/Makefile
	echo "LD=clang" >> ${DIR}/build/Makefile
else
	(cd ${DIR}/build;gcc -D_FILE_OFFSET_BITS=64 ../tools/xmake.c -o xmake.exe >>build.log 2>&1)
	echo "CC=gcc" > ${DIR}/build/Makefile
	echo "AS=gcc -c" >> ${DIR}/build/Makefile
	echo "LD=gcc " >> ${DIR}/build/Makefile
fi

if [ -e build/xmake.exe ]; then
	echo "run: all" >> ${DIR}/build/Makefile
	echo "" >> ${DIR}/build/Makefile
	cat Makefile >> ${DIR}/build/Makefile
else
	echo "ERROR: C compiler not found"
	exit -1
fi

# (cd ${DIR}/build;./build $1 $2)
echo "now cd build; ./xmake.exe all"
exit $?
:CMDENTRY

echo ============= ac90 configure %COMSPEC% ============
set OLDDIR=%CD%
rem chdir /d %1
mkdir build  >nul 2>&1
chdir /d build
if "%CD%" == "%OLDDIR%" (
	rem echo build %1 %2
	echo fatal error! 
 	goto :ENDOFFILE
)
del *.exe >build.log 2>&1

clang -D_FILE_OFFSET_BITS=64 -D_CRT_SECURE_NO_WARNINGS=1 ..\tools\xmake.c -o xmake.exe >>build.log 2>&1
IF EXIST xmake.exe (
	echo CC=clang > Makefile
	echo AS=clang -c >> Makefile
	echo LD=clang >> Makefile
	echo run: all >> Makefile
	type ..\tools\nl.txt >> Makefile
	type ..\Makefile >> Makefile
	echo "now cd build; ./xmake all"
	goto :ENDOFFILE
)
cl /Z7 /DDEBUG /D_FILE_OFFSET_BITS=64 %~dp0\tools\xmake.c /D_CRT_SECURE_NO_WARNINGS=1 /Fe:xmake.exe >>build.log 2>&1
IF EXIST xmake.exe (
	echo|set /p="CC=cl" > Makefile
	type ..\tools\nl.txt >> Makefile
	echo|set /p="AS=cl" >> Makefile
	type ..\tools\nl.txt >> Makefile
	echo|set /p="LD=cl" >> Makefile
	type ..\tools\nl.txt >> Makefile
	echo|set /p="run: all" >> Makefile
	type ..\tools\nl.txt >> Makefile
	type ..\tools\nl.txt >> Makefile
	type ..\Makefile >> Makefile
rem	copy ..\Makefile.vc Makefile
	echo "now cd build; .\xmake all"
	goto :ENDOFFILE
) ELSE (
	echo ERROR: C compiler not found
)
:ENDOFFILE
chdir /d %OLDDIR%



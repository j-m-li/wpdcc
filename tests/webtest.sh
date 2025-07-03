#	clang --target=wasm32 -S -I d/ -o tk.s d/tk.c
#	clang --target=wasm32 -c -I d/ -o tk.o tk.s
#	wasm-ld -allow-undefined --no-entry  -o tk.wasm tk.o
	clang -ansi --target=wasm32 -nostdlib \
		-Wl,--export-all,-allow-undefined,--no-entry -fno-builtin\
	       	-D C90=1 \
		-I ~/.local/wpdcc/include/ \
		-I d/ -o test.wasm \
		~/.local/wpdcc/lib/libwasm.a \
		../tests/test.c 

	zcat ~/.local/wpdcc/www/3o3_exe.cmd.gz > \
			~/.local/wpdcc/www/test.cmd
	cat test.wasm >> ~/.local/wpdcc/www/test.cmd
	zcat ~/.local/wpdcc/www/3o3_exe.cmd.gz > \
			~/.local/wpdcc/www/test1.cmd
	cat test.wasm >> ~/.local/wpdcc/www/test1.cmd
	(cd ~/.local/wpdcc/www/; http-server)
	rm -f test.wasm
#	llvm-objdump  --full-contents  -d tk.wasm


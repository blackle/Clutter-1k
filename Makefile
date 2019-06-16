
INCLUDE := `pkg-config --cflags clutter-1.0`
LIBS := -lclutter-1.0 -lgobject-2.0
CFLAGS := -fno-plt -Os -std=gnu11 -nostartfiles -Wall -Wextra
CFLAGS += -fno-stack-protector -fno-stack-check -fno-unwind-tables -fno-asynchronous-unwind-tables -fomit-frame-pointer
CFLAGS += -no-pie -fno-pic -fno-PIE -fno-PIC -march=core2 -ffunction-sections -fdata-sections

all : main main_bad

.PHONY: noelfver

packer : vondehi/vondehi.asm 
	cd vondehi; nasm -fbin -o vondehi vondehi.asm

noelfver:
	make -C noelfver

shader.h : shader.frag Makefile
	mono ./shader_minifier.exe $< -o $@

main.o: main.c shader.h Makefile
	gcc -m64 -c -o $@ $< $(INCLUDE) $(CFLAGS) -flto -fuse-linker-plugin

crt1.o: smol/rt/crt1.c
	gcc -m64 -c -o $@ $< $(CFLAGS) -flto -fuse-linker-plugin
main.needssmol.o: main.o crt1.o
	gcc -m64 -Wl,-i -o "$@" $^ -flto -fuse-linker-plugin $(CFLAGS) -nostdlib \
		-Wl,--entry -Wl,_start -Wl,--gc-sections -Wl,--print-gc-sections
main.symbols.asm: main.needssmol.o
	python3 smol/src/smol.py --det $(LIBS) -lc "$<" "$@"
main.smolstub.o: main.symbols.asm
	nasm -DUSE_INTERP -DALIGN_STACK -felf64 -I smol/rt/ -o "$@" "$^"
main.elf: main.needssmol.o main.smolstub.o
	ld -Map=smol.map --cref -m elf_x86_64 -nostartfiles -T smol/ld/link.ld --oformat=binary -o "$@" $^

main : main.elf.packed
	mv $< $@
	wc -c $@

main_bad : main.elf.bad_packed
	mv $< $@
	wc -c $@

%.xz : % Makefile
	-rm $@
	lzma --format=lzma -9 --extreme --lzma1=preset=9,lc=0,lp=0,pb=0,nice=120,depth=64,dict=16384 --keep --stdout $< > $@

%.packed : %.xz packer Makefile
	cat ./vondehi/vondehi $< > $@
	chmod +x $@

%.bad_packed : %.xz bad_header.sh Makefile
	cat bad_header.sh $< > $@
	chmod +x $@

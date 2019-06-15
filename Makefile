
LIBS := `pkg-config --cflags clutter-1.0` -lclutter-1.0 -lgobject-2.0
CFLAGS := -fno-plt -Os -std=gnu11 -nostartfiles -Wall -Wextra
CFLAGS += -fno-stack-protector -fno-stack-check -fno-unwind-tables -fno-asynchronous-unwind-tables -fomit-frame-pointer
CFLAGS += -no-pie -fno-pic -fno-PIE -fno-PIC -march=core2 -ffunction-sections -fdata-sections

all : main

main.elf : main.c Makefile
	gcc -o $@ $< $(CFLAGS) $(LIBS)

main : main_opt.elf.packed
	mv $< $@
	wc -c $@

.PHONY: noelfver

packer : vondehi/vondehi.asm 
	cd vondehi; nasm -fbin -o vondehi vondehi.asm

noelfver:
	make -C noelfver

%_opt.elf : %.elf Makefile noelfver
	cp $< $@
	strip $@
	strip -R .note -R .comment -R .eh_frame -R .eh_frame_hdr -R .note.gnu.build-id -R .got -R .got.plt -R .gnu.version -R .shstrtab -R .gnu.version_r -R .gnu.hash $@
	./noelfver/noelfver $@ > $@.nover
	mv $@.nover $@
	#remove section header
	./Section-Header-Stripper/section-stripper.py $@

	#clear out useless bits
	sed -i 's/_edata/\x00\x00\x00\x00\x00\x00/g' $@;
	sed -i 's/__bss_start/\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00/g' $@;
	sed -i 's/_end/\x00\x00\x00\x00/g' $@;
	sed -i 's/GLIBC_2\.2\.5/\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00/g' $@;

	chmod +x $@

%.xz : % Makefile
	-rm $@
	lzma --format=lzma -9 --extreme --lzma1=preset=9,lc=0,lp=0,pb=0,nice=120,depth=64,dict=16384 --keep --stdout $< > $@

%.packed : %.xz packer Makefile
	cat ./vondehi/vondehi $< > $@
	chmod +x $@


LIBS := `pkg-config --cflags clutter-1.0` -lclutter-1.0 -lgobject-2.0
CFLAGS := -fno-plt -Os -std=gnu11 -nostartfiles -Wall -Wextra
CFLAGS += -fno-stack-protector -fno-stack-check -fno-unwind-tables -fno-asynchronous-unwind-tables -fomit-frame-pointer
CFLAGS += -no-pie -fno-pic -fno-PIE -fno-PIC -march=core2 -ffunction-sections -fdata-sections

main : main.c Makefile
	gcc -o $@ $< $(CFLAGS) $(LIBS)

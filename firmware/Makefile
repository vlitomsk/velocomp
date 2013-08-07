CFLAGS = -Os -mmcu=atmega328p

all: clean compile make_hex

compile:
	avr-gcc -c $(CFLAGS) indicator_debug.c -o indicator_debug.o 
	avr-gcc -c $(CFLAGS) velo.c -o velo.o 
	avr-gcc $(CFLAGS) velo.o indicator_debug.o -o main.o

make_hex:
	avr-objcopy -j .text -j .data -O ihex main.o velo.hex

clean:
	rm *.o -f
	rm *.hex -f
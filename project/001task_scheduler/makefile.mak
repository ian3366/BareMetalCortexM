CC=arm-none-eabi-gcc
MACH=cortex-m4
CFLASS=-c -mcpu=$(MACH) -mthumb -std=gnu11 -Wall -O0
LDFLAGS= -nostdlib -T stm32_ls.ld -Wl,-Map=final.map

all:main.o led.o stm32_start_up.o final.elf 

main.o:main.c
	$(CC) $(CFLASS) -o $@ $^ 
	
led.o:led.c
	$(CC) $(CFLASS) -o $@ $^ 
	
stm32_start_up.o:stm32_start_up.c
	$(CC) $(CFLASS) -o $@ $^ 

final.elf:main.o led.o stm32_start_up.o
	$(CC) $(LDFLAGS) -o $@ $^ 

clean:
	rm -rf *.o *.elf
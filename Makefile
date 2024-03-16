TARGET=test
MCU=attiny412
FCPU=4000000UL
SOURCES=main.c attiny/i2c.c

PROGRAMMER=jtag2updi
PROG_PORT=/dev/ttyACM0
PROG_MCU=ATtiny412
PROG_BAUD=115200

OBJECTS=$(SOURCES:.c=.o)
CFLAGS=-c -Os -DF_CPU=$(FCPU) -I/home/lumpyh/workspace/avr/avr-libc/include -I ./attiny/
LDFLAGS=-L/home/lumpyh/workspace/avr/avr-libc/avr/devices/attiny412

.c.o:
	avr-gcc $(CFLAGS) -mmcu=$(MCU) $< -o $@

$(TARGET).elf: $(OBJECTS)
	avr-gcc $(LDFLAGS) -mmcu=$(MCU) $(OBJECTS) -o $(TARGET).elf

$(TARGET).hex: $(TARGET).elf
	avr-objcopy -O ihex -R .eeprom $(TARGET).elf $(TARGET).hex

all: $(TARGET).hex

size:
	avr-size --mcu=$(MCU) -C $(TARGET).elf

flash: $(TARGET).hex
	avrdude -p $(MCU) -b $(PROG_BAUD) -P $(PROG_PORT) -c $(PROGRAMMER) -Uflash:w:$(TARGET).hex:a

clean:
	rm -rf *.o
	rm -rf *.elf
	rm -rf *.hex

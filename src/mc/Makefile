####################################################################
# more information -- http://wiki.hacdc.org/index.php/AVR_Makefile #
####################################################################

TARGET = smarty_ir
ELF = $(TARGET).elf
HEX = image.hex

FORMAT = ihex
MCU = atmega32
PROGRAMMER = gpio
PORT = /dev/spidev0.0
OPT = s
WRITE_FLASH = -U flash:w:$(HEX)
AVRDUDE_FLAGS = -c $(PROGRAMMER) -p $(MCU) -P $(PORT) 

SRC_DIR = ./
FW_DIR = ../../../../../framework

SRC = $(SRC_DIR)/$(TARGET).c 
OBJS = $(subst .c,.o, $(SRC))


INCLUDES = -I. -I.. -I../common \
	-I$(FW_DIR)/dev/mega32

CC = avr-gcc
CFLAGS = -g -mmcu=$(MCU) -O$(OPT) -c \
	$(INCLUDES)


LDFLAGS = -g -mmcu=$(MCU)

OBJCOPY = avr-objcopy
HEXFLAGS = -j .text -j .data -O $(FORMAT)

$(TARGET) : $(HEX) 
	-@echo "  [$(TARGET)]"

program : $(HEX)
	 avrdude $(AVRDUDE_FLAGS) $(WRITE_FLASH)
	 
%.hex: $(ELF) 
	$(OBJCOPY) $(HEXFLAGS) $< $@
	
%.elf : $(OBJS)
	@$(CC) $(LDFLAGS) $(OBJS) -o $(ELF)

%.o : %.c
	@echo "    $< .."
	$(CC) $(CFLAGS) $< -o $@

test:
	avrdude -P $(PROGRAMMER) -c $(PROGRAMMER) -p $(MCU)

clean:
	-@rm -rf $(HEX) 


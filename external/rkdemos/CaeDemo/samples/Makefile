#common makefile header

DIR_INC = ../include
DIR_BIN = ../bin
DIR_LIB = ../libs
MW_DIR  = ./
TARGET	= cae_sample
BIN_TARGET = $(DIR_BIN)/$(TARGET)

CROSS_COMPILE = ../../../../buildroot/output/host/usr/bin/arm-rockchip-linux-gnueabihf-

CFLAGS =  -fPIC -O3 -I$(DIR_INC) -I./util

LDFLAGS := -L$(DIR_LIB)
LDFLAGS +=  -ldl -lpthread -lcae

C_FILES =$(shell find $(SAMDIR) -name '*.c')
OBJECTS := $(C_FILES:%.c=%.o)

$(BIN_TARGET) : $(OBJECTS)
	$(CROSS_COMPILE)gcc $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJECTS): %.o: %.c
	$(CROSS_COMPILE)gcc -c $(CFLAGS) $(INCLUDE) $< -o $@ 
clean:
	@rm -fr $(OBJECTS) $(BIN_TARGET)

.PHONY:clean

#common makefile foot

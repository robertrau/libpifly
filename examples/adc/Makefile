
TARGET = example_adc
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC))

INCLUDES = -I../../include
INCLUDES += -I./include
INCLUDES += -I$(PREFIX)/usr/include
INCLUDES += -I$(PREFIX)/usr/local/include

LIBS = -lbcm2835 -lpifly
LIB_DIRS = -L$(PREFIX)/usr/local/lib -L../../

CFLAGS = $(INCLUDES) -g --sysroot=/media/rrau/root
LFLAGS = $(LIB_DIRS) $(LIBS)

#CC = gcc
CC = arm-linux-gnueabihf-gcc
AR = arm-linux-gnueabihf-ar

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

TARGET: $(OBJ)
	 $(CC) -o $(TARGET) $(OBJ) $(LFLAGS)


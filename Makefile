
TARGET_LIB = libpifly
SRC_LIB = $(wildcard src/*.c)
SRC_LIB += $(wildcard src/*/*.c)
OBJ_LIB = $(patsubst src/%.c,src/output/%.o,$(SRC_LIB))

INCLUDES = -I./include
INCLUDES += -I$(PREFIX)/usr/include
INCLUDES += -I$(PREFIX)/usr/local/include

LIBS = -lbcm2835
LIB_DIRS = -L$(PREFIX)/usr/local/lib

CFLAGS = $(INCLUDES) -g --sysroot=/media/rrau/root

CC = arm-linux-gnueabihf-gcc
AR = arm-linux-gnueabihf-ar

src/output/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

TARGET_LIB: $(OBJ_LIB)
	$(AR) rcs $(TARGET_LIB).a $(OBJ_LIB)


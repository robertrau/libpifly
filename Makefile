
TARGET_LIB = libpifly
SRC_LIB = $(wildcard src/*.c)
SRC_LIB += $(wildcard src/*/*.c)
OBJ_LIB = $(patsubst src/%.c,src/output/%.o,$(SRC_LIB))

INCLUDES = -I./include
INCLUDES += -I/usr/include
INCLUDES += -I/usr/local/include

LIBS = -lbcm2835
LIB_DIRS = -L/usr/local/lib

CFLAGS = $(INCLUDES) -g

CC = gcc

src/output/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

TARGET_LIB: $(OBJ_LIB)
	ar rcs $(TARGET_LIB).a $(OBJ_LIB)


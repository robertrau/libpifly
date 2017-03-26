
TARGET = example_adc
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC))

INCLUDES = -I../../include
INCLUDES += -I./include
INCLUDES += -I/usr/include
INCLUDES += -I/usr/local/include

LIBS = -lbcm2835 -lpifly
LIB_DIRS = -L/usr/local/lib -L../../

CFLAGS = $(INCLUDES) -g
LFLAGS = $(LIB_DIRS) $(LIBS)

CC = gcc

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

TARGET: $(OBJ)
	 $(CC) -o $(TARGET) $(OBJ) $(LFLAGS)


.PHONY: clean

SRC = src
OUT = out

VPATH = $(SRC)

SRCS = $(wildcard $(SRC)/*.c)
OBJ = $(patsubst $(SRC)/%.c, $(OUT)/%.o, $(SRCS))

OPT ?= -g

CFLAGS = $(OPT) -pthread -Wall -Wextra -pedantic

main: $(OBJ)
	$(CC) $(CFLAGS) -o main $(OBJ)

$(OUT)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f main $(OUT)/*.o

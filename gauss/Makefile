.PHONY: clean

SRC = src
INC = inc
OUT = out

vpath %.c $(SRC)
vpath %.h $(INC)

SRCS = $(wildcard $(SRC)/*.c)
OBJ = $(patsubst $(SRC)/%.c, $(OUT)/%.o, $(SRCS))
CFLAGS = -I$(INC) $(OPT) -pthread -Wall -Wextra -pedantic

OPT ?= -g

main: $(OBJ)
	$(CC) $(CFLAGS) -o main $(OBJ)

$(OUT)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f main $(OUT)/*.o

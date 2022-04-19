.PHONY: all clean

export OPT ?= -ggdb3

all:
	$(MAKE) -C gauss
	$(MAKE) -C diekert
	$(MAKE) -C trace-hs

clean:
	$(MAKE) -C gauss clean
	$(MAKE) -C diekert clean
	$(MAKE) -C trace-hs clean
.PHONY: clean

SRC = $(wildcard *.hs)

HOPT ?= 

Main: $(SRC)
	ghc $(HOPT) -outputdir="out" --make -o Main Main

clean:
	rm Main out/*
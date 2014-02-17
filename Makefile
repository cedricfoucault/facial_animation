# *****************************************************************************
# File: Makefile

# TER Humains Virtuels
# Master d'informatique
# Christian Jacquemin, Université Paris 11

# Copyright (C) 2008 University Paris 11 
# This file is provided without support, instruction, or implied
# warranty of any kind.  University Paris 11 makes no guarantee of its
# fitness for a particular purpose and is not liable under any
# circumstances for any damages or loss whatsoever arising from the use
# or inability to use this file or items derived from it.
# ******************************************************************************
INCPATH = -I /usr/local/include -I /usr/X11R6/include 
LIBPATH = -L /usr/local/lib -L /usr/X11R6/lib
LIBRARY = -lGLU -lGL -lglut -lpthread -ljpeg -lpng

CFLAGS = $(INCPATH)
LDFALGS = $(LIBPATH) $(LIBRARY)
GCC=/usr/bin/g++ 
SRC=src

all:
	@echo "Specifiez une cible"

%.o:$(SRC)/%.cpp
	$(GCC) -ggdb $(CFLAGS) -o $@ -c $<

%:%.o $(SRC)/%.cpp texpng.o urotate.o
	$(GCC) $< texpng.o urotate.o $(LDFALGS) -o $@

# cette commande permet de supprimer tous les objets
clean:
	@rm -f `/bin/ls *.o | grep -v Makefile | grep -v $(SRC)`

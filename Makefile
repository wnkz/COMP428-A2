##
##  Makefile
##  COMP428-A2
##
##  Created by Grégoire Morpain on 2012-02-22
##  (c) Copyright 2012 Grégoire Morpain. All rights reserved.
##

CC          = mpicc
RM          = rm -vf
MKDIR       = @mkdir -p
CP          = @cp

DIR_SRC     = src
DIR_BUILD   = build
DIR_COPY    = bin

DIR_QSORT   = $(DIR_SRC)/qsort
DIR_PSRS    = $(DIR_SRC)/psrs
DIR_PQSORT  = $(DIR_SRC)/pqsort
DIR_SHARED	= $(DIR_SRC)/shared

NAME_QSORT  = qsort
NAME_PSRS   = psrs
NAME_PQSORT = pqsort

SRC_QSORT   = $(DIR_QSORT)/qsort.c
SRC_PSRS    = $(DIR_PSRS)/psrs.c
SRC_PQSORT  = $(DIR_PQSORT)/pqsort.c
SRC_SHARED	= $(DIR_SHARED)/compare.c \
							$(DIR_SHARED)/loadfromfile.c \
							$(DIR_SHARED)/writetofile.c

OBJ_QSORT   = $(SRC_QSORT:.c=.o)
OBJ_PSRS    = $(SRC_PSRS:.c=.o)
OBJ_PQSORT  = $(SRC_PQSORT:.c=.o)
OBJ_SHARED	=	$(SRC_SHARED:.c=.o)

INCLUDES		= -I$(DIR_SHARED)
CFLAGS      = -W -Wall -std=c99 $(INCLUDES)

release: DIR_OUT = $(DIR_BUILD)/release
release: all

debug: CFLAGS 	+= -g
debug: DIR_OUT   = $(DIR_BUILD)/debug
debug: all

createdirectories:
	$(MKDIR) $(DIR_BIN)
	$(MKDIR) $(DIR_OBJ)
	$(MKDIR) $(DIR_COPY)

all: DIR_BIN = $(DIR_OUT)/bin
all: DIR_OBJ = $(DIR_OUT)/obj
all: createdirectories $(NAME_QSORT) $(NAME_PSRS) $(NAME_PQSORT)

$(NAME_QSORT): $(OBJ_QSORT) $(OBJ_SHARED)
	$(CC) -o $(DIR_BIN)/$(NAME_QSORT) $(DIR_OBJ)/$(notdir $(OBJ_QSORT)) $(patsubst %,$(DIR_OBJ)/%,$(notdir $(OBJ_SHARED)))
	$(CP) $(DIR_BIN)/$(NAME_QSORT) $(DIR_COPY)
 
$(NAME_PSRS): $(OBJ_PSRS) $(OBJ_SHARED)
	$(CC) -o $(DIR_BIN)/$(NAME_PSRS) $(DIR_OBJ)/$(notdir $(OBJ_PSRS)) $(patsubst %,$(DIR_OBJ)/%,$(notdir $(OBJ_SHARED)))
	$(CP) $(DIR_BIN)/$(NAME_PSRS) $(DIR_COPY)

$(NAME_PQSORT): $(OBJ_PQSORT) $(OBJ_SHARED)
	$(CC) -o $(DIR_BIN)/$(NAME_PQSORT) $(DIR_OBJ)/$(notdir $(OBJ_PQSORT)) $(patsubst %,$(DIR_OBJ)/%,$(notdir $(OBJ_SHARED)))
	$(CP) $(DIR_BIN)/$(NAME_PQSORT) $(DIR_COPY)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $(DIR_OBJ)/$(notdir $@)

clean:
	$(RM) -r $(DIR_BUILD)

fclean: clean
	$(RM) -r $(DIR_COPY)

.PHONY: $(NAME_PSRS) $(NAME_PQSORT) release debug all clean fclean

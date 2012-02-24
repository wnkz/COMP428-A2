##
##  Makefile
##  COMP428-A2
##
##  Created by Grégoire Morpain on 2012-02-22
##  (c) Copyright 2012 Grégoire Morpain. All rights reserved.
##

CC               = g++
RM               = rm -vf
MKDIR            = @mkdir -p
CP               = @cp

DIR_SRC          = src
DIR_BUILD        = build
DIR_COPY         = bin

DIR_PSRS         = $(DIR_SRC)/psrs
DIR_PQSORT       = $(DIR_SRC)/pqsort

NAME_PSRS        = psrs
SRC_PSRS         = $(DIR_PSRS)/psrs.cpp
OBJ_PSRS         = $(SRC_PSRS:.cpp=.o)

NAME_PQSORT      = pqsort
SRC_PQSORT       = $(DIR_PQSORT)/pqsort.cpp
OBJ_PQSORT       = $(SRC_PQSORT:.cpp=.o)

CFLAGS           = -W -Wall

release: DIR_OUT = $(DIR_BUILD)/release
release: all

debug: CFLAGS 	+= -g
debug: DIR_OUT   = $(DIR_BUILD)/debug
debug: all

createdirectories:
	$(MKDIR) $(DIR_BIN)
	$(MKDIR) $(DIR_OBJ)
	$(MKDIR) $(DIR_COPY)

all: DIR_BIN     = $(DIR_OUT)/bin
all: DIR_OBJ     = $(DIR_OUT)/obj
all: createdirectories $(NAME_PSRS) $(NAME_PQSORT)
 
$(NAME_PSRS): $(OBJ_PSRS)
	$(CC) -o $(DIR_BIN)/$(NAME_PSRS) $(DIR_OBJ)/$(notdir $(OBJ_PSRS))
	$(CP) $(DIR_BIN)/$(NAME_PSRS) $(DIR_COPY)

$(NAME_PQSORT): $(OBJ_PQSORT)
	$(CC) -o $(DIR_BIN)/$(NAME_PQSORT) $(DIR_OBJ)/$(notdir $(OBJ_PSRS))
	$(CP) $(DIR_BIN)/$(NAME_PQSORT) $(DIR_COPY)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $(DIR_OBJ)/$(notdir $@)

clean:
	$(RM) -r $(DIR_BUILD)

fclean: clean
	$(RM) -r $(DIR_COPY)

.PHONY: $(NAME_PSRS) $(NAME_PQSORT) release debug all clean fclean

COMPILER := gcc
SRCDIR := src
TSTDIR := tests
OBJDIR := build
BINDIR := bin

SERVER := program/server.c
CLIENT := program/client.c

LANGUAGE := c
SOURCES := $(shell find $(SRCDIR)/ -type f -name *.$(LANGUAGE))
OBJECTS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SOURCES:.$(LANGUAGE)=.o))

# -g debug
CFLAGS := -Wall -g
INC := -I include/ -I third_party/
INC_MATH := -lm
INC_PTHREAD := -pthread 


$(OBJDIR)/%.o: $(SRCDIR)/%.$(LANGUAGE)
	@mkdir -p $(@D)
	$(COMPILER) $(CFLAGS) $(INC) -c -o $@ $< $(INC_MATH)

all: $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(COMPILER) $(CFLAGS) $(INC) $(CLIENT) $^ -o $(BINDIR)/client $(INC_MATH)
	$(COMPILER) $(CFLAGS) $(INC_PTHREAD) $(INC) $(SERVER) $^ -o $(BINDIR)/server $(INC_MATH)

clean:
	$(RM) -r $(OBJDIR)/* $(BINDIR)/* *.gcda *.gcno
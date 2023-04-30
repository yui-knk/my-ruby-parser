VPATH         = src
CC            = gcc
CFLAGS        = -O0 -g -Wall -I./include
LDFLAGS       = -L/usr/local/lib -L./
LIBS          = -lparser
OBJS          = main.o
DEPENDS       = libparser.dylib
DLDFLAGS      = -Wl,-rpath,'\$$ORIGIN/'
PROGRAM       = myparser

all: $(PROGRAM)

$(PROGRAM): $(OBJS) $(DEPENDS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) $(DLDFLAGS) -o $(PROGRAM)

.PHONY: lldb
lldb: $(PROGRAM)
	lldb $(PROGRAM)

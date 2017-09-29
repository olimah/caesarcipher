# makefile for caesarcipher
# Created January 2017 by Oliver Mahmoudi (contact@olivermahmoudi.com)

# Variables
BINARY=caesarcipher
CC=gcc
CP=cp
INSTALL=/usr/bin/install -m 755
INSTALLPATH=/usr/local/bin
MANPAGE=caesarcipher.1
MANPATH=/usr/local/share/man/man1
OBJ=caesarcipher.o

# Targets
%.o: %.c
	$(CC) -c -o $@ $<

$(BINARY): $(OBJ)
	$(CC) -o $@ $^

install: $(BINARY)
	$(INSTALL) $(BINARY) $(INSTALLPATH)
	$(CP) $(MANPAGE) $(MANPATH)

deinstall:
	rm -vf $(INSTALLPATH)/$(BINARY)
	rm -vf $(MANPATH)/$(MANPAGE)

.PHONY: clean
clean:
	rm -vf $(BINARY) *.o

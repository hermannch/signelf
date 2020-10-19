# Makefile for sign-elf

# list of object (.o) files

genkeypr_OBJ=					\
	genkeypr.o					\

libsign_OBJ=					\
	readelf.o					\
	signelf.o					\

sign_OBJ=						\
	sign.o						\
	
verify_OBJ=						\
	verify.o					\

DEV_HDR=						\
	signelf.h					\
	readelf.h					\
	pubkey.h					\

genkeypr_TARGET= genkeypr
libsign_TARGET= libsignelf.a
sign_TARGET= signelf
verify_TARGET= verifyelf

# default targets
TARGETS=						\
	$(libsign_TARGET)			\
	$(sign_TARGET)				\

CTAGS= ctags
TAGS= tags
INSTALL_DIR= usr/bin
INSTALL_DEV= usr/include/signelf1
LIB_DIR= usr/lib

# Include directories
INCLUDES=

LIBS= -lssl -lcrypto -lstdc++

MOC= /usr/bin/moc
LDFLAGS= -L. $(LIBS)

# Position independent code option
PIC= -fPIC

CFLAGS_WARNINGS = -Wall -Wextra -Wpedantic
ifneq ("$(DEBUG)", "")
	CFLAGS_OPTIMIZE := -Og -ggdb -g3
else
	CFLAGS_OPTIMIZE := -O2
endif	

CFLAGS= \
	$(CFLAGS_WARNINGS) \
	$(CFLAGS_OPTIMIZE) \
	$(PIC) \
	-I. \
	$(INCLUDES)

default all: $(TAGS) $(TARGETS)

privkey:
	openssl genrsa -out $@ 4096

pubkey: privkey
	openssl rsa -pubout -out $@ -in $<

privkey.h: privkey $(genkeypr_TARGET)
	./$(genkeypr_TARGET) priv privkey > $@
	
pubkey.h: pubkey $(genkeypr_TARGET)
	./$(genkeypr_TARGET) pub pubkey > $@
	
$(TAGS):
	$(CTAGS) *.h *.cpp

.c.o:
	$(CC) $(CFLAGS) -c $*.c

.cpp.o:
	$(CXX) $(CFLAGS) -c $*.cpp

sign.o: privkey.h sign.cpp
	$(CXX) $(CFLAGS) -c $*.cpp

verify.o: pubkey.h verify.cpp
	$(CXX) $(CFLAGS) -c $*.cpp

$(genkeypr_TARGET): $(genkeypr_OBJ)
	$(CXX) -o $(genkeypr_TARGET) $(genkeypr_OBJ) $(LDFLAGS)

$(libsign_TARGET): $(libsign_OBJ)
	$(AR) rcs $@ $(libsign_OBJ)
	
$(sign_TARGET): privkey.h $(sign_OBJ) $(libsign_TARGET)
	$(CXX) -o $(sign_TARGET) $(sign_OBJ) $(LDFLAGS) -lsignelf

$(verify_TARGET): pubkey.h $(verify_OBJ) $(libsign_TARGET)
	$(CXX) -o $(verify_TARGET) $(verify_OBJ) $(LDFLAGS) -lsignelf

install: install-dev $(TARGETS)
	mkdir -p $(DESTDIR)/$(INSTALL_DIR)
	mkdir -p $(DESTDIR)/$(LIB_DIR)
	install -m755 $(sign_TARGET) $(DESTDIR)/$(INSTALL_DIR)/$(sign_TARGET)
	install -m644 $(libsign_TARGET) $(DESTDIR)/$(LIB_DIR)/$(libsign_TARGET)

install-dev: $(DEV_HDR)
	mkdir -p $(DESTDIR)/$(INSTALL_DEV)
	cp $(DEV_HDR) $(DESTDIR)/$(INSTALL_DEV)

install-lang:

lang:

uninstall:
	rm -f $(DESTDIR)/$(INSTALL_DIR)/$(sign_TARGET)
	rm -f $(DESTDIR)/$(LIB_DIR)/$(libsign_TARGET)

cleankeys:
	rm -f privkey pubkey privkey.h pubkey.h

distclean clean:
	rm -f tags $(genkeypr_TARGET) $(libsign_TARGET) $(sign_TARGET) $(verify_TARGET) $(genkeypr_OBJ) $(libsign_OBJ) $(sign_OBJ) $(verify_OBJ)

.PHONY=install install-lang uninstall install-dev clean distclean cleankeys lang

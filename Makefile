CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -O2 -D_POSIX_C_SOURCE=199309L
INCLUDE = -Iinclude -Isrc

BUILDDIR = build
DISTDIR  = dist
PREFIX   = /usr/local

SRC = \
    src/core/ut_core.c \
    src/ut_now.c \
    src/ut_format.c \
    src/ut_parse.c \
    src/ut_calendar.c

OBJ = $(patsubst src/%.c,$(BUILDDIR)/%.o,$(SRC))

TARGET  = $(DISTDIR)/libuniversal_timestamp.a
TESTBIN = $(DISTDIR)/test_runner
PCFILE  = $(DISTDIR)/universal_timestamp.pc

all: $(TARGET) $(TESTBIN)

$(BUILDDIR)/%.o: src/%.c | $(BUILDDIR) $(BUILDDIR)/core
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/core:
	mkdir -p $(BUILDDIR)/core

$(DISTDIR):
	mkdir -p $(DISTDIR)

$(TARGET): $(OBJ) | $(DISTDIR)
	ar rcs $(TARGET) $(OBJ)

$(TESTBIN): test/test.c $(TARGET) | $(DISTDIR)
	$(CC) $(CFLAGS) $(INCLUDE) test/test.c -o $(TESTBIN) -L$(DISTDIR) -l:libuniversal_timestamp.a

$(PCFILE): universal_timestamp.pc.in | $(DISTDIR)
	sed 's|@PREFIX@|$(PREFIX)|g' $< > $@

test: $(TESTBIN)
	./$(TESTBIN)

install: $(TARGET) $(PCFILE)
	install -d $(DESTDIR)$(PREFIX)/lib
	install -d $(DESTDIR)$(PREFIX)/include
	install -d $(DESTDIR)$(PREFIX)/lib/pkgconfig
	install -m 644 $(TARGET) $(DESTDIR)$(PREFIX)/lib/
	install -m 644 include/universal_timestamp.h $(DESTDIR)$(PREFIX)/include/
	install -m 644 $(PCFILE) $(DESTDIR)$(PREFIX)/lib/pkgconfig/

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/libuniversal_timestamp.a
	rm -f $(DESTDIR)$(PREFIX)/include/universal_timestamp.h
	rm -f $(DESTDIR)$(PREFIX)/lib/pkgconfig/universal_timestamp.pc

clean:
	rm -rf $(BUILDDIR) $(DISTDIR)

.PHONY: all test install uninstall clean

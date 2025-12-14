CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -O2 -D_POSIX_C_SOURCE=199309L
INCLUDE = -Iinclude -Isrc

BUILDDIR = build
PREFIX   = /usr/local

SRC = \
    src/core/ut_core.c \
    src/ut_now.c \
    src/ut_format.c \
    src/ut_parse.c \
    src/ut_calendar.c

OBJ = $(patsubst src/%.c,$(BUILDDIR)/%.o,$(SRC))

TARGET  = libuniversal_timestamp.a
TESTBIN = test_runner
PCFILE  = universal_timestamp.pc

all: $(TARGET) $(TESTBIN)

$(BUILDDIR)/%.o: src/%.c | $(BUILDDIR) $(BUILDDIR)/core
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/core:
	mkdir -p $(BUILDDIR)/core

$(TARGET): $(OBJ)
	ar rcs $(TARGET) $(OBJ)

$(TESTBIN): test/test.c $(TARGET)
	$(CC) $(CFLAGS) $(INCLUDE) test/test.c -o $(TESTBIN) -L. -l:$(TARGET)

$(PCFILE): universal_timestamp.pc.in
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
	rm -f $(DESTDIR)$(PREFIX)/lib/$(TARGET)
	rm -f $(DESTDIR)$(PREFIX)/include/universal_timestamp.h
	rm -f $(DESTDIR)$(PREFIX)/lib/pkgconfig/$(PCFILE)

clean:
	rm -rf $(BUILDDIR) $(TARGET) $(TESTBIN) $(PCFILE)

.PHONY: all test install uninstall clean

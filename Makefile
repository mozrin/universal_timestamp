CC      = gcc
CXX     = g++
CFLAGS  = -std=c11 -Wall -Wextra -O2 -D_POSIX_C_SOURCE=199309L
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
INCLUDE = -Iinclude -Isrc

OBJDIR  = build
DISTDIR = dist
PREFIX  = /usr/local

SRC = \
    src/core/ut_core.c \
    src/ut_now.c \
    src/ut_format.c \
    src/ut_parse.c \
    src/ut_calendar.c

OBJ = $(patsubst src/%.c,$(OBJDIR)/%.o,$(SRC))

TARGET     = $(DISTDIR)/libuniversal_timestamp.a
TESTBIN    = $(DISTDIR)/test_runner
CPPTESTBIN = $(DISTDIR)/test_cpp
PCFILE     = $(DISTDIR)/universal_timestamp.pc

.DEFAULT_GOAL := help

help:
	@echo "Universal Timestamp Library - Build Targets"
	@echo "============================================"
	@echo ""
	@echo "Build:"
	@echo "  make build_c        - Build C library and test runner"
	@echo "  make build_cpp      - Build C++ test runner"
	@echo ""
	@echo "Test:"
	@echo "  make test           - Run C tests"
	@echo "  make cpp_test       - Run C++ tests"
	@echo "  make test_all       - Run all tests"
	@echo ""
	@echo "Install:"
	@echo "  make install_c      - Install C library only"
	@echo "  make install_cpp    - Install C++ wrapper (includes C library)"
	@echo ""
	@echo "Other:"
	@echo "  make clean          - Remove build artifacts"
	@echo "  make uninstall      - Remove installed files"

build:
	@echo "Please specify which library to build:"
	@echo "  make build_c        - Build C library and test runner"
	@echo "  make build_cpp      - Build C++ test runner"

build_c: $(TARGET) $(TESTBIN)

build_cpp: $(CPPTESTBIN)

$(OBJDIR)/%.o: src/%.c | objdir objdir_core
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

objdir:
	mkdir -p $(OBJDIR)

objdir_core:
	mkdir -p $(OBJDIR)/core

distdir:
	mkdir -p $(DISTDIR)

$(TARGET): $(OBJ) | distdir
	ar rcs $(TARGET) $(OBJ)

$(TESTBIN): test/test.c $(TARGET) | distdir
	$(CC) $(CFLAGS) $(INCLUDE) test/test.c -o $(TESTBIN) -L$(DISTDIR) -l:libuniversal_timestamp.a

$(CPPTESTBIN): wrappers/cpp/test_cpp.cpp $(TARGET) | distdir
	$(CXX) $(CXXFLAGS) -Iinclude -Iwrappers/cpp wrappers/cpp/test_cpp.cpp -o $(CPPTESTBIN) -L$(DISTDIR) -l:libuniversal_timestamp.a

$(PCFILE): universal_timestamp.pc.in | distdir
	sed 's|@PREFIX@|$(PREFIX)|g' $< > $@

test: $(TESTBIN)
	./$(TESTBIN)

cpp_test: $(CPPTESTBIN)
	./$(CPPTESTBIN)

test_all: test cpp_test

install_c: $(TARGET) $(PCFILE)
	install -d $(DESTDIR)$(PREFIX)/lib
	install -d $(DESTDIR)$(PREFIX)/include
	install -d $(DESTDIR)$(PREFIX)/lib/pkgconfig
	install -m 644 $(TARGET) $(DESTDIR)$(PREFIX)/lib/
	install -m 644 include/universal_timestamp.h $(DESTDIR)$(PREFIX)/include/
	install -m 644 $(PCFILE) $(DESTDIR)$(PREFIX)/lib/pkgconfig/

install_cpp: install_c
	install -m 644 wrappers/cpp/universal_timestamp.hpp $(DESTDIR)$(PREFIX)/include/

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/libuniversal_timestamp.a
	rm -f $(DESTDIR)$(PREFIX)/include/universal_timestamp.h
	rm -f $(DESTDIR)$(PREFIX)/include/universal_timestamp.hpp
	rm -f $(DESTDIR)$(PREFIX)/lib/pkgconfig/universal_timestamp.pc

clean:
	rm -rf $(OBJDIR) $(DISTDIR)

.PHONY: help build build_c build_cpp test cpp_test test_all install_c install_cpp uninstall clean

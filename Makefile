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
	@echo "  make build_python   - (No build needed)"
	@echo "  make build_bash     - Build Bash CLI utility"
	@echo ""
	@echo "  make test_c         - Run C tests"
	@echo "  make test_cpp       - Run C++ tests"
	@echo "  make test_python    - Run Python tests"
	@echo "  make test_rust      - Run Rust tests"
	@echo "  make test_all       - Run all tests"
	@echo ""
	@echo "Install:"
	@echo "  make install_c      - Install C library only"
	@echo "  make install_cpp    - Install C++ wrapper (includes C library)"
	@echo "  make install_python - Install Python wrapper (pip install)"
	@echo "  make install_python_force - Install Python wrapper (break system packages)"
	@echo "  make install_rust   - Show Rust install instructions"
	@echo ""

	@echo "Other:"
	@echo "  make clean          - Remove build artifacts"
	@echo "  make uninstall      - Remove installed files"

build:
	@echo "Please specify which library to build:"
	@echo "  make build_c        - Build C library and test runner"
	@echo "  make build_cpp      - Build C++ test runner"
	@echo "  make build_python   - (No build needed for Python)"

build_c: $(TARGET) $(TESTBIN)

build_cpp: $(CPPTESTBIN)

build_python:
	@echo "Python wrapper is pure Python (ctypes). No build step required."
	@echo "Run 'make install_python' to install."

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

test_c: $(TESTBIN)
	./$(TESTBIN)

test_cpp: $(CPPTESTBIN)
	./$(CPPTESTBIN)

test_python: $(TARGET)
	@echo "Verifying Python wrapper import (local)..."
	export LD_LIBRARY_PATH=$(PWD)/dist:$(LD_LIBRARY_PATH) && \
	cd wrappers/python && python3 -c "import universal_timestamp; print('Python wrapper OK')"

test_rust: $(TARGET)
	@echo "Running Rust tests (local)..."
	export LD_LIBRARY_PATH=$(PWD)/dist:$(LD_LIBRARY_PATH) && \
	export LIBRARY_PATH=$(PWD)/dist:$(LIBRARY_PATH) && \
	cd wrappers/rust && cargo test

test_go: $(TARGET)
	@echo "Running Go tests..."
	export CGO_CFLAGS="-I$(PWD)/include" && \
	export CGO_LDFLAGS="-L$(PWD)/dist -l:libuniversal_timestamp.a" && \
	cd wrappers/go && go test -v

test_all: test_c test_cpp test_python test_rust test_go
	cd wrappers/go && go test -v

test: test_all

install_c: $(TARGET) $(PCFILE)
	install -d $(DESTDIR)$(PREFIX)/lib
	install -d $(DESTDIR)$(PREFIX)/include
	install -d $(DESTDIR)$(PREFIX)/lib/pkgconfig
	install -m 644 $(TARGET) $(DESTDIR)$(PREFIX)/lib/
	install -m 644 include/universal_timestamp.h $(DESTDIR)$(PREFIX)/include/
	install -m 644 $(PCFILE) $(DESTDIR)$(PREFIX)/lib/pkgconfig/

check_c_installed:
	@if [ ! -f $(DESTDIR)$(PREFIX)/include/universal_timestamp.h ]; then \
		echo "Error: C library not found!"; \
		echo "Please run 'sudo make install_c' first."; \
		exit 1; \
	fi

install_cpp: check_c_installed
	install -m 644 wrappers/cpp/universal_timestamp.hpp $(DESTDIR)$(PREFIX)/include/

install_python: check_c_installed
	@if ! command -v pip >/dev/null 2>&1 && ! command -v pip3 >/dev/null 2>&1; then \
		echo "Error: pip not found!"; \
		echo "Please install pip (e.g., 'sudo apt install python3-pip') first."; \
		exit 1; \
	fi
	@echo "Installing Python wrapper..."
	@cd wrappers/python && pip install . 2>/dev/null || pip3 install . 2>/dev/null || \
	(echo "\nError: Installation failed (likely PEP 668)."; \
	 echo "To force installation (break system packages), run:"; \
	 echo "  make install_python_force"; \
	 echo "Or activate a virtual environment first."; \
	 exit 1)

install_python_force: check_c_installed
	@echo "Installing Python wrapper (forcing --break-system-packages)..."
	cd wrappers/python && (pip install . --break-system-packages || pip3 install . --break-system-packages)

install_rust: check_c_installed
	@echo "Rust wrapper is a library crate. No system install needed."
	@echo "Add to your Cargo.toml: universal_timestamp = { path = 'wrappers/rust' }"
	@echo "To check build: cd wrappers/rust && cargo build"

build_bash: $(TARGET)
	$(CC) $(CFLAGS) -Iinclude src/cli/uts_cli.c -o wrappers/bash/uts-cli -Ldist -luniversal_timestamp

test_bash: build_bash
	wrappers/bash/test_bash.sh

install_bash: build_bash
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 wrappers/bash/uts-cli $(DESTDIR)$(PREFIX)/bin/
	install -m 755 wrappers/bash/universal_timestamp.sh $(DESTDIR)$(PREFIX)/bin/

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/libuniversal_timestamp.a
	rm -f $(DESTDIR)$(PREFIX)/include/universal_timestamp.h
	rm -f $(DESTDIR)$(PREFIX)/include/universal_timestamp.hpp
	rm -f $(DESTDIR)$(PREFIX)/lib/pkgconfig/universal_timestamp.pc

clean:
	rm -rf $(OBJDIR) $(DISTDIR)

	@echo "  make test_c         - Run C tests"
	@echo "  make test_cpp       - Run C++ tests"
	@echo "  make test_python    - Run Python tests"
	@echo "  make test_rust      - Run Rust tests"
	@echo "  make test_all       - Run all tests"
	@echo ""
	@echo "Install:"
	@echo "  make install_c      - Install C library only"
	@echo "  make install_cpp    - Install C++ wrapper (includes C library)"
	@echo "  make install_python - Install Python wrapper (pip install)"
	@echo "  make install_python_force - Install Python wrapper (break system packages)"
	@echo "  make install_rust   - Show Rust install instructions"

.PHONY: help build build_c build_cpp build_python build_bash test test_c test_cpp test_python test_rust test_bash test_all install_c install_cpp install_python install_python_force install_rust install_bash uninstall clean check_c_installed

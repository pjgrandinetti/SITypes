# Makefile for SITypes static library

# Default target
.DEFAULT_GOAL := all
.SUFFIXES:

CC      := clang
AR      := ar
LEX     := flex
YACC    := bison -y
YFLAGS  := -d

SRC_DIR := src

# Third-party OCTypes layout
OCTYPES_DIR := third_party/OCTypes
OCT_INCLUDE  := $(OCTYPES_DIR)/include
OCT_LIBDIR   := $(OCTYPES_DIR)/lib

# Compiler flags (ensure OCT_INCLUDE is set here)
CFLAGS  := -I . -I src -I$(OCT_INCLUDE) -O3 -Wall -Wextra \
           -Wno-sign-compare -Wno-unused-parameter -Wno-missing-field-initializers

# Flex/Bison inputs (only *Parser.y)
LEX_SRC  := $(wildcard $(SRC_DIR)/*.l)
YACC_SRC := $(wildcard $(SRC_DIR)/*Parser.y)

# Generated sources
GEN_SCANNER  := $(patsubst $(SRC_DIR)/%Scanner.l,   %Scanner.c,   $(LEX_SRC))
GEN_PARSER_C := $(patsubst $(SRC_DIR)/%Parser.y,    %.tab.c, $(YACC_SRC))
GEN_PARSER_H := $(patsubst $(SRC_DIR)/%Parser.y,    %.tab.h, $(YACC_SRC))
GEN_C        := $(GEN_SCANNER) $(GEN_PARSER_C)
GEN_H        := $(GEN_PARSER_H)

# All C sources (include generated, exclude flex/yacc inputs)
STATIC_SRC := $(filter-out $(YACC_SRC) $(LEX_SRC), $(wildcard $(SRC_DIR)/*.c))
ALL_C      := $(GEN_C) $(notdir $(STATIC_SRC))
OBJ        := $(ALL_C:.c=.o)

# Test sources
TEST_SRC_DIR := tests
TEST_FILES   := $(wildcard $(TEST_SRC_DIR)/test_*.c) $(TEST_SRC_DIR)/main.c
TEST_OBJ     := $(notdir $(TEST_FILES:.c=.o))

# OCTypes downloads
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  OCT_LIB_BIN := libOCTypes-libOCTypes-macos-latest.zip
else ifeq ($(UNAME_S),Linux)
  OCT_LIB_BIN := libOCTypes-libOCTypes-ubuntu-latest.zip
else ifeq ($(OS),Windows_NT)
  OCT_LIB_BIN := libOCTypes-libOCTypes-windows-latest.zip
endif

OCT_LIB_ARCHIVE     := third_party/$(OCT_LIB_BIN)
OCT_HEADERS_BIN     := libOCTypes-headers.zip
OCT_HEADERS_ARCHIVE := third_party/$(OCT_HEADERS_BIN)

.PHONY: all octypes clean clean-objects test test-debug test-asan docs docs-setup clean-docs prepare

# Ensure OCTypes is downloaded & extracted
octypes: $(OCT_HEADERS_ARCHIVE) $(OCT_LIB_ARCHIVE) \
         $(OCT_INCLUDE)/OCTypes/OCLibrary.h $(OCT_LIBDIR)/libOCTypes.a

third_party:
	@mkdir -p third_party

$(OCT_LIB_ARCHIVE): | third_party
	@echo "Fetching OCTypes library: $(OCT_LIB_BIN)"
	@curl -L https://github.com/pjgrandinetti/OCTypes/releases/download/v0.1.0/$(OCT_LIB_BIN) -o $@

$(OCT_HEADERS_ARCHIVE): | third_party
	@echo "Fetching OCTypes headers: $(OCT_HEADERS_BIN)"
	@curl -L https://github.com/pjgrandinetti/OCTypes/releases/download/v0.1.0/$(OCT_HEADERS_BIN) -o $@

$(OCT_LIBDIR)/libOCTypes.a: $(OCT_LIB_ARCHIVE)
	@echo "Setting up OCTypes lib in $(OCT_LIBDIR)"
	@rm -rf $(OCT_LIBDIR)
	@mkdir -p $(OCT_LIBDIR)
	@unzip -q $(OCT_LIB_ARCHIVE) -d $(OCT_LIBDIR)

$(OCT_INCLUDE)/OCTypes/OCLibrary.h: $(OCT_HEADERS_ARCHIVE)
	@echo "Setting up OCTypes headers in $(OCT_INCLUDE)"
	@rm -rf $(OCT_INCLUDE)
	@mkdir -p $(OCT_INCLUDE)/OCTypes
	@unzip -q $(OCT_HEADERS_ARCHIVE) -d $(OCT_INCLUDE)
	@mv $(OCT_INCLUDE)/*.h $(OCT_INCLUDE)/OCTypes/ 2>/dev/null || true
	@echo "OCTypes headers ready in '$(OCT_INCLUDE)/OCTypes'"

# Generate parser headers before scanners
prepare: $(GEN_PARSER_H)

all: octypes prepare libSITypes.a

# Build the static library
libSITypes.a: $(OBJ)
	$(AR) rcs $@ $^

# Bison: produce *.tab.c and *.tab.h
%.tab.c %.tab.h: $(SRC_DIR)/%Parser.y
	$(YACC) $(YFLAGS) $<
	mv y.tab.c $*.tab.c
	mv y.tab.h $*.tab.h

# Flex scanners depend on parser headers
$(GEN_SCANNER): $(GEN_PARSER_H)

$(SRC_DIR)/%Scanner.c: $(SRC_DIR)/%Scanner.l %.tab.h
	$(LEX) -o $@ $<

# Compile objects
%.tab.o: %.tab.c %.tab.h | octypes
	$(CC) $(CFLAGS) -c -o $@ $<

%Scanner.o: %.c | octypes
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: src/%.c | octypes
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: tests/%.c | octypes
	$(CC) $(CFLAGS) -Isrc -Itests -c -o $@ $<

# Run tests
test: libSITypes.a $(TEST_OBJ)
	$(CC) $(CFLAGS) -Isrc -Itests $(TEST_OBJ) \
	  -L. -L$(OCT_LIBDIR) -lSITypes -lOCTypes -lm -o runTests
	./runTests

test-debug: libSITypes.a $(TEST_OBJ)
	$(CC) $(CFLAGS) -g -O0 -Isrc -Itests $(TEST_OBJ) \
	  -L. -L$(OCT_LIBDIR) -lSITypes -lOCTypes -lm -o runTests.debug
	@echo "Launching under LLDB…"
	@lldb -- ./runTests.debug

test-asan: libSITypes.a $(TEST_OBJ)
	$(CC) $(CFLAGS) -g -O1 -fsanitize=address -fno-omit-frame-pointer \
	  -Isrc -Itests $(TEST_OBJ) -L. -L$(OCT_LIBDIR) \
	  -lSITypes -lOCTypes -lm -o runTests.asan
	@echo "Running AddressSanitizer build…"
	@./runTests.asan

# Documentation
# Build API docs (Doxygen XML + Sphinx HTML)
docs:
	@echo "Generating Doxygen XML output…"
	@mkdir -p docs/doxygen/xml
	@doxygen Doxyfile
	@echo "Building Sphinx documentation…"
	@cd docs && sphinx-build -E -v -b html . _build/html

# Install Python doc requirements
docs-setup:
	@echo "Installing Python doc requirements…"
	@pip install -r docs/requirements.txt

# Clean documentation
clean-docs:
	@rm -rf docs/doxygen docs/_build

clean-objects:
	rm -f $(OBJ) $(TEST_OBJ)

clean:
	rm -f libSITypes.a $(OBJ) $(GEN_C) $(GEN_H) runTests runTests.debug runTests.asan *.dSYM core *.core
	rm -f *.tab.c *.tab.h *Scanner.c *Scanner.o *.o
	rm -rf docs/doxygen docs/_build

clean-docs:
	@rm -rf docs/doxygen docs/_build

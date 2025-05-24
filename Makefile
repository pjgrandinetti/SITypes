# Makefile for SITypes static library

# Default target
.DEFAULT_GOAL := all
.SUFFIXES:

# Tools
CC      := clang
AR      := ar
LEX     := flex
YACC    := bison -y
YFLAGS  := -d

RM      := rm -f
MKDIR_P := mkdir -p

SRC_DIR        := src
TEST_SRC_DIR   := tests

# Third-party OCTypes layout
OCTYPES_DIR    := third_party/OCTypes
OCT_INCLUDE    := $(OCTYPES_DIR)/include
OCT_LIBDIR     := $(OCTYPES_DIR)/lib

# Compiler flags
CFLAGS  := -I. -I$(SRC_DIR) -I$(OCT_INCLUDE) -O3 -Wall -Wextra \
           -Wno-sign-compare -Wno-unused-parameter \
           -Wno-missing-field-initializers -Wno-unused-function \
           -MMD -MP
CFLAGS_DEBUG := -O0 -g -Wall -Wextra -Werror -MMD -MP

# Source files
LEX_SRC       := $(wildcard $(SRC_DIR)/*.l)
YACC_SRC      := $(wildcard $(SRC_DIR)/*Parser.y)
GEN_PARSER_C  := $(patsubst $(SRC_DIR)/%Parser.y,%.tab.c,$(YACC_SRC))
GEN_PARSER_H  := $(patsubst $(SRC_DIR)/%Parser.y,%.tab.h,$(YACC_SRC))
GEN_SCANNER   := $(patsubst $(SRC_DIR)/%Scanner.l,%Scanner.c,$(LEX_SRC))
GEN_C         := $(GEN_PARSER_C) $(GEN_SCANNER)
GEN_H         := $(GEN_PARSER_H)

STATIC_SRC    := $(filter-out $(YACC_SRC) $(LEX_SRC),$(wildcard $(SRC_DIR)/*.c))
ALL_C         := $(GEN_C) $(notdir $(STATIC_SRC))
OBJ           := $(ALL_C:.c=.o)
DEP           := $(OBJ:.o=.d)

TEST_C_FILES  := $(wildcard $(TEST_SRC_DIR)/*.c)
TEST_OBJ      := $(notdir $(TEST_C_FILES:.c=.o))

# OCTypes downloads
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  OCT_LIB_BIN := libOCTypes-libOCTypes-macos-latest.zip
else ifeq ($(UNAME_S),Linux)
  OCT_LIB_BIN := libOCTypes-libOCTypes-ubuntu-latest.zip
endif
OCT_LIB_ARCHIVE     := third_party/$(OCT_LIB_BIN)
OCT_HEADERS_ARCHIVE := third_party/libOCTypes-headers.zip

# After detecting OS, define linker group flags
ifeq ($(UNAME_S),Linux)
  GROUP_START := -Wl,--start-group
  GROUP_END   := -Wl,--end-group
else
  GROUP_START :=
  GROUP_END   :=
endif

.PHONY: all octypes prepare test test-debug test-asan run-asan test-werror \
        install uninstall clean clean-objects clean-docs copy-octypes \
        docs doxygen html xcode

all: octypes prepare libSITypes.a

# Fetch and extract OCTypes
octypes: $(OCT_LIBDIR)/libOCTypes.a \
         $(OCT_INCLUDE)/OCTypes/OCLibrary.h

third_party:
	@$(MKDIR_P) third_party

$(OCT_LIB_ARCHIVE): | third_party
	@echo "Fetching OCTypes library: $(OCT_LIB_BIN)"
	@curl -L https://github.com/pjgrandinetti/OCTypes/releases/download/v0.1.1/$(OCT_LIB_BIN) -o $@

$(OCT_HEADERS_ARCHIVE): | third_party
	@echo "Fetching OCTypes headers"
	@curl -L https://github.com/pjgrandinetti/OCTypes/releases/download/v0.1.1/libOCTypes-headers.zip -o $@

$(OCT_LIBDIR)/libOCTypes.a: $(OCT_LIB_ARCHIVE)
	@echo "Extracting OCTypes library"
	@$(RM) -r $(OCT_LIBDIR)
	@$(MKDIR_P) $(OCT_LIBDIR)
	@unzip -q $(OCT_LIB_ARCHIVE) -d $(OCT_LIBDIR)

$(OCT_INCLUDE)/OCTypes/OCLibrary.h: $(OCT_HEADERS_ARCHIVE)
	@echo "Extracting OCTypes headers"
	@$(RM) -r $(OCT_INCLUDE)
	@$(MKDIR_P) $(OCT_INCLUDE)/OCTypes
	@unzip -q $(OCT_HEADERS_ARCHIVE) -d $(OCT_INCLUDE)
	@mv $(OCT_INCLUDE)/*.h $(OCT_INCLUDE)/OCTypes/ 2>/dev/null || true

prepare: $(GEN_PARSER_H)

# Library target
libSITypes.a: $(OBJ)
	$(AR) rcs $@ $^

# Pattern rules for compilation
%.o: $(SRC_DIR)/%.c | octypes
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: $(TEST_SRC_DIR)/%.c | octypes
	$(CC) $(CFLAGS) -Isrc -I$(TEST_SRC_DIR) -c -o $@ $<

%.o: %.c | octypes
	$(CC) $(CFLAGS) -c -o $@ $<

# Bison rule: generate .tab.c and .tab.h
%.tab.c %.tab.h: $(SRC_DIR)/%Parser.y
	$(YACC) $(YFLAGS) $<
	mv y.tab.c $*.tab.c
	mv y.tab.h $*.tab.h

# Flex rule: generate scanner .c
%Scanner.c: $(SRC_DIR)/%Scanner.l $(patsubst %Scanner.c,%.tab.h,$@)
	$(LEX) -o $@ $<

# Tests
# Build the test runner with a linker group so order doesn't matter
runTests: libSITypes.a $(TEST_OBJ)
	$(CC) $(CFLAGS) -Isrc -I$(TEST_SRC_DIR) $(TEST_OBJ) \
	  -L. -L$(OCT_LIBDIR) \
	  $(GROUP_START) -lOCTypes -lSITypes $(GROUP_END) \
	  -lm -o runTests

test: libSITypes.a $(TEST_OBJ)
	$(CC) $(CFLAGS) -Isrc -Itests $(TEST_OBJ) \
	  -L. -L$(OCT_LIBDIR) \
	  $(GROUP_START) -lOCTypes -lSITypes $(GROUP_END) \
	  -lm -o runTests
	./runTests

# Debug tests
test-debug: CFLAGS := $(CFLAGS) $(CFLAGS_DEBUG)
test-debug: clean all test

# AddressSanitizer test target: rebuild with ASan-enabled flags and run
test-asan: libSITypes.a $(TEST_OBJ)
	$(CC) $(CFLAGS) -g -O1 -fsanitize=address -fno-omit-frame-pointer -Isrc -I$(TEST_SRC_DIR) $(TEST_OBJ) \
	  -L. -L$(OCT_LIBDIR) $(GROUP_START) -lOCTypes -lSITypes $(GROUP_END) \
	  -lm -o runTests.asan
	@echo "Running AddressSanitizer build..."
	@./runTests.asan

# Treat warnings as errors
test-werror: CFLAGS := $(CFLAGS_DEBUG)
test-werror: clean all test

# Install/uninstall
PREFIX    ?= /usr/local
INCDIR    := $(PREFIX)/include/SITypes
LIBDIR    := $(PREFIX)/lib

install: libSITypes.a $(GEN_H)
	install -d $(DESTDIR)$(INCDIR)
	install -m 0644 $(GEN_H) $(DESTDIR)$(INCDIR)
	install -d $(DESTDIR)$(LIBDIR)
	install -m 0644 libSITypes.a $(DESTDIR)$(LIBDIR)

uninstall:
	$(RM) $(DESTDIR)$(LIBDIR)/libSITypes.a
	$(RM) $(DESTDIR)$(INCDIR)/*.h
	-rmdir --ignore-fail-on-non-empty $(DESTDIR)$(INCDIR)

# Documentation targets
.PHONY: docs doxygen html

doxygen:
	@echo "Generating Doxygen XML..."
	@cd docs && doxygen Doxyfile

html: doxygen
	@echo "Building Sphinx HTML..."
	@cd docs && sphinx-build -W -E -b html . _build/html

# Alias “make docs” to build HTML
docs: html

# Generate an Xcode project using CMake
xcode:
	@echo "Generating Xcode project in build-xcode..."
	@mkdir -p build-xcode
	@cmake -G "Xcode" -S . -B build-xcode

# Clean targets
clean-objects:
	$(RM) $(OBJ) $(TEST_OBJ) runTests runTests.asan

clean: clean-objects
	$(RM) libSITypes.a runTests runTests.asan runTests.debug
	$(RM) *.tab.c *.tab.h *Scanner.c
	$(RM) *.d
	$(RM) core.* *.dSYM
	$(RM) -r docs/doxygen docs/_build

clean-docs:
	@echo "Cleaning documentation..."
	@rm -rf docs/doxygen docs/_build

# Copy locally built OCTypes library and headers
.PHONY: copy-octypes

copy-octypes:
	@echo "Copying locally built OCTypes library and headers..."
	@$(RM) -r third_party/OCTypes
	@$(MKDIR_P) third_party/OCTypes/lib third_party/OCTypes/include/OCTypes
	@cp ../OCTypes/lib/libOCTypes.a third_party/OCTypes/lib/
	@cp ../OCTypes/include/OCTypes/*.h third_party/OCTypes/include/OCTypes/

# Include dependency files
-include $(DEP)

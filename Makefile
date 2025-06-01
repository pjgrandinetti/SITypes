# Makefile for SITypes static library (modern build layout with complete scanner support)

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
BUILD_DIR      := build
OBJ_DIR        := $(BUILD_DIR)/obj
GEN_DIR        := $(BUILD_DIR)/gen
BIN_DIR        := $(BUILD_DIR)/bin

# OCTypes integration
OCTYPES_DIR    := third_party/OCTypes
OCT_INCLUDE    := $(OCTYPES_DIR)/include
OCT_LIBDIR     := $(OCTYPES_DIR)/lib

CPPFLAGS := -I. -I$(SRC_DIR) -I$(OCT_INCLUDE)
CFLAGS   := -O3 -Wall -Wextra \
             -Wno-sign-compare -Wno-unused-parameter \
             -Wno-missing-field-initializers -Wno-unused-function \
             -MMD -MP -I$(OCT_INCLUDE)
CFLAGS_DEBUG := -O0 -g -Wall -Wextra -Werror -MMD -MP

# Flex/Bison sources
LEX_SRC        := $(wildcard $(SRC_DIR)/*.l)
YACC_SRC       := $(wildcard $(SRC_DIR)/*Parser.y)

# Generated files
GEN_PARSER_C   := $(patsubst $(SRC_DIR)/%Parser.y,$(GEN_DIR)/%Parser.tab.c,$(YACC_SRC))
GEN_PARSER_H   := $(patsubst $(SRC_DIR)/%Parser.y,$(GEN_DIR)/%Parser.tab.h,$(YACC_SRC))
GEN_SCANNER    := $(patsubst $(SRC_DIR)/%.l,$(GEN_DIR)/%.c,$(LEX_SRC))
GEN_C          := $(GEN_PARSER_C) $(GEN_SCANNER)
GEN_H          := $(GEN_PARSER_H)

# Static sources and object files
STATIC_SRC     := $(filter-out $(YACC_SRC) $(LEX_SRC),$(wildcard $(SRC_DIR)/*.c))
OBJ_SRC        := $(STATIC_SRC) $(GEN_C)
OBJ            := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(filter %.c,$(STATIC_SRC))) \
                  $(patsubst $(GEN_DIR)/%.c,$(OBJ_DIR)/%.o,$(GEN_C))
DEP            := $(OBJ:.o=.d)

# Test sources and objects
TEST_C_FILES   := $(wildcard $(TEST_SRC_DIR)/*.c)
TEST_OBJ       := $(patsubst $(TEST_SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(TEST_C_FILES))

# OS-specific linking
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  OCT_LIB_BIN := libOCTypes-libOCTypes-macos-latest.zip
else ifeq ($(UNAME_S),Linux)
  OCT_LIB_BIN := libOCTypes-libOCTypes-ubuntu-latest.zip
else ifneq ($(findstring MINGW,$(UNAME_S)),)
  OCT_LIB_BIN := libOCTypes-libOCTypes-windows-latest.zip
endif
OCT_LIB_ARCHIVE     := third_party/$(OCT_LIB_BIN)
OCT_HEADERS_ARCHIVE := third_party/libOCTypes-headers.zip

ifeq ($(UNAME_S),Linux)
  GROUP_START := -Wl,--start-group
  GROUP_END   := -Wl,--end-group
else
  GROUP_START :=
  GROUP_END   :=
endif

.PHONY: all dirs octypes prepare test test-debug test-asan run-asan \
        test-werror install uninstall clean clean-objects clean-docs sync-libs docs doxygen html xcode

all: dirs octypes prepare libSITypes.a

dirs:
	$(MKDIR_P) $(BUILD_DIR) $(OBJ_DIR) $(GEN_DIR) $(BIN_DIR)

# Download and extract OCTypes
octypes: $(OCT_LIBDIR)/libOCTypes.a $(OCT_INCLUDE)/OCTypes/OCLibrary.h

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
	@unzip -q $< -d $(OCT_LIBDIR)

$(OCT_INCLUDE)/OCTypes/OCLibrary.h: $(OCT_HEADERS_ARCHIVE)
	@echo "Extracting OCTypes headers"
	@$(RM) -r $(OCT_INCLUDE)
	@$(MKDIR_P) $(OCT_INCLUDE)/OCTypes
	@unzip -q $< -d $(OCT_INCLUDE)
	@mv $(OCT_INCLUDE)/*.h $(OCT_INCLUDE)/OCTypes/ 2>/dev/null || true

prepare: $(GEN_H)

# Library
libSITypes.a: $(OBJ)
	$(AR) rcs $@ $^

# Pattern rules
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | dirs
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(GEN_DIR)/%.c | dirs
	$(CC) $(CPPFLAGS) $(CFLAGS) -I$(GEN_DIR) -c -o $@ $<

$(OBJ_DIR)/%.o: $(TEST_SRC_DIR)/%.c | dirs
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

# Bison-generated parser source and header
$(GEN_DIR)/%Parser.tab.c $(GEN_DIR)/%Parser.tab.h: $(SRC_DIR)/%Parser.y | dirs
	$(YACC) $(YFLAGS) $<
	mv y.tab.c $(GEN_DIR)/$*Parser.tab.c
	mv y.tab.h $(GEN_DIR)/$*Parser.tab.h

# Flex-generated scanner source
# For scanners that *do* depend on a parser header
$(GEN_DIR)/%Scanner.c: $(SRC_DIR)/%Scanner.l $(GEN_DIR)/%Parser.tab.h | dirs
	$(LEX) -o $@ $<

# For scanners that do *not* depend on a parser (fallback rule)
$(GEN_DIR)/%.c: $(SRC_DIR)/%.l | dirs
	$(LEX) -o $@ $<

# Tests
$(BIN_DIR)/runTests: libSITypes.a $(TEST_OBJ)
	$(CC) $(CFLAGS) -Isrc -I$(TEST_SRC_DIR) $(TEST_OBJ) \
		-L. -L$(OCT_LIBDIR) $(GROUP_START) -lOCTypes -lSITypes $(GROUP_END) -lm -o $@

test: $(BIN_DIR)/runTests
	$<

test-debug: CFLAGS := $(CFLAGS) $(CFLAGS_DEBUG)
test-debug: clean all test

test-asan: CFLAGS += -DLEAK_SANITIZER
test-asan: libSITypes.a $(TEST_OBJ)
	$(CC) $(CFLAGS) -g -O1 -fsanitize=address -fno-omit-frame-pointer -Isrc -I$(TEST_SRC_DIR) $(TEST_OBJ) \
		-L. -L$(OCT_LIBDIR) $(GROUP_START) -lOCTypes -lSITypes $(GROUP_END) -lm -o $(BIN_DIR)/runTests.asan
	@./$(BIN_DIR)/runTests.asan

test-werror: CFLAGS := $(CFLAGS_DEBUG)
test-werror: clean all test

# Internal install target layout (for local staging)
INSTALL_DIR := install
INSTALL_LIB_DIR := $(INSTALL_DIR)/lib
INSTALL_INC_DIR := $(INSTALL_DIR)/include/SITypes

install: all
	$(MKDIR_P) $(INSTALL_LIB_DIR) $(INSTALL_INC_DIR)
	cp libSITypes.a $(INSTALL_LIB_DIR)/
	cp src/*.h $(INSTALL_INC_DIR)/

# Clean
clean-objects:
	$(RM) $(OBJ) $(TEST_OBJ)

clean:
	$(RM) -r $(BUILD_DIR) libSITypes.a runTests runTests.asan runTests.debug *.dSYM
	$(RM) *.tab.* *Scanner.c *.d core.*
	$(RM) -rf docs/doxygen docs/_build docs/html build-xcode install

clean-docs:
	@echo "Cleaning documentation..."
	@rm -rf docs/doxygen docs/_build

# Copy from installed OCTypes
sync-libs:
	@echo "Copying OCTypes from installed location..."
	@$(RM) -r third_party/OCTypes
	@$(MKDIR_P) third_party/OCTypes/lib third_party/OCTypes/include/OCTypes
	@cp ../OCTypes/install/lib/libOCTypes.a third_party/OCTypes/lib/
	@cp ../OCTypes/install/include/OCTypes/*.h third_party/OCTypes/include/OCTypes/

# Docs
doxygen:
	@echo "Generating Doxygen XML..."
	cd docs && doxygen Doxyfile

html: doxygen
	@echo "Building Sphinx HTML..."
	cd docs && sphinx-build -W -E -b html . _build/html

docs: html

xcode:
	@echo "Generating Xcode project in build-xcode..."
	@mkdir -p build-xcode
	@cmake -G "Xcode" -S . -B build-xcode

# Include generated dependency files
-include $(DEP)

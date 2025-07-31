# Makefile for SITypes static library (modern build layout with complete scanner support)

.DEFAULT_GOAL := all
.SUFFIXES:

# Xcode-build directory (ensure xcode target works)
XCODE_BUILD := build-xcode

# OCTypes “third_party” integration using unified layout
TP_DIR         := third_party
TP_LIB_DIR     := $(TP_DIR)/lib
TP_INCLUDE_DIR := $(TP_DIR)/include

OCT_INCLUDE    := $(TP_INCLUDE_DIR)/OCTypes
OCT_LIBDIR     := $(TP_LIB_DIR)

# Tools
CC      := clang
AR      := ar
LEX     := flex
YACC    := bison
YFLAGS  := -d

RM      := rm -f
MKDIR_P := mkdir -p

SRC_DIR        := src
TEST_SRC_DIR   := tests
BUILD_DIR      := build
OBJ_DIR        := $(BUILD_DIR)/obj
GEN_DIR        := $(BUILD_DIR)/gen
BIN_DIR        := $(BUILD_DIR)/bin

CPPFLAGS := -I. -I$(SRC_DIR) -I$(GEN_DIR) -I$(TP_INCLUDE_DIR) -I$(OCT_INCLUDE)
CFLAGS   := -fPIC -O3 -Wall -Wextra \
             -Wno-sign-compare -Wno-unused-parameter \
             -Wno-missing-field-initializers -Wno-unused-function \
             -MMD -MP -I$(OCT_INCLUDE)
CFLAGS_DEBUG := -fPIC -O0 -g -Wall -Wextra -Werror -MMD -MP

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
ARCH := $(shell uname -m)
ifeq ($(UNAME_S),Darwin)
  OCT_LIB_BIN := libOCTypes-macos-latest.zip
else ifeq ($(UNAME_S),Linux)
  ifeq ($(ARCH),aarch64)
    OCT_LIB_BIN := libOCTypes-ubuntu-latest.arm64.zip
  else
    OCT_LIB_BIN := libOCTypes-ubuntu-latest.x64.zip
  endif
else ifneq ($(findstring MINGW,$(UNAME_S)),)
  OCT_LIB_BIN := libOCTypes-windows-latest.zip
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

.PHONY: all dirs prepare test test-debug test-asan \
        test-werror install clean clean-objects clean-docs synclib docs doxygen html xcode xcode-open xcode-run octypes help

help:
	@echo "SITypes Makefile - Available targets:"
	@echo ""
	@echo "Building:"
	@echo "  all            Build the complete SITypes library (default)"
	@echo "  libSITypes.a   Build only the static library"
	@echo "  dirs           Create build directories"
	@echo "  prepare        Generate parser/scanner files"
	@echo "  octypes        Download and extract OCTypes dependencies"
	@echo ""
	@echo "Testing:"
	@echo "  test           Build and run all tests"
	@echo "  test-debug     Build tests with debug symbols (-g -O0)"
	@echo "  test-asan      Build and run tests with AddressSanitizer"
	@echo "  test-werror    Build with strict warnings (-Werror)"
	@echo ""
	@echo "Development:"
	@echo "  install        Install library and headers to install/"
	@echo "  synclib        Copy OCTypes from ../OCTypes/install/"
	@echo "  xcode          Generate Xcode project"
	@echo "  xcode-open     Generate and open Xcode project"
	@echo "  xcode-run      Build using Xcode (requires Xcode tools)"
	@echo ""
	@echo "Documentation:"
	@echo "  docs           Build complete documentation (Doxygen + Sphinx)"
	@echo "  doxygen        Generate Doxygen XML documentation"
	@echo "  html           Build Sphinx HTML documentation"
	@echo ""
	@echo "Cleaning:"
	@echo "  clean          Remove all build artifacts"
	@echo "  clean-objects  Remove only object files"
	@echo "  clean-docs     Remove only documentation files"
	@echo ""
	@echo "Examples:"
	@echo "  make           # Build everything"
	@echo "  make test      # Build and run tests"
	@echo "  make clean all # Clean rebuild"
	@echo "  make help      # Show this help"

all: dirs octypes prepare libSITypes.a

dirs:
	$(MKDIR_P) $(BUILD_DIR) $(OBJ_DIR) $(GEN_DIR) $(BIN_DIR)

# Download and extract OCTypes (only if not already present)
octypes: $(OCT_LIBDIR)/libOCTypes.a $(OCT_INCLUDE)/OCLibrary.h

third_party:
	@$(MKDIR_P) third_party

$(OCT_LIB_ARCHIVE): | $(TP_DIR)
	@if [ ! -f $(TP_LIB_DIR)/libOCTypes.a ]; then \
		echo "Fetching OCTypes library: $(OCT_LIB_BIN)"; \
		curl -L https://github.com/pjgrandinetti/OCTypes/releases/download/v0.1.0/$(OCT_LIB_BIN) -o $@; \
	fi

$(OCT_HEADERS_ARCHIVE): | $(TP_DIR)
	@if [ ! -f $(OCT_INCLUDE)/OCLibrary.h ]; then \
		echo "Fetching OCTypes headers"; \
		curl -L https://github.com/pjgrandinetti/OCTypes/releases/download/v0.1.0/libOCTypes-headers.zip -o $@; \
	fi

$(TP_LIB_DIR)/libOCTypes.a: | $(TP_DIR)
	@if [ ! -f $@ ]; then \
		if [ ! -f $(OCT_LIB_ARCHIVE) ]; then \
			echo "Fetching OCTypes library: $(OCT_LIB_BIN)"; \
			curl -L https://github.com/pjgrandinetti/OCTypes/releases/download/v0.1.0/$(OCT_LIB_BIN) -o $(OCT_LIB_ARCHIVE); \
		fi; \
		echo "Extracting OCTypes library into $(TP_LIB_DIR)"; \
		$(RM) -r $(TP_LIB_DIR); \
		$(MKDIR_P) $(TP_LIB_DIR); \
		unzip -q $(OCT_LIB_ARCHIVE) -d $(TP_LIB_DIR); \
	else \
		echo "OCTypes library already exists at $@"; \
	fi

$(OCT_INCLUDE)/OCLibrary.h: | $(TP_DIR)
	@if [ ! -f $@ ]; then \
		if [ ! -f $(OCT_HEADERS_ARCHIVE) ]; then \
			echo "Fetching OCTypes headers"; \
			curl -L https://github.com/pjgrandinetti/OCTypes/releases/download/v0.1.0/libOCTypes-headers.zip -o $(OCT_HEADERS_ARCHIVE); \
		fi; \
		echo "Extracting OCTypes headers into $(TP_INCLUDE_DIR)/OCTypes"; \
		$(RM) -r $(OCT_INCLUDE); \
		$(MKDIR_P) $(OCT_INCLUDE); \
		unzip -q $(OCT_HEADERS_ARCHIVE) -d $(OCT_INCLUDE); \
	else \
		echo "OCTypes headers already exist at $(OCT_INCLUDE)"; \
	fi

prepare: $(GEN_H)

# Library
libSITypes.a: $(OBJ)
	$(AR) rcs $@ $^

# Pattern rules
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | dirs
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(GEN_DIR)/%.c | dirs
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(TEST_SRC_DIR)/%.c | dirs
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

# Bison-generated parser source and header
$(GEN_DIR)/%Parser.tab.c $(GEN_DIR)/%Parser.tab.h: $(SRC_DIR)/%Parser.y | dirs
	$(YACC) $(YFLAGS) $<
	# Move modern Bison outputs into gen dir
	mv $(basename $(notdir $<)).tab.c $(GEN_DIR)/$*Parser.tab.c
	mv $(basename $(notdir $<)).tab.h $(GEN_DIR)/$*Parser.tab.h

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

# Run tests
test: octypes libSITypes.a $(TEST_OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) -I$(TEST_SRC_DIR) $(TEST_OBJ) \
	  -L. -L$(OCT_LIBDIR) -lSITypes -lOCTypes -lm -o runTests
	./runTests

test-debug: octypes libSITypes.a $(TEST_OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) -g -O0 -I$(TEST_SRC_DIR) $(TEST_OBJ) \
	  -L. -L$(OCT_LIBDIR) -lSITypes -lOCTypes -lm -o runTests.debug

test-asan: octypes libSITypes.a $(TEST_OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) -g -O1 -fsanitize=address -fno-omit-frame-pointer \
	  -I$(TEST_SRC_DIR) $(TEST_OBJ) -L. -L$(OCT_LIBDIR) \
	  -lSITypes -lOCTypes -lm -o runTests.asan
	@./runTests.asan

test-werror: CFLAGS := $(CFLAGS_DEBUG)
test-werror: clean all test

# Internal install target layout (for local staging)
INSTALL_DIR := install
INSTALL_LIB_DIR := $(INSTALL_DIR)/lib
INSTALL_INC_DIR := $(INSTALL_DIR)/include/SITypes

install: libSITypes.a
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
synclib:
	@echo "Copying OCTypes into third_party/lib and include..."
	@$(MKDIR_P) third_party
	@$(RM) -r third_party/lib third_party/include
	@$(MKDIR_P) third_party/lib third_party/include/OCTypes
	@cp ../OCTypes/install/lib/libOCTypes.a third_party/lib/
	@cp ../OCTypes/install/include/OCTypes/*.h third_party/include/OCTypes/

# Docs
doxygen:
	@echo "Generating Doxygen XML..."
	cd docs && doxygen Doxyfile

html: doxygen
	@echo "Building Sphinx HTML..."
	cd docs && sphinx-build -W -E -b html . _build/html

docs: html

# Include generated dependency files
-include $(DEP)
##────────────────────────────────────────────────────────────────────────────
## 11) XCODE target
##────────────────────────────────────────────────────────────────────────────

# Workspace root (one level up) for combined CMake configuration
ROOT_DIR := $(shell cd $(dir $(firstword $(MAKEFILE_LIST))).. && pwd)

xcode: clean dirs
	@echo "Generating combined SITypes+OCTypes Xcode project in $(XCODE_BUILD)/ ..."
	@mkdir -p $(XCODE_BUILD)
	@cmake -G "Xcode" -S $(ROOT_DIR) -B $(XCODE_BUILD)
	@echo "✅ Combined Xcode project created: $(XCODE_BUILD)/$(notdir $(ROOT_DIR)).xcodeproj"

xcode-open: xcode
	@echo "Opening SITypes Xcode project..."
	open $(XCODE_BUILD)/SITypes.xcodeproj

xcode-run: xcode
	@echo "Building SITypes (and OCTypes) inside Xcode workspace..."
	xcodebuild -project $(XCODE_BUILD)/SITypes.xcodeproj \
	           -configuration Debug \
	           -scheme SITypes \
	           -destination 'platform=macOS' \
	           build | xcpretty || true

##────────────────────────────────────────────────────────────────────────────
